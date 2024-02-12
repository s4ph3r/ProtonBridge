/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"

#include "OnBoard.h"
#include "hal_adc.h"
#include "hal_led.h"
#include "hal_key.h"


#include "gatt.h"

#include "hci.h"

#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"
#include "protonprofile.h"

#include "peripheral.h"

#include "gapbondmgr.h"

#include "protonBridge.h"
#include "npi.h" 


#include "stdlib.h"
#include "stdio.h"

//#include "simplekeys.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */



// How often to perform periodic event
#define SBP_PERIODIC_EVT_PERIOD                   2000

// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely

#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL    80//1580//80

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     1980//1600//800

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          600//1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         TRUE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// Company Identifier: Texas Instruments Inc. (13)
#define TI_COMPANY_ID                         0x000D

#define INVALID_CONNHANDLE                    0xFFFF

// Length of bd addr as a string
#define B_ADDR_STR_LEN                        15

//Размер кольцевого буффера 
#define SIZE_BUF                              128
/////////////////////////////////////////////////


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
uint8 MainMeasure[13] =  { 0x00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00 }; 
uint8 AdditMeasure[20] =  { 0x14,   0x00, 0x23, 0x00, 0x4B, 0x00, 0x0E, 0x0B, 0x11, 0x10, 0x37, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8 CallData[18] = { 0x00, 0x80, 0xD4, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */


//кольцевой (циклический) буфер
unsigned char cycleBuf[SIZE_BUF];
unsigned char cycleBufTail = 0;      //"указатель" хвоста буфера 
unsigned char cycleBufHead = 0;   //"указатель" головы буфера
unsigned char cycleBufCount = 0;  //счетчик символов




static uint8 protonBridge_TaskID;   // Task ID for internal task/event processing

//static gaprole_States_t gapProfileState = GAPROLE_INIT;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 scanRspData[] =
{
  // complete name
  0x0A,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  0x41, // A
  0x74, // t
  0x6f, // o
  0x6d, // m
  0x46, // F
  0x61, // a
  0x73, // s
  0x74, // t
  0x20,
  
  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),   
  HI_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  LO_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),   
  HI_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
    

};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8 advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x11,   // length of this data
  GAP_ADTYPE_128BIT_MORE,      // some of the UUID's, but not all
  PROTONPROFILE_SERV_UUID
};

// GAP GATT Attributes
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "";//"Proton Bridge";





/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void protonBridge_ProcessOSALMsg( osal_event_hdr_t *pMsg );
static void protonBridge_ProcessGATTMsg( gattMsgEvent_t *pMsg );
static void peripheralStateNotificationCB( gaprole_States_t newState );
static void performPeriodicTask( void );
static void protonProfileChangeCB( uint8 paramID );
static void NpiSerialCallback( uint8 port, uint8 events );     
   

static void protonBridge_HandleKeys( uint8 shift, uint8 keys ); //////////////////////////////////////////////////////////////////

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t protonBridge_PeripheralCBs =
{
  peripheralStateNotificationCB,  // Profile State Change Callbacks
  NULL                            // When a valid RSSI is read from controller (not used by application)
};

// GAP Bond Manager Callbacks
static gapBondCBs_t protonBridge_BondMgrCBs =
{
  NULL,                     // Passcode callback (not used by application)
  NULL                      // Pairing / Bonding state Callback (not used by application)
};

// GATT Profile Callbacks
static protonProfileCBs_t protonBridge_ProtonProfileCBs =
{
  protonProfileChangeCB    // Charactersitic value change callback
};



/*********************************************************************
 * PUBLIC FUNCTIONS
 */
////////////////////////////////////////////////////////////////////
//Функции кольцевого буффера
//
//"очищает" буфер
void FlushBuf(void)
{
  cycleBufTail = 0;
  cycleBufHead = 0;
  cycleBufCount = 0;
}
//положить символ в буфер
void PutChar(unsigned char sym)
{
  if (cycleBufCount < SIZE_BUF){   //если в буфере еще есть место
      cycleBuf[cycleBufTail] = sym;    //помещаем в него символ
      cycleBufCount++;                    //инкрементируем счетчик символов
      cycleBufTail++;                           //и индекс хвоста буфера
      if (cycleBufTail == SIZE_BUF) cycleBufTail = 0;
    }
}
//взять символ из буфера
unsigned char GetChar(void)
{
   unsigned char sym = 0;
   if (cycleBufCount > 0){                            //если буфер не пустой
      sym = cycleBuf[cycleBufHead];              //считываем символ из буфера
      cycleBufCount--;                                   //уменьшаем счетчик символов
      cycleBufHead++;                                  //инкрементируем индекс головы буфера
      if (cycleBufHead == SIZE_BUF) cycleBufHead = 0;
   }
   return sym;
}
/////////////////////////////////////////////////////////////////////////////

/*********************************************************************
 * @fn      ProtonBridge_Init
 *
 * @brief   Initialization function for the ProtonBridge App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void ProtonBridge_Init( uint8 task_id )
{
  protonBridge_TaskID = task_id;

  // Setup the GAP
  VOID GAP_SetParamValue( TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL );
  
  // Setup the GAP Peripheral Role Profile
  {

    // Device starts advertising upon initialization
    uint8 initial_advertising_enable = TRUE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16 gapRole_AdvertOffTime = 100;//0;

    uint8 enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16 desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16 desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16 desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    GAPRole_SetParameter( GAPROLE_ADVERT_OFF_TIME, sizeof( uint16 ), &gapRole_AdvertOffTime );

    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );
    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );

    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
  }

  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );

  // Set advertising interval
  {
    uint16 advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MAX, advInt );
    GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, advInt );
  }

  // Setup the GAP Bond Manager
  {
    uint32 passkey = 0; // passkey "000000"
    uint8 pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8 mitm = TRUE;
    uint8 ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    uint8 bonding = TRUE;
    GAPBondMgr_SetParameter( GAPBOND_DEFAULT_PASSCODE, sizeof ( uint32 ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_BONDING_ENABLED, sizeof ( uint8 ), &bonding );
  }

  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );            // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES );    // GATT attributes
  DevInfo_AddService();                           // Device Information Service
  ProtonProfile_AddService( GATT_ALL_SERVICES );  // Proton GATT Profile
  
  // Setup the ProtonProfile Characteristic Values
  {
    uint8 charValue1[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; 
    uint8 charValue2[20] = { 0x14,   0x00, 0x23, 0x00, 0x4B, 0x00, 0x0E, 0x0B, 0x11, 0x10, 0x37, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8 charValue3[10] = { 0x00, 0x00, 0x20, 0x41, 0x9A, 0x99, 0x99, 0x3E, 0x22, 0x00 }; 
    uint8 charValue4[10] = { 0x00, 0x00, 0x48, 0x42, 0xCD, 0xCC, 0x8C, 0x3F, 0x22, 0x00 };
    uint8 charValue5[10] = { 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x20, 0x40, 0x22, 0x20 };
    uint8 charValue6[8] = { 0x00, 0x00, 0x00, 0xA0, 0x0F, 0x00, 0x00, 0x00 };  
    uint8 charValue7[18] = { 0x00, 0x80, 0xD4, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8 charValue8[17] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8 charValue9[20] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    ProtonProfile_SetParameter( PROTONPROFILE_CHAR1, 13, charValue1);    
    ProtonProfile_SetParameter( PROTONPROFILE_CHAR2, 20, charValue2 ); 
    ProtonProfile_SetParameter( PROTONPROFILE_CHAR3, 10, charValue3 );
    ProtonProfile_SetParameter( PROTONPROFILE_CHAR4, 10, charValue4 );
    ProtonProfile_SetParameter( PROTONPROFILE_CHAR5, 10, charValue5 );
    ProtonProfile_SetParameter( PROTONPROFILE_CHAR6, 8,  charValue6 );
    ProtonProfile_SetParameter( PROTONPROFILE_CHAR7, 18, charValue7 );
    ProtonProfile_SetParameter( PROTONPROFILE_CHAR8, 17, charValue8 );
    ProtonProfile_SetParameter( PROTONPROFILE_CHAR9, 20, charValue9 );
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Register for all key events - This app will handle all key events
  RegisterForKeys( protonBridge_TaskID );

  P1DIR = ~(BV(0)); // Port 1 pins P1.0  as input (buttons),
  P1 = BV(0); // All pins on port 1 to low except for P1.0 (buttons)
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

  // Register callback with SimpleGATTprofile
  VOID ProtonProfile_RegisterAppCBs( &protonBridge_ProtonProfileCBs );

  // Enable clock divide on halt
  // This reduces active current while radio is active and CC254x MCU
  // is halted
  HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT );


   //////////////////////////////////////////////
  //UART Init
  NPI_InitTransport(NpiSerialCallback);   
  //NPI_PrintString("\r\n"); 
  //NPI_WriteTransport("ProtonBridge init OK\r\n", 22);  
  //NPI_PrintValue("123 = 0x", 0x88, 16);  

  
  
#if defined ( DC_DC_P0_7 )

  // Enable stack to toggle bypass control on TPS62730 (DC/DC converter)
  HCI_EXT_MapPmIoPortCmd( HCI_EXT_PM_IO_PORT_P0, HCI_EXT_PM_IO_PORT_PIN7 );

#endif // defined ( DC_DC_P0_7 )

  // Setup a delayed profile startup
  osal_set_event( protonBridge_TaskID, SBP_START_DEVICE_EVT );
 
  //call in order to allow more CPU time for driver
  HCI_EXT_HaltDuringRfCmd(HCI_EXT_HALT_DURING_RF_DISABLE);
  
  


}


/*********************************************************************
 * @fn      ProtonBridge_ProcessEvent
 *
 * @brief   ProtonBridge Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 ProtonBridge_ProcessEvent( uint8 task_id, uint16 events )
{

  VOID task_id; // OSAL required parameter that isn't used in this function

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = osal_msg_receive( protonBridge_TaskID )) != NULL )
    {
      protonBridge_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      // Release the OSAL message
      VOID osal_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & SBP_START_DEVICE_EVT )
  {
    // Start the Device
    VOID GAPRole_StartDevice( &protonBridge_PeripheralCBs );

    // Start Bond Manager
    VOID GAPBondMgr_Register( &protonBridge_BondMgrCBs );

    // Set timer for first periodic event
    osal_start_timerEx( protonBridge_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );

    return ( events ^ SBP_START_DEVICE_EVT );
  }

  if ( events & SBP_PERIODIC_EVT )
  {
    // Restart timer
    if ( SBP_PERIODIC_EVT_PERIOD )
    {
      osal_start_timerEx( protonBridge_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    }

    // Perform periodic application task
    performPeriodicTask();

    return (events ^ SBP_PERIODIC_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      protonBridge_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void protonBridge_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {     
     case KEY_CHANGE:
      protonBridge_HandleKeys( ((keyChange_t *)pMsg)->state, 
                                      ((keyChange_t *)pMsg)->keys );
      break;
    
    
    case GATT_MSG_EVENT:
      // Process GATT message
      protonBridge_ProcessGATTMsg( (gattMsgEvent_t *)pMsg );
      break;
      
    default:
      // do nothing
      break;
  }
}



/*********************************************************************
 * @fn      protonBridge_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void protonBridge_HandleKeys( uint8 shift, uint8 keys )
{
  VOID shift;  // Intentionally unreferenced parameter
  if ( keys & HAL_KEY_SW_1 )
  {
          //NPI_PrintString("KEYS\r\n");
       HalLedBlink( (HAL_LED_2 ), 3,50, 500 );
//       NPI_PrintString("KEYS\r\n"); 
       
  }

}



/*********************************************************************
 * @fn      protonBridge_ProcessGATTMsg
 *
 * @brief   Process GATT messages
 *
 * @return  none
 */
static void protonBridge_ProcessGATTMsg( gattMsgEvent_t *pMsg )
{  
  GATT_bm_free( &pMsg->msg, pMsg->method );
}

/*********************************************************************
 * @fn      peripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateNotificationCB( gaprole_States_t newState )
{
  
  switch ( newState )
  {
    case GAPROLE_STARTED:
      {
        uint8 ownAddress[B_ADDR_LEN];
        uint8 systemId[DEVINFO_SYSTEM_ID_LEN];

        GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

        // use 6 bytes of device address for 8 bytes of system ID value
        systemId[0] = ownAddress[0];
        systemId[1] = ownAddress[1];
        systemId[2] = ownAddress[2];

        // set middle bytes to zero
        systemId[4] = 0x00;
        systemId[3] = 0x00;

        // shift three bytes up
        systemId[7] = ownAddress[5];
        systemId[6] = ownAddress[4];
        systemId[5] = ownAddress[3];

        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);

 
      }
      break;

    case GAPROLE_ADVERTISING:
      {
       // NPI_PrintString("BT=Advertising\r\n"); 
       // HalLedBlink( (HAL_LED_2 ), 0,50, 1000 ); //uint8 leds, uint8 cnt, uint8 duty, uint16 time
        
        HalLedBlink( (HAL_LED_3 ), 3,50, 200 );

      }
      break;
      
    case GAPROLE_CONNECTED:
      {        
      //  NPI_PrintString("BT=Connected\r\n"); 
       // HalLedBlink( (HAL_LED_2 ), 0,50, 100 ); //uint8 leds, uint8 cnt, uint8 duty, uint16 time
 
        HalLedBlink( (HAL_LED_3 ), 1,50, 1000 );
      }
      break;

    case GAPROLE_CONNECTED_ADV:
      {
       // NPI_PrintString("BT=Connected_Advertising\r\n"); 

      }
      break;      
    case GAPROLE_WAITING:
      {
     //   NPI_PrintString("BT=Advertising_Wait\r\n");
          
        uint8 advertEnabled = TRUE;
      
         //Enabled connectable advertising.
         GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8),
                             &advertEnabled);
      }
      break;

    case GAPROLE_WAITING_AFTER_TIMEOUT:
      {
      //  NPI_PrintString("BT=Timed_Out\r\n");
      }
      break;

    case GAPROLE_ERROR:
      {
       // NPI_PrintString("BT=Error\r\n");
      }
      break;

    default:
      {

      }
      break;

  }

  //gapProfileState = newState;



}

/*********************************************************************
 * @fn      performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets
 *          called every five seconds as a result of the SBP_PERIODIC_EVT
 *          OSAL event. In this example, the value of the third
 *          characteristic in the SimpleGATTProfile service is retrieved
 *          from the profile, and then copied into the value of the
 *          the fourth characteristic.
 *
 * @param   none
 *
 * @return  none
 */
static void performPeriodicTask( void )
{

  


  
 // HalLedBlink( (HAL_LED_2 ), 3,50, 200 );
  
  

  
  
  if (cycleBufCount>1)
  {
    while(cycleBufCount!=0)
    {
        switch (GetChar())
          {
          case 0xA0: //Команда записи полученных из UART данных в переменные  
              
              switch (GetChar())
                {
                case 0x00: //Флаги
                     MainMeasure[0]=GetChar();
                    break;
                    
                case 0x01: //Нак. доза
                     MainMeasure[1]=GetChar();
                     MainMeasure[2]=GetChar();
                     MainMeasure[3]=GetChar();
                     MainMeasure[4]=GetChar();
                    break;

                case 0x02: //Мощн. дозы
                     MainMeasure[5]=GetChar();
                     MainMeasure[6]=GetChar();
                     MainMeasure[7]=GetChar();
                     MainMeasure[8]=GetChar();
                    break;

                 case 0x03: //Кол. имп.за 2с.
                     MainMeasure[9]=GetChar();
                     MainMeasure[10]=GetChar();
                    break;
                    
                case 0x04: //Заряд батареи
                     MainMeasure[11]=GetChar();
                    break;
                    
                case 0x05: //Температура
                     MainMeasure[12]=GetChar();
                    break; 
                    
                //-------
                 //Как выяснилось, эти характиристики не используются.   
                case 0x10: //Общее число импульсов
                     AdditMeasure[0]=GetChar();
                     AdditMeasure[1]=GetChar();
                     AdditMeasure[2]=GetChar();
                     AdditMeasure[3]=GetChar();
                     AdditMeasure[4]=GetChar();
                     AdditMeasure[5]=GetChar();
                     AdditMeasure[6]=GetChar();
                     AdditMeasure[7]=GetChar();
                    break;               
                    
                 case 0x11: //Число импульсов коррекции на мёртвое время датчика
                     AdditMeasure[8]=GetChar();
                     AdditMeasure[9]=GetChar();
                     AdditMeasure[10]=GetChar();
                     AdditMeasure[11]=GetChar();
                    break;              
                    
                 case 0x12: //Общее число импульсов за N секунд
                     AdditMeasure[12]=GetChar();
                     AdditMeasure[13]=GetChar();
                     AdditMeasure[14]=GetChar();
                     AdditMeasure[15]=GetChar();
                    break;                  
                    
                 case 0x13: //Время измерения дозы в секундах.
                     AdditMeasure[16]=GetChar();
                     AdditMeasure[17]=GetChar();
                     AdditMeasure[18]=GetChar();
                     AdditMeasure[19]=GetChar();
                    break;                 
                    
                    //------------------------
                    
                 case 0x60: //Чувствительность датчика
                     CallData[0]=GetChar();
                     CallData[1]=GetChar();
                     CallData[2]=GetChar();
                     CallData[3]=GetChar();
                    break;  
                    
                 case 0x61: //Фоновый счёт датчика
                     CallData[4]=GetChar();
                     CallData[5]=GetChar();
                     CallData[6]=GetChar();
                     CallData[7]=GetChar();
                    break; 
                    
                 case 0x62: //Мертвое время датчика
                     CallData[8]=GetChar();
                     CallData[9]=GetChar();
                     CallData[10]=GetChar();
                     CallData[11]=GetChar();
                    break;   
                    
                 case 0x63: //Время измерения мощности дозы
                     CallData[12]=GetChar();
                     CallData[13]=GetChar();
                    break;                
                    
                 case 0x64: //Пароль для записи калибровки в RAM
                     CallData[14]=GetChar();
                     CallData[15]=GetChar();
                     CallData[16]=GetChar();
                     CallData[17]=GetChar();               
                    break;                
                    
                default:
                  
                  break;
                }
              break;
                
          default:
            break;              
    
          case 0xB0: //Команда чтения данных из переменных для отправки в UART
            
            
            break;
            
        } 
        
        
        
        
    }
    
    
  }              

  

  ProtonProfile_SetParameter( PROTONPROFILE_CHAR1, 13, &MainMeasure);
  ProtonProfile_SetParameter( PROTONPROFILE_CHAR2, 20, &AdditMeasure);
  ProtonProfile_SetParameter( PROTONPROFILE_CHAR7,18, &CallData);
  
}

/*********************************************************************
 * @fn      protonProfileChangeCB
 *
 * @brief   Callback from ProtonProfile indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
static void protonProfileChangeCB( uint8 paramID )
{
  uint8 newValue1[10];
  uint8 newValue2[10];
  uint8 newValue3[10];
  uint8 newValue4[8];
  uint8 newValue5[18];
  uint8 newValue6[17];
  uint8 newValue7[20];

  switch( paramID )
  {
    case PROTONPROFILE_CHAR3:
      ProtonProfile_GetParameter( PROTONPROFILE_CHAR3, &newValue1 );
      break;

    case PROTONPROFILE_CHAR4:
      ProtonProfile_GetParameter( PROTONPROFILE_CHAR4, &newValue2 );
      break;      
      
    case PROTONPROFILE_CHAR5:
      ProtonProfile_GetParameter( PROTONPROFILE_CHAR5, &newValue3 );
      break;
      
    case PROTONPROFILE_CHAR6:
      ProtonProfile_GetParameter( PROTONPROFILE_CHAR6, &newValue4 );
      break;       
      
    case PROTONPROFILE_CHAR7:
      ProtonProfile_GetParameter( PROTONPROFILE_CHAR7, &newValue5 );
      break;        
      
    case PROTONPROFILE_CHAR8:
      ProtonProfile_GetParameter( PROTONPROFILE_CHAR8, &newValue6 );
      break;          
      
    case PROTONPROFILE_CHAR9:
      ProtonProfile_GetParameter( PROTONPROFILE_CHAR9, &newValue7 );
      break;          
      
    default:
      // should not reach here!
      break;
  }
}


//Обработка прерывания получения данный по UART
static void NpiSerialCallback( uint8 port, uint8 events )
{
  
    (void)port;
 
    if (events & (HAL_UART_RX_TIMEOUT | HAL_UART_RX_FULL))   //Serial port has data
    {
        uint8 numBytes = 0;
 
        numBytes = NPI_RxBufLen();           //Read out how many bytes are in the serial buffer
        
        if(numBytes == 0)
        {
            return;
        }
        else
        {
          
            //Application buffer
            uint8 *buffer = osal_mem_alloc(numBytes);

            if(buffer)
            {
                //Read serial buffer data, release serial data
                NPI_ReadTransport(buffer,numBytes);   
                
                //Набиваем циклический буффер
                for (uint8 i=0; i<numBytes; i++)
                {
                  PutChar(buffer[i]);
                }
             

                 //VOID osal_memcpy( &temp, &buffer, 4 );
                 //VOID osal_memcpy( &MainMeasure[5], &buffer, 4 );
                //NPI_PrintValue("buffer:", &buffer, 16); 
                //NPI_WriteTransport(&numBytes, 1);  
                
                 //NPI_PrintString("\r\nRecive data:");  
                 //Send the received data to the serial port-implement loopback            
                 //NPI_WriteTransport(buffer, numBytes);  
                // NPI_PrintValue("\r\nnumBytes:", numBytes, 16); 

                 
                //Release application buffer
                osal_mem_free(buffer);
            }
        }
    }
}






/*********************************************************************
 * @fn      bdAddr2Str
 *
 * @brief   Convert Bluetooth address to string. Only needed when
 *          LCD display is used.
 *
 * @return  none

char *bdAddr2Str( uint8 *pAddr )
{
  uint8       i;
  char        hex[] = "0123456789ABCDEF";
  static char str[B_ADDR_STR_LEN];
  char        *pStr = str;

  *pStr++ = '0';
  *pStr++ = 'x';

  // Start from end of addr
  pAddr += B_ADDR_LEN;

  for ( i = B_ADDR_LEN; i > 0; i-- )
  {
    *pStr++ = hex[*--pAddr >> 4];
    *pStr++ = hex[*pAddr & 0x0F];
  }

  *pStr = 0;

  return str;
}
 */

/*********************************************************************
*********************************************************************/
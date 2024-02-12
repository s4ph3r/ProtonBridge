#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "protonprofile.h"
#include "OSAL_Clock.h"

#define SERVAPP_NUM_ATTR_SUPPORTED 15
#define ATT_UUID_SIZE 16

//Если заработает снести нахрен
CONST uint8 protonServUUID[ATT_UUID_SIZE] =
{
	PROTON_SERVICE_UUID
};

CONST uint8 protonTextDescriptionUUID[ATT_UUID_SIZE] =
{
	PROTON_TD_UUID
};


static ProtonCBs_t* protonAppCBs_t = NULL;
//attribute definitions
static CONST gattAttrType_t protonService = {ATT_UUID_SIZE, protonServUUID};

static uint8 protonTextDescriptionCharProps = GATT_PROP_NOTIFY|GATT_PROP_READ;
static uint8 protonTextDescriptionValue = 0x00;
static gattCharCfg_t *protonTextDescriptionConfig;
static uint8 protonTextDescriptionUserDesc[]="Text Description variable\0";


//attribute table
static gattAttribute_t ProtonProfileAttrTable[5]={
	
	//Service
	{
		{ ATT_BT_UUID_SIZE, primaryServiceUUID },
		GATT_PERMIT_READ,
		0,
		(uint8 *)&protonServUUID
	},

	//TextDescription
	{
		{ ATT_BT_UUID_SIZE, characterUUID },
		GATT_PERMIT_READ,
		0,
		&protonTextDescriptionCharProps
	},
	{
		{ATT_BT_UUID_SIZE, protonTextDescriptionUUID },
		GATT_PERMIT_READ,
		0,
		(uint8 *)&protonTextDescriptionValue
	},
	{
		{ATT_BT_UUID_SIZE , clientCharCfgUUID},
		GATT_PERMIT_READ | GATT_PERMIT_WRITE,
		0,
		(uint8 *)&protonTextDescriptionConfig
	},
	{
		{ ATT_BT_UUID_SIZE, charUserDescUUID },
		GATT_PERMIT_READ,
		0,
		protonTextDescriptionUserDesc
	}
};


static uint8 proton_ReadAttrCB( uint16 connHandle,
                               gattAttribute_t *pAttr, uint8 *pValue,
                               uint8 *pLen, uint16 offset,
                               uint8 maxLen, uint8 method );

static bStatus_t proton_WriteAttrCB( uint16 connHandle,
                                    gattAttribute_t *pAttr, uint8 *pValue,
                                    uint8 len, uint16 offset, uint8 method );

static void proton_HandleConnStatusCB( uint16 connHandle, uint8 changeType );

CONST gattServiceCBs_t PROTON_CBs =
{
	proton_ReadAttrCB, // Read callback function pointer
	proton_WriteAttrCB, // Write callback function pointer
	NULL
};

bStatus_t Proton_AddService()
{
	uint8 status = SUCCESS;
	GATTServApp_InitCharCfg( INVALID_CONNHANDLE, protonTextDescriptionConfig );
	VOID linkDB_Register( proton_HandleConnStatusCB );
	
        status = GATTServApp_RegisterService(ProtonProfileAttrTable, 
                                             GATT_NUM_ATTRS(ProtonProfileAttrTable), 
                                             GATT_MAX_ENCRYPT_KEY_SIZE,
                                             &PROTON_CBs );
	return ( status );
}


bStatus_t Proton_RegisterAppCBs( ProtonCBs_t *appCallbacks ){
  if ( appCallbacks )
  {
    protonAppCBs_t = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

bStatus_t Proton_SetParameter( uint8 param, uint8 len, void *value ){
    bStatus_t ret = SUCCESS;
    switch ( param )
    {
    case PTD_NAME_ATTR:
      if(len == sizeof(uint8))
      {
        protonTextDescriptionValue = *((uint8*)value);
        GATTServApp_ProcessCharCfg (protonTextDescriptionConfig, (uint8 *)&protonTextDescriptionValue
                                    , FALSE, ProtonProfileAttrTable 
                                    ,GATT_NUM_ATTRS(ProtonProfileAttrTable), INVALID_TASK_ID, NULL);
      }
      else{
        ret = bleInvalidRange;
      }
      break;
 
    default:
       ret = INVALIDPARAMETER;
      break;
  
    } 
   return(ret);
}

static uint8 proton_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen, uint8 method )
{
                             
    bStatus_t status = SUCCESS;
    if ( offset > 0 )
    {
      return ( ATT_ERR_ATTR_NOT_LONG );
    }
    
    if ( pAttr->type.len == ATT_UUID_SIZE )
    {    
      // 128-bit UUID
      uint8 uuid[ATT_UUID_SIZE];
      osal_memcpy(uuid, pAttr->type.uuid, ATT_UUID_SIZE);
    
      if(//osal_memcmp(uuid,hhPWM2UUID,ATT_UUID_SIZE)||
       //osal_memcmp(uuid,hhPWM1UUID,ATT_UUID_SIZE)||
       osal_memcmp(uuid,protonTextDescriptionUUID,ATT_UUID_SIZE)//||
        // osal_memcmp(uuid,hhButton1UUID,ATT_UUID_SIZE)
        )
      {
       *pLen = 1;
        pValue[0] = *pAttr->pValue;
      }
    } 
    else
    {
    // 16-bit UUID
      *pLen = 0;
      status = ATT_ERR_INVALID_HANDLE;
    }


  return ( status );
}

static bStatus_t proton_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                uint8 *pValue, uint8 len, uint16 offset, uint8 method ){

     bStatus_t status = SUCCESS;
     uint8 notify = 0xFF;
     if ( pAttr->type.len == ATT_UUID_SIZE )
    {
        const uint8 uuid[ATT_UUID_SIZE] = {
          PROTON_TD_UUID
        };
        if(osal_memcmp(uuid,protonTextDescriptionUUID,ATT_UUID_SIZE))
        {
            if ( offset == 0 )
            {
              if ( len != 1 ){
                status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            }
            else
            {
                status = ATT_ERR_ATTR_NOT_LONG;
            }
            if ( status == SUCCESS )
            {
              uint8 *pCurValue = (uint8 *)pAttr->pValue;
              *pCurValue = pValue[0];
              notify = PTD_NAME_ATTR;        
        }
        }
            else if(osal_memcmp(uuid,protonTextDescriptionUUID,ATT_UUID_SIZE)){
              
                  if ( offset == 0 )
                {
                  if ( len != 1 ){
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                    }
                }
                else
                {
                    status = ATT_ERR_ATTR_NOT_LONG;
                }
                if ( status == SUCCESS )
                {
                  uint8 *pCurValue = (uint8 *)pAttr->pValue;
                  *pCurValue = pValue[0];
                  notify = PTD_NAME_ATTR;
            }
        
      }
    }
   else if (pAttr->type.len== ATT_BT_UUID_SIZE)
  {
    uint16 uuid= BUILD_UINT16(pAttr->type.uuid[0],pAttr->type.uuid[1]);
    switch(uuid){
    case GATT_CLIENT_CHAR_CFG_UUID:
      status=GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset, GATT_CLIENT_CFG_NOTIFY);
      break;
    default:
      status = ATT_ERR_ATTR_NOT_FOUND;
    }
  }
  else{
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If an attribute changed then callback function to notify application of change
  if ( (notify != 0xFF) && protonAppCBs_t && protonAppCBs_t->pfnProtonCB )
    protonAppCBs_t->pfnProtonCB(notify);  
                                  
   return ( status );                               
                                                               
}

static void proton_HandleConnStatusCB( uint16 connHandle, uint8 changeType ){
if ( connHandle != LOOPBACK_CONNHANDLE )
  {
    if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
         ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) && 
           ( !linkDB_Up( connHandle ) ) ) )
    { 
      GATTServApp_InitCharCfg ( connHandle, protonTextDescriptionConfig);
      //GATTServApp_InitCharCfg ( connHandle, hhButton2Config);
      
    }
  }

}
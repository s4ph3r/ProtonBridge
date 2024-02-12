/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "protonprofile.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        29 //....................

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Proton GATT Profile Service UUID: 0xFFF0
CONST uint8 protonProfileServUUID[16] =
{ 
  PROTONPROFILE_SERV_UUID
};

// Characteristic 1 UUID: 
CONST uint8 protonProfilechar1UUID[16] =
{ 
  PROTONPROFILE_CHAR1_UUID
};

// Characteristic 2 UUID: 
CONST uint8 protonProfilechar2UUID[16] =
{ 
  PROTONPROFILE_CHAR2_UUID
};

// Characteristic 3 UUID: 
CONST uint8 protonProfilechar3UUID[16] =
{ 
  PROTONPROFILE_CHAR3_UUID
};

// Characteristic 4 UUID: 
CONST uint8 protonProfilechar4UUID[16] =
{ 
  PROTONPROFILE_CHAR4_UUID
};

// Characteristic 5 UUID: 
CONST uint8 protonProfilechar5UUID[16] =
{ 
  PROTONPROFILE_CHAR5_UUID
};
   
// Characteristic 6 UUID: 
CONST uint8 protonProfilechar6UUID[16] =
{ 
  PROTONPROFILE_CHAR6_UUID
};   

// Characteristic 7 UUID: 
CONST uint8 protonProfilechar7UUID[16] =
{ 
  PROTONPROFILE_CHAR7_UUID
};

// Characteristic 8 UUID: 
CONST uint8 protonProfilechar8UUID[16] =
{ 
  PROTONPROFILE_CHAR8_UUID
};
   
// Characteristic 9 UUID: 
CONST uint8 protonProfilechar9UUID[16] =
{ 
  PROTONPROFILE_CHAR9_UUID
};  

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static protonProfileCBs_t *protonProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

//  Profile Service attribute
static CONST gattAttrType_t protonProfileService = { 16, protonProfileServUUID };


//  Profile Characteristic 1 Properties
static uint8 protonProfileChar1Props = GATT_PROP_NOTIFY;

// Characteristic 1 Value
static uint8 protonProfileChar1[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//  Profile Characteristic 1 Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
static gattCharCfg_t *protonProfileChar1Config;
                                        
//  Profile Characteristic 1 User Description
static uint8 protonProfileChar1UserDesp[20] = "MeasValue and Flags";

//  Profile Characteristic 2 Properties
static uint8 protonProfileChar2Props = GATT_PROP_READ;

// Characteristic 2 Value
static uint8 protonProfileChar2[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//  Profile Characteristic 2 User Description
static uint8 protonProfileChar2UserDesp[17] = "MeasValues2";


//  Profile Characteristic 3 Properties
static uint8 protonProfileChar3Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 3 Value
static uint8 protonProfileChar3[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//  Profile Characteristic 3 User Description
static uint8 protonProfileChar3UserDesp[17] = "First Treshold";

//  Profile Characteristic 4 Properties
static uint8 protonProfileChar4Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 4 Value
static uint8 protonProfileChar4[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//  Profile Characteristic 4 User Description
static uint8 protonProfileChar4UserDesp[17] = "Second Treshold";

//  Profile Characteristic 5 Properties
static uint8 protonProfileChar5Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 5 Value
static uint8 protonProfileChar5[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//  Profile Characteristic 5 User Description
static uint8 protonProfileChar5UserDesp[17] = "Third Treshold";

//  Profile Characteristic 6 Properties
static uint8 protonProfileChar6Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 6 Value
static uint8 protonProfileChar6[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

//  Profile Characteristic 6 User Description
static uint8 protonProfileChar6UserDesp[17] = "Device Managment";

//  Profile Characteristic 7 Properties
static uint8 protonProfileChar7Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 7 Value
static uint8 protonProfileChar7[18] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//  Profile Characteristic 7 User Description
static uint8 protonProfileChar7UserDesp[17] = "Basic CalData";

// Characteristic 8 Value
static uint8 protonProfileChar8[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//  Profile Characteristic 8 User Description
static uint8 protonProfileChar8UserDesp[17] = "Ancillary Data";

//  Profile Characteristic 8 Properties
static uint8 protonProfileChar8Props = GATT_PROP_READ | GATT_PROP_WRITE;

//  Profile Characteristic 9 Properties
static uint8 protonProfileChar9Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 9 Value
static uint8 protonProfileChar9[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//  Profile Characteristic 9 User Description
static uint8 protonProfileChar9UserDesp[17] = "Device Name";


/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t protonProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  //  Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&protonProfileService            /* pValue */
  },

  
  
      // Characteristic 1 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &protonProfileChar1Props 
    },

      // Characteristic Value 1
      { 
        { 16, protonProfilechar1UUID },
        0, 
        0, 
        protonProfileChar1 
      },

      // Characteristic 1 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar1UserDesp 
      },      
      
      // Characteristic 1 configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&protonProfileChar1Config 
      },
 
    // Characteristic 2 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &protonProfileChar2Props 
    },

      // Characteristic Value 2
      { 
        { 16, protonProfilechar2UUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar2 
      },

      // Characteristic 2 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar2UserDesp 
      },           
      
    // Characteristic 3 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &protonProfileChar3Props 
    },

      // Characteristic Value 3
      { 
        { 16, protonProfilechar3UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        protonProfileChar3 
      },

      // Characteristic 3 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar3UserDesp 
      },


    // Characteristic 4 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &protonProfileChar4Props 
    },

      // Characteristic Value 4
      { 
        { 16, protonProfilechar4UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        protonProfileChar4 
      },

      // Characteristic 4 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar4UserDesp 
      },      
     
      
      
    // Characteristic 5 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &protonProfileChar5Props 
    },

      // Characteristic Value 5
      { 
        { 16, protonProfilechar5UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        protonProfileChar5 
      },

      // Characteristic 5 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar5UserDesp 
      },   
      
     // Characteristic 6 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &protonProfileChar6Props 
    },

      // Characteristic Value 6
      { 
        { 16, protonProfilechar6UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        protonProfileChar6 
      },

      // Characteristic 6 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar6UserDesp 
      },        

    // Characteristic 7 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &protonProfileChar7Props 
    },

      // Characteristic Value 7
      { 
        { 16, protonProfilechar7UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        protonProfileChar7 
      },

      // Characteristic 7 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar7UserDesp 
      },         
      
    // Characteristic 8 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &protonProfileChar8Props 
    },

      // Characteristic Value 8
      { 
        { 16, protonProfilechar8UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        protonProfileChar8 
      },

      // Characteristic 8 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar8UserDesp 
      },         
      
    // Characteristic 9 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &protonProfileChar9Props 
    },

      // Characteristic Value 9
      { 
        { 16, protonProfilechar9UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        protonProfileChar9 
      },

      // Characteristic 9 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        protonProfileChar9UserDesp 
      },         
      
      
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t protonProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                                           uint8 *pValue, uint8 *pLen, uint16 offset,
                                           uint8 maxLen, uint8 method );
static bStatus_t protonProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                            uint8 *pValue, uint8 len, uint16 offset,
                                            uint8 method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// proton Profile Service Callbacks
CONST gattServiceCBs_t protonProfileCBs =
{
  protonProfile_ReadAttrCB,  // Read callback function pointer
  protonProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      protonProfile_AddService
 *
 * @brief   Initializes the proton Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t ProtonProfile_AddService( uint32 services )
{
  uint8 status;
  
  // Allocate Client Characteristic Configuration table
  protonProfileChar1Config = (gattCharCfg_t *)osal_mem_alloc( sizeof(gattCharCfg_t) *
                                                              linkDBNumConns );
  if ( protonProfileChar1Config == NULL )
  {     
    return ( bleMemAllocError );
  }
  
  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, protonProfileChar1Config );
  
  if ( services & PROTONPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( protonProfileAttrTbl, 
                                          GATT_NUM_ATTRS( protonProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &protonProfileCBs );
  }
  else
  {
    status = SUCCESS;
  }
  
  return ( status );
}

/*********************************************************************
 * @fn      protonProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t ProtonProfile_RegisterAppCBs( protonProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    protonProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      protonProfile_SetParameter
 *
 * @brief   Set a proton Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t ProtonProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    
      case PROTONPROFILE_CHAR1:
      if ( len == 13) //sizeof ( uint8 ) ) 
      {
      
        VOID osal_memcpy( protonProfileChar1, value, 13 );
         // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( protonProfileChar1Config, protonProfileChar1, FALSE,
                                    protonProfileAttrTbl, GATT_NUM_ATTRS( protonProfileAttrTbl ),
                                 INVALID_TASK_ID, protonProfile_ReadAttrCB );     
     
        
      }
      else
      {
        ret = bleInvalidRange;
      }
      break; 
    
 
    case PROTONPROFILE_CHAR2:
      if ( len == 20 ) 
      {
        VOID osal_memcpy( protonProfileChar2, value, 20 );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case PROTONPROFILE_CHAR3:
      if ( len == 10 ) 
      {
        VOID osal_memcpy( protonProfileChar3, value, 10 );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case PROTONPROFILE_CHAR4:
      if ( len == 10 ) 
      {
        VOID osal_memcpy( protonProfileChar4, value, 10 );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case PROTONPROFILE_CHAR5:
      if ( len == 10 ) 
      {
        VOID osal_memcpy( protonProfileChar5, value, 10 );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case PROTONPROFILE_CHAR6:
      if ( len == 8 ) 
      {
        VOID osal_memcpy( protonProfileChar6, value, 8 );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;      
      
     case PROTONPROFILE_CHAR7:
      if ( len == 18 ) 
      {
        VOID osal_memcpy( protonProfileChar7, value, 18 );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;   
      
    case PROTONPROFILE_CHAR8:
      if ( len == 17 ) 
      {
        VOID osal_memcpy( protonProfileChar8, value, 17 );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;           

    case PROTONPROFILE_CHAR9:
      if ( len == 20 ) 
      {
        VOID osal_memcpy( protonProfileChar9, value, 20 );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;           
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      protonProfile_GetParameter
 *
 * @brief   Get a proton Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t ProtonProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case PROTONPROFILE_CHAR1:
      VOID osal_memcpy( value, protonProfileChar1, 13 );
      break;    
    
    case PROTONPROFILE_CHAR2:
      VOID osal_memcpy( value, protonProfileChar2, 20 );
      break;      

    case PROTONPROFILE_CHAR3:
      VOID osal_memcpy( value, protonProfileChar3, 10 );
      break;  

    case PROTONPROFILE_CHAR4:
      VOID osal_memcpy( value, protonProfileChar4, 10 );
      break;

    case PROTONPROFILE_CHAR5:
      VOID osal_memcpy( value, protonProfileChar5, 10 );
      break;      
      
     case PROTONPROFILE_CHAR6:
      VOID osal_memcpy( value, protonProfileChar6, 8 );
      break; 

     case PROTONPROFILE_CHAR7:
      VOID osal_memcpy( value, protonProfileChar7, 18 );
      break;        
      
     case PROTONPROFILE_CHAR8:
      VOID osal_memcpy( value, protonProfileChar8, 17 );
      break;    
      
     case PROTONPROFILE_CHAR9:
      VOID osal_memcpy( value, protonProfileChar9, 20 );
      break;        
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          protonProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t protonProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                                           uint8 *pValue, uint8 *pLen, uint16 offset,
                                           uint8 maxLen, uint8 method )
{
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
 
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads

      // characteristics 1 and 2 have read permissions
      // characteritisc 3 does not have read permissions; therefore it is not
      //   included here
      // characteristic 4 does not have read permissions, but because it
      //   can be sent as a notification, it is included here

        
      default:
        // Should never get here! (characteristics 3 and 4 do not have read permissions)
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    
    uint8 uuid[16]; 
    osal_memcpy(uuid, pAttr->type.uuid, 16);
    
    
    if(osal_memcmp(uuid,protonProfilechar1UUID,16))
      {
        *pLen = 13;
        VOID osal_memcpy( pValue, pAttr->pValue, 13 );
      }
    
     if(osal_memcmp(uuid,protonProfilechar2UUID,16))
      {
        *pLen = 20;
        VOID osal_memcpy( pValue, pAttr->pValue, 20 );
      }   
 
      if(osal_memcmp(uuid,protonProfilechar3UUID,16))
      {
        *pLen = 10;
        VOID osal_memcpy( pValue, pAttr->pValue, 10 );
      }   
    
      if(osal_memcmp(uuid,protonProfilechar4UUID,16))
      {
        *pLen = 10;
        VOID osal_memcpy( pValue, pAttr->pValue, 10 );
      }     
    
      if(osal_memcmp(uuid,protonProfilechar5UUID,16))
      {
        *pLen = 10;
        VOID osal_memcpy( pValue, pAttr->pValue, 10 );
      }        
 
      if(osal_memcmp(uuid,protonProfilechar6UUID,16))
      {
        *pLen = 8;
        VOID osal_memcpy( pValue, pAttr->pValue, 8 );
      }      
    
       if(osal_memcmp(uuid,protonProfilechar7UUID,16))
      {
        *pLen = 18;
        VOID osal_memcpy( pValue, pAttr->pValue, 18 );
      }   
    
      if(osal_memcmp(uuid,protonProfilechar8UUID,16))
      {
        *pLen = 17;
        VOID osal_memcpy( pValue, pAttr->pValue, 17 );
      }     
    
      if(osal_memcmp(uuid,protonProfilechar9UUID,16))
      {
        *pLen = 20;
        VOID osal_memcpy( pValue, pAttr->pValue, 20 );
      }       
    
  }

  return ( status );
}

/*********************************************************************
 * @fn      protonProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t protonProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                            uint8 *pValue, uint8 len, uint16 offset,
                                            uint8 method )
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  
  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {

      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        break;
        
      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    uint8 uuid[16]; 
    osal_memcpy(uuid, pAttr->type.uuid, 16);
    
    
    if(osal_memcmp(uuid,protonProfilechar3UUID,16))
      {
        if ( offset == 0 )  
        {  
          if ( len != 10 )  
          {  
            status = ATT_ERR_INVALID_VALUE_SIZE;  
          }  
        }  
        else  
        {  
          status = ATT_ERR_ATTR_NOT_LONG;  
        }  
          
        if ( status == SUCCESS )  
        {  
          VOID osal_memcpy( pAttr->pValue, pValue, 10 );  
          notifyApp = PROTONPROFILE_CHAR3;  
        }        
      }

    if(osal_memcmp(uuid,protonProfilechar4UUID,16))
      {
        if ( offset == 0 )  
        {  
          if ( len != 10 )  
          {  
            status = ATT_ERR_INVALID_VALUE_SIZE;  
          }  
        }  
        else  
        {  
          status = ATT_ERR_ATTR_NOT_LONG;  
        }  
          
        if ( status == SUCCESS )  
        {  
          VOID osal_memcpy( pAttr->pValue, pValue, 10 );  
          notifyApp = PROTONPROFILE_CHAR4;  
        }        
      }    

    if(osal_memcmp(uuid,protonProfilechar5UUID,16))
      {
        if ( offset == 0 )  
        {  
          if ( len != 10 )  
          {  
            status = ATT_ERR_INVALID_VALUE_SIZE;  
          }  
        }  
        else  
        {  
          status = ATT_ERR_ATTR_NOT_LONG;  
        }  
          
        if ( status == SUCCESS )  
        {  
          VOID osal_memcpy( pAttr->pValue, pValue, 10 );  
          notifyApp = PROTONPROFILE_CHAR5;  
        }        
      }
 
    if(osal_memcmp(uuid,protonProfilechar6UUID,16))
      {
        if ( offset == 0 )  
        {  
          if ( len != 8 )  
          {  
            status = ATT_ERR_INVALID_VALUE_SIZE;  
          }  
        }  
        else  
        {  
          status = ATT_ERR_ATTR_NOT_LONG;  
        }  
          
        if ( status == SUCCESS )  
        {  
          VOID osal_memcpy( pAttr->pValue, pValue, 8 );  
          notifyApp = PROTONPROFILE_CHAR6;  
        }        
      }  
    
    if(osal_memcmp(uuid,protonProfilechar7UUID,16))
      {
        if ( offset == 0 )  
        {  
          if ( len != 18 )  
          {  
            status = ATT_ERR_INVALID_VALUE_SIZE;  
          }  
        }  
        else  
        {  
          status = ATT_ERR_ATTR_NOT_LONG;  
        }  
          
        if ( status == SUCCESS )  
        {  
          VOID osal_memcpy( pAttr->pValue, pValue, 18 );  
          notifyApp = PROTONPROFILE_CHAR7;  
        }        
      }     

    if(osal_memcmp(uuid,protonProfilechar8UUID,16))
      {
        if ( offset == 0 )  
        {  
          if ( len != 17 )  
          {  
            status = ATT_ERR_INVALID_VALUE_SIZE;  
          }  
        }  
        else  
        {  
          status = ATT_ERR_ATTR_NOT_LONG;  
        }  
          
        if ( status == SUCCESS )  
        {  
          VOID osal_memcpy( pAttr->pValue, pValue, 17 );  
          notifyApp = PROTONPROFILE_CHAR8;  
        }        
      }   
    
    if(osal_memcmp(uuid,protonProfilechar9UUID,16))
      {
        if ( offset == 0 )  
        {  
          if ( len != 20 )  
          {  
            status = ATT_ERR_INVALID_VALUE_SIZE;  
          }  
        }  
        else  
        {  
          status = ATT_ERR_ATTR_NOT_LONG;  
        }  
          
        if ( status == SUCCESS )  
        {  
          VOID osal_memcpy( pAttr->pValue, pValue, 20 );  
          notifyApp = PROTONPROFILE_CHAR9;  
        }        
      }     
    
    
    
    //status = ATT_ERR_INVALID_HANDLE;
  }

  // If a charactersitic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && protonProfile_AppCBs && protonProfile_AppCBs->pfnProtonProfileChange )
  {
    protonProfile_AppCBs->pfnProtonProfileChange( notifyApp );  
  }
  
  return ( status );
}

/*********************************************************************
*********************************************************************/

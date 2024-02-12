
#ifndef PROTONPROFILE_H
#define PROTONPROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Profile Parameters
#define PROTONPROFILE_CHAR1                   0  // RW uint8 - Profile Characteristic 1 value 
#define PROTONPROFILE_CHAR2                   1  // RW uint8 - Profile Characteristic 2 value
#define PROTONPROFILE_CHAR3                   2  // RW uint8 - Profile Characteristic 3 value
#define PROTONPROFILE_CHAR4                   3  // RW uint8 - Profile Characteristic 4 value
#define PROTONPROFILE_CHAR5                   4  // RW uint8 - Profile Characteristic 5 value
#define PROTONPROFILE_CHAR6                   5  // RW uint8 - Profile Characteristic 6 value   
#define PROTONPROFILE_CHAR7                   6  // RW uint8 - Profile Characteristic 7 value 
#define PROTONPROFILE_CHAR8                   7  // RW uint8 - Profile Characteristic 8 value 
#define PROTONPROFILE_CHAR9                   8  // RW uint8 - Profile Characteristic 9 value     
 
   
// Profile Service UUID
#define PROTONPROFILE_SERV_UUID             0x81, 0x25, 0xC7, 0xA1, 0x23, 0x2D, 0xA4, 0x87, 0xFD, 0x4F, 0x8C, 0xC2, 0x4A, 0x2A, 0x46, 0x63
    
// Key Pressed UUID
#define PROTONPROFILE_CHAR1_UUID            0xBD, 0xF7, 0x54, 0xAF, 0xB9, 0x14, 0xED, 0x81,0x04, 0x43, 0x1A, 0x7A, 0x7E, 0x76, 0xBC, 0x70
#define PROTONPROFILE_CHAR2_UUID            0xB9, 0x3F, 0x7B, 0x7E, 0xB9, 0x97, 0xD0, 0x9B,0x06, 0x4C, 0xE9, 0xA1, 0xC8, 0xED, 0x26, 0x8E
#define PROTONPROFILE_CHAR3_UUID            0x8C, 0x86, 0x28, 0x84, 0x4C, 0x32, 0xD6, 0x8E,0xD4, 0x46, 0x98, 0x1D, 0x20, 0xE8, 0x71, 0x3F
#define PROTONPROFILE_CHAR4_UUID            0x05, 0xFA, 0x02, 0xC1, 0xD5, 0x4C, 0x82, 0x9D,0x7F, 0x4D, 0xB7, 0x4D, 0x67, 0xD4, 0x95, 0x2E
#define PROTONPROFILE_CHAR5_UUID            0xCA, 0xA7, 0x1C, 0xA3, 0x64, 0x9C, 0x2F, 0x9A,0x12, 0x4C, 0x84, 0x8D, 0x2F, 0x24, 0xDE, 0xF8
#define PROTONPROFILE_CHAR6_UUID            0x57, 0xE1, 0x8A, 0x54, 0x9E, 0x87, 0x0E, 0xBF,0x48, 0x4A, 0x4A, 0xAC, 0xCD, 0xCF, 0x50, 0xEA
#define PROTONPROFILE_CHAR7_UUID            0x2D, 0x61, 0x58, 0xAA, 0xAB, 0xBA, 0x49, 0x87,0x16, 0x40, 0xC1, 0x03, 0x1F, 0x03, 0xF7, 0x57
#define PROTONPROFILE_CHAR8_UUID            0x73, 0xA8, 0x54, 0x94, 0x44, 0x59, 0x5A, 0x8B,0x80, 0x40, 0x41, 0x75, 0x67, 0x3A, 0x42, 0xE2
#define PROTONPROFILE_CHAR9_UUID            0x3B, 0x57, 0x22, 0x36, 0x62, 0x83, 0xCA, 0x94,0xB0, 0x49, 0x7D, 0xC3, 0x06, 0x9C, 0x6C, 0xBB   
   
// Profile Services bit fields
#define PROTONPROFILE_SERVICE               0x00000001

// Length of Characteristic 5 in bytes
#define PROTONPROFILE_CHAR5_LEN           5  

/*********************************************************************
 * TYPEDEFS
 */

  
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*protonProfileChange_t)( uint8 paramID );

typedef struct
{
  protonProfileChange_t        pfnProtonProfileChange;  // Called when characteristic value changes
} protonProfileCBs_t;

    

/*********************************************************************
 * API FUNCTIONS 
 */


/*
 * ProtonProfile_AddService- Initializes the Proton GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t ProtonProfile_AddService( uint32 services );

/*
 * ProtonProfile_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t ProtonProfile_RegisterAppCBs( protonProfileCBs_t *appCallbacks );

/*
 * ProtonProfile_SetParameter - Set a Proton GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t ProtonProfile_SetParameter( uint8 param, uint8 len, void *value );
  
/*
 * ProtonProfile_GetParameter - Get a Proton GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t ProtonProfile_GetParameter( uint8 param, void *value );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PROTONPROFILE_H */

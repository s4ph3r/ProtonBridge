
#ifndef PROTONBRIDGE_H
#define PROTONBRIDGE_H

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


// ProtonBridge Task Events
#define SBP_START_DEVICE_EVT                              0x0001
#define SBP_PERIODIC_EVT                                  0x0002

  
uint8 Application_StartAdvertise(uint16 duration, uint16 interval);  
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the BLE Application
 */
extern void ProtonBridge_Init( uint8 task_id );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 ProtonBridge_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PROTONBRIDGE_H */


//  * @file           : usbd_cdc_if.h


#ifndef __USBD_CDC_IF_1H__
#define __USBD_CDC_IF_1H__

#ifdef __cplusplus
 extern "C" {
#endif


#define USBMIDIBUFSIZE 512 										//must be power of 2
#define USBMIDIMASK (USBMIDIBUFSIZE-1)
   
#include "usbd_midi.h"
#include "usbd_desc.h"

//circuit buffer for midi rx data
typedef struct _tUsbMidiCable
{ 
  uint16_t curidx; 												//current pointer position
  uint16_t rdidx;  												//reading pointer position
  uint8_t buf[USBMIDIBUFSIZE];
} tUsbMidiCable; 

extern tUsbMidiCable usbmidicable1; //rx buf for virtual midi cable 1
//extern tUsbMidiCable usbmidicable2; //rx buf for vortual midi cable 2

extern USBD_MIDI_ItfTypeDef  USBD_Interface_fops_FS;

//void USB_LP_CAN1_RX0_IRQHandler(void);

void USBD_MIDI_Message(uint8_t byte1, uint8_t byte2, uint8_t byte3);

void USBD_AddEvent( uint8_t cable );

#endif /* __USBD_CDC_IF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

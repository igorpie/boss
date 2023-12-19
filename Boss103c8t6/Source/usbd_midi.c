
//  * @file    usbd_cdc.c
// edited

//#include "usbd_cdc.h"
#include "usbd_ctlreq.h"

uint8_t flag_USB_DataIn = 0;

uint8_t get_flag_DataIn(){
	uint8_t res;
	__disable_irq();
	res = flag_USB_DataIn;
	__enable_irq();
	return res;
}

void set_flag_DataIn(){
	__disable_irq();
	flag_USB_DataIn = 1;
	__enable_irq();
}

void reset_flag_DataIn(){
	__disable_irq();
	flag_USB_DataIn = 0;
	__enable_irq();
}


#include "usbd_midi.h"
#include "usbd_desc.h"
//#include "stm32h7xx_hal_conf.h"
#include "usbd_ctlreq.h"
//#include "stm32h7xx_hal.h"

static uint8_t  USBD_MIDI_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);	// прерывание что пришли данные от ПК

static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length);
USBD_HandleTypeDef *pInstance = NULL; 

uint32_t APP_Rx_ptr_in  = 0;
uint32_t APP_Rx_ptr_out = 0;
uint32_t APP_Rx_length  = 0;
uint8_t  USB_Tx_State = 0;

__ALIGN_BEGIN uint8_t USB_Rx_Buffer[MIDI_DATA_OUT_PACKET_SIZE] __ALIGN_END ;	// from PC
__ALIGN_BEGIN uint8_t APP_Rx_Buffer[APP_RX_DATA_SIZE] __ALIGN_END ;// send to PC


/* USB MIDI interface class callbacks structure */
USBD_ClassTypeDef  USBD_MIDI = 
{
  USBD_MIDI_Init,
  USBD_MIDI_DeInit,
  NULL,
  NULL,
  NULL,
  USBD_MIDI_DataIn,
  USBD_MIDI_DataOut,
  NULL,
  NULL,
  NULL,
  NULL,// HS
  USBD_MIDI_GetCfgDesc,// FS
  NULL,// OTHER SPEED
  NULL,// DEVICE_QUALIFIER
};

/* USB MIDI device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_MIDI_CfgDesc[USB_MIDI_CONFIG_DESC_SIZ] __ALIGN_END = {
  
  
  // configuration descriptor for single midi cable
  0x09, 0x02, LOBYTE(USB_MIDI_CONFIG_DESC_SIZ), HIBYTE(USB_MIDI_CONFIG_DESC_SIZ), 0x02, 0x01, 0x00, 0x80, 0x31,

  // The Audio Interface Collection
  0x09, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, // Standard AC Interface Descriptor
  0x09, 0x24, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x01, // Class-specific AC Interface Descriptor
  0x09, 0x04, 0x01, 0x00, 0x02, 0x01, 0x03, 0x00, 0x00, // MIDIStreaming Interface Descriptors
  0x07, 0x24, 0x01, 0x00, 0x01, 0x41, 0x00,             // Class-Specific MS Interface Header Descriptor

  // MIDI IN JACKS
  0x06, 0x24, 0x02, 0x01, 0x01, 0x00,
  0x06, 0x24, 0x02, 0x02, 0x02, 0x00,

  // MIDI OUT JACKS
  0x09, 0x24, 0x03, 0x01, 0x03, 0x01, 0x02, 0x01, 0x00,
  0x09, 0x24, 0x03, 0x02, 0x06, 0x01, 0x01, 0x01, 0x00,

  // OUT endpoint descriptor
  0x09, 0x05, MIDI_OUT_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x25, 0x01, 0x01, 0x01,

  // IN endpoint descriptor
  0x09, 0x05, MIDI_IN_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x25, 0x01, 0x01, 0x03,
};


static uint8_t USBD_MIDI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx){
  pInstance = pdev;
  USBD_LL_OpenEP(pdev,MIDI_IN_EP,USBD_EP_TYPE_BULK,MIDI_DATA_IN_PACKET_SIZE);
  USBD_LL_OpenEP(pdev,MIDI_OUT_EP,USBD_EP_TYPE_BULK,MIDI_DATA_OUT_PACKET_SIZE);
  USBD_LL_PrepareReceive(pdev,MIDI_OUT_EP,(uint8_t*)(USB_Rx_Buffer),MIDI_DATA_OUT_PACKET_SIZE);
  return 0;
}

static uint8_t USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx){
  pInstance = NULL;
  USBD_LL_CloseEP(pdev,MIDI_IN_EP);
  USBD_LL_CloseEP(pdev,MIDI_OUT_EP);
  return 0;
}

static uint8_t USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum){		// to PC
#if 0
  if (APP_Rx_length != 0) USB_Tx_State = USB_TX_CONTINUE;
  else  {
    APP_Rx_ptr_out = 0;
    APP_Rx_ptr_in = 0;
    USB_Tx_State = USB_TX_READY;
  }
  USBD_MIDI_DataOut (pInstance, 1);							
  return USBD_OK;
#endif
  set_flag_DataIn();
  return USBD_OK;

}



static uint8_t USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum) {	// from PC
/*
  uint16_t USB_Rx_Cnt;
  USBD_MIDI_ItfTypeDef *pmidi;
  
  USB_Rx_Cnt = ((PCD_HandleTypeDef*)pdev->pData)->OUT_ep[epnum].xfer_count;
  if (USB_Rx_Cnt)  {
    pmidi = (USBD_MIDI_ItfTypeDef *)(pdev->pUserData);    
    pmidi->pIf_MidiRx((uint8_t *)&USB_Rx_Buffer, USB_Rx_Cnt);
  }
  USBD_LL_PrepareReceive(pdev , MIDI_OUT_EP , (uint8_t*)(USB_Rx_Buffer) , MIDI_DATA_OUT_PACKET_SIZE);
*/
  uint16_t USB_Rx_Cnt;
  USBD_MIDI_ItfTypeDef *pmidi;

  //USB_Rx_Cnt = ((PCD_HandleTypeDef*)pdev->pData)->OUT_ep[epnum].xfer_count;
  USB_Rx_Cnt = USBD_LL_GetRxDataSize(pdev, epnum);
  if (USB_Rx_Cnt) //ускорение костыля - если функция вызвана, а данных нет, не тратим время впустую
  {
    pmidi = (USBD_MIDI_ItfTypeDef *)(pdev->pUserData);
    pmidi->pIf_MidiRx((uint8_t *)&USB_Rx_Buffer, USB_Rx_Cnt);
  }
  USBD_LL_PrepareReceive(pdev,MIDI_OUT_EP,(uint8_t*)(USB_Rx_Buffer),MIDI_DATA_OUT_PACKET_SIZE);

  return USBD_OK;
}



static uint8_t *USBD_MIDI_GetCfgDesc (uint16_t *length){
  *length = sizeof (USBD_MIDI_CfgDesc);
  return USBD_MIDI_CfgDesc;
}



uint8_t USBD_MIDI_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_MIDI_ItfTypeDef *fops) {
  uint8_t ret = USBD_FAIL;
  if(fops != NULL){
    pdev->pUserData= fops;
    ret = USBD_OK;    
  }
  return ret;
}


//MIDI TX START FUNCTION. Send to PC
void USBD_MIDI_SendPacket(void) {
	uint16_t USB_Tx_ptr;
	uint16_t USB_Tx_length;
/*  
	if (USB_Tx_State == 1) return;

	if (APP_Rx_ptr_out == APP_RX_DATA_SIZE) APP_Rx_ptr_out = 0;

	if(APP_Rx_ptr_out == APP_Rx_ptr_in) {
	  USB_Tx_State = USB_TX_READY;
	  return;
	}

	if(APP_Rx_ptr_out > APP_Rx_ptr_in) APP_Rx_length = APP_RX_DATA_SIZE - APP_Rx_ptr_out;
	else APP_Rx_length = APP_Rx_ptr_in - APP_Rx_ptr_out;

	if (APP_Rx_length > MIDI_DATA_IN_PACKET_SIZE) {
	  USB_Tx_ptr = APP_Rx_ptr_out;
	  USB_Tx_length = MIDI_DATA_IN_PACKET_SIZE;
	  APP_Rx_ptr_out += MIDI_DATA_IN_PACKET_SIZE;
	  APP_Rx_length -= MIDI_DATA_IN_PACKET_SIZE;
	}
	else {
	  USB_Tx_ptr = APP_Rx_ptr_out;
	  USB_Tx_length = APP_Rx_length;
	  APP_Rx_ptr_out += APP_Rx_length;
	  APP_Rx_length = 0;
	}
	USB_Tx_State = USB_TX_BUSY;

	USBD_LL_Transmit(pInstance, MIDI_IN_EP, &APP_Rx_Buffer[USB_Tx_ptr], USB_Tx_length);
*/
    if (APP_Rx_ptr_out == APP_RX_DATA_SIZE)  APP_Rx_ptr_out = 0;

    if(APP_Rx_ptr_out == APP_Rx_ptr_in) {
      USB_Tx_State = USB_TX_READY;
      return;
    }

    if(APP_Rx_ptr_out > APP_Rx_ptr_in) {
      APP_Rx_length = APP_RX_DATA_SIZE - APP_Rx_ptr_out;
    }
    else  APP_Rx_length = APP_Rx_ptr_in - APP_Rx_ptr_out;

    if (APP_Rx_length > MIDI_DATA_IN_PACKET_SIZE)    {
      USB_Tx_ptr = APP_Rx_ptr_out;
      USB_Tx_length = MIDI_DATA_IN_PACKET_SIZE;
      APP_Rx_ptr_out += MIDI_DATA_IN_PACKET_SIZE;
      APP_Rx_length -= MIDI_DATA_IN_PACKET_SIZE;
    }
    else  {
      USB_Tx_ptr = APP_Rx_ptr_out;
      USB_Tx_length = APP_Rx_length;
      APP_Rx_ptr_out += APP_Rx_length;
      APP_Rx_length = 0;
    }
    USB_Tx_State = USB_TX_BUSY;
    //USBD_LL_FlushEP(pInstance, MIDI_IN_EP);
    USBD_LL_Transmit(pInstance, MIDI_IN_EP,&APP_Rx_Buffer[USB_Tx_ptr],USB_Tx_length);

}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

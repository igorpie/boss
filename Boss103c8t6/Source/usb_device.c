

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_midi.h"
#include "usbd_midi_if.h"

/* USB Device Core handle declaration */
USBD_HandleTypeDef hUsbDeviceFS;

/* init function */
void MX_USB_DEVICE_Init(void)
{
  /* Init Device Library,Add Supported Class and Start the library*/
  USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
  USBD_RegisterClass(&hUsbDeviceFS, &USBD_MIDI);
  USBD_MIDI_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);
  USBD_Start(&hUsbDeviceFS);
}

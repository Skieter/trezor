#ifndef __MESSENGER_H__
#define __MESSENGER_H__

#include <hidapi/hidapi.h>
void messenger_tx_rx(hid_device *handle, unsigned char *in, unsigned char *out, int len);
void messenger_tx_rx_sp(hid_device *handle, unsigned char *in, unsigned char *out, int len);

#endif

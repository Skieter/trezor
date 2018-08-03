#include "messenger.h"
#include <hidapi/hidapi.h>

void messenger_tx_rx(hid_device *handle, unsigned char *in, unsigned char *out, int len)
{
    hid_write(handle, out, len);
    hid_read_timeout(handle, in, 64, 60000);
}

void messenger_tx_rx_sp(hid_device *handle, unsigned char *in, unsigned char *out, int len)
{
    hid_write(handle, out, len);
    hid_read_timeout(handle, in, 64, 1000);
}

//============================================================================
// Name        : trezor.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "messages.pb.h"
#include "messages-bitcoin.pb.h"
#include "messages-common.pb.h"
#include <string.h>
#include <iostream>

extern "C"
{
#include <stdio.h>
#include <hidapi/hidapi.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "messenger.h"
}

void getButtonAck(void);
void getAddress(void);
void setAddress(Address *msg);
void getPublicKey(void);
void setPublicKey(PublicKey *msg);
void getSignMessage(void);
void setSignMessage(MessageSignature *msg);
void getSignTx(void);
void setTxRequest(TxRequest *msg);
void setFailure(Failure *msg);


void processMsg(unsigned char *buf);
void sendTxInput(void);
void sendTxMeta(void);
void sendTxOutput(void);

struct hid_device_info *devs, *cur_dev;
hid_device *handle;
uint8_t buffer[128];
unsigned char out[128], in[512], in_formatted[512];
unsigned char *out_ptr;

/*
deviceinitialize
devicegetAddress
sendtoaddress
getnewaddress
createrawtransaction
devicesigntx
*/

//build firmware with amended protobuf array size exceeds with original fw ~/dev/particl/trezor/trezor-mcu/build/firmware/firmware
//sign firmware - /trezor-mcu/bootloader$ python3 firmware_sign.py -f ~/dev/particl/trezor/trezor.bin
//flash fw - /trezor/python-trezor$ ./trezorctl firmware_update -f ~/dev/particl/trezor/trezor.bin
int main()
{
	if(hid_init())
		return 1;

	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	handle = hid_open(0x534c, 0x0001, NULL);
	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}

	//getAddress();
	//initialize();
	//getPublicKey();
	//getSignMessage();
	getSignTx();

	if(handle)
	{
		hid_close(handle);
		hid_exit();
	}

    return 0;
}

void getAddress(void)
{
	GetAddress msg;
	msg.Clear();
	msg.set_coin_name("Bitcoin");


	msg.SerializeToArray(buffer, sizeof(buffer));

	uint32_t len = msg.ByteSize();

    out_ptr = &out[0];

    uint16_t msg_id = MessageType_GetAddress;
    *out_ptr++ = '?';
    *out_ptr++ = '#';
    *out_ptr++ = '#';
    *out_ptr++ = (msg_id >> 8) & 0xFF;
    *out_ptr++ = (msg_id & 0xFF);
    *out_ptr++ = (len >> 24) & 0xFF;
    *out_ptr++ = (len >> 16) & 0xFF;
    *out_ptr++ = (len >> 8) & 0xFF;
    *out_ptr++ = len & 0xFF;
    for(uint16_t i = 0; i < len; i++)
    	*out_ptr++ = buffer[i];

    messenger_tx_rx(handle, in, out, 64);
    processMsg(in);
}

void setAddress(Address *msg)
{
	std::cout << msg->address().c_str();
}

/*void initialize(void)
{
	Initialize msg;
    msg.has_skip_passphrase = true;
    msg.has_state = false;
    msg.skip_passphrase = true;

    char in[64];

    msg_write(MessageType_MessageType_Initialize, &msg);
    msg_send_receive(handle, in, (char *)msg_out_data(), 64);
    msg_read((const uint8_t *)in, 64);
    for(int i = 0; i < 2; i++)
    {
		msg_send_receive(handle, in, (char *)"?##", 3);
		msg_read((const uint8_t *)in, 64);
    }
}

void getFeatures(void)
{
	GetFeatures msg;
    msg.dummy_field = 0;

    char in[64];

    msg_write(MessageType_MessageType_GetFeatures, &msg);
    msg_send_receive(handle, in, (char *)msg_out_data(), 64);
    msg_read((const uint8_t *)in, sizeof in);
}

void setFeatures(Features *msg)
{
	printf(msg->device_id);
}*/

void getPublicKey(void)
{
	GetPublicKey msg;
	msg.Clear();
	msg.set_show_display(false);

	msg.SerializeToArray(buffer, sizeof(buffer));

	uint32_t len = msg.ByteSize();

    out_ptr = &out[0];

    uint16_t msg_id = MessageType_GetPublicKey;
    *out_ptr++ = '?';
    *out_ptr++ = '#';
    *out_ptr++ = '#';
    *out_ptr++ = (msg_id >> 8) & 0xFF;
    *out_ptr++ = (msg_id & 0xFF);
    *out_ptr++ = (len >> 24) & 0xFF;
    *out_ptr++ = (len >> 16) & 0xFF;
    *out_ptr++ = (len >> 8) & 0xFF;
    *out_ptr++ = len & 0xFF;
    for(uint16_t i = 0; i < len; i++)
    	*out_ptr++ = buffer[i];

    messenger_tx_rx(handle, in, out, 64);
    for(int i = 0; i < 3; i++)
    {
        out_ptr = &out[0];

        *out_ptr++ = '?';
        *out_ptr++ = '#';
        *out_ptr++ = '#';
        messenger_tx_rx(handle, &in[64*(i+1)], out, 64);
    }
    memcpy(in_formatted, in, 64);
    memcpy(&in_formatted[64], &in[65], 63);
    memcpy(&in_formatted[127], &in[129], 63);
    memcpy(&in_formatted[190], &in[193], 63);
    processMsg(in_formatted);
}

void setPublicKey(PublicKey *msg)
{
	std::cout << msg->node().public_key().c_str();

	printf("\n");

	std::cout << msg->xpub().c_str();
}

void getSignMessage(void)
{
	SignMessage msg;
	msg.set_coin_name("Bitcoin");
	msg.set_message("hello");

	msg.SerializeToArray(buffer, sizeof(buffer));

	uint32_t len = msg.ByteSize();

    out_ptr = &out[0];

    uint16_t msg_id = MessageType_SignMessage;
    *out_ptr++ = '?';
    *out_ptr++ = '#';
    *out_ptr++ = '#';
    *out_ptr++ = (msg_id >> 8) & 0xFF;
    *out_ptr++ = (msg_id & 0xFF);
    *out_ptr++ = (len >> 24) & 0xFF;
    *out_ptr++ = (len >> 16) & 0xFF;
    *out_ptr++ = (len >> 8) & 0xFF;
    *out_ptr++ = len & 0xFF;
    for(uint16_t i = 0; i < len; i++)
    	*out_ptr++ = buffer[i];

    messenger_tx_rx(handle, in, out, 64);
    processMsg(in);
}

void setSignMessage(MessageSignature *msg)
{
	std::cout << msg->signature().c_str();
}

void getSignTx(void)
{
	SignTx msg;
	msg.set_coin_name("Bitcoin");
	msg.set_inputs_count(1);
	msg.set_outputs_count(1);

	msg.SerializeToArray(buffer, sizeof(buffer));

	uint32_t len = msg.ByteSize();

    out_ptr = &out[0];

    uint16_t msg_id = MessageType_SignTx;
    *out_ptr++ = '?';
    *out_ptr++ = '#';
    *out_ptr++ = '#';
    *out_ptr++ = (msg_id >> 8) & 0xFF;
    *out_ptr++ = (msg_id & 0xFF);
    *out_ptr++ = (len >> 24) & 0xFF;
    *out_ptr++ = (len >> 16) & 0xFF;
    *out_ptr++ = (len >> 8) & 0xFF;
    *out_ptr++ = len & 0xFF;
    for(uint16_t i = 0; i < len; i++)
    	*out_ptr++ = buffer[i];

    messenger_tx_rx(handle, in, out, 64);
    processMsg(in);
}

/*
 * Each input requires a signature.
 * Signatures are signed on the SignatureHash according to the script on the output the input is spending (prevout)
 */
//153f1b9c28397fff9dd0839514cace649f90715b41c4d09a81e790f6f77e83f4:txid
//2NCaG7HUW58BYb8VQuKzNhHpPWUEfK8abQk:testnet address /0
//0.04:amt
//2MzMN8PUj9bV8pfoG8fevVaGTj2kqNPvBG8: address /1
//0100000001f4837ef7f690e7819ad0c4415b71909f64ceca149583d09dff7f39289c1b3f150100000000ffffffff0140420f000000000017a9144df22bc43652afd7c4774b6681e13f39d33497948700000000
void setTxRequest(TxRequest *msg)
{
	if(msg->request_type() == TxRequest_RequestType_TXINPUT)
	{
		sendTxInput();
	}

	if(msg->request_type() == TxRequest_RequestType_TXMETA)
	{
		sendTxMeta();
	}

	if(msg->request_type() == TxRequest_RequestType_TXOUTPUT)
	{
		sendTxOutput();
	}
}

void sendTxInput(void)
{
	char hex[] = "e2656ee5aa59f2f71dc1f7cb7504a7dc37ae841690b9d9e740ed1df7000b9fd4";
	char *p;
	unsigned char result[32], *r;
	unsigned char c;

	for (p = hex, r = result; *p; p += 2) {
	    if (sscanf(p, "%02X", (unsigned int *)&c) != 1) {
	        break;
	    }
	    *r++ = c;
	}

	TxAck_TransactionType tx;
	TxAck_TransactionType_TxInputType *input = tx.add_inputs();
	input->set_prev_hash(result, 32);
	input->set_prev_index(0);

	TxAck resp;

	resp.set_allocated_tx(&tx);

	resp.SerializeToArray(buffer, sizeof(buffer));

	uint32_t len = resp.ByteSize();

    out_ptr = &out[0];

    uint16_t msg_id = MessageType_TxAck;
    *out_ptr++ = '?';
    *out_ptr++ = '#';
    *out_ptr++ = '#';
    *out_ptr++ = (msg_id >> 8) & 0xFF;
    *out_ptr++ = (msg_id & 0xFF);
    *out_ptr++ = (len >> 24) & 0xFF;
    *out_ptr++ = (len >> 16) & 0xFF;
    *out_ptr++ = (len >> 8) & 0xFF;
    *out_ptr++ = len & 0xFF;
    for(uint16_t i = 0; i < len; i++)
		*out_ptr++ = buffer[i];

    messenger_tx_rx(handle, in, out, 64);
    processMsg(in);
}

void sendTxMeta(void)
{
	TxAck_TransactionType tx;
	tx.set_outputs_cnt(1);

	TxAck resp;
	resp.set_allocated_tx(&tx);

	resp.SerializeToArray(buffer, sizeof(buffer));

	uint32_t len = resp.ByteSize();

	out_ptr = &out[0];

	uint16_t msg_id = MessageType_TxAck;
	*out_ptr++ = '?';
	*out_ptr++ = '#';
	*out_ptr++ = '#';
	*out_ptr++ = (msg_id >> 8) & 0xFF;
	*out_ptr++ = (msg_id & 0xFF);
	*out_ptr++ = (len >> 24) & 0xFF;
	*out_ptr++ = (len >> 16) & 0xFF;
	*out_ptr++ = (len >> 8) & 0xFF;
	*out_ptr++ = len & 0xFF;
	for(uint16_t i = 0; i < len; i++)
		*out_ptr++ = buffer[i];

	messenger_tx_rx(handle, in, out, 64);
	processMsg(in);

}

void sendTxOutput(void)
{
	TxAck_TransactionType tx;
	TxAck_TransactionType_TxOutputType *output = tx.add_outputs();
	output->set_amount(3);
	output->set_script_type(TxAck_TransactionType_TxOutputType_OutputScriptType_PAYTOADDRESS);

	TxAck resp;

	resp.set_allocated_tx(&tx);

	resp.SerializeToArray(buffer, sizeof(buffer));

	uint32_t len = resp.ByteSize();

    out_ptr = &out[0];

    uint16_t msg_id = MessageType_TxAck;
    *out_ptr++ = '?';
    *out_ptr++ = '#';
    *out_ptr++ = '#';
    *out_ptr++ = (msg_id >> 8) & 0xFF;
    *out_ptr++ = (msg_id & 0xFF);
    *out_ptr++ = (len >> 24) & 0xFF;
    *out_ptr++ = (len >> 16) & 0xFF;
    *out_ptr++ = (len >> 8) & 0xFF;
    *out_ptr++ = len & 0xFF;
    for(uint16_t i = 0; i < len; i++)
		*out_ptr++ = buffer[i];

    messenger_tx_rx(handle, in, out, 64);
    processMsg(in);
    //TODO
}

void setButtonRequest(ButtonAck *msg)
{
	getButtonAck();
}

void getButtonAck(void)
{
	ButtonAck msg;


	msg.SerializeToArray(buffer, sizeof(buffer));

	uint32_t len = msg.ByteSize();

    out_ptr = &out[0];

    uint16_t msg_id = MessageType_ButtonAck;
    *out_ptr++ = '?';
    *out_ptr++ = '#';
    *out_ptr++ = '#';
    *out_ptr++ = (msg_id >> 8) & 0xFF;
    *out_ptr++ = (msg_id & 0xFF);
    *out_ptr++ = (len >> 24) & 0xFF;
    *out_ptr++ = (len >> 16) & 0xFF;
    *out_ptr++ = (len >> 8) & 0xFF;
    *out_ptr++ = len & 0xFF;
    for(uint16_t i = 0; i < len; i++)
    	*out_ptr++ = buffer[i];

    messenger_tx_rx(handle, in, out, 64);

    out_ptr = &out[0];

    *out_ptr++ = '?';
    *out_ptr++ = '#';
    *out_ptr++ = '#';
    messenger_tx_rx(handle, &in[64], out, 64);

    memcpy(in_formatted, in, 64);
    memcpy(&in_formatted[64], &in[65], 63);
    processMsg(in_formatted);
}

void setFailure(Failure *msg)
{
	//std::cout << msg->message().c_str();
}

void processMsg(unsigned char *buf)
{
	switch(buf[4])
	{
		case MessageType_Address:
		{
			Address addr;
			addr.ParseFromArray(&buf[9], buf[8]);
			setAddress(&addr);
		}
		break;

		case MessageType_ButtonRequest:
		{
			getButtonAck();
		}
		break;

		case MessageType_PublicKey:
		{
			PublicKey pk;
			pk.ParseFromArray(&buf[9], buf[8]);
			setPublicKey(&pk);
		}
		break;

		case MessageType_MessageSignature:
		{
			MessageSignature sig;
			sig.ParseFromArray(&buf[9], buf[8]);
			setSignMessage(&sig);
		}
		break;

		case MessageType_TxRequest:
		{
			TxRequest req;
			req.ParseFromArray(&buf[9], buf[8]);
			setTxRequest(&req);
		}
		break;

		case MessageType_Failure:
		{
			Failure f;
			f.ParseFromArray(&buf[9], buf[8]);
			setFailure(&f);
		}
	}
}



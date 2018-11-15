//////////////////////////////////////////////
// Cybiko USB access library
// Author: Michael Tulupov (vaxxabait@gmail.com)
// License: public domain

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cyusb.h"

// Cybiko Xtreme Vendor and Product ID
#define CYUSB_VID 0x0b66
#define CYUSB_PID 0x0041

// Debug
#define ERR printf
#define DBG printf


//////////////////////////////////////////////
// Default packet

const cyusb_pkt_t CYUSB_PKT_DEFAULT = {
	.header = 0x4d4d,		// Header - always "MM"
	.type = CMD_STAT,
	.func = PING,
	.cnt = 0,
	.len = 4,			// Command/status packet always have 4 byte of payload
	.data = "ABCD",
	.cmdstat = {
		._A = 0,
		._B = 0,
		._C = 0,
		._D = 0
	},
	.crc = 0			// Wrong CRC for this packet - prevents default packet to be processed
};



//////////////////////////////////////////////
// Initialize the library

int cyusb_init(cyusb_ctx_t *ctx){
	int err;

	ctx->usb = NULL;
	err = libusb_init(&(ctx->usb));

	if(err < 0) {
		ERR("CYUSB: Init Error %d\n",err);
		return CYUSB_ERR;
	}

	libusb_set_debug(ctx->usb, 3); //set verbosity level to 3, as suggested in the documentation
	return CYUSB_OK;
}



//////////////////////////////////////////////
// Find all Cybikos

int cyusb_find(cyusb_ctx_t *ctx){
	libusb_device **devs;
	struct libusb_device_descriptor desc;
	ssize_t cnt;
	int err;

	cnt = libusb_get_device_list(ctx->usb, &devs); //get the list of devices

	if(cnt < 0) {
		ERR("CYUSB: List USB device error\n");
		return CYUSB_ERR;
	}

	ctx->num = 0;
	for(int i = 0; i < cnt; i++) {
		err = libusb_get_device_descriptor(devs[i], &desc);
		if (err < 0) {
			ERR("CYUSB: Get USB device descriptor error\n");
			return CYUSB_ERR;
		}
		if (desc.idVendor == CYUSB_VID && desc.idProduct == CYUSB_PID) {
			DBG("CYUSB: Found Cybiko Xtreme: ID=%ld\n",ctx->num);
			ctx->num++;
		}
	}

	ctx->dev = malloc(ctx->num * sizeof(cyusb_dev_t));

	cnt = 0;
	for(int i = 0; i < cnt; i++) {
		err = libusb_get_device_descriptor(devs[i], &desc);
		if (err < 0) {
			ERR("CYUSB: Get USB device descriptor error\n");
			return CYUSB_ERR;
		}
		if (desc.idVendor == CYUSB_VID && desc.idProduct == CYUSB_PID) {
			ctx->dev[cnt].usb = devs[i];
			cnt++;
		} else {
			libusb_unref_device(devs[i]);
		}
	}

	libusb_free_device_list(devs,0);

	return CYUSB_OK;
}



//////////////////////////////////////////////
// Open one Cybiko

int cyusb_open(cyusb_dev_t *dev){
/*	dev_handle = libusb_open_device_with_vid_pid(ctx, 5118, 7424); //these are vendorID and productID I found for my usb device
	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
38
	        cout<<"Kernel Driver Active"<<endl;
39
	        if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
40
	            cout<<"Kernel Driver Detached!"<<endl;
41
	    }
42
	    r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
43
	    if(r < 0) {
44
	        cout<<"Cannot Claim Interface"<<endl;
45
	        return 1;
46
	    }*/
}



//////////////////////////////////////////////
// Write to Cybiko

int cyusb_write(const cyusb_dev_t *dev, const cyusb_pkt_t *pkt){
}



//////////////////////////////////////////////
// Read from Cybiko

int cyusb_read(const cyusb_dev_t *dev, cyusb_pkt_t *pkt){
}



//////////////////////////////////////////////
// Close Cybiko

void cyusb_close(cyusb_dev_t *dev){
/*	  r = libusb_release_interface(dev_handle, 0); //release the claimed interface
58
	    if(r!=0) {
59
	        cout<<"Cannot Release Interface"<<endl;
60
	        return 1;
61
	    }
62
	    cout<<"Released Interface"<<endl;
63
	 
64
	    libusb_close(dev_handle); //close the device we opened
	*/
}



//////////////////////////////////////////////
// De-initialize the library

int cyusb_exit(cyusb_ctx_t *ctx){
	for(int i = 0; i < ctx->num; i++) {
		libusb_unref_device(ctx->dev[i].usb);
	}
	free(ctx->dev);
	libusb_exit(ctx->usb);
}
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

// String descriptor to get CyID
#define CYUSB_CYID_IDX 0x38


// Debug
#define ERR printf
#define DBG printf


#define CHECK_SUCCESS  {if(status != LIBUSB_SUCCESS) { ERR("CYUSB: libusb call failed at line %0d, status %d\n",__LINE__,status); return CYUSB_ERR; }}



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
	int status;

	ctx->usb = NULL;
	
	status = libusb_init(&(ctx->usb));
	CHECK_SUCCESS;

status = libusb_set_option (ctx->usb, LIBUSB_OPTION_LOG_LEVEL,LIBUSB_LOG_LEVEL_WARNING);
//status = libusb_set_option (ctx->usb, LIBUSB_OPTION_LOG_LEVEL,LIBUSB_LOG_LEVEL_DEBUG);
	CHECK_SUCCESS;
	
	return CYUSB_OK;
}



//////////////////////////////////////////////
// Find all Cybikos

int cyusb_find(cyusb_ctx_t *ctx){
	libusb_device **devs;
	struct libusb_device_descriptor desc;
	ssize_t cnt;
	ssize_t cybiko_cnt;
	ssize_t bytes;
	int status;
	char full_id[CYUSB_CYID_HEXID_LEN+1] = "";

	// Get all USB devices
	cnt = libusb_get_device_list(ctx->usb, &devs);
	DBG("CYUSB: Found %ld USB devices\n",cnt);

	// Filter Cybiko Xtremes, count them
	ctx->num = 0;
	for(int i = 0; i < cnt; i++) {

		status = libusb_get_device_descriptor(devs[i], &desc);
		CHECK_SUCCESS;

		if (desc.idVendor == CYUSB_VID && desc.idProduct == CYUSB_PID) {
			DBG("CYUSB: Found Cybiko Xtreme: ID=%ld\n",ctx->num);
			ctx->num++;
		}
	}

	// Allocate memory for Cybiko Xtremes
	ctx->dev = malloc(ctx->num * sizeof(cyusb_dev_t));

	// Again scan all devices for Cybikos
	cybiko_cnt = 0;
	for(int i = 0; i < cnt; i++) {

		status = libusb_get_device_descriptor(devs[i], &desc);
		CHECK_SUCCESS;

		if (desc.idVendor == CYUSB_VID && desc.idProduct == CYUSB_PID) {

			// Add Cybiko to list
			ctx->dev[cybiko_cnt].usb = devs[i];

			// Open it
			status = libusb_open(ctx->dev[cybiko_cnt].usb, &(ctx->dev[cybiko_cnt].handle));
			CHECK_SUCCESS;

			// Get user CyID
			bytes = libusb_get_string_descriptor_ascii(ctx->dev[cybiko_cnt].handle, CYUSB_CYID_IDX, full_id,  CYUSB_CYID_HEXID_LEN);

			ctx->dev[cybiko_cnt].CyID = malloc(CYUSB_CYID_LEN);

			// Parse and copy IDs
			sscanf(full_id, "%lx @%s", &(ctx->dev[cybiko_cnt].HexID), ctx->dev[cybiko_cnt].CyID);

			DBG("CYUSB: Cybiko Xtreme %ld: CyID = @%s, hex ID = %lx\n",cybiko_cnt,ctx->dev[cybiko_cnt].CyID,ctx->dev[cybiko_cnt].HexID);

			// Close Cybiko port
			libusb_close(ctx->dev[cybiko_cnt].handle);


			cybiko_cnt++;
		} else {
			// If not Cybiko - remove
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
//////////////////////////////////////////////
// Cybiko USB access library - low level functions
// Author: Michael Tulupov (vaxxabait@gmail.com)
// License: public domain


#ifndef _CYUSB_H
#define _CYUSB_H

#include <stdbool.h>
#include <stdint.h>
#include <libusb-1.0/libusb.h>


// Error codes
#define CYUSB_OK 0
#define CYUSB_ERR -1


#define CYUSB_CYID_LEN 8
#define CYUSB_CYID_HEXID_LEN 46

//////////////////////////////////////////////
// Cybiko device

typedef struct {
	libusb_device * usb;				// USB device
	libusb_device_handle * handle;	// USB handle
	char * CyID;						// Cybiko ID
	uint64_t	HexID;
	bool ignorecrc;					// Do not check CRC at read
} cyusb_dev_t;



//////////////////////////////////////////////
// Library context

typedef struct {
	libusb_context *usb;		// USB library context
	ssize_t num;				// Number of Cybiko's connected
	cyusb_dev_t *dev;		// USB device list
} cyusb_ctx_t;



//////////////////////////////////////////////
// Protocol packet

// 4 bytes of command/status
typedef struct {
	uint8_t _A;
	uint8_t _B;
	uint8_t _C;
	uint8_t _D;
} cyusb_cmdstat_t;

// USB packet
typedef struct {
	uint16_t header;		// Header "MM"

	// uint8_t
	enum type {			// Packet type:
		DATA=0x11,		// Data
		CMD_STAT=0x12		// Command or Status
	} type;

	// uint8_t
	enum data_type {		// Function
		SHELLCMD = 0x01,	// Shell command
		_TYPE_02 = 0x02,
		PING = 0x09, 		// Ping
		_TYPE_1D = 0x1D,
		_TYPE_1E = 0x1E,
		_TYPE_21 = 0x21,
		_TYPE_22 = 0x22,
		FILEUP = 0xC7,		// File upload
		_TYPE_C9 = 0xC9,
		FILEDOWN = 0xCA		// File download
	} func;

	uint8_t cnt;			// Packet count

	uint16_t len;			// Packet length

	uint8_t * data;			// Payload when type=DATA
	cyusb_cmdstat_t cmdstat;	// Payload when type=CMD_STAT

	uint32_t crc;			// Checksum
} cyusb_pkt_t;



//////////////////////////////////////////////
// Low level functions

// Initialize the library
int cyusb_init(cyusb_ctx_t *ctx);

// Find all Cybikos
int cyusb_find(cyusb_ctx_t *ctx);

// Open one Cybiko
int cyusb_open(cyusb_dev_t *dev);

// Write to Cybiko
int cyusb_write(const cyusb_dev_t *dev, const cyusb_pkt_t *pkt);

// Read from Cybiko
int cyusb_read(const cyusb_dev_t *dev, cyusb_pkt_t *pkt);

// Close Cybiko
void cyusb_close(cyusb_dev_t *dev);

// De-initialize the library
int cyusb_exit(cyusb_ctx_t *ctx);

#endif
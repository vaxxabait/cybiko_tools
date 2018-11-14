
#ifndef _CYUSB_H
#define _CYUSB_H


//////////////////////////////////////////////
// Global context

typedef struct {
	char usb_addr;
	char CyID[8];		// Cybiko ID
	bool read_crc_ignore;
} cyusb_cxt_t;


typedef struct {
	uint16_t header = 0x4d4d;	// Header "MM"

	// uint8_t
	enum type {			// Packet type:
		DATA=0x11,		// Data
		CMD_STAT=0x12		// Command or Status
	} type;

	// uint8_t
	enum data_type {		// Payload type:
		SHELLCMD = 0x01		// Shell command
		[0x02] = "SIMPLE ???",
		PING = 0x09, 		// Ping
	[0x1D] = "0x1D ???",
	[0x1E] = "0x1E ???",
	[0x21] = "0x21 ???",
	[0x22] = "0x22 ???",
		FILEUP = 0xC7,		// File upload
	[0xC9] = "0xC9 ???",
		FILEDOWN = 0xCA		// File download
	} data_type;

	uint8_t cnt;			// Packet count
	
	uint16_t len;			// Packet length
	
	char 				// Payload

	uint32_t crc;			// Checksum
} cyusb_pkt_t;

#endif
--////////////////////////////////////////////////////////////////////////
-- Description: Cybiko Xtreme USB protocol dissector for Wireshark
-- Author: Michael Tulupov
-- Email: vaxxabait@gmail.com


--////////////////////////////////////////////////////////////////////////
-- Create new protocol

local cybiko_xtreme = Proto("cybiko_xtreme", "Cybiko Xtreme")


--////////////////////////////////////////////////////////////////////////
-- Full payload

local field_rawdata = ProtoField.bytes(
	"cybiko_xtreme.rawdata",
	"Raw data",
	base.SPACE
)


--////////////////////////////////////////////////////////////////////////
-- Protocol fields

-- Header
local header_types = {
    [0x4d4d] = "MM"
}
local field_header = ProtoField.uint16(
	"cybiko_xtreme.header",
	"Header",
	base.HEX,
	header_types
)

-- Message type
local msg_types = {
	[0x11] = "DATA",
	[0x12] = "COMMAND/STATUS",
}
local field_msgtype = ProtoField.uint8(
	"cybiko_xtreme.message_type",
	"Message type",
	base.HEX,
	msg_types
)

-- Packet type
local pkttype_types = {
	[0x01] = "Shell command",
	[0x02] = "SIMPLE ???",
	[0x09] = "Ping",
	[0x1D] = "0x1D ???",
	[0x1E] = "0x1E ???",
	[0x21] = "0x21 ???",
	[0x22] = "0x22 ???",
	[0xC7] = "File upload",
	[0xC9] = "0xC9 ???",
	[0xCA] = "File download"
}
local field_pkttype = ProtoField.uint8(
	"cybiko_xtreme.packet_type",
	"Packet type",
	base.HEX,
	pkttype_types
)

-- Packet count
local field_pktcnt = ProtoField.uint8(
	"cybiko_xtreme.packet_count",
	"Packet count",
	base.DEC
)

-- Packet length
local field_pktlen = ProtoField.uint16(
	"cybiko_xtreme.packet_length",
	"Packet length",
	base.DEC
)

-- Packet data
local field_pktdata = ProtoField.bytes(
	"cybiko_xtreme.packet_data",
	"Packet data",
	base.SPACE
)

-- Command
local cmd_types = {
	[0x02] = "Download packet by ID",
	[0x04] = "0x04 ???",
	[0x05] = "0x05 ???"
}
local field_cmd = ProtoField.uint8(
	"cybiko_xtreme.cmd",
	"Command",
	base.HEX,
	cmd_types
)

-- Packet request
local field_pktreq = ProtoField.uint8(
	"cybiko_xtreme.packet_req",
	"Packet request ID",
	base.DEC
)

-- Checksum
local field_chksum = ProtoField.uint32(
	"cybiko_xtreme.checksum",
	"Checksum",
	base.HEX
)

-- String text
local field_strtext = ProtoField.string(
	"cybiko_xtreme.string_text",
	"Text",
	base.ASCII
)



--////////////////////////////////////////////////////////////////////////
-- USB fields

local usb_datalen_field = Field.new("usb.data_len")
local usb_ep_field = Field.new("usb.endpoint_address.number")
local usb_trtype_field = Field.new("usb.transfer_type")

local field_datalen = ProtoField.uint16(
	"cybiko_xtreme.data_len",
	"Data length",
	base.DEC
)


--////////////////////////////////////////////////////////////////////////
-- Register fields

cybiko_xtreme.fields =
{
	-- Common fields
	field_rawdata,
	field_datalen,

	-- Protocol fields
	field_header,
	field_msgtype,
	field_pkttype,
	field_pktcnt,
	field_pktlen,
	field_pktdata,
	field_cmd,
	field_pktreq,
	field_chksum,

	field_strtext
}


--////////////////////////////////////////////////////////////////////////
-- Helper fnctions

function append_to_title(pinfo, text)
    pinfo.cols.info:set(tostring(pinfo.cols.info)..text)
end


--////////////////////////////////////////////////////////////////////////
-- Main dissector

function cybiko_xtreme.dissector(buffer, pinfo, tree)

	-- Fill USB Fields
	local data_len = usb_datalen_field()
	local ep_num = usb_ep_field()
	local transfer_type = usb_trtype_field()

	-- Get complete packet
	local rawdata = buffer(0x40, data_len.value)


	--////////////////////////////////////////////////////////////////////////
	-- Parse

	local header = 0;
	local msgtype = 0;
	local pkttype = 0;
	local pktcnt = 0;
	local pktlen = 0;
	local pktdata;
	local cmd = 0;
	local pktreq = 0;
	local chksum_arr;
	local chksum;

	local strtext;


	if data_len.value > 0 then

		-- Header - first 16 bits
		header = rawdata(0,1):uint() + bit.lshift(rawdata(1,1):uint(), 8)
		msgtype = rawdata(2,1);
		pkttype = rawdata(3,1);
		pktcnt = rawdata(4,1);
		pktlen = rawdata(6,1):uint() + bit.lshift(rawdata(5,1):uint(), 8);
		pktdata = rawdata(7,pktlen);
		cmd = rawdata(7,1);
		pktreq = rawdata(8,1);
		chksum_arr = rawdata(7+pktlen,4);

		chksum = chksum_arr(3,1):uint() + bit.lshift(chksum_arr(2,1):uint(), 8) +
			bit.lshift(chksum_arr(1,1):uint(), 16) + bit.lshift(chksum_arr(0,1):uint(), 24)
	end

	--////////////////////////////////////////////////////////////////////////
	-- Show result

	if transfer_type.value == 0x03 and data_len.value > 0 then

		pinfo.cols["protocol"] = "Cybiko Xtreme USB"

		-- Create protocol subtree
		local subtree = tree:add(cybiko_xtreme, buffer())

		-- Common fields
		subtree:add(field_rawdata, rawdata)
		subtree:add(field_datalen, data_len.value)

		-- Protocol fields
		subtree:add(field_header, header)
		subtree:add(field_msgtype, msgtype)
		subtree:add(field_pkttype, pkttype)
		subtree:add(field_pktcnt, pktcnt)
		subtree:add(field_pktlen, pktlen)
		subtree:add(field_pktdata, pktdata)
		subtree:add(field_chksum, chksum)

		-- Command packet or response
		if pktlen == 4 then

		end
		
		if ep_num.value == 0x01 then
			append_to_title(pinfo, ": Cybiko to HOST")

			if msgtype:uint() == 0x11 then
				append_to_title(pinfo, ", DATA")
				subtree:add(field_strtext, pktdata)

			elseif msgtype:uint() == 0x12 then
				append_to_title(pinfo, ", STATUS")

				subtree:add(field_cmd, cmd)
				subtree:add(field_pktreq, pktreq)
			end
		
		elseif ep_num.value == 0x02 then
			append_to_title(pinfo, ": HOST to Cybiko")
			
			if msgtype:uint() == 0x11 then
				append_to_title(pinfo, ", DATA")
				subtree:add(field_strtext, pktdata)

			elseif msgtype:uint() == 0x12 then
				append_to_title(pinfo, ", COMMAND")

				subtree:add(field_cmd, cmd)
				subtree:add(field_pktreq, pktreq)
			end
		end



	else
-- TODO ignore packet - remove from analysis
	end
end


--////////////////////////////////////////////////////////////////////////
-- Register dissector

register_postdissector(cybiko_xtreme)

// Copyright (c) 2016 Philip Herron
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef BINDING_BUILD
# include "Config.h"
#endif

#include "RtpPacket.h"

#include <string>
#include <cstring>
#include <sstream>
#include <stdexcept>

// ntohs + htohl
#include <arpa/inet.h>

struct _RTPHeader {
#if IS_BIG_ENDIAN
	uint8_t version:2;
	uint8_t padding:1;
	uint8_t extension:1;
	uint8_t csrccount:4;
	
	uint8_t marker:1;
	uint8_t payloadtype:7;
#else // little endian
	uint8_t csrccount:4;
	uint8_t extension:1;
	uint8_t padding:1;
	uint8_t version:2;
	
	uint8_t payloadtype:7;
	uint8_t marker:1;
#endif // RTP_BIG_ENDIAN
	
	uint16_t sequencenumber;
	uint32_t timestamp;
	uint32_t ssrc;
} __attribute__((packed)) ;

struct _RTPExtensionHeader {
    uint16_t extid;
    uint16_t length;
} __attribute__((packed)) ;

Overflow::RtpPacket::RtpPacket(const unsigned char *buffer,
                               uint16_t length)
    : mTimestmap(0),
      mExtensionID(-1),
      mExtension(nullptr),
      mExtensionLength(0),
      mPayload(nullptr),
      mPayloadLength(0)
{
    struct _RTPHeader *header = (struct _RTPHeader*)buffer;
    mVersion = static_cast<int>(header->version);
    mSequenceNumber = ntohs(header->sequencenumber);
    mType = static_cast<int>(header->payloadtype);
    mMarker = header->marker == 0 ? false : true;
    mTimestmap = ntohl(header->timestamp);

    if (mVersion != 2)
    {
        std::ostringstream message;
        message << "Invalid RTP packet version: " << mVersion;
        throw std::runtime_error(message.str());
    }

    uint16_t payload_offset = sizeof(struct _RTPHeader) + (header->csrccount * sizeof(uint32_t));

    bool has_extension = (header->extension == 0) ? false : true;
    if (has_extension) {
        size_t extension_header_offset = payload_offset;
        struct _RTPExtensionHeader extension_header;
        memset(&extension_header, 0, sizeof(struct _RTPExtensionHeader));
        memcpy(&extension_header, buffer + extension_header_offset, sizeof(struct _RTPExtensionHeader));
        
        mExtensionID = ntohs(extension_header.extid);
        mExtensionLength = (sizeof(uint32_t) * ntohs(extension_header.length));

        mExtension = (unsigned char*)malloc(mExtensionLength);
        memset(mExtension, 0, mExtensionLength);
        
        const unsigned char *extension_data_pointer = buffer
            + payload_offset
            + sizeof(struct _RTPExtensionHeader);
        memcpy(mExtension, extension_data_pointer, mExtensionLength);
        
        payload_offset += sizeof(struct _RTPExtensionHeader) + mExtensionLength;
    }
    
    mPayloadLength = length - payload_offset;
    
    mPayload = (unsigned char *)malloc(mPayloadLength);
    memcpy(mPayload, buffer + payload_offset, mPayloadLength);
}

Overflow::RtpPacket::RtpPacket(const Response* response)
    : RtpPacket(response->bytesPointer(), response->length())
{
}

Overflow::RtpPacket::~RtpPacket()
{
    if (hasExtension())
        free(mExtension);
    
    free(mPayload);
}

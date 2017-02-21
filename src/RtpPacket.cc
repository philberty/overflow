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

#include <glog/logging.h>

#include "RtpPacket.h"

#include <string>
#include <sstream>
  using std::ostringstream;
#include <stdexcept>
  using std::runtime_error;

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

Overflow::RtpPacket::RtpPacket(const unsigned char *buffer, uint16_t length): m_timestmap(0),
                                                                              m_extensionID(-1),
                                                                              m_extension(nullptr),
                                                                              m_extensionLength(0),
                                                                              m_payload(nullptr),
                                                                              m_payloadLength(0) {
    struct _RTPHeader header;
    memcpy(&header, buffer, sizeof(struct _RTPHeader));

    m_version = static_cast<int>(header.version);
    m_sequenceNumber = ntohs(header.sequencenumber);
    m_type = static_cast<int>(header.payloadtype);
    m_marker = header.marker == 0 ? false : true;
    m_timestmap = ntohl(header.timestamp);

    if (m_version != 2) {
        ostringstream message;
        message << "Invalid RTP packet version: " << m_version;
        throw runtime_error{ message.str() };
    }

    uint16_t payload_offset = sizeof(struct _RTPHeader) + (header.csrccount * sizeof(uint32_t));

    bool has_extension = (header.extension == 0) ? false : true;
    if (has_extension) {
        size_t extension_header_offset = payload_offset;
        struct _RTPExtensionHeader extension_header;
        memset(&extension_header, 0, sizeof(struct _RTPExtensionHeader));
        memcpy(&extension_header, buffer + extension_header_offset, sizeof(struct _RTPExtensionHeader));
        
        m_extensionID = ntohs(extension_header.extid);
        m_extensionLength = (sizeof(uint32_t) * ntohs(extension_header.length));

        m_extension = (unsigned char*)malloc(m_extensionLength);
        memset(m_extension, 0, m_extensionLength);
        
        const unsigned char *extension_data_pointer = buffer
            + payload_offset
            + sizeof(struct _RTPExtensionHeader);
        memcpy(m_extension, extension_data_pointer, m_extensionLength);
        
        payload_offset += sizeof(struct _RTPExtensionHeader) + m_extensionLength;
    }
    
    m_payloadLength = length - payload_offset;
    
    m_payload = (unsigned char *)malloc(m_payloadLength);
    if (!m_payload) {
        LOG(ERROR) << "SEQ NUM: " << m_sequenceNumber <<
            " - VERSION: " << m_version <<
            " - PAYLOAD_LENGTH: " << m_payloadLength <<
            " - OFFSET: " << payload_offset <<
            " - TOTAL: " << length;
    }
    memcpy(m_payload, buffer + payload_offset, m_payloadLength);
}

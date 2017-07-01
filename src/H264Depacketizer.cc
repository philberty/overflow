// Copyright (c) 2017 Philip Herron.
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

#include <cppcodec/base64_default_rfc4648.hpp>

#include "H264Depacketizer.h"

#include <cstring>


Overflow::H264Depacketizer::H264Depacketizer(const SessionDescription* palette,
                                             const RtpPacket *packet,
                                             bool isFirstPayload)
    : mPalette(palette),
      mPacket(packet)
{
    const unsigned char *rtp_packet_payload = mPacket->payloadData();
    size_t rtp_packet_payload_length = mPacket->payloadLength();

    if (isFirstPayload) {
        // append fmtp nalu config lines
        std::string config = mPalette->getFmtpH264ConfigParameters();

        size_t pos = config.find(",");
        std::string first_nalu = config.substr(0, pos);
        std::string second_nalu = config.substr(pos + 1, config.length());

        // They are base 64 encoded
        std::vector<uint8_t> first_nalu_decoded = base64::decode(first_nalu.c_str(), first_nalu.size());
        std::vector<uint8_t> second_nalu_decoded = base64::decode(second_nalu.c_str(), second_nalu.size());
        
        push4ByteNaluHeaderToCurrentPayload();
        pushBytesToCurrentPayload((const unsigned char *)&(first_nalu_decoded[0]),
                                  first_nalu_decoded.size());

        push4ByteNaluHeaderToCurrentPayload();
        pushBytesToCurrentPayload((const unsigned char *)&(second_nalu_decoded[0]),
                                  second_nalu_decoded.size());
    }

    int payload_type = getH264NaluTypeFromByte(rtp_packet_payload[0]);

    switch(payload_type) {
    case 0:
        pushBytesToCurrentPayload(rtp_packet_payload, rtp_packet_payload_length);
        break;

    case 7:
    case 8:
        push3ByteNaluHeaderToCurrentPayload();
        pushBytesToCurrentPayload(rtp_packet_payload, rtp_packet_payload_length);
        break;
        
    case 24:
        push3ByteNaluHeaderToCurrentPayload();
        pushBytesToCurrentPayload(rtp_packet_payload + 1, rtp_packet_payload_length - 1);
        break;

    case 25:
    case 26:
    case 27:
        push3ByteNaluHeaderToCurrentPayload();
        pushBytesToCurrentPayload(rtp_packet_payload + 3, rtp_packet_payload_length - 3);
        break;

    case 28:
    case 29: {
        unsigned char start_bit = rtp_packet_payload[1] >> 7;
        if (start_bit)
        {
            unsigned char *temp_payload = (unsigned char*)malloc(rtp_packet_payload_length - 1);
            memcpy(temp_payload, rtp_packet_payload + 1, rtp_packet_payload_length - 1);
            
            unsigned char header = (rtp_packet_payload[0] & 0xE0) + (rtp_packet_payload[1] & 0x1F);
            temp_payload[0] = header;
            
            push3ByteNaluHeaderToCurrentPayload();
            pushBytesToCurrentPayload(temp_payload, rtp_packet_payload_length - 1);
            
            free(temp_payload);
        }
        else
        {
            pushBytesToCurrentPayload(rtp_packet_payload + 2, rtp_packet_payload_length - 2);
        }
    }
    break;
        
    default:
        push3ByteNaluHeaderToCurrentPayload();
        pushBytesToCurrentPayload(rtp_packet_payload, rtp_packet_payload_length);
        break;
    }    
}

const unsigned char*
Overflow::H264Depacketizer::bytes() const
{
    return &(mPayload[0]);
}

size_t
Overflow::H264Depacketizer::length() const
{
    return mPayload.size();
}

int
Overflow::H264Depacketizer::getH264NaluTypeFromByte(const unsigned char byte) const
{
    return byte & 0x1F;
}

void
Overflow::H264Depacketizer::pushBytesToCurrentPayload(const unsigned char* bytes,
                                                      size_t length)
{
    size_t i;
    for (i = 0; i < length; ++i)
    {
        mPayload.push_back(bytes[i]);
    }
}

void
Overflow::H264Depacketizer::push4ByteNaluHeaderToCurrentPayload()
{
    unsigned char nalu_header[] = { 0x00, 0x00, 0x00, 0x01 };
    pushBytesToCurrentPayload(nalu_header, sizeof(nalu_header));
}

void
Overflow::H264Depacketizer::push3ByteNaluHeaderToCurrentPayload()
{
    unsigned char nalu_header[] = { 0x00, 0x00, 0x01 };
    pushBytesToCurrentPayload(nalu_header, sizeof(nalu_header));
}

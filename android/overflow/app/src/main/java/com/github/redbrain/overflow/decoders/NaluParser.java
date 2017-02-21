package com.github.redbrain.overflow.decoders;

import android.util.Log;

import java.nio.ByteBuffer;
import java.util.LinkedList;
import java.util.List;

/**
 * Created by redbrain on 25/01/17.
 */

public class NaluParser {

    private static final int kThresholdLength = 10;

    public static List<NaluSegment> ParseNaluSegments(ByteBuffer buffer) {
        List<NaluSegment> segmentList = new LinkedList<>();
        int length = buffer.limit();

        // there cannot be an nalu if its less than this size.
        if (length < kThresholdLength) {
            Log.i("NaluParser", "not big enough: " + length);
            return segmentList;
        }


        int lastStartingOffset = -1;
        for (int i = 0; i < (length - kThresholdLength); ++i) {

            // Match the 3 byte offset
            if (buffer.get(i) == 0x00 && buffer.get(i + 1) == 0x00 && buffer.get(i + 2) == 0x01) {
                int naluType = (buffer.get(i+3) & 0x1F);
                NaluSegment segment = new NaluSegment(naluType, 3, i);

                // This is actually a 4 byte segment
                if (i > 0 && buffer.get(i-1) == 0x00) {
                    int currentSegmentOffset = segment.getOffset();
                    segment.setHeaderSize(4);
                    segment.setOffset(currentSegmentOffset - 1);
                }

                //
                if (lastStartingOffset > -1) {
                    NaluSegment lastSegment = segmentList.get(segmentList.size() - 1);
                    int bufferLength = segment.getOffset() - lastSegment.getOffset();
                    byte[] segmentBuffer = new byte[bufferLength];

                    buffer.position(lastSegment.getOffset());
                    buffer.get(segmentBuffer, 0, bufferLength);

                    lastSegment.setBuffer(segmentBuffer);
                    segmentList.remove(segmentList.size() - 1);
                    segmentList.add(lastSegment);
                }
                segmentList.add(segment);
                lastStartingOffset = segment.getOffset();
            }
        }

        if (segmentList.size() > 0) {
            NaluSegment lastSegment = segmentList.get(segmentList.size() - 1);
            int bufferLength = length - lastSegment.getOffset();
            byte[] segmentBuffer = new byte[bufferLength];

            buffer.position(lastSegment.getOffset());
            buffer.get(segmentBuffer, 0, bufferLength);

            lastSegment.setBuffer(segmentBuffer);
            segmentList.remove(segmentList.size() - 1);
            segmentList.add(lastSegment);
        }
        return segmentList;
    }

}

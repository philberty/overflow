package com.github.redbrain.overflow.decoders;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;

import static android.webkit.ConsoleMessage.MessageLevel.LOG;

/**
 * Created by redbrain on 24/01/17.
 */

public class H264Decoder extends MediaCodec.Callback {

    private static final String TAG = "H264";
    private static final String kMimeType = "video/avc";

    private final int kPPS = 8;
    private final int kSPS = 7;
    private final int kKEY_FRAME = 5;

    private MediaCodec media_codec;
    private MediaFormat media_format;

    private NaluSegment pps;
    private NaluSegment sps;

    private BlockingQueue<Integer> next_decode_buffer_input_queue;

    public H264Decoder() {}

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int index) {
        next_decode_buffer_input_queue.add(new Integer(index));
    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {
        codec.releaseOutputBuffer(index, true);
    }

    @Override
    public void onError(MediaCodec codec, MediaCodec.CodecException e) {
        // reset back
        ReleaseCodec();
    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {
        media_format = format;
    }

    public void Start(Surface target_surface, int width, int height) throws IOException {
        synchronized (this) {
            media_codec = MediaCodec.createDecoderByType(kMimeType);
            media_codec.setCallback(this);

            next_decode_buffer_input_queue = new ArrayBlockingQueue<Integer>(100);

            media_format = MediaFormat.createVideoFormat(kMimeType, 320, 320);
            if (sps != null && pps != null) {
                media_format.setByteBuffer("csd-0", ByteBuffer.wrap(pps.getBuffer()));
                media_format.setByteBuffer("csd-1", ByteBuffer.wrap(sps.getBuffer()));
            }

            media_codec.configure(media_format, target_surface, null, 0);
            media_codec.start();
        }
    }

    public void Stop() {
        synchronized (this) {
            StopAndRelease();
        }
    }

    public void ClearParameters() {
        sps = null;
        pps = null;
    }

    public void decode_payload(ByteBuffer buffer)
    {
        List<NaluSegment> segments = NaluParser.ParseNaluSegments(buffer);
        long presentation_time = 0;

        for (NaluSegment segment : segments) {
            int flags = 0;

            if (segment.getType() == kPPS) {
                flags = MediaCodec.BUFFER_FLAG_CODEC_CONFIG;
                pps = segment;
            }
            else if (segment.getType() == kSPS) {
                flags = MediaCodec.BUFFER_FLAG_CODEC_CONFIG;
                sps = segment;
            }
            else if (segment.getType() == kKEY_FRAME) {
                flags = MediaCodec.BUFFER_FLAG_KEY_FRAME;
            }

            decode_nalu_segment(segment, flags, presentation_time);
        }
    }

    private void decode_nalu_segment(NaluSegment segment, int flags, long presentation_time) {
        if (media_codec == null) {
            return;
        }

        try {
            Integer buffer_index = next_decode_buffer_input_queue.poll(1000, TimeUnit.MILLISECONDS);
            if (buffer_index != null) {
                ByteBuffer input_buffer = media_codec.getInputBuffer(buffer_index);

                input_buffer.put(segment.getBuffer());
                media_codec.queueInputBuffer(buffer_index, 0, segment.getLength(), presentation_time, flags);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void StopAndRelease() {
        synchronized (this) {
            StopCodec();
            ReleaseCodec();
        }
    }

    private void StopCodec() {
        synchronized (this) {
            media_codec.stop();
        }
    }

    private void FlushCodec() {
        synchronized (this) {
            media_codec.flush();
        }
    }

    private void ReleaseCodec() {
        synchronized (this) {
            media_codec.release();
        }
    }

}

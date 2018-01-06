package com.github.redbrain.overflowtest;

import android.graphics.SurfaceTexture;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Surface;
import android.view.TextureView;

import com.github.redbrain.overflow.RtspClient;
import com.github.redbrain.overflow.RtspClientDelegate;
import com.github.redbrain.overflow.decoders.H264Decoder;

import java.io.IOException;
import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity implements RtspClientDelegate, TextureView.SurfaceTextureListener {

    private TextureView playerTextureView;
    private H264Decoder decoder = new H264Decoder();
    private RtspClient rtspClient;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        playerTextureView = (TextureView) findViewById(R.id.player_texture_view);
        playerTextureView.setSurfaceTextureListener(this);

        rtspClient = new RtspClient(this, "rtsp://192.168.1.108:8555/test");

    }

    @Override
    public void onPaletteType(RtspClient.SessionType type) {

    }

    @Override
    public void onRtspClientStateChange(RtspClient.State oldState, RtspClient.State newState) {

    }

    @Override
    public void onRtpPacketExtension(ByteBuffer buffer) {

    }

    @Override
    public void onPayload(ByteBuffer buffer) {
        decoder.decode_payload(buffer);
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        try {
            decoder.Start(new Surface(surface), width, height);
            rtspClient.start();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        decoder.Stop();
        rtspClient.stop();
        rtspClient.join();

        rtspClient = new RtspClient(this, "rtsp://192.168.1.108:8555/test");

        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {

    }
}

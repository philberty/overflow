package com.github.redbrain.overflow;

import android.graphics.SurfaceTexture;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;

import com.github.redbrain.overflow.decoders.H264Decoder;

import java.io.IOException;
import java.nio.ByteBuffer;

public class RtspPlayerActivity extends AppCompatActivity implements TextureView.SurfaceTextureListener, RtspClientDelegate {

    private TextureView player_texture_view;
    private H264Decoder decoder = new H264Decoder();

    private RtspClient rtsp_client;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rtsp_player);

        player_texture_view = (TextureView) findViewById(R.id.player_texture_view);
        player_texture_view.setSurfaceTextureListener(this);

        rtsp_client = new RtspClient(this, "rtsp://192.168.1.108:8554/test.264");

    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {

        try {

            decoder.Start(new Surface(surface), width, height);

            boolean ok = rtsp_client.Start();
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
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {

    }

    @Override
    public void timeout() {

    }

    @Override
    public void payload(ByteBuffer buffer) {
        decoder.decode_payload(buffer);
    }
}

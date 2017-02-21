package com.github.redbrain.overflow;

import android.util.Log;

/**
 * Created by redbrain on 14/01/17.
 */

public class RtspClient {

    static {
        System.loadLibrary("overflow");
    }

    private final long  _client;
    private long _delegate;

    public RtspClient(RtspClientDelegate delegate, String url) {
        _client = _new_client(delegate, url);
    }

    public boolean Start() {
        return _Start() == 1;
    }

    public native void Stop();

    private native long _new_client(RtspClientDelegate delegate, String url);

    private native void Close();

    private native int _Start();

}

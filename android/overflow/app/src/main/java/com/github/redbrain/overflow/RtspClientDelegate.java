package com.github.redbrain.overflow;

import java.nio.ByteBuffer;

/**
 * Created by redbrain on 15/01/17.
 */

public interface RtspClientDelegate {

    void timeout();

    void payload(ByteBuffer buffer);

}

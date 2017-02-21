package com.github.redbrain.overflow.decoders;

/**
 * Created by redbrain on 25/01/17.
 */

public class NaluSegment {

    private int type;
    private int header_size;
    private int offset;
    private byte[] buffer;

    public NaluSegment(int type, int header_size, int offset) {
        setType(type);
        setHeaderSize(header_size);
        setOffset(offset);
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public int getHeaderSize() {
        return header_size;
    }

    public void setHeaderSize(int header_size) {
        this.header_size = header_size;
    }

    public int getOffset() {
        return offset;
    }

    public void setOffset(int offset) {
        this.offset = offset;
    }

    public byte[] getBuffer() {
        return buffer;
    }

    public void setBuffer(byte[] buffer) {
        this.buffer = buffer;
    }

    public int getLength() {
        return getBuffer().length;
    }
}

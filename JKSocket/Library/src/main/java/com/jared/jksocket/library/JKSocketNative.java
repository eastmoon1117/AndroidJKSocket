package com.jared.jksocket.library;

import android.util.Log;

/**
 * Created by huaixi on 2018/2/3.
 */

public class JKSocketNative {

    static {
        System.loadLibrary("jksocket");
    }

    public native void nativeInit();
    public native void nativeClassInit();
    public native void nativeCleanup();

    public native void startThread();
    public native int registerSocket(String id);
    public native int socketSend(String id, int cmd, String data);

    private void onCallback(int type) {
        Log.d("JKSocketNative", "Type:" + type);
    }
}

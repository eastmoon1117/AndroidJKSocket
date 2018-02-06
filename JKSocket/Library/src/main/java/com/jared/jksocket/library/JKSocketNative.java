package com.jared.jksocket.library;

import android.util.Log;

/**
 * Created by huaixi on 2018/2/3.
 */

public class JKSocketNative {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("jksocket");
    }

    public native void nativeInit();
    public native void nativeClassInit();
    public native void nativeCleanup();

    public native String stringFromJNI();
    public native int sumFromJNI(int a, int b);
    public native void startThread();

    private void onCallback(int type) {
        Log.d("NdkJniUtils", "Type:" + type);
    }
}

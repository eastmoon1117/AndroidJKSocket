package com.jared.jksocket;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.jared.jksocket.constant.SocketCmd;
import com.jared.jksocket.constant.SocketHandler;
import com.jared.jksocket.daemon.Daemon;
import com.jared.jksocket.library.JKSocketNative;

public class MainActivity extends AppCompatActivity {

    JKSocketNative jkSocketNative = new JKSocketNative();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        jkSocketNative.nativeInit();
        jkSocketNative.nativeClassInit();

        findViewById(R.id.start_daemon).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Daemon.run(getApplication(), getPackageName());
            }
        });

        findViewById(R.id.register_socket).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                jkSocketNative.registerSocket(SocketHandler.Test1);
                jkSocketNative.socketSend(SocketHandler.Test1, SocketHandler.Test2, SocketCmd.CMD1, "I arm from test1");
            }
        });

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        jkSocketNative.nativeCleanup();
    }
}

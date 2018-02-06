package com.jared.jnidaemon;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.jared.daemon.Daemon;
import com.jared.jksocket.library.JKSocketNative;

public class MainActivity extends AppCompatActivity {

    private TextView tv;
    JKSocketNative jkSocketNative = new JKSocketNative();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        jkSocketNative.nativeInit();
        jkSocketNative.nativeClassInit();
        jkSocketNative.startThread();

        // Example of a call to a native method
        tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(jkSocketNative.stringFromJNI() + ":" + jkSocketNative.sumFromJNI(4, 3));

        findViewById(R.id.start_daemon).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Daemon.createSocketFile(getApplication());
                Daemon.run(getApplication(), "");
                //tv.setText(jniUtils.stringFromJNI() + ":" + jniUtils.sumFromJNI(4, 3));
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        jkSocketNative.nativeCleanup();
    }
}

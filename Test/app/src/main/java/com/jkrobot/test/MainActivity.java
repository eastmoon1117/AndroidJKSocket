package com.jkrobot.test;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

public class MainActivity extends AppCompatActivity {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.start_daemon).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String str = stringFromJNI();
            }
        });
    }

    public native String stringFromJNI();
}

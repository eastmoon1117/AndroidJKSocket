package com.jared.jksocket.service;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import com.jared.jksocket.daemon.Daemon;

public class DaemonService extends Service {
	@Override
	public void onCreate() {
		super.onCreate();
		Daemon.run(this, getPackageName());
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		/* do something here */
		return super.onStartCommand(intent, flags, startId);
	}
}

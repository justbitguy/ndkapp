package com.example.admin.ndkapplication;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

/**
 * Created by zhangjw on 2016/8/26.
 */

public class MyService extends Service {

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i("daemon_java", "HostMonitor: onCreate! I can not be Killed!");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i("daemon_java", "HostMonitor: onStartCommand! I can not be Killed!");
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }
}
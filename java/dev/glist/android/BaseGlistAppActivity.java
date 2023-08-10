package dev.glist.android;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.res.Configuration;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.util.concurrent.BlockingDeque;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

import dev.glist.android.lib.GlistNative;

public abstract class BaseGlistAppActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    public static String LIBRARY_NAME = "GlistApp";
    private SurfaceView view;
    private final ScheduledExecutorService mainExecutor = Executors.newSingleThreadScheduledExecutor();
    private final BlockingDeque<Runnable> executeQueue = new LinkedBlockingDeque<>(30);
    private boolean surfaceSet = false;
    private ScheduledFuture<?> task;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        view = GlistNative.init(this, LIBRARY_NAME);

        executeQueue.offerLast(GlistNative::onCreate);
        initExecutors();
    }

    private void initExecutors() {
        task = mainExecutor.scheduleAtFixedRate(() -> {
            if (!surfaceSet) {
                return;
            }
            for (Runnable runnable : executeQueue) {
                runnable.run();
            }
            executeQueue.clear();
        }, 0, 1000 / 30, TimeUnit.MILLISECONDS);
    }

    private void shutdownExecutors() {
        if (task != null) {
            task.cancel(false);
        }
        mainExecutor.shutdown();
        for (Runnable runnable : executeQueue) {
            runnable.run();
        }
        executeQueue.clear();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        executeQueue.offerLast(GlistNative::onDestroy);
        org.fmod.FMOD.close();
        shutdownExecutors();
    }

    @Override
    protected void onStart() {
        super.onStart();
        ClassLoader loader = GlistNative.class.getClassLoader();
        executeQueue.offerLast(() -> GlistNative.onStart(loader));
    }

    @Override
    protected void onStop() {
        super.onStop();
        executeQueue.offerLast(GlistNative::onStop);
        surfaceSet = false;
    }

    @Override
    protected void onPause() {
        super.onPause();
        executeQueue.offerLast(GlistNative::onPause);
        executeQueue.offerLast(GlistNative::disableOrientationListener);
    }

    @Override
    protected void onResume() {
        super.onResume();
        executeQueue.offerLast(GlistNative::onResume);
        executeQueue.offerLast(GlistNative::enableOrientationListener);
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int i, int i1, int i2) {
        surfaceSet = true;
        executeQueue.offerFirst(() -> GlistNative.setSurface(holder.getSurface()));
        executeQueue.offerLast(GlistNative::onResize);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
    }

    @Override
    public void setRequestedOrientation(int requestedOrientation) {
        super.setRequestedOrientation(requestedOrientation);
        GlistNative.onOrientationChanged(requestedOrientation);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if(!surfaceSet) {
            return true;
        }

        int[] coords = new int[2];
        view.getLocationInWindow(coords);
        int pointers = event.getPointerCount();
        int[] pointerIds = new int[pointers];
        for (int i = 0; i < pointers; i++) {
            pointerIds[i] = event.getPointerId(i);
        }
        int[] x = new int[pointers];
        int[] y = new int[pointers];
        int[] types = new int[pointers];
        int actionIndex = event.getActionIndex();
        int actionMasked = event.getActionMasked();
        for (int i = 0; i < pointers; i++) {
            types[i] = event.getToolType(i);
            x[i] = (int) (event.getAxisValue(MotionEvent.AXIS_X, i) - coords[0]);
            y[i] = (int) (event.getAxisValue(MotionEvent.AXIS_Y, i) - coords[1]);
        }
        return GlistNative.onTouchEvent(pointers, pointerIds, x, y, types, actionIndex, actionMasked);
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (GlistNative.getOrientationListener() != null) {
            GlistNative.getOrientationListener().checkOrientation();
        }
    }
}
package dev.glist.android.lib; // Do not change! GlistEngine links to this package.

import android.annotation.SuppressLint;
import android.content.pm.ApplicationInfo;
import android.content.res.AssetManager;
import android.os.Build;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.WindowInsets;
import android.view.WindowManager;

import androidx.annotation.StringRes;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AlertDialog;

import java.util.Objects;

import dev.glist.android.BaseGlistAppActivity;
import dev.glist.android.GlistAppActivity;
import dev.glist.glistapp.R;

@SuppressLint("StaticFieldLeak")
public class GlistNative {

    private static BaseGlistAppActivity activity;
    private static GlistOrientationListener orientationListener;


    public static SurfaceView init(BaseGlistAppActivity activity, String libraryName) {
        activity.getBaseContext().getApplicationInfo();
        System.loadLibrary("fmod");
        boolean isDebug = ((activity.getBaseContext().getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE) != 0);
        // this needs to be changed if project name inside CMakeLists.txt is changed.
        if (isDebug) {
            System.loadLibrary(libraryName + "d");
        } else {
            System.loadLibrary(libraryName);
        }

        GlistNative.activity = activity;
        ActionBar actionBar = activity.getSupportActionBar();
        if (actionBar != null) {
            actionBar.hide();
        }
        setAssetManager(activity.getAssets());
        activity.setContentView(R.layout.main);
        SurfaceView view = activity.findViewById(R.id.surfaceview);
        view.getHolder().addCallback(activity);
        return view;
    }

    public static void enableOrientationListener() {
        if (orientationListener == null) {
            orientationListener = new GlistOrientationListener(activity);
        }
        orientationListener.enable();
    }

    public static void disableOrientationListener() {
        if (orientationListener == null) {
            return;
        }
        orientationListener.disable();
    }

    public static native void onCreate();
    public static native void onDestroy();
    public static native void onStart(ClassLoader classLoader);
    public static native void onStop();
    public static native void onPause();
    public static native void onResume();

    public static native void setSurface(Surface surface);
    public static native void setAssetManager(AssetManager assets);
    public static native boolean onTouchEvent(int pointerCount, int[] fingerIds, int[] x, int[] y);

    public static void showAlertDialog(int dialogId, String message, String title, String cancelText, String negativeText, String positiveText) {
        activity.runOnUiThread(() -> {
            AlertDialog.Builder builder = new AlertDialog.Builder(activity);
            builder.setMessage(message);
            builder.setTitle(title);
            if (cancelText != null && !cancelText.isEmpty()) {
                builder.setNeutralButton(cancelText, (dialog, which) -> onDialogButtonCallback(dialogId, which));
            }
            if (negativeText != null && !negativeText.isEmpty()) {
                builder.setNegativeButton(negativeText, (dialog, which) -> onDialogButtonCallback(dialogId, which));
            }
            if (positiveText != null && !positiveText.isEmpty()) {
                builder.setPositiveButton(positiveText, (dialog, which) -> onDialogButtonCallback(dialogId, which));
            }
            builder.setOnCancelListener(dialog -> onDialogDismissCallback(dialogId));
            builder.setOnDismissListener(dialog -> onDialogClosedCallback(dialogId));
            builder.create().show();
        });
    }

    public static void showAlertDialog(int dialogId, String message, String title, @StringRes Integer cancelTextId, @StringRes Integer negativeTextId, @StringRes Integer positiveTextId) {
        activity.runOnUiThread(() -> {
            AlertDialog.Builder builder = new AlertDialog.Builder(activity);
            builder.setMessage(message);
            builder.setTitle(title);
            if (cancelTextId != null) {
                builder.setNeutralButton(cancelTextId, (dialog, which) -> onDialogButtonCallback(dialogId, which));
            }
            if (negativeTextId != null) {
                builder.setNegativeButton(negativeTextId, (dialog, which) -> onDialogButtonCallback(dialogId, which));
            }
            if (positiveTextId != null) {
                builder.setPositiveButton(positiveTextId, (dialog, which) -> onDialogButtonCallback(dialogId, which));
            }
            builder.setOnCancelListener(dialog -> onDialogDismissCallback(dialogId));
            builder.setOnDismissListener(dialog -> onDialogClosedCallback(dialogId));
            builder.create().show();
        });
    }
    public static void showAlertDialogWithType(int dialogId, String message, String title, String type) {
        if (Objects.equals(type, "ok")) {
            showAlertDialog(dialogId, message, title, null, null, android.R.string.ok);
        } else if (Objects.equals(type, "okcancel")) {
            showAlertDialog(dialogId, message, title, android.R.string.cancel, null, android.R.string.ok);
        } else if (Objects.equals(type, "yesno")) {
            showAlertDialog(dialogId, message, title, null, android.R.string.no, android.R.string.yes);
        } else if (Objects.equals(type, "yesnocancel")) {
            showAlertDialog(dialogId, message, title, android.R.string.cancel, android.R.string.no, android.R.string.yes);
        }
    }

    public static void setFullscreen(boolean fullscreen) {
        if (fullscreen) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                activity.getWindow().getInsetsController().hide(WindowInsets.Type.statusBars());
            } else {
                activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
            }
        } else {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                activity.getWindow().getInsetsController().show(WindowInsets.Type.statusBars());
            } else {
                activity.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
            }
        }
    }

    public static void setDeviceOrientation(int orientation) {
        activity.setRequestedOrientation(orientation);
        onOrientationChanged(orientation);
    }

    public static native void onOrientationChanged(int orientation);

    public static native void onDialogButtonCallback(int dialogId, int which);
    public static native void onDialogDismissCallback(int dialogId);
    public static native void onDialogClosedCallback(int dialogId);

    public static BaseGlistAppActivity getActivity() {
        return activity;
    }
}

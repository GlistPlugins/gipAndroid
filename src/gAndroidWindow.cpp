/*
 * gAndroidWindow.cpp
 *
 *  Created on: June 24, 2023
 *      Author: Metehan Gezer
 */

#include "gAndroidWindow.h"
#include "gAppManager.h"

#ifdef ANDROID

ANativeWindow* gAndroidWindow::nativewindow = nullptr;
gAndroidWindow* window = nullptr;

gAndroidWindow::gAndroidWindow() {
    window = this;
	isclosed = true;
	isrendering = false;
}

gAndroidWindow::~gAndroidWindow() {
    close();
}

void gAndroidWindow::initialize(int uwidth, int uheight, int windowMode, bool isResizable) {
    gLogi("gAndroidWindow") << "initialize";
	const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, // request OpenGL ES 3.0
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
	};
	EGLConfig config;
	EGLint numConfigs;
	EGLint format;

	if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
		gLogi("gAndroidWindow") << "eglGetDisplay() returned error " << eglGetError();
        exit(-1);
		return;
	}
	if (!eglInitialize(display, 0, 0)) {
		gLogi("gAndroidWindow") << "eglInitialize() returned error " << eglGetError();
        exit(-1);
		return;
	}

	if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
		gLogi("gAndroidWindow") << "eglChooseConfig() returned error " << eglGetError();
		close();
		return;
	}

	if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
		gLogi("gAndroidWindow") << "eglGetConfigAttrib() returned error " << eglGetError();
		close();
		return;
	}

	if (!(surface = eglCreateWindowSurface(display, config, nativewindow, 0))) {
		gLogi("gAndroidWindow") << "eglCreateWindowSurface() returned error " << eglGetError();
		close();
		return;
	}
    const EGLint attribList[] = {
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
	};

	if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, attribList))) {
		gLogi("gAndroidWindow") << "eglCreateContext() returned error " << eglGetError();
		close();
		return;
	}

	if (!eglMakeCurrent(display, surface, surface, context)) {
		gLogi("gAndroidWindow") << "eglMakeCurrent() returned error " << eglGetError();
		close();
		return;
	}

	if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
		!eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
		gLogi("gAndroidWindow") << "eglQuerySurface() returned error " << eglGetError();
		close();
		return;
	}

	glViewport(0, 0, width, height);
	if(uwidth == 0) {
		uwidth = width;
	}
	if(uheight == 0) {
		uheight = height;
	}
	scalex = (float) width / (float) uwidth;
	scaley = (float) height / (float) uheight;
	isclosed = false;
	isrendering = true;
	gBaseWindow::initialize(width, height, windowMode, false);
}


bool gAndroidWindow::getShouldClose() {
	return isclosed;
}

void gAndroidWindow::update() {
    if(!isrendering) {
        return;
    }
	if(!eglSwapBuffers(display, surface)) {
        EGLint err = eglGetError();
        if(err == EGL_BAD_SURFACE) {
            isrendering = false;
			close();
            return;
        }
		gLogi("gAndroidWindow") << "eglSwapBuffers() returned error " << err;
	}
}

void gAndroidWindow::close() {
    if(!display) {
        return;
    }
    isrendering = false;
    gLogi("gAndroidWindow") << "close";
	eglMakeCurrent(display,EGL_NO_SURFACE,EGL_NO_SURFACE, EGL_NO_CONTEXT );
	eglDestroySurface(display, surface);
	eglDestroyContext(display,context);
	eglTerminate(display);
	display = nullptr;
	isclosed = true;
}

void gAndroidWindow::setVsync(bool vsync) {
    gBaseWindow::setVsync(vsync);
}

void gAndroidWindow::setCursor(int cursorNo) {
}

void gAndroidWindow::setCursorMode(gCursorMode cursorMode) {
}

void gAndroidWindow::setClipboardString(std::string text) {
}

std::string gAndroidWindow::getClipboardString() {
	return "todo";
}

void gAndroidWindow::setWindowSize(int width, int height) {
}

void gAndroidWindow::setWindowResizable(bool isResizable) {
}

void gAndroidWindow::setWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight) {
}

void gAndroidWindow::resize() {
    update();
    if(!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        gLogi("gAndroidWindow") << "eglQuerySurface() returned error " << eglGetError();
        close();
        return;
    }
    glViewport(0, 0, width, height);
    setSize(width, height);
}

void gAndroidWindow::showKeyboard() {
	// Raises the soft keyboard via the static GlistNative.showKeyboard(), which
	// marshals to the UI thread and calls InputMethodManager. Called from a text
	// control when it gains edit focus (gGUITextbox), on the loop thread.
	jclass nativeclass = gAndroidUtil::getJavaGlistAndroid();
	gAndroidUtil::callJavaStaticVoidMethod(nativeclass, "showKeyboard", "()V");
}

void gAndroidWindow::hideKeyboard() {
	jclass nativeclass = gAndroidUtil::getJavaGlistAndroid();
	gAndroidUtil::callJavaStaticVoidMethod(nativeclass, "hideKeyboard", "()V");
}

extern "C" {
JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_setSurface(JNIEnv *env, jclass clazz, jobject surface) {
	if(surface != nullptr) {
		gAndroidWindow::nativewindow = ANativeWindow_fromSurface(env, surface);
	} else {
		if(window) {
			window->close();
		}
		ANativeWindow_release(gAndroidWindow::nativewindow);
	}
}

JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onResize(JNIEnv *env, jclass clazz) {
	if(appmanager) {
		appmanager->submitToMainThread([]() {
			if(!window) {
				return;
			}
			window->resize();
		});
	}
}

JNIEXPORT jboolean JNICALL Java_dev_glist_android_lib_GlistNative_onTouchEvent(JNIEnv *env, jclass clazz, jint pointerCount, jintArray pointerIds, jintArray x, jintArray y, jintArray types, jint actionIndex, jint actionMasked) {
	if(!window) {
		return false;
	}

	// The isCopy out-parameter is optional; passing a heap-allocated jboolean here
	// leaked one per touch event. The types array was also read from y by mistake,
	// so every input carried its y coordinate as its InputType.
	int* _pointerIds = env->GetIntArrayElements(pointerIds, nullptr);
	int* _x = env->GetIntArrayElements(x, nullptr);
	int* _y = env->GetIntArrayElements(y, nullptr);
	int* _types = env->GetIntArrayElements(types, nullptr);
	TouchInput inputs[pointerCount];
	for(int i = 0; i < pointerCount; ++i) {
		inputs[i] = {(InputType) _types[i], _pointerIds[i], i, _x[i], _y[i]};
	}
	// callEvent dispatches synchronously and the handlers copy what they keep, so
	// the JNI arrays can be handed back as soon as it returns. JNI_ABORT because
	// nothing was written into them.
	env->ReleaseIntArrayElements(pointerIds, _pointerIds, JNI_ABORT);
	env->ReleaseIntArrayElements(x, _x, JNI_ABORT);
	env->ReleaseIntArrayElements(y, _y, JNI_ABORT);
	env->ReleaseIntArrayElements(types, _types, JNI_ABORT);
	gTouchEvent event{pointerCount, inputs, actionIndex, (ActionType) actionMasked};
	window->callEvent(event);
	return true;
}

JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onCharTyped(JNIEnv *env, jclass clazz, jint codepoint) {
	// A Unicode code point committed by the soft keyboard. Fed into the same
	// gCharTypedEvent path the desktop uses; gAppManager queues the GUI delivery
	// onto the loop thread on mobile, so this UI-thread call does not race drawing.
	if(!window) {
		return;
	}
	gCharTypedEvent event{(unsigned int) codepoint};
	window->callEvent(event);
}

JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onKey(JNIEnv *env, jclass clazz, jint keycode, jboolean pressed) {
	// Edit keys the soft keyboard sends as key events rather than text - backspace,
	// enter and the like - as engine key codes. Same queued delivery as above.
	if(!window) {
		return;
	}
	if(pressed) {
		gKeyPressedEvent event{(int) keycode};
		window->callEvent(event);
	} else {
		gKeyReleasedEvent event{(int) keycode};
		window->callEvent(event);
	}
}

JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onOrientationChanged(JNIEnv *env, jclass clazz, jint orientation) {
	if(!window) {
		return;
	}

    gDeviceOrientationChangedEvent event{(DeviceOrientation) orientation};
	window->callEvent(event);
}

}

#endif /* ANDROID */
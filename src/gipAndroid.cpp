/*
 * gAndroid.cpp
 *
 *  Created on: July 26, 2023
 *      Author: Metehan Gezer
 */

#include "gipAndroid.h"
#include "gAndroidUtil.h"
#include "gAppManager.h"

#include <thread>
#include <jni.h>

// This function is added to prevent compiler from ignoring this
// translation unit.
// Do not remove!
void gipAndroid::onEngineStart() {

}

extern "C" {

JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onCreate(JNIEnv* env, jclass clazz) {
    //gLogi("GlistNative") << "onCreate";
    androidMain();
#ifdef DEBUG
    assert(appmanager); // appmanager should not be null after androidMain();
#endif
}

JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onDestroy(JNIEnv* env, jclass clazz) {
    //gLogi("GlistNative") << "onDestroy";
    delete appmanager;
    appmanager = nullptr;
}

std::unique_ptr<std::thread> thread;
JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onStart(JNIEnv* env, jclass clazz, jobject classloader) {
    gLogi("GlistNative") << "onStart";
    if(thread) {
        throw std::runtime_error("cannot call onStart without calling onStop first");
    }

	jobject globalclassloader = env->NewGlobalRef(classloader);
	thread = std::make_unique<std::thread>([globalclassloader]() {
		gAndroidUtil::attachMainThread(globalclassloader);
		appmanager->initialize();
		appmanager->setup();
		appmanager->loop();
		gAndroidUtil::getJavaVM()->DetachCurrentThread();
    });
	thread->detach();
}

JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onStop(JNIEnv* env, jclass clazz) {
    //gLogi("GlistNative") << "onStop";
    if(appmanager) {
        appmanager->stop();
    }
    if(thread) {
        thread->join(); // wait for shutdown
        thread = nullptr;
    }
}

JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onPause(JNIEnv* env, jclass clazz) {
    //gLogi("GlistNative") << "onPause";
    if(appmanager) {
        gAppPauseEvent event{};
        appmanager->getEventHandler()(event);
    }
}

JNIEXPORT void JNICALL Java_dev_glist_android_lib_GlistNative_onResume(JNIEnv* env, jclass clazz) {
    //gLogi("GlistNative") << "onResume";
    if(appmanager) {
        gAppResumeEvent event{};
        appmanager->getEventHandler()(event);
    }
}

}

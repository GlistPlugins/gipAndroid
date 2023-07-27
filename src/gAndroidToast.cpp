/*
 * gAndroidToast.cpp
 *
 *  Created on: July 27, 2023
 *      Author: Metehan Gezer
 */

#include "gAndroidToast.h"
#include "gAndroidUtil.h"

void gShowToast(const std::string& text, ToastDuration duration) {
	JNIEnv* env = gAndroidUtil::getJNIEnv();
	jclass nativeclass = gAndroidUtil::getJavaGlistAndroid();
	gAndroidUtil::callJavaStaticVoidMethod(nativeclass, "showToast",
										   "(Ljava/lang/String;I)V",
										   (jstring) JavaString(text),
										   duration
	);
}
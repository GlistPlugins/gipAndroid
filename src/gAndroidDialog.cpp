/*
 * gAndroidDialog.cpp
 *
 *  Created on: July 26, 2023
 *      Author: Metehan Gezer
 */

#include "gAndroidDialog.h"
#include "gAndroidUtil.h"
#include "gUtils.h"
#include <vector>

struct DialogData {
	int dialogid;
	DialogButtonCallback buttoncallback;
	DialogCancelCallback dismisscallback;
};

std::vector<DialogData> dialogs;

void gShowDialog(int dialogId, const std::string& message, const std::string& title,
				 const std::string& cancelText,
				 const std::string& negativeText,
				 const std::string& positiveText, DialogButtonCallback buttonCallback,
				 DialogCancelCallback dismissCallback) {
	JNIEnv* env = gAndroidUtil::getJNIEnv();
	jclass nativeclass = gAndroidUtil::getJavaGlistAndroid();
	dialogs.push_back({dialogId, buttonCallback, dismissCallback});
	gAndroidUtil::callJavaStaticVoidMethod(nativeclass, "showAlertDialog",
										   "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
										   dialogId,
										   (jstring) JavaString(message),
										   (jstring) JavaString(title),
										   (jstring) JavaString(cancelText),
										   (jstring) JavaString(negativeText),
										   (jstring) JavaString(positiveText)
	);
}

void gShowDialog(int dialogId, const std::string& message, const std::string& title,
				 const std::string& type,
				 DialogButtonCallback buttonCallback,
				 DialogCancelCallback dismissCallback) {
	JNIEnv* env = gAndroidUtil::getJNIEnv();
	jclass nativeclass = gAndroidUtil::getJavaGlistAndroid();
	dialogs.push_back({dialogId, buttonCallback, dismissCallback});
	gAndroidUtil::callJavaStaticVoidMethod(nativeclass, "showAlertDialogWithType",
										   "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
										   dialogId,
										   (jstring) JavaString(message),
										   (jstring) JavaString(title),
										   (jstring) JavaString(type)
	);
}

extern "C" {
JNIEXPORT void JNICALL
Java_dev_glist_android_lib_GlistNative_onDialogButtonCallback(JNIEnv* env,
															  jclass clazz,
															  jint dialogId,
															  jint which) {
	for (const auto &item : dialogs) {
		if (item.dialogid != dialogId) {
			continue;
		}
		item.buttoncallback(dialogId, (DialogButton) which);
	}
}

JNIEXPORT void JNICALL
Java_dev_glist_android_lib_GlistNative_onDialogDismissCallback(JNIEnv* env,
															   jclass clazz,
															   jint dialogId) {
	for (const auto &item : dialogs) {
		if (item.dialogid != dialogId) {
			continue;
		}
		item.dismisscallback(dialogId);
	}
}

JNIEXPORT void JNICALL
Java_dev_glist_android_lib_GlistNative_onDialogClosedCallback(JNIEnv* env,
															  jclass clazz,
															  jint dialogId) {
	dialogs.erase(
			std::remove_if(dialogs.begin(), dialogs.end(), [dialogId](DialogData& item) -> bool {
				return item.dialogid == dialogId;
			}), dialogs.end());
	gLogi("gDialog") << "Dialog closed: " << dialogId;
}
}
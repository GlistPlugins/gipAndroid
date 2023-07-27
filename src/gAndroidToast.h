/*
 * gAndroidToast.h
 *
 *  Created on: July 27, 2023
 *      Author: Metehan Gezer
 */

#ifndef GANDROIDTOAST_H
#define GANDROIDTOAST_H

#include <string>

enum ToastDuration {
	TOASTDURATION_SHORT = 0,
	TOASTDURATION_LONG = 1,
};

void gShowToast(const std::string& text, ToastDuration duration);

#endif //GANDROIDTOAST_H

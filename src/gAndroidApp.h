/*
 * gAndroidApp.h
 *
 *  Created on: July 28, 2023
 *      Author: Metehan Gezer
 */

#ifndef GANDROIDAPP_H
#define GANDROIDAPP_H

#include "gBaseApp.h"

class gAndroidApp : public gBaseApp {
public:
	gAndroidApp();
	virtual ~gAndroidApp();
	gAndroidApp(int argc, char **argv) = delete;

	/**
	 * Called when current activity is invisible.
	 * Application will stop rendering after this but will
	 * still receive updates.
	 */
	virtual void pause();
	/**
	 * Called when current activity is visible again.
	 * Application will continue rendering.
	 */
	virtual void resume();

};


#endif //GANDROIDAPP_H

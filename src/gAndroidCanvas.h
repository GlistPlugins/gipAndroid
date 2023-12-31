/*
 * gAndroidCanvas.h
 *
 *  Created on: July 27, 2023
 *      Author: Metehan Gezer
 */

#ifndef GANDROIDCANVAS_H
#define GANDROIDCANVAS_H

#include "gBaseCanvas.h"
#include "gAndroidUtil.h"
#include "gBaseApp.h"

class gAndroidCanvas : public gBaseCanvas {
public:
	gAndroidCanvas(gBaseApp* root);
	virtual ~gAndroidCanvas();

	virtual void deviceOrientationChanged(DeviceOrientation deviceorientation);

	virtual void touchMoved(int x, int y, int fingerId);
	virtual void touchPressed(int x, int y, int fingerId);
	virtual void touchReleased(int x, int y, int fingerId);

	virtual void pause();
	virtual void resume();
private:

};
#endif //GANDROIDCANVAS_H

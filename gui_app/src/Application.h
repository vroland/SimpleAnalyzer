/*
 * GUIApplication.h
 *
 *  Created on: 04.09.2013
 *      Author: valentin
 */

#ifndef GUIAPPLICATION_H_
#define GUIAPPLICATION_H_
#include <wx/wx.h>

class Application: public wxApp {
public:
	virtual bool OnInit();
	virtual ~Application();
};

DECLARE_APP(Application)

#endif /* GUIAPPLICATION_H_ */

/*
 * GUIApplication.h
 *
 *  Created on: 04.09.2013
 *      Author: valentin
 */

#ifndef GUIAPPLICATION_H_
#define GUIAPPLICATION_H_
#include <wx/wx.h>

/**
 * @brief Regelt den allgemeinen Ablauf des Programms.
 *
 * Eine eigene Anwendungsklasse wird von wxWidgets gefordert. Das zugrunde liegende
 * System organisiert über diese Klasse den Programmablauf (MainLoop) und Events.
 */
class Application: public wxApp {
public:
	/**
	 * Der Destruktor.
	 */
	virtual ~Application();
private:
	/**
	 * Wird beim Start der Anwendung ausgeführt und öffnet das Hauptfenster.
	 */
	virtual bool OnInit();
};

//Makro zur Deklaration der Anwendung.
DECLARE_APP(Application)

#endif /* GUIAPPLICATION_H_ */

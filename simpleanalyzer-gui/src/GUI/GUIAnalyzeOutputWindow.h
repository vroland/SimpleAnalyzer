/*
 * GUIAnalyzeOutputWindow.h
 *
 *  Created on: 12.09.2013
 *      Author: valentin
 */

#ifndef GUIANALYZEOUTPUTWINDOW_H_
#define GUIANALYZEOUTPUTWINDOW_H_

#include "PropertiesBox.h"
#include <wx/grid.h>
#include <wx/wx.h>

/**
 * @brief Übersichtsfenster über die Analysedaten.
 *
 * Dieses Fenster zeigt eine Tabelle mit den zur Analyse markierten Zeitpunkten für alle Objekte
 * und derenDatensätze und Materialen.
 * Nicht-zeitabhängige Sensordaten werden immer angezeigt.
 */
class GUIAnalyzeOutputWindow: public wxFrame {
public:

	/**
	 * Der Konstruktor.
	 */
	GUIAnalyzeOutputWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height);

	/**
	 * Methode zum aktualisieren des Fensters, alle Objekte werden erneut analysiert und die
	 * aktualisierten Ergebnisse angezeigt.
	 */
	void Update();

	/**
	 * Der Destruktor.
	 */
	virtual ~GUIAnalyzeOutputWindow();
protected:
	/**
	 * Event-Tabellendeklaration für wxWidgets.
	 */
	DECLARE_EVENT_TABLE()
private:
	/**
	 * Behandelt das Drücken von Strg+C und Strg+A.
	 */
	void OnKeyPress(wxKeyEvent& event);

	/**
	 * Selektiert alle Zellen der Tabelle.
	 */
	void SelectAll();

	/**
	 * Kopiert die Inhalte der Tabelle in die Zwischenablage.
	 * Basierend auf http://forums.wxwidgets.org/viewtopic.php?f=20&t=2200#p148731.
	 */
	void ToClipboard();

	/**
	 * Die Tabellenkomponente.
	 */
	wxGrid* table;
};

#endif /* GUIANALYZEOUTPUTWINDOW_H_ */

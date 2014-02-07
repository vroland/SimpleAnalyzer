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
private:
	/**
	 * Die Tabellenkomponente
	 */
	wxGrid* table;
};

#endif /* GUIANALYZEOUTPUTWINDOW_H_ */

/*
 * GUIAnalyzePoint.h
 *
 *  Created on: 21.09.2013
 *      Author: valentin
 */

#ifndef GUIANALYZEPOINT_H_
#define GUIANALYZEPOINT_H_

#include <wx/wx.h>
/**
 * @brief Analysefenster für einen Punkt
 *
 */
class GUIAnalyzePointWindow: public wxDialog {
public:
	/**
	 * Der Konstruktor.
	 */
	GUIAnalyzePointWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height);
	/**
	 * Der Destruktor.
	 */
	virtual ~GUIAnalyzePointWindow();
protected:
	/**
	 * Event-Tabellendeklaration für wxWidgets.
	 */
	DECLARE_EVENT_TABLE()
private:
	/**
	 * Ermittelt Temperatur und Art des Punktes (Interpoliert/Extrapoliert). Wird durch Event ausgelöst.
	 */
	void analyzePoint(wxCommandEvent &event);
	/**
	 * Beschriftung der Fensterkomponenten.
	 */
	wxStaticText* label;
	/**
	 * Eingabefeld für die X-Koordinate.
	 */
	wxTextCtrl* xedit;
	/**
	 * Eingabefeld für die Y-Koordinate.
	 */
	wxTextCtrl* yedit;
	/**
	 * Eingabefeld für die Z-Koordinate.
	 */
	wxTextCtrl* zedit;
	/**
	 * Beschriftung für den Interpolationsmodus.
	 */
	wxStaticText* interpolationModeLabel;
	/**
	 * Dropdown-Menü für den Interpolationsmodus.
	 */
	wxComboBox* interpolationModeList;
	/**
	 * Button zum Auslösen der Analyseprozedur.
	 */
	wxButton* calcbt;
};

#endif /* GUIANALYZEPOINT_H_ */

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
	DECLARE_EVENT_TABLE()
private:
	/**
	 * Ermittelt Temperatur und Art des Punktes (Interpoliert/Extrapoliert). Wird durch Event ausgelöst.
	 */
	void analyzePoint(wxCommandEvent &event);

	wxStaticText* label;
	wxTextCtrl* xedit;
	wxTextCtrl* yedit;
	wxTextCtrl* zedit;
	wxStaticText* interpolationModeLabel;
	wxComboBox* interpolationModeList;
	wxButton* calcbt;
};

#endif /* GUIANALYZEPOINT_H_ */

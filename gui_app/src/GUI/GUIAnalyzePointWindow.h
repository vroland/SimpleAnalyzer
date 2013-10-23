/*
 * GUIAnalyzePoint.h
 *
 *  Created on: 21.09.2013
 *      Author: valentin
 */

#ifndef GUIANALYZEPOINT_H_
#define GUIANALYZEPOINT_H_

#include <wx-2.8/wx/wx.h>
class GUIAnalyzePointWindow: public wxDialog {
public:
	GUIAnalyzePointWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height);
	void analyzePoint(wxCommandEvent &event);
	virtual ~GUIAnalyzePointWindow();
protected:
	DECLARE_EVENT_TABLE()
private:
	wxStaticText* label;
	wxTextCtrl* xedit;
	wxTextCtrl* yedit;
	wxTextCtrl* zedit;
	wxStaticText* interpolationmodetext;
	wxComboBox* interpolationmodelist;
	wxButton* calcbt;
};

#endif /* GUIANALYZEPOINT_H_ */

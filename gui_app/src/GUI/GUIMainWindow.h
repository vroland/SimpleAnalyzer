/*
 * GUIMainWindow.h
 *
 *  Created on: 04.09.2013
 *      Author: valentin
 */

#ifndef GUIMAINWINDOW_H_
#define GUIMAINWINDOW_H_

#include "GUIGLCanvas.h"
#include "PropertiesBox.h"
#include "ViewpropBox.h"
#include "GUIAnalyzeOutputWindow.h"
#include "GUIAnalyzePointWindow.h"
#include "GUICutRenderWindow.h"
#include <wx/wx.h>
#include <wx/artprov.h>

class GUIMainWindow: public wxFrame {
public:
	GUIMainWindow(const wxChar *title, int xpos, int ypos, int width, int height);
	void OnMenuImportObj(wxCommandEvent &event);
	void OnMenuImportSD(wxCommandEvent &event);
	void OnMenuImportTSD(wxCommandEvent &event);
	void OnMenuFileQuit(wxCommandEvent &event);
	void OnMenuHelpAbout(wxCommandEvent &event);
	void OnRecalcBtClick(wxCommandEvent &event);
	void OnResize(wxSizeEvent &event);
	void OnMaterialSelect(wxCommandEvent &event);
	void OnAnalyze(wxCommandEvent &event);
	void OnImmUpPropChange(wxCommandEvent &event);
	void OnGeneralPropChange(wxCommandEvent &event);
	void OnViewPropChange(wxCommandEvent &event);
	void OnViewPropSpinChange(wxSpinEvent &event);
	void OnSensorDataChange(wxCommandEvent &event);
	void OnSDTimelineChange(wxCommandEvent &event);
	void OnSDTLMarkerClear(wxCommandEvent &event);
	void OnSDTLNextMarker(wxCommandEvent &event);
	void OnSDTLPrevMarker(wxCommandEvent &event);
	void OnAnalyzeMarkerChange(wxCommandEvent &event);
	void OnActiveObjectChangePopup(wxCommandEvent &event);
	void OnActiveObjectChange(wxCommandEvent &event);
	void OnActiveObjectDelete(wxCommandEvent&event);
	void OnAnalyzePoint(wxCommandEvent &event);
	void OnRenderCut(wxCommandEvent &event);
	void addObject(ObjectData* obj);
	void setActiveObject(int index);
	void OnExportViewportImage(wxCommandEvent &event);
	void OnExportVTK(wxCommandEvent &event);
	void OnFindMaxTSD(wxCommandEvent &event);
	void OnAutoUpdateChange(wxCommandEvent &event);
	GUIGLCanvas* getGLCanvas();
	virtual ~GUIMainWindow();
	bool analyze_window_valid;
	bool render_cut_window_valid;
protected:
	DECLARE_EVENT_TABLE()
private:
	GUIGLCanvas* gl_context;
	wxToolBar* toolbar;
	wxMenuBar* mwMenuBar;
	wxMenu* mwFileMenu;
	wxMenu* mwHelpMenu;
	wxMenu* mwImportMenu;
	wxMenu* mwExportMenu;
	wxMenu* mwAnalyzeMenu;
	wxMenu* mwEditMenu;
	PropertiesBox *propbox;
	ViewpropBox* viewbox;
	GUIAnalyzeOutputWindow* analyzerframe;
	GUICutRenderWindow* rendercutwindow;
	wxScrolledWindow* prop_scroll_win;
	wxScrolledWindow* view_scroll_win;
	bool updating;
	void assignCurrentObjectProps();
	void updateObjectPropGUI();
	void assignViewProps();
	void updateViewPropGUI();
};

#endif /* GUIMAINWINDOW_H_ */

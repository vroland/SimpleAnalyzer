/*
 * GUICutRenderWindow.h
 *
 *  Created on: 01.10.2013
 *      Author: valentin
 */

#ifndef GUICUTRENDERWINDOW_H_
#define GUICUTRENDERWINDOW_H_
#include <wx-2.8/wx/wx.h>
#include "GUIRenderCutCanvas.h"
#include "../libraries/interpolate/GeometryClasses.h"
#include "Renderer.h"
class GUICutRenderWindow: public wxFrame {
public:
	GUICutRenderWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height);
	void renderCutBtClick(wxCommandEvent &event);
	void OnResize(wxSizeEvent &event);
	void OnCutPropsChanged(wxCommandEvent &event);
	void refreshVisualisation();
	void exportImage(wxCommandEvent &event);
	CutRender_info* getCutRenderProperties();
	virtual ~GUICutRenderWindow();

	wxTextCtrl* p1xedit;
	wxTextCtrl* p1yedit;
	wxTextCtrl* p1zedit;
	wxTextCtrl* p2xedit;
	wxTextCtrl* p2yedit;
	wxTextCtrl* p2zedit;
	wxTextCtrl* p3xedit;
	wxTextCtrl* p3yedit;
	wxTextCtrl* p3zedit;
	wxTextCtrl* mmperpixeledit;
protected:
	DECLARE_EVENT_TABLE();
private:
	wxStaticText* p1label;
	wxStaticText* p2label;
	wxStaticText* p3label;
	wxStaticText* mmperpixellabel;
	wxStaticText* trilabel;
	wxButton* calcbt;
	wxButton* exportbt;
	GUIRenderCutCanvas* canvas;
};
#endif /* GUICUTRENDERWINDOW_H_ */

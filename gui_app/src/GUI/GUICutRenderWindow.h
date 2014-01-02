/*
 * GUICutRenderWindow.h
 *
 *  Created on: 01.10.2013
 *      Author: valentin
 */

#ifndef GUICUTRENDERWINDOW_H_
#define GUICUTRENDERWINDOW_H_
#include <wx/wx.h>
#include "GUIRenderCutCanvas.h"
#include "../libraries/interpolate/GeometryClasses.h"
#include "../processing/utils.h"
#include <wx/spinctrl.h>
#include "Renderer.h"
#include <wx/colordlg.h>
using namespace Utils;

class GUICutRenderWindow: public wxFrame {
public:
	GUICutRenderWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height);
	void renderCutBtClick(wxCommandEvent &event);
	void OnResize(wxSizeEvent &event);
	void OnCutPropsChanged(wxCommandEvent &event);
	void refreshVisualisation();
	void OnExportImage(wxCommandEvent &event);
	void OnExportCSV(wxCommandEvent &event);
	void OnSCutPropsChanged_spin(wxSpinEvent &event);
	void OnColorScaleChanged(wxCommandEvent &event);
	void OnColorScaleChanged_spin(wxSpinEvent &event);
	void OnCSColorBtClick(wxCommandEvent &event);
	CutRender_info* getCutRenderProperties();
	virtual ~GUICutRenderWindow();
protected:
	DECLARE_EVENT_TABLE();
private:
	//void render_thread(wxImage* image,float* value_img,int width,int height,int startheight,int delta_h,CutRender_info* info,Viewport_info* vis_info,Vector3D* xvec,Vector3D* yvec,Vector3D* v0,vector<tetgenio*>* bases,ObjectData* obj);
	void renderImage(wxImage* image);
	wxScrolledWindow* scroll_pane;
	wxTextCtrl* p1xedit;
	wxTextCtrl* p1yedit;
	wxTextCtrl* p1zedit;
	wxTextCtrl* p2xedit;
	wxTextCtrl* p2yedit;
	wxTextCtrl* p2zedit;
	wxTextCtrl* p3xedit;
	wxTextCtrl* p3yedit;
	wxTextCtrl* p3zedit;
	wxSpinCtrl* imgWidthEdit;
	wxSpinCtrl* imgHeightEdit;
	wxSpinCtrl* threadcountedit;
	wxTextCtrl* mmperpixeledit;
	wxStaticText* p1label;
	wxStaticText* p2label;
	wxStaticText* p3label;
	wxStaticText* mmperpixellabel;
	wxStaticText* trilabel;
	wxStaticText* optionslbl;
	wxStaticText* whlbl;
	wxStaticText* threadcountlbl;
	wxStaticText* scalelbl;
	wxStaticText* scalemodelbl;
	wxComboBox* scalemodecb;
	wxStaticText* scalefontpropslbl;
	wxSpinCtrl* scalefontsizeedit;
	wxButton* scalefontcolorbt;
	wxSpinCtrl* scalestepedit;
	wxButton* calcbt;
	wxButton* export_img_bt;
	wxButton* export_csv_bt;
	GUIRenderCutCanvas* canvas;
	wxImage* image;
	float *value_img;
	int core_count;
};
#endif /* GUICUTRENDERWINDOW_H_ */

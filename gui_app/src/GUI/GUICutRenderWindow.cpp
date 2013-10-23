/*
 * GUICutRenderWindow.cpp
 *
 *  Created on: 01.10.2013
 *      Author: valentin
 */

#include "GUICutRenderWindow.h"
#include "constants.h"
#include <vector>
#include "../processing/Analyzer.h"
#include "../processing/utils.h"
#include "../libraries/interpolate/Interpolator.h"
#include "GUIMainWindow.h"
BEGIN_EVENT_TABLE(GUICutRenderWindow, wxFrame)
	EVT_BUTTON(ID_RENDER_CUT_BT, GUICutRenderWindow::renderCutBtClick)
	EVT_TEXT(ID_CUT_TRI_EDIT, GUICutRenderWindow::OnCutPropsChanged)
	EVT_SIZE(GUICutRenderWindow::OnResize)
END_EVENT_TABLE()

GUICutRenderWindow::GUICutRenderWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height):
	wxFrame(parent, -1, title, wxPoint(xpos, ypos), wxSize(width, height), wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT) {
	trilabel= new wxStaticText(this,wxID_ANY,wxT("Dreiecksebene (Punkt1 ist Mittelpunkt):"));
	p1label = new wxStaticText(this,wxID_ANY,wxT("Punkt 1:"));
	p1xedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));
	p1yedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));
	p1zedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));

	p2label = new wxStaticText(this,wxID_ANY,wxT("Punkt 2:"));
	p2xedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("1.0"));
	p2yedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));
	p2zedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));

	p3label = new wxStaticText(this,wxID_ANY,wxT("Punkt 3:"));
	p3xedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));
	p3yedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("1.0"));
	p3zedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));

	mmperpixellabel = new wxStaticText(this,wxID_ANY,wxT("Maßstab (mm/px):"));
	mmperpixeledit  = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("15"));

	calcbt= new wxButton(this,ID_RENDER_CUT_BT,wxT("analysieren"));
	canvas = new GUIRenderCutCanvas(this);
	canvas->recalculate_img = 0;
	p1xedit->SetValue(p1xedit->GetValue()); //TEXT-Event auslösen
	Update();
}
void GUICutRenderWindow::OnCutPropsChanged(wxCommandEvent &event) {
	refreshVisualisation();
}
void GUICutRenderWindow::refreshVisualisation() {
	GUIMainWindow* parent = (GUIMainWindow*) GetParent();
	parent->getGLCanvas()->renderer.setCutRenderInfo(this->getCutRenderProperties());
	parent->getGLCanvas()->Refresh(false,NULL);
}
void GUICutRenderWindow::OnResize(wxSizeEvent &event) {
	int width = 0;
	int height = 0;
	GetSize(&width,&height);
	trilabel->SetSize(10,10,300,20);
	p1label->SetSize(20,30,300,20);
	p1xedit->SetSize(90,30,100,20);
	p1yedit->SetSize(190,30,100,20);
	p1zedit->SetSize(290,30,100,20);
	p2label->SetSize(20,60,300,20);
	p2xedit->SetSize(90,60,100,20);
	p2yedit->SetSize(190,60,100,20);
	p2zedit->SetSize(290,60,100,20);
	p3label->SetSize(20,90,300,20);
	p3xedit->SetSize(90,90,100,20);
	p3yedit->SetSize(190,90,100,20);
	p3zedit->SetSize(290,90,100,20);
	mmperpixellabel->SetSize(410,30,200,40);
	mmperpixeledit->SetSize(410,60,100,20);
	calcbt->SetSize(410,90,100,20);
	canvas->SetSize(10,120,width-20,height-canvas->GetPosition().y-10);
	refreshVisualisation();
	canvas->Refresh(false,NULL);

}
CutRender_info* GUICutRenderWindow::getCutRenderProperties() {
	CutRender_info* info = new CutRender_info();
	info->tri = new Triangle(new Vector3D(atof(p1xedit->GetValue().ToAscii()),atof(p1yedit->GetValue().ToAscii()),atof(p1zedit->GetValue().ToAscii())),
				new Vector3D(atof(p2xedit->GetValue().ToAscii()),atof(p2yedit->GetValue().ToAscii()),atof(p2zedit->GetValue().ToAscii())),
				new Vector3D(atof(p3xedit->GetValue().ToAscii()),atof(p3yedit->GetValue().ToAscii()),atof(p3zedit->GetValue().ToAscii())));
	info->mmperpixel = atof(mmperpixeledit->GetValue().ToAscii());
	info->img_width = canvas->GetSize().x;
	info->img_height= canvas->GetSize().y;
	return info;
}
void GUICutRenderWindow::renderCutBtClick(wxCommandEvent &event) {
	canvas->recalculate_img = 1;
	canvas->Refresh(false,NULL);
}
GUICutRenderWindow::~GUICutRenderWindow() {
	GUIMainWindow* parent = (GUIMainWindow*) GetParent();
	if (parent!=NULL) {
		parent->render_cut_window_valid = false;
		parent->getGLCanvas()->renderer.setCutRenderInfo(NULL);
		parent->getGLCanvas()->Refresh(false,NULL);
	} else {
		cout << "parent is null!" << endl;
	}
}


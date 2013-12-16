/*
 * GUIRenderCutCanvas.cpp
 *
 *  Created on: 03.10.2013
 *      Author: valentin
 */

#include "GUIRenderCutCanvas.h"
#include "GUICutRenderWindow.h"
#include <vector>
#include "../processing/utils.h"
#include "../processing/ObjectData.h"
#include "GUIMainWindow.h"
#include "Renderer.h"
#include <iostream>
BEGIN_EVENT_TABLE(GUIRenderCutCanvas, wxPanel)
	EVT_PAINT    (GUIRenderCutCanvas::onCanvasPaint)
	EVT_MOUSEWHEEL(GUIRenderCutCanvas::OnMouseWheel)
	EVT_MOTION   (GUIRenderCutCanvas::OnMouseMove)
	EVT_SIZE   (GUIRenderCutCanvas::OnResize)
	EVT_LEFT_DOWN(GUIRenderCutCanvas::OnMouseDown)
	EVT_KEY_DOWN(GUIRenderCutCanvas::OnKeyDown)
END_EVENT_TABLE()
using namespace std;
using namespace Utils;

GUIRenderCutCanvas::GUIRenderCutCanvas(wxWindow* parent):wxPanel(parent, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas")) {
	zoom = 1;
	deltaX = 0;
	deltaY = 0;
	original_x = 0;
	original_y = 0;
	current_mx = 0;
	current_my = 0;
	image = NULL;
	value_img = NULL;
}
void GUIRenderCutCanvas::setImage(wxImage* img) {
	image = img;
}
void GUIRenderCutCanvas::setValueImg(float* img) {
	value_img = img;
}
void GUIRenderCutCanvas::OnMouseWheel(wxMouseEvent &event) {
	if (zoom-event.m_wheelRotation/1000.>0) {
		zoom+=-event.m_wheelRotation/1000.;
	}
	cout << zoom << endl;
	Refresh();
}
void GUIRenderCutCanvas::OnMouseMove(wxMouseEvent &event) {
	if (event.m_leftDown) {
		deltaX+=(event.m_x-original_x);
		deltaY+=(event.m_y-original_y);
		original_x = event.m_x;
		original_y = event.m_y;
	}
	current_mx = event.m_x;
	current_my = event.m_y;
	Refresh();
}
void GUIRenderCutCanvas::OnResize(wxSizeEvent &event) {

}
void GUIRenderCutCanvas::OnMouseDown(wxMouseEvent &event) {
	original_x = event.m_x;
	original_y = event.m_y;
}
void GUIRenderCutCanvas::OnKeyDown(wxKeyEvent &event) {

}
void GUIRenderCutCanvas::onCanvasPaint(wxPaintEvent &event) {
	 // Create paint DC
	wxPaintDC dc(this);
	// Create graphics context from it
	//wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	int width = 0;
	int height = 0;
	int imgwidth = 0;
	int imgheight = 0;
	if (image!=NULL) {
		imgwidth = image->GetWidth();
		imgheight = image->GetHeight();
	}
	GetSize(&width,&height);
	int zwidth = int(imgwidth*zoom);
	int zheight = int(imgheight*zoom);
	// Clear BG
	dc.SetPen( *wxGREY_PEN );
	dc.SetBrush( *wxGREY_BRUSH);
	dc.DrawRectangle(0, 0, width, height);
	int imgx = width/2+deltaX-zwidth/2;
	int imgy = height/2+deltaY-zheight/2;
	if (image!=NULL) {
		wxBitmap drawbmp(*image);
		drawbmp = drawbmp.Rescale(0,0,2147483647,2147483647,zwidth,zheight);
		dc.DrawBitmap(drawbmp,imgx,imgy);
		dc.SetPen(*wxBLACK);
		int bx = width/2+deltaX-zwidth/2;
		int by = height/2+deltaY-zheight/2;
		dc.DrawLine(bx,by,bx+zwidth,by);
		dc.DrawLine(bx,by,bx,by+zheight);
		dc.DrawLine(bx,by+zheight,bx+zwidth,by+zheight);
		dc.DrawLine(bx+zwidth,by,bx+zwidth,by+zheight);
	}
	wxImage statusimg = wxImage(width,30,true);		//temporäres bild für Statusleiste
	statusimg.InitAlpha();
	for (int i=0;i<30*width;i++) {
		statusimg.SetAlpha(i%width,i/width,150);
	}
	wxBitmap statusbmp(statusimg);
	dc.DrawBitmap(statusbmp,0,height-30);
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	if (current_mx>=imgx && current_mx<imgx+zwidth && current_my>=imgy && current_my<imgy+zheight) {
		int x = int((current_mx-imgx)/zoom);
		int y = int((current_my-imgy)/zoom);
		float value = 0;
		if (value_img!=NULL) {
			value = value_img[y*imgwidth+x];
		}
		value = int(value*100.)/100.;
		dc.DrawText(wxT("X:")+floattowxstr(x)+wxT(" Y: ")+floattowxstr(y)+wxT(" Wert: ")+floattowxstr(value)+wxT("°C"),20,height-25);
	}
}
GUIRenderCutCanvas::~GUIRenderCutCanvas() {
}


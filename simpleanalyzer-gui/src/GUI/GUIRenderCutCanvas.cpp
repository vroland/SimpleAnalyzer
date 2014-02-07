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
END_EVENT_TABLE()
using namespace std;
using namespace Utils;

GUIRenderCutCanvas::GUIRenderCutCanvas(wxWindow* parent):wxPanel(parent, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas")) {
	zoom = 1;
	deltaX = 0;
	deltaY = 0;
	current_mx = 0;
	current_my = 0;
	image = NULL;
	value_img = NULL;
	scalepanel = new GUIColorScalePanel();
	mouse_to_scalepanel = false;
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
	Refresh();
}
void GUIRenderCutCanvas::OnMouseMove(wxMouseEvent &event) {
	int width = 0;
	int height = 0;
	GetSize(&width,&height);
	wxPoint img_point(width/2+deltaX-int(image->GetWidth()*zoom)/2,
					  height/2+deltaY-int(image->GetHeight()*zoom)/2);
	wxPoint img_dim(image->GetWidth(),image->GetHeight());
	if (mouse_to_scalepanel) {
		scalepanel->handleMouse(event,img_point,img_dim,zoom);
	} else {
		scalepanel->fitBounds(img_dim,false);
		if (event.m_leftDown) {
			deltaX+=(event.m_x-current_mx);
			deltaY+=(event.m_y-current_my);
			//original_x = event.m_x;
			//original_y = event.m_y;
		}
	}
	if (!event.m_leftDown && mouse_to_scalepanel) {
		mouse_to_scalepanel = false;
	}
	current_mx = event.m_x;
	current_my = event.m_y;
	Refresh();
}
void GUIRenderCutCanvas::OnResize(wxSizeEvent &event) {

}
void GUIRenderCutCanvas::OnMouseDown(wxMouseEvent &event) {
	wxRect panel_rect;
	int width = 0;
	int height = 0;
	GetSize(&width,&height);
	wxPoint img_point(width/2+deltaX-int(image->GetWidth()*zoom)/2,
					  height/2+deltaY-int(image->GetHeight()*zoom)/2);
	wxPoint img_dim(image->GetWidth(),image->GetHeight());
	wxPoint mouse_pos = event.GetPosition();
	if (scalepanel->mouseOnDisplayArea(img_point,zoom,mouse_pos)) {
		scalepanel->handleMouse(event,img_point,img_dim,zoom);
		mouse_to_scalepanel = true;
	}
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
		wxImage drawimg = image->Copy();
		drawimg.Rescale(zwidth,zheight,wxIMAGE_QUALITY_NORMAL);
		dc.DrawBitmap(wxBitmap(drawimg),imgx,imgy);
		dc.SetPen(*wxBLACK);
		int bx = width/2+deltaX-zwidth/2;
		int by = height/2+deltaY-zheight/2;
		dc.DrawLine(bx,by,bx+zwidth,by);
		dc.DrawLine(bx,by,bx,by+zheight);
		dc.DrawLine(bx,by+zheight,bx+zwidth,by+zheight);
		dc.DrawLine(bx+zwidth,by,bx+zwidth,by+zheight);
	}
	wxBitmap scalepanelbmp(*scalepanel->getImage());
	scalepanelbmp = scalepanelbmp.Rescale(0,0,10000000,1000000,zwidth,zheight);
	dc.DrawBitmap(scalepanelbmp,imgx,imgy);
	wxPoint img_pos(imgx,imgy);
	scalepanel->paintTo(dc,zoom,img_pos);

	wxImage statusimg = wxImage(width,30,true);		//temporäres bild für Statusleiste
	statusimg.InitAlpha();
	for (int i=0;i<30*width;i++) {
		statusimg.SetAlpha(i%width,i/width,150);
	}
	wxBitmap statusbmp(statusimg);
	dc.DrawBitmap(statusbmp,0,height-30);
	dc.SetTextForeground(wxColour(255,255,255));
	if (current_mx>=imgx && current_mx<imgx+zwidth && current_my>=imgy && current_my<imgy+zheight) {
		int x = int((current_mx-imgx)/zoom);
		int y = int((current_my-imgy)/zoom);
		float value = 0;
		if (value_img!=NULL) {
			value = value_img[y*imgwidth+x];
		}
		wxString val_str = floattowxstr(value)+wxT("°C");
		if (abs(value+300)<.0001) {
			val_str = wxT(" - ");
		}
		value = int(value*100.)/100.;
		dc.DrawText(wxT("X:")+floattowxstr(x)+wxT(" Y: ")+floattowxstr(y)+wxT(" Wert: ")+val_str,20,height-25);
	}
}
GUIColorScalePanel* GUIRenderCutCanvas::getScalePanel() {
	return scalepanel;
}
GUIRenderCutCanvas::~GUIRenderCutCanvas() {
	delete scalepanel;
}


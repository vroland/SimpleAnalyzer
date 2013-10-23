/*
 * GUITimeline.cpp
 *
 *  Created on: 22.10.2013
 *      Author: valentin
 */

#include "GUITimeline.h"
#include <iostream>
#include "../processing/utils.h"
#include <cmath>
using namespace std;
using namespace Utils;

BEGIN_EVENT_TABLE(GUITimeline, wxPanel)
    EVT_PAINT    (GUITimeline::OnPaint)
    EVT_MOUSEWHEEL(GUITimeline::OnMouseWheel)
    EVT_MOTION   (GUITimeline::OnMouseMove)
    EVT_SIZE   (GUITimeline::OnResize)
    EVT_LEFT_DOWN(GUITimeline::OnMouseDown)
END_EVENT_TABLE()

GUITimeline::GUITimeline(wxWindow *parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name):wxPanel(parent, id,  pos, size, style, name)
{

	minvalue = 0;
	maxvalue = 100;
	value = minvalue;
	zoom = 1;
	maxdigits = 1;
	delta_v_view = 0;
	prev_mouse_x = -10000;
	cout << "timeline init..." << endl;
}
void GUITimeline::OnMouseWheel(wxMouseEvent &event) {
	float delta = -event.m_wheelRotation/1000.;
	zoom*=exp(delta);
	if (zoom<1./exp10(int(log10(maxvalue*zoom))+1)) {	//Minimal 1-er Schritte
		zoom = 1./exp10(int(log10(maxvalue*zoom)+1));
	}
	if (zoom>2) {
		zoom = 2.;
	}
	Refresh(false,NULL);
}
void GUITimeline::OnMouseDown(wxMouseEvent &event) {
	posToVal(event.m_x);
}
int GUITimeline::calcStepWidth() {
	int to_ten = int(exp10(int(log10(maxvalue*zoom))));
	int part = int((maxvalue*zoom)/to_ten);
	float refine_factor = 1;
	if (part<=5) {
		refine_factor = 1/2.;
	}
	if (part<=2) {
		refine_factor = 1/5.;
	}
	if (part==1) {
		refine_factor = 1/10.;
	}
	return to_ten*refine_factor;
}
void GUITimeline::posToVal(int mouse_x) {
	int width = 0;
	int height = 0;
	GetSize(&width,&height);
	float pixelsperstep = (float)width/((maxvalue-minvalue)*zoom);
	float stepwidth = calcStepWidth();
	int viewstart = int(delta_v_view*1./pixelsperstep*width/stepwidth);
	value = int((mouse_x-viewstart)/pixelsperstep);
	if (value<minvalue) {
		value = minvalue;
	}
	if (value>maxvalue) {
		value = maxvalue;
	}
	Refresh(false,NULL);
}
void GUITimeline::OnMouseMove(wxMouseEvent &event) {	// Ermöglicht ziehen
	if (event.m_leftDown) {
		posToVal(event.m_x);
	}
	if (event.m_middleDown) {
		if (prev_mouse_x>-10000) {
			delta_v_view+=(event.m_x-prev_mouse_x)*.1;
		}
		prev_mouse_x = event.m_x;
		Refresh(false,NULL);
	} else {
		prev_mouse_x = -10000;
	}
}
void GUITimeline::OnResize(wxSizeEvent &event) {
	Refresh(false,NULL);
}
void GUITimeline::OnPaint(wxPaintEvent&) {
	int w = 0;
	int h = 0;
	GetSize(&w,&h);
	wxPaintDC dc(this);
	// Create graphics context from it
	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	int width = 0;
	int height = 0;
	int captionheight = 20;
	GetSize(&width,&height);
	if (gc)
	{
		cout << "draw! delta_view: "<<delta_v_view<< " zoom: "<<zoom << endl;
		float pixelsperstep = (float)width/((maxvalue-minvalue)*zoom);
		float stepwidth = calcStepWidth();
		cout << stepwidth << endl;
		int viewstart = int(delta_v_view*1./pixelsperstep*width/stepwidth);
		int start_index = -int(viewstart/pixelsperstep/stepwidth);
		// Clear BG
		gc->SetPen( wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND) );
		gc->SetBrush( wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND) );
		gc->DrawRectangle(0, 0, width, height);
		gc->SetBrush(*wxTRANSPARENT_BRUSH);
		// Border
		gc->SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT),1,wxSOLID));
		gc->DrawRectangle(0, 0, width, height);

		// Unavailable areas
		gc->SetPen( *wxTRANSPARENT_PEN);
		gc->SetBrush( wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT),wxSOLID));
		cout << -viewstart << " " <<minvalue*pixelsperstep<<endl;
		if (-viewstart<minvalue*pixelsperstep) {
			gc->DrawRectangle(0,1,viewstart+minvalue*pixelsperstep,height);
		}
		if (viewstart+maxvalue*pixelsperstep-width<0) {
			int x = viewstart+maxvalue*pixelsperstep;
			if (x<0) x = 0;
			gc->DrawRectangle(x,1,width,height);
		}
		//Label bg
		gc->SetBrush( wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW),wxSOLID));
		gc->SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE),1,wxSOLID));
		gc->DrawRectangle(0,height-captionheight,width,height);

		dc.SetBackground(*wxTRANSPARENT_BRUSH);
		dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
		wxGraphicsPath linepath = gc->CreatePath();
		for (int i=start_index;i<=start_index+1./pixelsperstep*width/stepwidth;i++) {
			float x = viewstart+i*stepwidth*pixelsperstep;
			linepath.MoveToPoint(x,0);
			linepath.AddLineToPoint(x,height-captionheight);

			dc.DrawText(floattowxstr(i*stepwidth,maxdigits),x,height-captionheight);
		}
		gc->SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT),1,wxSOLID));
		gc->StrokePath(linepath);
		gc->SetPen( *wxGREEN_PEN);
		gc->StrokeLine(viewstart+(float)value*pixelsperstep,0,viewstart+(float)value*pixelsperstep,height-captionheight);
		dc.SetTextForeground(*wxGREEN);
		dc.DrawText(floattowxstr(value),viewstart+(float)value*pixelsperstep,height/2);
		delete gc;
	}
}
int GUITimeline::getValue() {
	return value;
}
int GUITimeline::getMaxValue() {
	return maxvalue;
}
int GUITimeline::getMinValue() {
	return minvalue;
}
void GUITimeline::setValue(int val) {
	value = val;
	if (value<minvalue) {
		value = minvalue;
		cerr << "Timeline value must not be smaller than the minimum value!"<<cout;
	}
	if (value>maxvalue) {
		value = minvalue;
		cerr << "Timeline value must not be smaller than the maximum value!"<<cout;
	}
}
void GUITimeline::setMaxValue(int val) {
	maxvalue = val;
}
void GUITimeline::setMinValue(int val) {
	minvalue = val;
}

GUITimeline::~GUITimeline() {

}

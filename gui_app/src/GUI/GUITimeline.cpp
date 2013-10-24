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

DEFINE_EVENT_TYPE(wxEVT_TIMELINE)

BEGIN_EVENT_TABLE(GUITimeline, wxPanel)
    EVT_PAINT    (GUITimeline::OnPaint)
    EVT_MOUSEWHEEL(GUITimeline::OnMouseWheel)
    EVT_MOTION   (GUITimeline::OnMouseMove)
    EVT_SIZE   (GUITimeline::OnResize)
    EVT_LEFT_DOWN(GUITimeline::OnMouseDown)
    EVT_KEY_DOWN(GUITimeline::OnKeyDown)
END_EVENT_TABLE()

#define REFINE_STEPS 3
const float refine_factors[REFINE_STEPS] = {.5,.2,.1};
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
	names = NULL;
}
void GUITimeline::sendTimelineEvent() {
	wxCommandEvent event( wxEVT_TIMELINE, GetId() );
	event.SetEventObject( this );
	GetEventHandler()->ProcessEvent( event );
}
void GUITimeline::OnMouseWheel(wxMouseEvent &event) {
	SetFocus();
	float delta = -event.m_wheelRotation/1000.;
	zoom*=exp(delta);

	if (zoom<.000001) {	//Minimal 1-er Schritte
		zoom = .000001;

	}
	if (zoom>2) {
		zoom = 2.;
	}
	Refresh(false,NULL);
}
void GUITimeline::OnMouseDown(wxMouseEvent &event) {
	SetFocus();
	posToVal(event.m_x);
}
void GUITimeline::OnKeyDown(wxKeyEvent &event) {
	if (event.m_keyCode==WXK_LEFT) {
		value--;
		if (value<minvalue) {
			value = minvalue;
		}
		sendTimelineEvent();
	}
	if (event.m_keyCode==WXK_RIGHT) {
		value++;
		if (value>maxvalue) {
			value = maxvalue;
		}
		sendTimelineEvent();
	}
	Refresh(false,NULL);
}
int GUITimeline::calcStepWidth() {
	int to_ten = int(exp10(int(log10(maxvalue*zoom))));
	float refine_factor = 1;
	wxPaintDC dc(this);
	float pixelsperstep = (float)GetSize().x/((maxvalue-minvalue)*zoom);
	for (int i=0;i<REFINE_STEPS;i++) {		// go until Text is unreadable
		int stepwidth = to_ten*refine_factors[i];
		int viewstart = int(delta_v_view/zoom);
		int start_index = -int(viewstart/pixelsperstep/stepwidth);

		int curr_max = start_index+1./pixelsperstep*GetSize().x/stepwidth;
		curr_max = curr_max*stepwidth;

		wxString str = floattowxstr(curr_max);
		int text_extend = dc.GetTextExtent(str).x;
		if (stepwidth*pixelsperstep>text_extend+2) {
			refine_factor = refine_factors[i];
		} else {
			break;
		}
	}

	return to_ten*refine_factor;
}
void GUITimeline::posToVal(int mouse_x) {
	int width = 0;
	int height = 0;
	GetSize(&width,&height);
	float pixelsperstep = (float)width/((maxvalue-minvalue)*zoom);
	int viewstart = int(delta_v_view/zoom);
	value = int((mouse_x-viewstart)/pixelsperstep);
	if (value<minvalue) {
		value = minvalue;
	}
	if (value>maxvalue) {
		value = maxvalue;
	}
	sendTimelineEvent();
	Refresh(false,NULL);
}
void GUITimeline::OnMouseMove(wxMouseEvent &event) {	// Ermöglicht ziehen
	SetFocus();
	if (event.m_leftDown) {
		posToVal(event.m_x);
	}
	if (event.m_middleDown) {
		if (prev_mouse_x>-10000) {
			delta_v_view+=(event.m_x-prev_mouse_x);
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
	SetDoubleBuffered(true);
	// Create graphics context from it
	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	int width = 0;
	int height = 0;
	int captionheight = 20;
	GetSize(&width,&height);
	if (gc)
	{
		float pixelsperstep = (float)width/((maxvalue-minvalue)*zoom);
		float stepwidth = calcStepWidth();
		int viewstart = int(delta_v_view/zoom);
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
		if (names==NULL) {
			dc.DrawText(floattowxstr(value),viewstart+(float)value*pixelsperstep,height/2);
		} else {
			if (int(names->size())<(maxvalue-minvalue)) {
				cerr << "name vector has wrong size!" << endl;
			} else {
				wxString str = wxString::FromAscii((names->at(value-minvalue)).c_str())+wxT(" (")+floattowxstr(value)+wxT(")");
				int text_width = dc.GetTextExtent(str).x;
				int text_x = viewstart+(float)value*pixelsperstep;
				if (text_x+text_width>width) {
					text_x -= text_width+10;
				}
				dc.DrawText(str,text_x,height/2);
			}
		}
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
void GUITimeline::setNameList(vector<string>* namelist) {
	names = namelist;
}
GUITimeline::~GUITimeline() {

}

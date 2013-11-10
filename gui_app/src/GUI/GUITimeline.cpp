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
#include <algorithm>
using namespace std;
using namespace Utils;

const wxEventType wxEVT_TIMELINE_CHANGE = wxNewEventType();

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
	markers = NULL;
}
void GUITimeline::sendTimelineEvent() {
	wxCommandEvent event( wxEVT_TIMELINE_CHANGE, GetId() );
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
			delta_v_view+=(event.m_x-prev_mouse_x)*zoom;
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
	//wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	int width = 0;
	int height = 0;
	int captionheight = 20;
	GetSize(&width,&height);

	float pixelsperstep = (float)width/((maxvalue-minvalue)*zoom);
	float stepwidth = calcStepWidth();
	int viewstart = int(delta_v_view/zoom);
	int start_index = -int(viewstart/pixelsperstep/stepwidth);
	// Clear BG
	dc.SetPen( wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND) );
	dc.SetBrush( wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND) );
	dc.DrawRectangle(0, 0, width, height);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	// Border
	dc.SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT),1,wxSOLID));
	dc.DrawRectangle(0, 0, width, height);

	// Unavailable areas
	dc.SetPen( *wxTRANSPARENT_PEN);
	dc.SetBrush( wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT),wxSOLID));
	if (-viewstart<minvalue*pixelsperstep) {
		dc.DrawRectangle(0,1,viewstart+minvalue*pixelsperstep,height);
	}
	if (viewstart+maxvalue*pixelsperstep-width<0) {
		int x = viewstart+maxvalue*pixelsperstep;
		if (x<0) x = 0;
		dc.DrawRectangle(x,1,width,height);
	}
	//Label bg
	dc.SetBrush( wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW),wxSOLID));
	dc.SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE),1,wxSOLID));
	dc.DrawRectangle(0,height-captionheight,width,height);

	dc.SetBackground(*wxTRANSPARENT_BRUSH);
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	dc.SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT),1,wxSOLID));
	for (int i=start_index;i<=start_index+1./pixelsperstep*width/stepwidth;i++) {
		float x = viewstart+i*stepwidth*pixelsperstep;
		dc.DrawLine(x,0,x,height-captionheight);
		dc.DrawText(floattowxstr(i*stepwidth,maxdigits),x,height-captionheight);
	}
	dc.SetPen( *wxGREEN_PEN);
	dc.DrawLine(viewstart+(float)value*pixelsperstep,0,viewstart+(float)value*pixelsperstep,height-captionheight);
	// Markers
	if (markers!=NULL) {
		dc.SetPen( wxColour (255,100,0,255));
		for (size_t i=0;i<markers->size();i++) {
			dc.DrawLine(viewstart+(minvalue+markers->at(i))*pixelsperstep,0,viewstart+(minvalue+markers->at(i))*pixelsperstep,height-captionheight);
		}
	} else {
		cerr << "marker list should not be NULL!" << endl;
	}
	dc.SetTextForeground(*wxGREEN);
	if (names==NULL) {
		dc.DrawText(floattowxstr(value),viewstart+(float)value*pixelsperstep,10);
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
			dc.DrawText(str,text_x,10);
		}
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
	sendTimelineEvent();
	Refresh(true,NULL);
}
void GUITimeline::setMaxValue(int val) {
	maxvalue = val;
}
void GUITimeline::setMinValue(int val) {
	minvalue = val;
}
void GUITimeline::setNameList(vector<string>* namelist) {
	names = namelist;
	Refresh(false,NULL);
}
void GUITimeline::setMarkerList(vector<int>* mlist) {
	markers = mlist;
	Refresh(false,NULL);
}
void GUITimeline::setMarked(int pos,bool state) {
	if (markers!=NULL) {
		vector<int>::iterator p = find(markers->begin(),markers->end(),pos);
		if (p==markers->end() && state) {
			markers->resize(markers->size()+1,pos);
		}
		if (p!=markers->end() && !state)  {
			markers->erase(p);
		}
		sort(markers->begin(), markers->end());
		Refresh(false,NULL);
	} else {
		cerr << "marker list must not be NULL!" << endl;
	}
}
bool GUITimeline::isMarked(int pos) {
	if (markers!=NULL) {
		return (find(markers->begin(),markers->end(),pos)!=markers->end());
	} else {
		cerr << "marker list must not be NULL!" << endl;
		return false;
	}
}
void GUITimeline::clearMarkers() {
	markers->clear();
	Refresh(false,NULL);
}
vector<int>* GUITimeline::getMarkers() {
	return markers;
}
void GUITimeline::setMarkers(vector<int>* mlist) {
	if (markers!=NULL) {
		markers->resize(mlist->size());
		for (size_t i=0;i<markers->size();i++) {
			if (markers->at(i)) {
				markers->at(i) = mlist->at(i);
			}
		}
		sort(markers->begin(), markers->end());
		Refresh(false,NULL);
	} else {
		cerr << "marker list must not be NULL! (list copy func)" << endl;
	}
}
GUITimeline::~GUITimeline() {

}

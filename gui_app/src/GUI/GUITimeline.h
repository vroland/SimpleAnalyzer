/*
 * Timeline.h
 *
 *  Created on: 22.10.2013
 *      Author: valentin
 */

#ifndef TIMELINE_H_
#define TIMELINE_H_

#include <wx-2.8/wx/wx.h>

enum GUI_TIMELINE_STYLE {
	GTL_DEFAULT = 0,
};
class GUITimeline: public wxPanel {
public:
	GUITimeline(wxWindow *parent,
			   wxWindowID id,
			   const wxString& title,
			   const wxPoint& pos = wxDefaultPosition,
			   const wxSize& size = wxDefaultSize,
			   long style = GTL_DEFAULT,
			   const wxString& name = wxT("Timeline"));
	int getValue();
	int getMaxValue();
	int getMinValue();
	void setValue(int val);
	void setMaxValue(int val);
	void setMinValue(int val);
	virtual ~GUITimeline();
protected:
	int value;
	int maxvalue;
	int minvalue;
	int maxdigits;
	float zoom;
	float delta_v_view;
	int prev_mouse_x;
	virtual wxSize DoGetBestSize() const {
		return wxSize(100,100);
	}
	void OnPaint(wxPaintEvent&);
	void OnMouseWheel(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);
	void OnResize(wxSizeEvent &event);
	void OnMouseDown(wxMouseEvent &event);
	void posToVal(int mouse_x);
	int calcStepWidth();
private:
	DECLARE_EVENT_TABLE()
};

#endif /* TIMELINE_H_ */

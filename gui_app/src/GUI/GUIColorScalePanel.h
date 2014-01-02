/*
 * GUIColorScalePanel.h
 *
 *  Created on: 23.12.2013
 *      Author: valentin
 */

#ifndef GUICOLORSCALEPANEL_H_
#define GUICOLORSCALEPANEL_H_
#include "Renderer.h"
#include <wx/wx.h>
enum ScaleMode {
	SCM_NONE = 0,
	SCM_HORIZONTAL,
	SCM_VERTICAL,
};
class GUIColorScalePanel {
public:
	GUIColorScalePanel();
	void refresh(int img_width,int img_height);
	void paintTo(wxDC& dc,float zoom,wxPoint& img_coords);
	void handleMouse(wxMouseEvent& event,wxPoint& img_coords,wxPoint& img_dim,float zoom);
	void fitBounds(wxPoint& img_dim,bool to_scale);
	void getDisplayArea(wxRect* rect,float zoom);
	bool mouseOnDisplayArea(wxPoint& img_coords,float zoom,wxPoint& mouse_pos);
	int scale_step;
	int font_size;
	ScaleMode mode;
	wxColour text_color;
	wxImage* scale_img;
	int getX();
	int getY();
	virtual ~GUIColorScalePanel();
private:
	int current_mx;
	int current_my;
	float x;
	float y;
	float width;
	float height;
	bool scaling;
	bool transforming;
	bool prev_mouse_down;
};

#endif /* GUICOLORSCALEPANEL_H_ */

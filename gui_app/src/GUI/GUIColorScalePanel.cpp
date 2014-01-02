/*
 * GUIColorScalePanel.cpp
 *
 *  Created on: 23.12.2013
 *      Author: valentin
 */

#include "GUIColorScalePanel.h"
#include <iostream>
#include "../processing/utils.h"
#include <wx/artprov.h>

using namespace std;
using namespace Utils;

extern Visualization_info visualization_info;

#define MIN_WIDTH 5
#define MIN_HEIGHT 5

GUIColorScalePanel::GUIColorScalePanel() {
	width = 20;
	height = 100;
	x = 0;
	y = 0;
	current_mx = 0;
	current_my = 0;
	scaling = false;
	scale_step = 10;
	text_color = wxColour(255,255,255);
	transforming = 0;
	mode = SCM_HORIZONTAL;
	scale_img = new wxImage(100,100);
	font_size = 12;
}
void GUIColorScalePanel::paintTo(wxDC& dc,float zoom,wxPoint& img_coords) {
	if (mode!=SCM_NONE) {
		wxRect display_area;
		getDisplayArea(&display_area,zoom);
		wxPoint sc_topleft(img_coords+display_area.GetBottomRight()-wxPoint(10,10));
		wxRect scalearea = wxRect(sc_topleft,img_coords+display_area.GetBottomRight());
		dc.SetBrush(*wxWHITE_BRUSH);
		dc.SetPen(*wxBLACK_PEN);
		//Color
		dc.DrawRectangle(scalearea);
		dc.DrawLine(sc_topleft.x+2,sc_topleft.y+2,sc_topleft.x+8,sc_topleft.y+8);
		dc.DrawLine(sc_topleft.x+5,sc_topleft.y+8,sc_topleft.x+8,sc_topleft.y+8);
		dc.DrawLine(sc_topleft.x+8,sc_topleft.y+5,sc_topleft.x+8,sc_topleft.y+8);
		if (transforming) {
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.SetPen(*wxWHITE_PEN);
			dc.DrawRectangle(display_area.GetTopLeft()+img_coords,display_area.GetSize());
		}
	}
}
void GUIColorScalePanel::refresh(int img_width,int img_height) {
	if (mode!=SCM_NONE) {
		wxRect display_area;
		getDisplayArea(&display_area,1.0);
		wxBitmap color_bmp = wxBitmap(img_width,img_height);
		wxBitmap alpha_bmp = wxBitmap(img_width,img_height);
		wxMemoryDC col_dc(color_bmp);
		wxMemoryDC alpha_dc(alpha_bmp);
		col_dc.SetBrush(text_color);
		col_dc.SetPen(text_color);
		alpha_dc.SetBrush(*wxBLACK_BRUSH);
		alpha_dc.SetPen(*wxBLACK_PEN);
		col_dc.DrawRectangle(0,0,img_width,img_height);
		alpha_dc.DrawRectangle(0,0,img_width,img_height);
		wxImage temp_color_img = wxImage(display_area.width,display_area.height);
		for (int i=0;i<display_area.width;i++) {
			for (int j=0;j<display_area.height;j++) {
				float* color = hsvToRgb((1.-(mode==SCM_HORIZONTAL?i/(float)display_area.width:j/(float)display_area.height))*.66666,1,1);
				temp_color_img.SetRGB(i,j,color[0]*255,color[1]*255,color[2]*255);
				delete[] color;
			}
		}

		col_dc.SetBrush(*wxWHITE_BRUSH);
		col_dc.SetPen(*wxBLACK_PEN);
		alpha_dc.SetBrush(*wxWHITE_BRUSH);
		alpha_dc.SetPen(*wxWHITE_PEN);
		alpha_dc.DrawRectangle(display_area);
		col_dc.DrawBitmap(wxBitmap(temp_color_img),display_area.x,display_area.y);

		//wxFont* font = wxFont::New(font_size*zoom, wxFONTFAMILY_DEFAULT, wxFONTFLAG_NOT_ANTIALIASED);
		wxFont font(int(font_size),wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTSTYLE_NORMAL);
		col_dc.SetFont(font);
		col_dc.SetTextForeground(text_color);
		col_dc.SetPen(wxPen(text_color));
		alpha_dc.SetFont(font);
		alpha_dc.SetTextForeground(wxColour(255,255,255));
		alpha_dc.SetPen(*wxWHITE_PEN);
		int delta_t_vis = (visualization_info.max_visualisation_temp-visualization_info.min_visualisation_temp);
		for (int i=0;i<=(delta_t_vis-delta_t_vis%scale_step)/scale_step;i++) {
			wxString text = floattowxstr(int(visualization_info.min_visualisation_temp+i*scale_step));
			wxSize t_size = col_dc.GetTextExtent(text);
			switch (mode) {
			case SCM_VERTICAL: {
				float display_step = (float)display_area.height/(delta_t_vis)*scale_step;
				wxPoint textpoint(display_area.x+display_area.width,display_area.y+i*display_step);
				col_dc.DrawText(text,textpoint.x+8,textpoint.y-t_size.y/2.);
				col_dc.DrawLine(textpoint.x,textpoint.y,textpoint.x+5,textpoint.y);
				alpha_dc.DrawText(text,textpoint.x+8,textpoint.y-t_size.y/2.);
				alpha_dc.DrawLine(textpoint.x,textpoint.y,textpoint.x+5,textpoint.y);
				break;
			}
			case SCM_HORIZONTAL: {
				float display_step = (float)display_area.width/(delta_t_vis)*scale_step;
				wxPoint textpoint(display_area.x+i*display_step,display_area.y+display_area.height);
				col_dc.DrawText(text,textpoint.x-t_size.x/2.,textpoint.y+8);
				col_dc.DrawLine(textpoint.x,textpoint.y,textpoint.x,textpoint.y+5);
				alpha_dc.DrawText(text,textpoint.x-t_size.x/2.,textpoint.y+8);
				alpha_dc.DrawLine(textpoint.x,textpoint.y,textpoint.x,textpoint.y+5);
				break;
			}
			default:
				break;
			}
		}
		col_dc.SelectObject(wxNullBitmap);
		alpha_dc.SelectObject(wxNullBitmap);
		delete scale_img;
		scale_img = new wxImage(color_bmp.GetWidth(),color_bmp.GetHeight());
		scale_img->InitAlpha();
		wxImage alpha_img = alpha_bmp.ConvertToImage();
		wxImage color_img = color_bmp.ConvertToImage();
		for (int x=0;x<scale_img->GetWidth();x++) {
			for (int y=0;y<scale_img->GetHeight();y++) {
				scale_img->SetRGB(x,y,color_img.GetRed(x,y),color_img.GetGreen(x,y),color_img.GetBlue(x,y));
				scale_img->SetAlpha(x,y,alpha_img.GetRed(x,y));
			}
		}
	} else {
		delete scale_img;
		scale_img = new wxImage(img_width,img_height);
		scale_img->InitAlpha();
		for (int i=0;i<img_width;i++) {
			for (int j=0;j<img_height;j++) {
				float* color = hsvToRgb((1.-(mode==SCM_HORIZONTAL?i/(float)img_width:j/(float)img_height))*.66666,1,1);
				scale_img->SetRGB(i,j,0,0,0);
				scale_img->SetAlpha(i,j,0);
				delete[] color;
			}
		}
	}
}
void GUIColorScalePanel::getDisplayArea(wxRect* rect,float zoom) {
	rect->x = zoom*x;
	rect->y = zoom*y;
	rect->width = zoom*width;
	rect->height= zoom*height;
}
bool GUIColorScalePanel::mouseOnDisplayArea(wxPoint& img_coords,float zoom,wxPoint& mouse_pos) {
	wxRect display_area;
	getDisplayArea(&display_area,zoom);
	display_area.x+=img_coords.x;
	display_area.y+=img_coords.y;
	wxPoint sc_topleft(display_area.GetBottomRight()-wxPoint(10,10));
	wxRect scalearea = wxRect(sc_topleft,display_area.GetBottomRight());
	return (display_area.Contains(mouse_pos) || scalearea.Contains(mouse_pos));
}
int GUIColorScalePanel::getX() {
	return x;
}
int GUIColorScalePanel::getY() {
	return y;
}
void GUIColorScalePanel::fitBounds(wxPoint& img_dim,bool to_scale) {
	bool changed = 0;
	if (x<0) {
		x = 0;
		changed = 1;
	}
	if (y<0) {
		y = 0;
		changed = 1;
	}
	if (width<MIN_WIDTH) {
		width = MIN_WIDTH;
		changed = 1;
	}
	if (height<MIN_HEIGHT) {
		height = MIN_HEIGHT;
		changed = 1;
	}
	if (width>img_dim.x) {
		width = img_dim.x;
		changed = 1;
	}
	if (height>img_dim.y) {
		height = img_dim.y;
		changed = 1;
	}
	if (to_scale && !(img_dim.x-x<MIN_WIDTH || img_dim.y-y<MIN_HEIGHT)) {	// image dimensions must not me lower than MIN_WIDTH/HEIGHT
		if (x+width>img_dim.x) {
			width = img_dim.x-x;
			changed = 1;
		}
		if (y+height>img_dim.y) {
			height = img_dim.y-y;
			changed = 1;
		}
	} else {
		if (x+width>img_dim.x) {
			x = img_dim.x-width;
			changed = 1;
		}
		if (y+height>img_dim.y) {
			y = img_dim.y-height;
			changed = 1;
		}
	}
	if (changed)
		refresh(img_dim.x,img_dim.y);
}
void GUIColorScalePanel::handleMouse(wxMouseEvent& event,wxPoint& img_coords,wxPoint& img_dim,float zoom) {
	if (mode!=SCM_NONE) {
		wxRect display_area;
		getDisplayArea(&display_area,zoom);
		display_area.x+=img_coords.x;
		display_area.y+=img_coords.y;
		if (prev_mouse_down!=event.m_leftDown && !prev_mouse_down) {
			wxPoint sc_topleft(display_area.GetBottomRight()-wxPoint(10,10));
			wxRect scalearea = wxRect(sc_topleft,display_area.GetBottomRight());
			if (scalearea.Contains(event.GetPosition())) {
				scaling = true;
			} else {
				scaling = false;
			}
		} else {
			if (event.m_leftDown) {
				if (scaling) {
					width+=(event.m_x-current_mx)/zoom;
					height+=(event.m_y-current_my)/zoom;
					fitBounds(img_dim,true);
				} else {
					x+=(event.m_x-current_mx)/zoom;
					y+=(event.m_y-current_my)/zoom;
					fitBounds(img_dim,false);
				}
			}
		}
		transforming = event.m_leftDown;
	}
	if (prev_mouse_down!=event.m_leftDown) {
		refresh(img_dim.x,img_dim.y);
	}
	prev_mouse_down = event.m_leftDown;
	current_mx = event.m_x;
	current_my = event.m_y;
}
GUIColorScalePanel::~GUIColorScalePanel() {
	// TODO Auto-generated destructor stub
}


/*
 * GUIRenderCutCanvas.h
 *
 *  Created on: 03.10.2013
 *      Author: valentin
 */

#ifndef GUIRENDERCUTCANVAS_H_
#define GUIRENDERCUTCANVAS_H_
#include <wx/wx.h>
#include "GUIColorScalePanel.h"
class GUIRenderCutCanvas: public wxPanel {
public:
	GUIRenderCutCanvas(wxWindow* parent);
	void onCanvasPaint(wxPaintEvent &event);
	void renderImage(wxImage& image);
	void OnMouseWheel(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);
	void OnResize(wxSizeEvent &event);
	void OnMouseDown(wxMouseEvent &event);
	void OnKeyDown(wxKeyEvent &event);
	void setImage(wxImage* img);
	void setValueImg(float* img);
	GUIColorScalePanel* getScalePanel();
	virtual ~GUIRenderCutCanvas();
protected:
	float zoom;
	float deltaX;
	float deltaY;
	int current_mx;
	int current_my;
	bool mouse_to_scalepanel;
	wxImage* image;
	float* value_img;
	GUIColorScalePanel* scalepanel;
	DECLARE_EVENT_TABLE()
};

#endif /* GUIRENDERCUTCANVAS_H_ */

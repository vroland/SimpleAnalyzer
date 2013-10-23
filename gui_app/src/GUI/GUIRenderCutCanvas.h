/*
 * GUIRenderCutCanvas.h
 *
 *  Created on: 03.10.2013
 *      Author: valentin
 */

#ifndef GUIRENDERCUTCANVAS_H_
#define GUIRENDERCUTCANVAS_H_
#include <wx-2.8/wx/wx.h>
class GUIRenderCutCanvas: public wxPanel {
public:
	GUIRenderCutCanvas(wxWindow* parent);
	void onCanvasPaint(wxPaintEvent &event);
	void renderImage(wxImage& image);
	bool recalculate_img;
	bool draw_grid;
	virtual ~GUIRenderCutCanvas();
protected:
	DECLARE_EVENT_TABLE()
};

#endif /* GUIRENDERCUTCANVAS_H_ */

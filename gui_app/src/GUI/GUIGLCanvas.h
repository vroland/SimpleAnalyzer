/*
 * GUIGLCanvas.h
 *
 *  Created on: 05.09.2013
 *      Author: valentin
 */

#ifndef GUIGLCANVAS_H_
#define GUIGLCANVAS_H_

#include "Renderer.h"
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "../processing/ObjectData.h"

class GUIGLCanvas: public wxGLCanvas {
public:
	GUIGLCanvas(wxFrame* parent);
	void OnPaint(wxPaintEvent& event);
	void OnMouseWheel(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);
	void setRenderObject(ObjectData* obj);
	void refreshRenderObject();
	void OnResize(wxSizeEvent &event);
	virtual ~GUIGLCanvas();
	Renderer renderer;
protected:
    DECLARE_EVENT_TABLE()
private:
    vector<tetgenio*> objects;
    bool is_initialized;
    bool do_refresh;
    int prev_mouse_x;
    int prev_mouse_y;
};

#endif /* GUIGLCANVAS_H_ */

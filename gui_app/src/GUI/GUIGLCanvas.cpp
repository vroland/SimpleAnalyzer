/*
 * GUIGLCanvas.cpp
 *
 *  Created on: 05.09.2013
 *      Author: valentin
 */

#include "GUIGLCanvas.h"
#include "../libraries/interpolate/GeometryClasses.h"
extern vector<ObjectData*> data_objects;
extern int current_data_object_index;

BEGIN_EVENT_TABLE(GUIGLCanvas, wxGLCanvas)
    EVT_PAINT    (GUIGLCanvas::OnPaint)
    EVT_MOUSEWHEEL(GUIGLCanvas::OnMouseWheel)
    EVT_MOTION   (GUIGLCanvas::OnMouseMove)
    EVT_SIZE   (GUIGLCanvas::OnResize)
END_EVENT_TABLE()
int attrib_list[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER };

GUIGLCanvas::GUIGLCanvas(wxFrame *parent) :wxGLCanvas(parent, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas"),attrib_list) {
	is_initialized = false;
	do_refresh = false;
	prev_mouse_x = -1;
	prev_mouse_y = -1;
}
void GUIGLCanvas::OnResize(wxSizeEvent &event) {
	do_refresh = true;
	Refresh(false,NULL);

}
void GUIGLCanvas::OnMouseWheel(wxMouseEvent &event) {
	renderer.viewport.zoom -=event.m_wheelRotation/360.;
	if (renderer.viewport.zoom<0) {
		renderer.viewport.zoom = 0;
	}
	Refresh(false,NULL);
}
void GUIGLCanvas::OnPaint(wxPaintEvent &event) {
	SetCurrent();
	wxPaintDC(this);
	if (!is_initialized) {
		renderer.initGL(GetSize().x,GetSize().y);
		is_initialized = true;
	}
	if (do_refresh) {
		renderer.resize(GetSize().x,GetSize().y);
		if (current_data_object_index>-1) {
			renderer.setObject(data_objects.at(current_data_object_index));
		}
		do_refresh = false;
	}
	renderer.render();
	SwapBuffers();
}
void GUIGLCanvas::setRenderObject(ObjectData* obj) {
	SetCurrent();
	renderer.setObject(obj);
	do_refresh = true;
	Refresh(false,NULL);
}
void GUIGLCanvas::refreshRenderObject() {
	do_refresh = true;
	Refresh(false,NULL);

}
void GUIGLCanvas::OnMouseMove(wxMouseEvent &event) {
	bool refresh = false;
	if (prev_mouse_x>-1) {
		if (event.m_leftDown) {
			renderer.viewport.rotationX +=event.m_x-prev_mouse_x;
			renderer.viewport.rotationY +=event.m_y-prev_mouse_y;
			refresh = true;
		}
		if (event.m_middleDown) {
			Vector3D invec = Vector3D((event.m_x-prev_mouse_x)*.1,-(event.m_y-prev_mouse_y)*.1,0);
			Matrix3D view;
			view.rotateX(renderer.viewport.rotationY*M_PI/180.);
			view.rotateY(renderer.viewport.rotationX*M_PI/180.);
			view.transpose();
			Vector3D* outvec = view.mult(&invec);

			renderer.viewport.cameraPosition->add(outvec);
			delete outvec;
			refresh = true;
		}
	}
	prev_mouse_x = event.m_x;
	prev_mouse_y = event.m_y;
	if (refresh) {
		Refresh(false,NULL);
	}
}
GUIGLCanvas::~GUIGLCanvas(){
	for (unsigned int i=0;i<objects.size();i++) {
		delete objects.at(i);
	}
}


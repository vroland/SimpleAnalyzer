/*
 * GUIGLCanvas.cpp
 *
 *  Created on: 05.09.2013
 *      Author: Valentin Roland
 */

#include "GUIGLCanvas.h"
#include "../SimpleAnalyzerApp.h"
#include "../libraries/interpolate/GeometryClasses.h"

//Eventtabelle zum Verknüpfen der Events
BEGIN_EVENT_TABLE(GUIGLCanvas, wxGLCanvas)
EVT_PAINT (GUIGLCanvas::OnPaint)
EVT_MOUSEWHEEL(GUIGLCanvas::OnMouseWheel)
EVT_MOTION (GUIGLCanvas::OnMouseMove)
EVT_SIZE (GUIGLCanvas::OnResize)
END_EVENT_TABLE()

//Liste der Attribute zum erstellen des OpenGL-Context
int attrib_list[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

GUIGLCanvas::GUIGLCanvas(wxFrame *parent) :
		wxGLCanvas(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0,
				wxT("GLCanvas"), attrib_list) {

	//Statusvariablen
	is_initialized = false;
	do_refresh = false;
	//Zwischenspeicher zum ermitteln von Mausbewegungen
	prev_mouse_x = -1;
	prev_mouse_y = -1;
}

void GUIGLCanvas::OnResize(wxSizeEvent &event) {

	//Aktualisieren des Renderers vormerken
	do_refresh = true;

	Update();
	Refresh();
}

void GUIGLCanvas::OnMouseWheel(wxMouseEvent &event) {

	renderer.getViewport()->zoom -= event.m_wheelRotation / 360.;

	//Zommfaktor gültig?
	if (renderer.getViewport()->zoom < 0) {
		renderer.getViewport()->zoom = 0;
	}

	Update();
	Refresh();
}

void GUIGLCanvas::OnPaint(wxPaintEvent &event) {

	//Aufrufen des OpenGL-Kontexts
	wxPaintDC(this);
	SetCurrent();

	//OpenGL initialisieren?
	if (!is_initialized) {
		renderer.initGL(GetSize().x, GetSize().y);
		is_initialized = true;
	}

	//Viewport aktualisieren?
	if (do_refresh) {
		renderer.resize(GetSize().x, GetSize().y);
		if (wxGetApp().getCurrentDataObjectIndex() > -1) {
			renderer.setObject(wxGetApp().getActiveObject());
		}
		do_refresh = false;
	}

	//Fensterinhalt zeichnen
	renderer.render();
	//Doublebuffer austauschen
	SwapBuffers();
}

void GUIGLCanvas::setRenderObject(ObjectData* obj) {

	//Aufrufen des OpenGL-Kontexts
	SetCurrent();
	//Setzen des neuen Objekts
	renderer.setObject(obj);

	//aktualisieren
	do_refresh = true;
	Refresh(false, NULL);
}

void GUIGLCanvas::refresh() {

	do_refresh = true;
	Refresh(false, NULL);
}

Renderer* GUIGLCanvas::getRenderer() {
	return &renderer;
}

void GUIGLCanvas::OnMouseMove(wxMouseEvent &event) {

	//Aktualisierung nötig?
	bool refresh = false;

	//Mausstatus initialisiert?
	if (prev_mouse_x > -1) {

		//Kamera drehen?
		if (event.m_leftDown) {
			renderer.getViewport()->rotationX += event.m_x - prev_mouse_x;
			renderer.getViewport()->rotationY += event.m_y - prev_mouse_y;
			refresh = true;
		}

		//Kamera verschieben?
		if (event.m_middleDown) {
			//Verschiebungsvektor
			Vector3D invec = Vector3D((event.m_x - prev_mouse_x) * .1,
					-(event.m_y - prev_mouse_y) * .1, 0);
			//Matrix zum Transformieren des Verschiebungsvektors durch die Kameradrehung
			Matrix3D view;
			view.rotateX(renderer.getViewport()->rotationY * M_PI / 180.);
			view.rotateY(renderer.getViewport()->rotationX * M_PI / 180.);
			view.transpose();
			//Transformieren des Verschiebungsvektors
			Vector3D* outvec = view.mult(&invec);

			//Verschieben der Kamera
			renderer.getViewport()->cameraPosition->add(outvec);
			delete outvec;
			refresh = true;
		}
	}

	//Speichern des alten Mausstatus
	prev_mouse_x = event.m_x;
	prev_mouse_y = event.m_y;

	//Aktualisierung wenn nötig
	if (refresh) {
		Refresh(false, NULL);
	}
}

GUIGLCanvas::~GUIGLCanvas() {
}


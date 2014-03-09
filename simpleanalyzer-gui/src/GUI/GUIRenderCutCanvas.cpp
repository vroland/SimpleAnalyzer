/*
 * GUIRenderCutCanvas.cpp
 *
 *  Created on: 03.10.2013
 *      Author: valentin
 */

#include "GUIRenderCutCanvas.h"
#include "GUICutRenderWindow.h"
#include <vector>
#include "../processing/utils.h"
#include "../processing/ObjectData.h"
#include "GUIMainWindow.h"
#include "Renderer.h"
#include <iostream>

//Eventtabelle zum Verknüpfen der Events
BEGIN_EVENT_TABLE(GUIRenderCutCanvas, wxPanel)
	EVT_PAINT    (GUIRenderCutCanvas::onCanvasPaint)
	EVT_MOUSEWHEEL(GUIRenderCutCanvas::OnMouseWheel)
	EVT_MOTION   (GUIRenderCutCanvas::OnMouseMove)
	EVT_SIZE   (GUIRenderCutCanvas::OnResize)
	EVT_LEFT_DOWN(GUIRenderCutCanvas::OnMouseDown)
END_EVENT_TABLE()

using namespace std;
using namespace Utils;

GUIRenderCutCanvas::GUIRenderCutCanvas(wxWindow* parent) :
		wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0,
				wxT("GLCanvas")) {

	//Statusvariablen initialisieren
	zoom = 1;
	deltaX = 0;
	deltaY = 0;
	current_mx = 0;
	current_my = 0;
	image = NULL;
	value_img = NULL;
	mouse_to_scalepanel = false;
	//Die Temperaturkala
	scalepanel = new GUIColorScalePanel();
	SetDoubleBuffered(true);
}

void GUIRenderCutCanvas::setImage(wxImage* img) {
	image = img;
}

void GUIRenderCutCanvas::setValueImg(float* img) {
	value_img = img;
}

void GUIRenderCutCanvas::OnMouseWheel(wxMouseEvent &event) {

	//Bleibt der Zoomfaktor > 0 ?
	if (zoom - event.m_wheelRotation / 1000. > 0) {
		zoom += -event.m_wheelRotation / 1000.;
	}

	Refresh();
}

void GUIRenderCutCanvas::OnMouseMove(wxMouseEvent &event) {

	int width = 0;
	int height = 0;
	GetSize(&width, &height);

	//Position der Temperaturverteilungsgrafik auf der Zeichenfläche
	wxPoint img_point(width / 2 + deltaX - int(image->GetWidth() * zoom) / 2,
			height / 2 + deltaY - int(image->GetHeight() * zoom) / 2);
	//Größe der Temperaturverteilungsgrafik
	wxPoint img_dim(image->GetWidth(), image->GetHeight());

	//Müssen die Mauseingaben zur Skala weitergeleitet werden?
	if (mouse_to_scalepanel) {
		scalepanel->handleMouse(event, img_point, img_dim, zoom);
	} else {
		//anpassen der Größe und Position der Grafik and die Temperaturverteilungsgrafik
		scalepanel->fitBounds(img_dim, false);

		//Verschieben der Ansicht auf die Grafik?
		if (event.m_leftDown) {
			deltaX += (event.m_x - current_mx);
			deltaY += (event.m_y - current_my);
		}
	}

	//Zurücksetzen der Umleitung der Mauseingaben nach loslassen der Maustaste
	if (!event.m_leftDown && mouse_to_scalepanel) {
		mouse_to_scalepanel = false;
	}

	//Speichern der Mausinformationen zum ermitteln der Bewegung
	current_mx = event.m_x;
	current_my = event.m_y;

	Refresh();
	SetFocus();
}

void GUIRenderCutCanvas::OnResize(wxSizeEvent &event) {

}

void GUIRenderCutCanvas::OnMouseDown(wxMouseEvent &event) {

	int width = 0;
	int height = 0;
	GetSize(&width, &height);

	//Position der Temperaturverteilungsgrafik auf der Zeichenfläche
	wxPoint img_point(width / 2 + deltaX - int(image->GetWidth() * zoom) / 2,
			height / 2 + deltaY - int(image->GetHeight() * zoom) / 2);
	//Größe der Temperaturverteilungsgrafik
	wxPoint img_dim(image->GetWidth(), image->GetHeight());
	//Position der Maus
	wxPoint mouse_pos = event.GetPosition();

	//Wurde auf die Skala geklickt?
	if (scalepanel->mouseOnDisplayArea(img_point, zoom, mouse_pos)) {
		scalepanel->handleMouse(event, img_point, img_dim, zoom);
		//Die Mausbewegung bis zum Loslassen der Maustaste von der Skala behandeln lassen
		mouse_to_scalepanel = true;
	}
}

void GUIRenderCutCanvas::onCanvasPaint(wxPaintEvent &event) {

	//Device Kontext zum zeichnen erstellen
	wxPaintDC dc(this);

	int width = 0;
	int height = 0;
	GetSize(&width, &height);
	int imgwidth = 0;
	int imgheight = 0;

	//Ist ein Bild mit der Zeichenfläche verknüpft?
	if (image != NULL) {
		imgwidth = image->GetWidth();
		imgheight = image->GetHeight();
	}

	//Durch den Zoomfaktor veränderte Bildmaße
	int zwidth = int(imgwidth * zoom);
	int zheight = int(imgheight * zoom);
	//Hintergrund löschen
	dc.SetPen(*wxGREY_PEN);
	dc.SetBrush(*wxGREY_BRUSH);
	dc.DrawRectangle(0, 0, width, height);
	//Position für die Grafik auf der Zeichenfläche errechnen
	int imgx = width / 2 + deltaX - zwidth / 2;
	int imgy = height / 2 + deltaY - zheight / 2;

	//Ist ein Bild mit der Zeichenfläche verknüpft?
	if (image != NULL) {
		//Bild zum zeichnen kopieren und skalieren
		wxImage drawimg = image->Copy();
		drawimg.Rescale(zwidth, zheight, wxIMAGE_QUALITY_NORMAL);
		//Bild zeichnen
		dc.DrawBitmap(wxBitmap(drawimg), imgx, imgy);

		//Rahmen für das Bild zeichnen
		dc.SetPen(*wxBLACK);
		int bx = width / 2 + deltaX - zwidth / 2;
		int by = height / 2 + deltaY - zheight / 2;
		dc.DrawLine(bx, by, bx + zwidth, by);
		dc.DrawLine(bx, by, bx, by + zheight);
		dc.DrawLine(bx, by + zheight, bx + zwidth, by + zheight);
		dc.DrawLine(bx + zwidth, by, bx + zwidth, by + zheight);
	}

	//zeichnen der Skala
	wxPoint img_pos(imgx, imgy);
	scalepanel->paintTo(dc, zoom, img_pos);

	//Bild für den Hintergrund der Statusleiste
	wxImage statusimg = wxImage(width, 30, true);
	statusimg.InitAlpha();

	//Hintergrund der Statusleiste halbtransparent machen
	for (int i = 0; i < 30 * width; i++) {
		statusimg.SetAlpha(i % width, i / width, 150);
	}

	//zeichnen des Hintergrunds für die Statusleiste
	wxBitmap statusbmp(statusimg);
	dc.DrawBitmap(statusbmp, 0, height - 30);

	/*
	 * zeichnen der Beschriftung der Statusleiste
	 */

	dc.SetTextForeground(wxColour(255, 255, 255));

	//befindet sich der Mauszeiger über dem Bild?
	if (current_mx >= imgx && current_mx < imgx + zwidth && current_my >= imgy
			&& current_my < imgy + zheight) {

		//die dem Mauszeiger entsprechende Position auf der Grafik
		int x = int((current_mx - imgx) / zoom);
		int y = int((current_my - imgy) / zoom);

		//der Temperaturwert an der Stelle des Mauszeigers
		float value = 0;

		//ist eine Temperaturverteilung erstellt?
		if (value_img != NULL) {
			value = value_img[y * imgwidth + x];
		}

		//Ausgeben der Mausposition und der Temperatur in der Statusleiste
		wxString val_str = floattowxstr(value) + wxT("°C");
		if (abs(value + 300) < .0001) {
			val_str = wxT(" - ");
		}
		value = int(value * 100.) / 100.;
		dc.DrawText(wxT("X:") + floattowxstr(x) + wxT(" Y: ") + floattowxstr(y)
				+ wxT(" Wert: ") + val_str, 20, height - 25);
	}
}

GUIColorScalePanel* GUIRenderCutCanvas::getScalePanel() {
	return scalepanel;
}

GUIRenderCutCanvas::~GUIRenderCutCanvas() {
	delete scalepanel;
}


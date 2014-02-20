/*
 * GUIColorScalePanel.cpp
 *
 *  Created on: 23.12.2013
 *      Author: valentin
 */

#include "GUIColorScalePanel.h"
#include <iostream>
#include "../SimpleAnalyzerApp.h"
#include "../processing/utils.h"
#include <wx/artprov.h>

using namespace std;
using namespace Utils;

//Minimale Maße der Skala
#define MIN_WIDTH 5
#define MIN_HEIGHT 5

GUIColorScalePanel::GUIColorScalePanel() {

	//Standardwerte für die Skala setzen
	width = 20;
	height = 100;
	x = 0;
	y = 0;
	current_mx = 0;
	current_my = 0;
	step_width = 10;
	text_color = wxColour(255, 255, 255);
	mode = SCM_HORIZONTAL;
	image = new wxImage(100, 100);
	font_size = 12;

	//keine aktuelle Transformation
	scaling = false;
	transforming = false;
	prev_mouse_down = false;
}

void GUIColorScalePanel::paintTo(wxDC& dc, float zoom, wxPoint& img_coords) {

	//Soll die Skala dargestellt werden?
	if (mode != SCM_NONE) {
		//Fläche des Farbbands
		wxRect display_area;
		getDisplayArea(&display_area, zoom);

		//Zeichnen des Overlay-Bildes mit der Skala
		wxBitmap drawbmp(*image);
		drawbmp = drawbmp.Rescale(0, 0, 10000000, 1000000,
				drawbmp.GetWidth() * zoom, drawbmp.GetHeight() * zoom);
		dc.DrawBitmap(drawbmp, img_coords.x, img_coords.y);

		//obere linke Ecke des Rechtecks zum Skalieren
		wxPoint scale_rect_topleft(
				img_coords + display_area.GetBottomRight() - wxPoint(10, 10));
		//Fläche des Rechtecks zum Skalieren
		wxRect scalearea = wxRect(scale_rect_topleft,
				img_coords + display_area.GetBottomRight());

		//Zeichnen der Fläche zum Rechtecks zum Skalieren
		dc.SetBrush(*wxWHITE_BRUSH);
		dc.SetPen(*wxBLACK_PEN);
		dc.DrawRectangle(scalearea);
		dc.DrawLine(scale_rect_topleft.x + 2, scale_rect_topleft.y + 2,
				scale_rect_topleft.x + 8, scale_rect_topleft.y + 8);
		dc.DrawLine(scale_rect_topleft.x + 5, scale_rect_topleft.y + 8,
				scale_rect_topleft.x + 8, scale_rect_topleft.y + 8);
		dc.DrawLine(scale_rect_topleft.x + 8, scale_rect_topleft.y + 5,
				scale_rect_topleft.x + 8, scale_rect_topleft.y + 8);

		//Wird die Skala Transformiert?
		if (transforming) {
			//Weiße Umrandung um die Skala zeichnen
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.SetPen(*wxWHITE_PEN);
			dc.DrawRectangle(display_area.GetTopLeft() + img_coords,
					display_area.GetSize());
		}
	}
}

void GUIColorScalePanel::refresh(int img_width, int img_height) {

	//Soll die Skala dargestellt werden?
	if (mode != SCM_NONE) {
		//Fläche des Farbbandes
		wxRect display_area;
		getDisplayArea(&display_area, 1.0);

		//Temporäre Zwischenbilder für Farbe und Transparenz
		wxBitmap color_bmp = wxBitmap(img_width, img_height);
		wxBitmap alpha_bmp = wxBitmap(img_width, img_height);

		//Erstellen von DeviceContexts zum Zeichnen auf die bilder
		wxMemoryDC col_dc(color_bmp);
		wxMemoryDC alpha_dc(alpha_bmp);

		//Schwarzfärben der Bilder (entspricht im Transparentbild volltransparent)
		col_dc.SetBrush(text_color);
		col_dc.SetPen(text_color);
		alpha_dc.SetBrush(*wxBLACK_BRUSH);
		alpha_dc.SetPen(*wxBLACK_PEN);
		col_dc.DrawRectangle(0, 0, img_width, img_height);
		alpha_dc.DrawRectangle(0, 0, img_width, img_height);

		//Temporäres Bild für das Farbband
		wxImage temp_color_img = wxImage(display_area.width,
				display_area.height);

		//Für jedes Pixel des Farbbands
		for (int i = 0; i < display_area.width; i++) {
			for (int j = 0; j < display_area.height; j++) {
				//Zwischenvariable zur Ermittlung des Farbwerts
				float inverse_hue = 0;

				//Ist die Skala Horizontal?
				if (mode == SCM_HORIZONTAL) {
					//Hhorizontaler Farbverlauf
					inverse_hue = i / (float) display_area.width;
				} else {
					//vertikaler Farbverlauf
					inverse_hue = j / (float) display_area.height;
				}

				//Berechnen der Farbe
				float* color = hsvToRgb((1. - inverse_hue) * .66666, 1, 1);
				//Einfärben des Pixels
				temp_color_img.SetRGB(i, j, color[0] * 255, color[1] * 255,
						color[2] * 255);
				delete[] color;
			}
		}

		col_dc.SetBrush(*wxWHITE_BRUSH);
		col_dc.SetPen(*wxBLACK_PEN);
		//Weiße Farbe auf dem Bild für die Transparenz heißt undurchsichtig
		alpha_dc.SetBrush(*wxWHITE_BRUSH);
		alpha_dc.SetPen(*wxWHITE_PEN);

		//Undurchsichtigmachen des Bereichs für das Farbband
		alpha_dc.DrawRectangle(display_area);
		//Zeichnen des Farbbands auf das Farbbild
		col_dc.DrawBitmap(wxBitmap(temp_color_img), display_area.x,
				display_area.y);

		//Erstellen einer Schrift mit der gewünschten Schriftgröße
		wxFont font(int(font_size), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
				wxFONTSTYLE_NORMAL);

		//Übertragen der Einstellungen für die Schrift in die DCs
		col_dc.SetFont(font);
		col_dc.SetTextForeground(text_color);
		col_dc.SetPen(wxPen(text_color));
		alpha_dc.SetFont(font);
		alpha_dc.SetTextForeground(wxColour(255, 255, 255));
		alpha_dc.SetPen(*wxWHITE_PEN);

		//Die Visualisierungseinstellungen
		Visualization_info* vis_info = wxGetApp().getVisualizationInfo();

		//Spannweite des darzustellenden Temperaturbereichs
		int delta_t_vis = (vis_info->max_visualisation_temp
				- vis_info->min_visualisation_temp);

		//Beschriftung der Skala:

		//So viele Schritte, wie mit der angegebenen Schrittweite auf die Skala passen...
		for (int i = 0; i <= (delta_t_vis - delta_t_vis % step_width) / step_width; i++) {

			//Beschriftungstext an dieser Stelle
			wxString text = floattowxstr(
					int(vis_info->min_visualisation_temp + i * step_width));
			//Größe des Schriftzuges
			wxSize t_size = col_dc.GetTextExtent(text);

			//Darstellungsmodus?
			switch (mode) {
			case SCM_VERTICAL: {
				//Pixel pro Schritt
				float display_step = (float) display_area.height / (delta_t_vis)
						* step_width;
				//Position des Beschriftungstexts
				wxPoint textpoint(display_area.x + display_area.width,
						display_area.y + i * display_step);

				//Zeichnen des Texts und Markierungsstrichs auf Farb- und Transparenzbild
				col_dc.DrawText(text, textpoint.x + 8,
						textpoint.y - t_size.y / 2.);
				col_dc.DrawLine(textpoint.x, textpoint.y, textpoint.x + 5,
						textpoint.y);
				alpha_dc.DrawText(text, textpoint.x + 8,
						textpoint.y - t_size.y / 2.);
				alpha_dc.DrawLine(textpoint.x, textpoint.y, textpoint.x + 5,
						textpoint.y);
				break;
			}
			case SCM_HORIZONTAL: {
				//Pixel pro Schritt
				float display_step = (float) display_area.width / (delta_t_vis)
						* step_width;
				//Position des Beschriftungstexts
				wxPoint textpoint(display_area.x + i * display_step,
						display_area.y + display_area.height);

				//Zeichnen des Texts und Markierungsstrichs auf Farb- und Transparenzbild
				col_dc.DrawText(text, textpoint.x - t_size.x / 2.,
						textpoint.y + 8);
				col_dc.DrawLine(textpoint.x, textpoint.y, textpoint.x,
						textpoint.y + 5);
				alpha_dc.DrawText(text, textpoint.x - t_size.x / 2.,
						textpoint.y + 8);
				alpha_dc.DrawLine(textpoint.x, textpoint.y, textpoint.x,
						textpoint.y + 5);
				break;
			}
			default:
				break;
			}
		}

		//lösen der Zwischenbilder von den DCs
		col_dc.SelectObject(wxNullBitmap);
		alpha_dc.SelectObject(wxNullBitmap);

		//zurücksetzen des Ausgabebilds
		delete image;
		image = new wxImage(color_bmp.GetWidth(), color_bmp.GetHeight());
		image->InitAlpha();

		//Zwischenbilder als wxImage, um einzelne Pixels auslesbar zu machen
		wxImage alpha_img = alpha_bmp.ConvertToImage();
		wxImage color_img = color_bmp.ConvertToImage();

		//Übertragen von Farbe und Transparenz in das Ausgabebild
		for (int x = 0; x < image->GetWidth(); x++) {
			for (int y = 0; y < image->GetHeight(); y++) {
				image->SetRGB(x, y, color_img.GetRed(x, y),
						color_img.GetGreen(x, y), color_img.GetBlue(x, y));
				image->SetAlpha(x, y, alpha_img.GetRed(x, y));
			}
		}
	} else {
		//zurücksetzen des Ausgabebilds
		delete image;
		image = new wxImage(img_width, img_height);
		image->InitAlpha();

		//Ausgabebild volltransparent machen
		for (int i = 0; i < img_width; i++) {
			for (int j = 0; j < img_height; j++) {
				image->SetRGB(i, j, 0, 0, 0);
				image->SetAlpha(i, j, 0);
			}
		}
	}
}

void GUIColorScalePanel::getDisplayArea(wxRect* rect, float zoom) {

	rect->x = zoom * x;
	rect->y = zoom * y;
	rect->width = zoom * width;
	rect->height = zoom * height;
}

bool GUIColorScalePanel::mouseOnDisplayArea(wxPoint& img_coords, float zoom,
		wxPoint& mouse_pos) {

	//Fläche des Farbbands
	wxRect display_area;
	getDisplayArea(&display_area, zoom);

	//Auf Fensterkoordinaten umrechnen
	display_area.x += img_coords.x;
	display_area.y += img_coords.y;

	//Berechnen der Fläche zum Skalieren der Skala
	wxPoint sc_topleft(display_area.GetBottomRight() - wxPoint(10, 10));
	wxRect scalearea = wxRect(sc_topleft, display_area.GetBottomRight());

	//Ist der Mauszeiger über einer dieser Flächen?
	return (display_area.Contains(mouse_pos) || scalearea.Contains(mouse_pos));
}

int GUIColorScalePanel::getX() {
	return x;
}

int GUIColorScalePanel::getY() {
	return y;
}

void GUIColorScalePanel::fitBounds(wxPoint& img_dim, bool to_scale) {

	//hat eine Veränderung stattgefunden?
	bool changed = 0;

	//Position gültig?
	if (x < 0) {
		x = 0;
		changed = 1;
	}
	if (y < 0) {
		y = 0;
		changed = 1;
	}

	//Größe gültig?
	if (width < MIN_WIDTH) {
		width = MIN_WIDTH;
		changed = 1;
	}
	if (height < MIN_HEIGHT) {
		height = MIN_HEIGHT;
		changed = 1;
	}
	if (width > img_dim.x) {
		width = img_dim.x;
		changed = 1;
	}
	if (height > img_dim.y) {
		height = img_dim.y;
		changed = 1;
	}

	//Soll oder muss die Skalengröße verändert werden?
	if (to_scale
			&& !(img_dim.x - x < MIN_WIDTH || img_dim.y - y < MIN_HEIGHT)) {

		//Anpassen der Skalengröße an die Bildgröße
		if (x + width > img_dim.x) {
			width = img_dim.x - x;
			changed = 1;
		}
		if (y + height > img_dim.y) {
			height = img_dim.y - y;
			changed = 1;
		}
	} else {
		//Anpassen der Sklenposition an das Bild
		if (x + width > img_dim.x) {
			x = img_dim.x - width;
			changed = 1;
		}
		if (y + height > img_dim.y) {
			y = img_dim.y - height;
			changed = 1;
		}
	}

	//hat eine Veränderung stattgefunden?
	if (changed) {
		//Skale neu berechnen
		refresh(img_dim.x, img_dim.y);
	}
}
void GUIColorScalePanel::handleMouse(wxMouseEvent& event, wxPoint& img_coords,
		wxPoint& img_dim, float zoom) {

	//Soll die Skala dargestellt werden?
	if (mode != SCM_NONE) {
		//Fläche des Farbbands
		wxRect display_area;
		getDisplayArea(&display_area, zoom);

		//auf Fensterkoordinaten umrechnen
		display_area.x += img_coords.x;
		display_area.y += img_coords.y;

		//Ist die linke Maustaste gedrückt worden?
		if (prev_mouse_down != event.m_leftDown && !prev_mouse_down) {
			//Fläche zum Skalieren
			wxPoint sc_topleft(display_area.GetBottomRight() - wxPoint(10, 10));
			wxRect scalearea = wxRect(sc_topleft,
					display_area.GetBottomRight());

			//wurde auf die Fläche zum Skalieren geklickt?
			if (scalearea.Contains(event.GetPosition())) {
				scaling = true;
			} else {
				scaling = false;
			}
		} else {
			//Ist die linke Maustaste gedrückt?
			if (event.m_leftDown) {
				//wird gerade skaliert?
				if (scaling) {
					//Skala  skalieren
					width += (event.m_x - current_mx) / zoom;
					height += (event.m_y - current_my) / zoom;
					fitBounds(img_dim, true);
				} else {
					//Skala  verschieben
					x += (event.m_x - current_mx) / zoom;
					y += (event.m_y - current_my) / zoom;
					fitBounds(img_dim, false);
				}
			}
		}

		//findet eine Transformation statt?
		transforming = event.m_leftDown;
	}

	//wurde geklickt oder die linke Maustaste losgelassen?
	if (prev_mouse_down != event.m_leftDown) {
		//Skalenbild aktualisieren
		refresh(img_dim.x, img_dim.y);
	}

	//vorherigen Maussatus speichern
	prev_mouse_down = event.m_leftDown;
	//vorherige Mausposition speichern
	current_mx = event.m_x;
	current_my = event.m_y;
}

int GUIColorScalePanel::getFontSize() const {
	return font_size;
}

void GUIColorScalePanel::setFontSize(int fontSize) {
	font_size = fontSize;
}

GUIColorScalePanel::ScaleMode GUIColorScalePanel::getMode() const {
	return mode;
}

void GUIColorScalePanel::setMode(ScaleMode mode) {
	this->mode = mode;
}

const wxColour& GUIColorScalePanel::getTextColor() const {
	return text_color;
}

void GUIColorScalePanel::setTextColor(const wxColour& textColor) {
	text_color = textColor;
}

int GUIColorScalePanel::getStepWidth() const {
	return step_width;
}

void GUIColorScalePanel::setStepWidth(int stepWidth) {
	step_width = stepWidth;
}

wxImage* GUIColorScalePanel::getImage() const {
	return image;
}

GUIColorScalePanel::~GUIColorScalePanel() {
}


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

/**
 * @brief Farbige Temperaturskala für zweidimensionale Temperaturverteilung.
 *
 * Farbige Temperaturskala für zweidimensionale Temperaturverteilung. Wird für die Darststellung
 * einer farbigen Temperaturskala im Anzeigefenster auf der als zweidimensionale Temperaturverteilung erzeugten
 * Grafik verwendet.
 */
class GUIColorScalePanel {
public:
	/**
	 * @enum
	 * Modus der Skalendarstellung.
	 */
	enum ScaleMode {
		SCM_NONE = 0,
		SCM_HORIZONTAL,
		SCM_VERTICAL,
	};
	/**
	 * Der Konstruktor.
	 */
	GUIColorScalePanel();
	/**
	 * Zeichnet die Temperaturskala neu.
	 * @param img_width Breite des Bildes, für das die Skala gezeichnet wird.
	 * @param img_height Höhe des Bildes, für das die Skala gezeichnet wird.
	 */
	void refresh(int img_width,int img_height);
	/**
	 * Zeichnet die Temperaturskala mit einem bestimmten device context.
	 * @param zoom Faktor zum Skalieren der Skala.
	 * @param img_coords Position der Grafik auf der Zeichenfläche.
	 */
	void paintTo(wxDC& dc,float zoom,wxPoint& img_coords);
	/**
	 * Behandelt die Mausaktionen und verändert ggf. Größe oder Position des Skala.
	 * @param event Das zu behandelnde Maus-event.
	 * @param img_coords Position der Grafik auf der Zeichenfläche.
	 * @param img_dim Größe der Grafik.
	 * @param zoom aktueller Vergrößerungsfaktor des Betrachtungsfensters.
	 */
	void handleMouse(wxMouseEvent& event,wxPoint& img_coords,wxPoint& img_dim,float zoom);
	/**
	 * Gibt die bei einem bestimmten Zoomfaktor eingenommene Fläche zurück.
	 */
	void getDisplayArea(wxRect* rect,float zoom);
	/**
	 * Passt die Größe und Position der Skala an die Größe der Grafik an.
	 * @param img_dim Größe der Grafik.
	 * @param to_scale Größe statt der Position verändern.
	 */
	void fitBounds(wxPoint& img_dim,bool to_scale);
	/**
	 * Gibt zurück, ob sich die Maus über der Fläche der Skala befindet.
	 * @param img_coords Position der Grafik auf der Zeichenfläche.
	 * @param zoom aktueller Vergrößerungsfaktor des Betrachtungsfensters.
	 * @param mouse_pos Position der Maus auf der Zeichenfläche.
	 */
	bool mouseOnDisplayArea(wxPoint& img_coords,float zoom,wxPoint& mouse_pos);
	/**
	 * @return horizontale Position auf der Zeichenfläche.
	 */
	int getX();
	/**
	 * @return vertikale Position auf der Zeichenfläche.
	 */
	int getY();
	/**
	 * @return Schriftgröße der Skala.
	 */
	int getFontSize() const;
	/**
	 * Setzt die Schriftgröße der Skala.
	 */
	void setFontSize(int fontSize);
	/**
	 * @return Modus der Skala.
	 */
	ScaleMode getMode() const;
	/**
	 * Setzt den Modus der Skala.
	 */
	void setMode(ScaleMode mode);
	/**
	 * @return Schriftfarbe der Skala.
	 */
	const wxColour& getTextColor() const;
	/**
	 * Setzt die Schriftfarbe der Skala.
	 */
	void setTextColor(const wxColour& textColor);
	/**
	 * Gibt die Schrittweite der Skalenbeschriftung.
	 */
	int getStepWidth() const;
	/**
	 * Setzt die Schrittweite der Skalenbeschriftung.
	 */
	void setStepWidth(int stepWidth);
	/**
	 * @return Skala als Grafik.
	 */
	wxImage* getImage() const;

	/**
	 * Der Destruktor.
	 */
	virtual ~GUIColorScalePanel();
private:
	/**
	 * Schrittweite der Beschriftung.
	 */
	int step_width;
	/**
	 * Die Schriftgröße.
	 */
	int font_size;
	/**
	 * Der Darstellungsmodus.
	 */
	ScaleMode mode;
	/**
	 * Die Schriftfarbe.
	 */
	wxColour text_color;
	/**
	 * Bild, das die Skala ohne Steuerelemente enthält.
	 */
	wxImage* image;
	/**
	 * Zwischenspeicher für die Mausposition, zum behandeln von Mausinteraktionen.
	 */
	int current_mx;
	/**
	 * Zwischenspeicher für die Mausposition, zum behandeln von Mausinteraktionen.
	 */
	int current_my;
	/**
	 * Position (X) der Skala.
	 */
	float x;
	/**
	 * Position Y) der Skala.
	 */
	float y;
	/**
	 * Breite der Skala.
	 */
	float width;
	/**
	 * Höhe der Skala.
	 */
	float height;
	/**
	 * Wird gerade in der Größe verändert.
	 */
	bool scaling;
	/**
	 * Wird gerade transformiert (Größe oder Position).
	 */
	bool transforming;
	/**
	 * zwischenspeicher für den Mausstatus.
	 */
	bool prev_mouse_down;
};

#endif /* GUICOLORSCALEPANEL_H_ */

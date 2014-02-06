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
 * Modus der Skalendarstellung.
 */
enum ScaleMode {
	SCM_NONE = 0,
	SCM_HORIZONTAL,
	SCM_VERTICAL,
};
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
	 * Gibt zurück, ob sich die Maus über der Fläche der Skala befindet.
	 * @param img_coords Position der Grafik auf der Zeichenfläche.
	 * @param zoom aktueller Vergrößerungsfaktor des Betrachtungsfensters.
	 * @param mouse_pos Position der Maus auf der Zeichenfläche.
	 */
	bool mouseOnDisplayArea(wxPoint& img_coords,float zoom,wxPoint& mouse_pos);
	/**
	 * Gibt die horizontale Position auf der Zeichenfläche zurück.
	 */
	int getX();
	/**
	 * Gibt die vertikale Position auf der Zeichenfläche zurück.
	 */
	int getY();
	/**
	 * Gibt die Schriftgröße der Skala zurück.
	 */
	int getFontSize() const;
	/**
	 * Setzt die Schriftgröße der Skala.
	 */
	void setFontSize(int fontSize);
	/**
	 * Gibt den Modus der Skala zurück.
	 */
	ScaleMode getMode() const;
	/**
	 * Setzt den Modus der Skala.
	 */
	void setMode(ScaleMode mode);
	/**
	 * Gibt die Schriftfarbe der Skala zurück.
	 */
	const wxColour& getTextColor() const;
	/**
	 * Setzt die Schriftfarbe der Skala.
	 */
	void setTextColor(const wxColour& textColor);
	/**
	 * Gibt die Schrittweite der Skalenbeschriftung zurück.
	 */
	int getStepWidth() const;
	/**
	 * Setzt die Schrittweite der Skalenbeschriftung.
	 */
	void setStepWidth(int stepWidth);
	/**
	 * Gibt die Skala als Bild zurück.
	 */
	wxImage* getImage() const;

	/**
	 * Der Destruktor.
	 */
	virtual ~GUIColorScalePanel();
private:
	/**
	 * Passt die Größe und Position der Skala an die Größe der Grafik an.
	 * @param img_dim Größe der Grafik.
	 * @param to_scale Größe statt der Position verändern.
	 */
	void fitBounds(wxPoint& img_dim,bool to_scale);

	int step_width;
	int font_size;
	ScaleMode mode;
	wxColour text_color;
	wxImage* image;
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

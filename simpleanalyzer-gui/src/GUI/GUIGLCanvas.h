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

/**
 * @brief Zeichenfläche für das 3D-Fenster
 *
 * Klasse zum Verwalten der im 3D-Fenster angezeigten Inhalte.
 * Auch zuständig für Drehen, Verschieben und Zoomen der Ansicht.
 */
class GUIGLCanvas: public wxGLCanvas {
public:
	/**
	 * Der Konstruktor.
	 * @param parent Das Fenster, auf dem sich die Zeichenfläche befindet.
	 */
	GUIGLCanvas(wxFrame* parent);
	/**
	 * Setzt das darzustellende Objekt.
	 */
	void setRenderObject(ObjectData* obj);
	/**
	 * Gibt den Renderer der Zeichenfläche zurück.
	 */
	Renderer* getRenderer();
	/**
	 * Zeichnet den Inhalt des 3D-Fensters neu und aktualisiert den Renderer, z.B. bei geänderter
	 * Fenstergröße oder geänderten Eigenschaften des angezeigten Objekts.
	 */
	void refresh();
	/**
	 * Der Destruktor.
	 */
	virtual ~GUIGLCanvas();
protected:
	/**
	 * Event-Tabellendeklaration für wxWidgets.
	 */
    DECLARE_EVENT_TABLE()
private:
	/**
	 * Behandelt das Zeichenevent und zeichnet die Inhalte des 3D-Fensters.
	 */
	void OnPaint(wxPaintEvent& event);
	/**
	 * Behandelt Mausradbewegungen (zoomen).
	 */
	void OnMouseWheel(wxMouseEvent &event);
	/**
	 * Behandelt Mausbewegungen (verschieben und drehen der Ansicht).
	 */
	void OnMouseMove(wxMouseEvent &event);
	/**
	 * Behandelt Größenänderungen der Zeichenfläche.
	 */
	void OnResize(wxSizeEvent &event);
	/**
	 * Der verwendete Renderer.
	 */
	Renderer renderer;
	/**
	 * Initialisiertungsstatus des Objekts.
	 */
    bool is_initialized;
    /**
     * Statusvariable, gibt an ob beim Zeichnen auch der Renderer aktualisiert wird.
     * Dies tritt beispielsweise bei Größenänderungen oder Änderungen am Objekt ein, da
     * die Daten teilweise neu an den Renderer übermittelt werden müssen.
     */
    bool do_refresh;
    /**
     * Zwischenspeicher für die vorherige Mausposition (X).
     */
    int prev_mouse_x;
    /**
	 * Zwischenspeicher für die vorherige Mausposition (Y).
	 */
    int prev_mouse_y;
};

#endif /* GUIGLCANVAS_H_ */

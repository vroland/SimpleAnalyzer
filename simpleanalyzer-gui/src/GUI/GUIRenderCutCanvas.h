/*
 * GUIRenderCutCanvas.h
 *
 *  Created on: 03.10.2013
 *      Author: Valentin Roland
 */

#ifndef GUIRENDERCUTCANVAS_H_
#define GUIRENDERCUTCANVAS_H_
#include <wx/wx.h>
#include "GUIColorScalePanel.h"

/**
 * @brief Zeichenfläche für die 2D-Temperaturverteilung.
 *
 * Zeichenfläche für das Fenster zur Berechnung einer 2D-Temperaturverteilung.
 * Zeigt die berechnete Grafik, Skala und eine Statusleiste an.
 * Verwaltet auch Mauseingaben zum Verschieben und Zoomen der Ansicht.
 */
class GUIRenderCutCanvas: public wxPanel {
public:
	/**
	 * Der Konstruktor.
	 * @param parent Das Fenster, auf dem die Zeichenfläche liegen soll.
	 */
	GUIRenderCutCanvas(wxWindow* parent);

	/**
	 * Setzt die aktuell angezeigte Grafik.
	 */
	void setImage(wxImage* img);

	/**
	 * Setzt die zum anzeigen von werten verwendete Temperaturverteilung.
	 */
	void setValueImg(float* img);

	/**
	 * Gibt das Temperaturskala-Objekt zurück.
	 */
	GUIColorScalePanel* getScalePanel();

	/**
	 * Der Destruktor.
	 */
	virtual ~GUIRenderCutCanvas();
protected:
	/**
	 * Event-Tabellendeklaration für wxWidgets.
	 */
	DECLARE_EVENT_TABLE()

private:
	/**
	 * Zeichnet die Temperaturverteilung und die Anzeigeelemente (Informationsleiste, Skala).
	 */
	void onCanvasPaint(wxPaintEvent &event);

	/**
	 * Behandelt das Zoomen in der Grafik.
	 */
	void OnMouseWheel(wxMouseEvent &event);

	/**
	 * Behandelt das verschieben der Ansicht und speichert die Mauszeigerposition zur Ermittlung des Wertes
	 * an dieser Stelle in onCanvasPaint().
	 */
	void OnMouseMove(wxMouseEvent &event);

	/**
	 * Behandelt Größenänderungen der Zeichenfläche.
	 */
	void OnResize(wxSizeEvent &event);

	/**
	 * Behandelt klicken mit der Maus, deren Status zum verschieben der Ansicht benötigt wird.
	 */
	void OnMouseDown(wxMouseEvent &event);

	/**
	 * Der aktuelle Zoomfaktor für die Zeichenfläche.
	 */
	float zoom;

	/**
	 * horizontale Verschiebung der Ansicht.
	 */
	float deltaX;

	/**
	 * vertikale Verschiebung der Ansicht.
	 */
	float deltaY;

	/**
	 * Zwischenspeicher für die horizontale Mausposition.
	 */
	int current_mx;

	/**
	 * Zwischenspeicher für die vertikale Mausposition.
	 */
	int current_my;

	/**
	 * Müssen die Mausaktionen zur Skala weitergeleitet werden? (Wird diese gerade Transformiert?)
	 */
	bool mouse_to_scalepanel;

	/**
	 * Die aktuelle dargestellte Temperaturverteilung als Grafik.
	 */
	wxImage* image;

	/**
	 * Die aktuelle dargestellte Temperaturverteilung.
	 */
	float* value_img;

	/**
	 * Die Temperaturskala.
	 */
	GUIColorScalePanel* scalepanel;
};

#endif /* GUIRENDERCUTCANVAS_H_ */

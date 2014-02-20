/*
 * Timeline.h
 *
 *  Created on: 22.10.2013
 *      Author: valentin
 */

#ifndef TIMELINE_H_
#define TIMELINE_H_

#include <wx/wx.h>
#include <string>
#include <vector>
#include "../processing/ObjectData.h"

using namespace std;

/**
 * Typ wxEVT_TIMELINE_CHANGE zum Auslösen eines Events bei Veränderung der Zeitleiste.
 * Die Definition als globale Konstante ist durch das GUI-System vorgegeben.
 */
extern const wxEventType wxEVT_TIMELINE_CHANGE;

/**
 * @brief Eine Zeitleistenkomponente.
 *
 * Die Komponente kann Zeitpunkte als Zeitleiste darstellen, wobei die Zeitpunkte anhand von
 * Indices ausgewählt werden können. Zusätzlich kann eine Liste von Namen für die Zeitpunkte
 * festgelegt werden, wodurch auch der Name des gewählten Zeitpunkts angezeigt wird.
 * Weiterhin können Zeitpunkte markiert werden.
 */
class GUITimeline: public wxPanel {
public:
	/**
	 * Darstellungsstil der Zeitleiste.
	 */
	enum GUI_TIMELINE_STYLE {
		GTL_DEFAULT = 0,
	};

	/**
	 * Der Konstruktor.
	 * @param parent Die übergeordnete Komponente.
	 * @param id Die ID des Objekts.
	 * @param pos Die Position der Zeitleiste.
	 * @param size Die Größe der Zeitleiste.
	 * @param style Darstellungsstil der Zeitleiste.
	 * @param name Name der Zeitleiste (Komponentenname, nicht sichtbar).
	 */
	GUITimeline(wxWindow *parent, wxWindowID id, const wxPoint& pos =
			wxDefaultPosition, const wxSize& size = wxDefaultSize, long style =
			GTL_DEFAULT, const wxString& name = wxT("Timeline"));

	/**
	 * Sucht den Zeitpunkt zwischen zwei markierten Stellen auf der Sensordaten-Zeitleiste, für den der
	 * Wäremeenergiegehalt maximal wird. Dabei wird der Bereich zwischen den beiden markierten Stellen ausgewählt, zwischen denen
	 * sich der aktuell ausgewählte Zeitpunkt befindet.
	 * @param obj Das zu untersuchende Objekt.
	 * @param fast Schnelle Methode verwenden. D.h., es wird statt der Temperaturverteilung nur die Durchschnittstemperatur verglichen.
	 */
	void findMaxValue(ObjectData* obj, bool fast);

	/**
	 * gibt den Index des aktuell ausgewählten Zeitpunkts zurück.
	 */
	int getValue();

	/**
	 * Gibt den maximal auswählbaren Index zurück.
	 */
	int getMaxValue();

	/**
	 * Gibt den minimal auswählbaren Index zurück.
	 */
	int getMinValue();

	/**
	 * Setzt den Index des aktuell ausgewählten Zeitpunkts.
	 */
	void setValue(int val);

	/**
	 * Setzt den maximal auswählbaren Index.
	 */
	void setMaxValue(int val);

	/**
	 * Setzt den minimal auswählbaren Index.
	 */
	void setMinValue(int val);

	/**
	 * Setzt die Liste der Namen für die jeweiligen Indices der Zeitpunkte.
	 * @param namelist Liste mit einem Namen für jeden Index.
	 */
	void setNameList(vector<string>* namelist);

	/**
	 * Markiert/Demarkiert einen bestimmten Zeitpunkt.
	 * @param pos Index des zu setzenden Zeitpunkts.
	 * @param state Status des Punktes (markiert - true, nicht markiert - false).
	 */
	void setMarked(int pos, bool state);

	/**
	 * Gibt zurück, ob ein Zeitpunkt markiert ist.
	 * @param pos Index des Zeitpunkts.
	 */
	bool isMarked(int pos);

	/**
	 * Entfernt alle Markierungen.
	 */
	void clearMarkers();

	/**
	 * Setzt die Liste der markierten Stellen.
	 * @param mlist Liste mit den Indices der markierten stellen.
	 */
	void setMarkerList(vector<int>* mlist);

	/**
	 * Gibt die Liste der markierten Stellen zurück.
	 */
	vector<int>* getMarkers();

	/**
	 * Markiert eine Liste von Indices.
	 * @param mlist Liste aller zu markierenden Indices.
	 */
	void setMarkers(vector<int>* mlist);

	/**
	 * Der Destruktor.
	 */
	virtual ~GUITimeline();
protected:
	/**
	 * Event-Tabellendeklaration für wxWidgets.
	 */
	DECLARE_EVENT_TABLE()
private:
	/**
	 * Zeichnet die Zeitleiste.
	 */
	void OnPaint(wxPaintEvent&);

	/**
	 * Behandellt Scrolleingaben (zoomen).
	 */
	void OnMouseWheel(wxMouseEvent &event);

	/**
	 * Behandelt Mausbewegungen (verschieben der Ansicht).
	 */
	void OnMouseMove(wxMouseEvent &event);

	/**
	 * Behandelt Größenänderungen der Zeitleiste.
	 */
	void OnResize(wxSizeEvent &event);

	/**
	 * Behandelt klicken (verschieben der Ansicht, setzten des aktuellen Zeitpunkts).
	 */
	void OnMouseDown(wxMouseEvent &event);

	/**
	 * Behandelt Tastendruck (setzen des aktuellen Zeitpunkts).
	 */
	void OnKeyDown(wxKeyEvent &event);

	/**
	 * Setzt den aktuellen Zeitpunkt anhand der Mausposition.
	 */
	void posToVal(int mouse_x);

	/**
	 * Löst ein wxEVT_TIMELINE_CHANGE-Event aus.
	 */
	void sendTimelineEvent();

	/**
	 * Berechnet die für die aktuelle Darstellung günstigste Schrittweite für die Beschriftung.
	 */
	int calcStepWidth();

	/**
	 * Der Index des aktuell ausgewählten Zeitpunkts.
	 */
	int value;

	/**
	 * Der größte anzuzeigende Zeitpunkt.
	 */
	int maxvalue;

	/**
	 * Der kleinste anzuzeigende Zeitpunkt.
	 */
	int minvalue;

	/**
	 * Maximale Anzahl an anzuzeigenden Nachkommastellen.
	 */
	int maxdigits;

	/**
	 * Aktueller Zoomfaktor.
	 */
	float zoom;

	/**
	 * Verschiebung der Ansicht.
	 */
	float delta_v_view;

	/**
	 * Zwischenspeicher für die vorherige horizontale Mausposition.
	 */
	int prev_mouse_x;

	/**
	 * Liste der Zeitpunktnamen.
	 */
	vector<string>* names;

	/**
	 * Liste der markierten Zeitpunkte.
	 */
	vector<int>* markers;
};

#endif /* TIMELINE_H_ */

/*
 * GUITimeline.cpp
 *
 *  Created on: 22.10.2013
 *      Author: valentin
 */

#include "GUITimeline.h"
#include <iostream>
#include "../processing/utils.h"
#include <cmath>
#include <algorithm>
#include "../processing/Analyzer.h"
using namespace std;
using namespace Utils;

//Definition des Eventtyps für die Änderung von Eigenschaften der Timeline
const wxEventType wxEVT_TIMELINE_CHANGE = wxNewEventType();

//Eventtabelle zum Verknüpfen der Events
BEGIN_EVENT_TABLE(GUITimeline, wxPanel)
    EVT_PAINT    (GUITimeline::OnPaint)
    EVT_MOUSEWHEEL(GUITimeline::OnMouseWheel)
    EVT_MOTION   (GUITimeline::OnMouseMove)
    EVT_SIZE   (GUITimeline::OnResize)
    EVT_LEFT_DOWN(GUITimeline::OnMouseDown)
    EVT_KEY_DOWN(GUITimeline::OnKeyDown)
END_EVENT_TABLE()

//Anzahl der Schritte für die verfeinerung der Skaleneinteilung beim Zoom
#define SCALE_REFINE_STEPS 3

//Faktoren der Schritte für die verfeinerung der Skaleneinteilung beim Zoom
const float refine_factors[SCALE_REFINE_STEPS] = { .5, .2, .1 };

GUITimeline::GUITimeline(wxWindow *parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style, const wxString& name) :
		wxPanel(parent, id, pos, size, style, name) {

	//initialisieren der Timelineigenschaften
	minvalue = 0;
	maxvalue = 100;
	value = minvalue;
	zoom = 1;
	maxdigits = 1;
	delta_v_view = 0;
	prev_mouse_x = -10000;
	names = NULL;
	markers = NULL;
}

void GUITimeline::sendTimelineEvent() {
	//Auslösen eines wxEVT_TIMELINE_CHANGE-Events
	wxCommandEvent event(wxEVT_TIMELINE_CHANGE, GetId());
	event.SetEventObject(this);
	GetEventHandler()->ProcessEvent(event);
}

void GUITimeline::OnMouseWheel(wxMouseEvent &event) {
	//Setzt den Fokus für Eingaben auf die Timeline
	SetFocus();

	//Veränderung des Zoomfaktors
	float delta = -event.m_wheelRotation / 1000.;
	zoom *= exp(delta);

	//Ist der Zoomfaktor gültig?
	if (zoom < .000001) {	//Minimal 1-er Schritte
		zoom = .000001;

	}
	if (zoom > 2) {
		zoom = 2.;
	}

	Refresh(false, NULL);
}

void GUITimeline::OnMouseDown(wxMouseEvent &event) {
	SetFocus();
	//setzen des aktuellen Zeitpunkts auf die Klickposition
	posToVal(event.m_x);
}

void GUITimeline::OnKeyDown(wxKeyEvent &event) {

	//Pfeil nach links gedrückt?
	if (event.m_keyCode == WXK_LEFT) {
		value--;

		//neuer Wert gültig?
		if (value < minvalue) {
			value = minvalue;
		}
		sendTimelineEvent();
	}

	//Pfeil nach rechts gedrückt?
	if (event.m_keyCode == WXK_RIGHT) {
		value++;

		//neuer Wert gültig?
		if (value > maxvalue) {
			value = maxvalue;
		}
		sendTimelineEvent();
	}

	Refresh(false, NULL);
}

void GUITimeline::findMaxValue(ObjectData* obj, bool fast) {

	//aktueller Wert
	int val = getValue();
	//Bereich, in dem Gesucht werden soll
	int marker1 = getMinValue() - 1;
	int marker2 = getMaxValue() + 1;

	//für alle markierten Stellen...
	for (size_t i = 0; i < markers->size(); i++) {
		int marker = markers->at(i);

		//ist der aktuelle Zeitpunkt markiert?
		if (val == marker) {
			wxMessageBox(
					wxT("Der Cursor muss sich zwischen zwei Markierungen befinden!"),
					wxT("Fehler"));
			return;
		}

		//finden der eingrenzenden markierten Stellen
		if (marker < val && marker > marker1) {
			marker1 = marker;
		}

		if (marker > val && marker < marker2) {
			marker2 = marker;
		}
	}

	//eingrenzende markierte Stellen im gültigen Bereich?
	if (marker1 == getMinValue() - 1 || marker2 == getMaxValue() + 1) {
		wxMessageBox(
				wxT("Der Cursor muss sich zwischen zwei Markierungen befinden!"),
				wxT("Fehler"));
		return;
	}

	//aktuelles Energiemaximum
	double max_energy = -1;
	//aktueller Index des Energiemaximums
	int max_index = marker1;

	//schnelle Methode verwenden?
	if (fast) {

		//für alle Zeitpunkte zwischen den markierten Stellen...
		for (int i = 0; i < (marker2 - marker1) + 1; i++) {
			double val = 0;
			//aktueller Sensordatensatz
			SensorData* sd = &obj->getSensorDataList()->at(
					obj->getCurrentSensorIndex());

			//Summe aller Temperaturmesswerte
			for (int k = 0; k < int(sd->data.at(i).size()); k++) {
				val += sd->data.at(i).at(k).temperature;
			}

			//neues Temperaturmaximum?
			if (val > max_energy) {
				max_energy = val;
				max_index = marker1 + i;
			}
		}
	} else {
		//Struktur zum speichern der Analysedaten
		Analyzer::AnalyzerData_object data;
		Analyzer analyzer;

		//aktueller Sensordatensatz
		SensorData* sd = &obj->getSensorDataList()->at(
				obj->getCurrentSensorIndex());

		//speichern des aktuell gewählten Zeitpunktes für die Temperaturverteilung
		int original_time_index = sd->current_time_index;

		//für alle Zeitpunkte zwischen den markierten Stellen...
		for (int i = 0; i < (marker2 - marker1) + 1; i++) {
			//Berechnen der Temperaturverteilung und Analyse des Objekts zu diesem Zeitpunkt
			sd->current_time_index = marker1 + i;
			analyzer.analyzeObject(obj, &data, false,
					obj->getCurrentSensorIndex());

			//neues Energiemaximum?
			if (data.data_sets.at(0).heat_energy > max_energy) {
				max_energy = data.data_sets.at(0).heat_energy;
				max_index = marker1 + i;
			}
		}

		//zurücksetzen des Objekts auf den vorher ausgewählten Zeitpunkt
		sd->current_time_index = original_time_index;
		obj->calculateIO();
	}

	//auswählen des gefundenen Zeitpunkts
	setValue(max_index);
}

int GUITimeline::calcStepWidth() {

	//Zehnerpotenz des beim aktuellen Zoom dargestellten Bereichs (zoom * maxvalue)
	int to_ten = int(pow(10,int(log10(maxvalue * zoom))));
	//faktor zur Verfeinerung der kleinsten Skaleneinteilung
	float refine_factor = 1;
	//Device Context zum ermitteln zur ausdehnung des dargestellten Texts
	wxPaintDC dc(this);
	//Pixel pro Skaleneinteilung
	float pixelsperstep = (float) GetSize().x / ((maxvalue - minvalue) * zoom);

	//Versuchen, die Einteilung weiter zu verfeinern, bis sich Text überschneidet
	for (int i = 0; i < SCALE_REFINE_STEPS; i++) {

		//zu testende Schrittweite
		int stepwidth = to_ten * refine_factors[i];
		//Verschiebung der Ansicht bezogen auf den Zoomfaktor
		int viewstart = int(delta_v_view / zoom);
		//die am weitesten links liegende, sichtbare Einteilung
		int start_index = -int(viewstart / pixelsperstep / stepwidth);

		//die am weitesten rechts liegende, sichtbare Einteilung
		int curr_max = start_index
				+ 1. / pixelsperstep * GetSize().x / stepwidth;
		curr_max = curr_max * stepwidth;

		wxString str = floattowxstr(curr_max);
		//Ausdehnung der Beschriftung für die am weitesten rechts liegende, sichtbare Einteilung
		int text_extend = dc.GetTextExtent(str).x;

		//Ist diese noch lesbar?
		if (stepwidth * pixelsperstep > text_extend + 2) {
			//weiter verfeinern
			refine_factor = refine_factors[i];
		} else {
			break;
		}
	}

	//kleinstmögliche gut lesbare Skaleneinteilung
	return to_ten * refine_factor;
}

void GUITimeline::posToVal(int mouse_x) {

	int width = 0;
	int height = 0;
	GetSize(&width, &height);

	//Pixel pro Zeitpunkt in der Skalendarstellung (bezogen auf die Breite)
	float pixelsperstep = (float) width / ((maxvalue - minvalue) * zoom);
	int viewstart = int(delta_v_view / zoom);
	//Wert an der gegebenen Position
	value = int((mouse_x - viewstart) / pixelsperstep);

	//Ist der wert gültig?
	if (value < minvalue) {
		value = minvalue;
	}
	if (value > maxvalue) {
		value = maxvalue;
	}

	sendTimelineEvent();
	Refresh(false, NULL);
}

void GUITimeline::OnMouseMove(wxMouseEvent &event) {	// Ermöglicht ziehen

	SetFocus();

	//aktuellen Zeitpunkt auswählen?
	if (event.m_leftDown) {
		posToVal(event.m_x);
	}

	//Ansicht verschieben?
	if (event.m_middleDown) {
		//Wurde eine Vergleichsposition gespeichert?
		if (prev_mouse_x > -9999) {
			delta_v_view += (event.m_x - prev_mouse_x) * zoom;
		}

		//Speichern der alten Mausposition zum Ermitteln der Bewegung
		prev_mouse_x = event.m_x;
		Refresh(false, NULL);
	} else {
		//Vergleichsposition der Maus verwerfen, nicht länger gültig
		prev_mouse_x = -10000;
	}
}

void GUITimeline::OnResize(wxSizeEvent &event) {
	Refresh(false, NULL);
}

void GUITimeline::OnPaint(wxPaintEvent&) {

	//Zeichenkontext
	wxPaintDC dc(this);
	SetDoubleBuffered(true);
	int width = 0;
	int height = 0;
	int captionheight = 20;
	GetSize(&width, &height);

	//Pixel pro Zeitpunkt in der Skalendarstellung (bezogen auf die Breite)
	float pixelsperstep = (float) width / ((maxvalue - minvalue) * zoom);
	//Schrittweite der Beschriftung
	float stepwidth = calcStepWidth();

	//durch verschieben gegebener Start des dargestellten Bereichs
	int viewstart = int(delta_v_view / zoom);
	int start_index = -int(viewstart / pixelsperstep / stepwidth);

	//Zeichnen des Hintergrunds
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	dc.DrawRectangle(0, 0, width, height);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	//Zeichnen der Umrandung
	dc.SetPen(
			wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT), 1,
					wxSOLID));
	dc.DrawRectangle(0, 0, width, height);

	//Zeichnen des nicht auswählbaren Bereichs
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(
			wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT),
					wxSOLID));

	//nicht auswählbaren Bereich vor dem Minimalwert
	if (-viewstart < minvalue * pixelsperstep) {
		dc.DrawRectangle(0, 1, viewstart + minvalue * pixelsperstep, height);
	}
	//nicht auswählbaren Bereich nach dem Maximalwert
	if (viewstart + maxvalue * pixelsperstep - width < 0) {
		int x = viewstart + maxvalue * pixelsperstep;
		if (x < 0)
			x = 0;
		dc.DrawRectangle(x, 1, width, height);
	}

	//Hintergrund der Beschriftung
	dc.SetBrush(
			wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW),
					wxSOLID));
	dc.SetPen(
			wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 1,
					wxSOLID));
	dc.DrawRectangle(0, height - captionheight, width, height);

	//Beschriften der Zeitleiste
	dc.SetBackground(*wxTRANSPARENT_BRUSH);
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	dc.SetPen(
			wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), 1,
					wxSOLID));

	//So viele Schritte, wie angezeigt werden können
	for (int i = start_index;
			i <= start_index + 1. / pixelsperstep * width / stepwidth; i++) {

		float x = viewstart + i * stepwidth * pixelsperstep;
		dc.DrawLine(x, 0, x, height - captionheight);
		dc.DrawText(floattowxstr(i * stepwidth, maxdigits), x,
				height - captionheight);
	}

	// Sind markierte Stellen angelegt?
	if (markers != NULL) {
		dc.SetPen(wxColour(255, 100, 0, 255));
		//Alle markierten Stellen einzeichnen
		for (size_t i = 0; i < markers->size(); i++) {
			dc.DrawLine(viewstart + (minvalue + markers->at(i)) * pixelsperstep,
					0, viewstart + (minvalue + markers->at(i)) * pixelsperstep,
					height - captionheight);
		}
	} else {
		cerr << "marker list should not be NULL!" << endl;
	}

	//Einzeichnen des aktuellen Zeitpunkts
	dc.SetPen(*wxGREEN_PEN);
	dc.DrawLine(viewstart + (float) value * pixelsperstep, 0,
			viewstart + (float) value * pixelsperstep, height - captionheight);

	dc.SetTextForeground(*wxGREEN);
	//Ist eine Namensliste vorhanden?
	if (names == NULL) {
		//aktuellen Zeitpunkt mit dem Index beschriften
		dc.DrawText(floattowxstr(value),
				viewstart + (float) value * pixelsperstep, 10);
	} else {
		//Ist die Namensliste korrekt?
		if (int(names->size()) < (maxvalue - minvalue)) {
			cerr << "name vector has wrong size!" << endl;
		} else {
			//Beschriften des aktuellen Zeitpunkts mit Index und Namen
			wxString str = wxString::FromAscii((names->at(value - minvalue)).c_str())
				+ wxT(" (") + floattowxstr(value) + wxT(")");

			int text_width = dc.GetTextExtent(str).x;
			int text_x = viewstart+(float)value*pixelsperstep;

			//Text zu besseren Lesbarkeit verschieben?
			if (text_x+text_width>width) {
				text_x -= text_width+10;
			}

			dc.DrawText(str,text_x,10);
		}
	}
}

int GUITimeline::getValue() {
	return value;
}

int GUITimeline::getMaxValue() {
	return maxvalue;
}

int GUITimeline::getMinValue() {
	return minvalue;
}

void GUITimeline::setValue(int val) {
	value = val;

	//neuer Wert gültig?
	if (value < minvalue) {
		value = minvalue;
		cerr << "Timeline value must not be smaller than the minimum value!"
				<< endl;
	}
	if (value > maxvalue) {
		value = minvalue;
		cerr << "Timeline value must not be smaller than the maximum value!"
				<< endl;
	}

	sendTimelineEvent();
	Refresh(true, NULL);
}

void GUITimeline::setMaxValue(int val) {
	maxvalue = val;
}

void GUITimeline::setMinValue(int val) {
	minvalue = val;
}

void GUITimeline::setNameList(vector<string>* namelist) {
	names = namelist;
	Refresh(false, NULL);
}

void GUITimeline::setMarkerList(vector<int>* mlist) {
	markers = mlist;
	Refresh(false, NULL);
}

void GUITimeline::setMarked(int pos, bool state) {

	// Sind markierte Stellen angelegt?
	if (markers != NULL) {
		//Position des gesuchten Index in der Liste
		vector<int>::iterator p = find(markers->begin(), markers->end(), pos);

		//soll eine Stelle markiert werden und ist es noch nicht?
		if (p == markers->end() && state) {
			markers->resize(markers->size() + 1, pos);
		}

		//soll eine Markierung gelöscht werden und  existiert noch?
		if (p != markers->end() && !state) {
			markers->erase(p);
		}

		//neu Sortieren
		sort(markers->begin(), markers->end());
		Refresh(false, NULL);
	} else {
		cerr << "marker list must not be NULL!" << endl;
	}
}

bool GUITimeline::isMarked(int pos) {

	// Sind markierte Stellen angelegt?
	if (markers != NULL) {
		return (find(markers->begin(), markers->end(), pos) != markers->end());
	} else {
		cerr << "marker list must not be NULL!" << endl;
		return false;
	}
}

void GUITimeline::clearMarkers() {
	markers->clear();
	Refresh(false, NULL);
}

vector<int>* GUITimeline::getMarkers() {
	return markers;
}

void GUITimeline::setMarkers(vector<int>* mlist) {

	// Sind markierte Stellen angelegt?
	if (markers != NULL) {
		//Listengröße für die markierten Stellen anpassen
		markers->resize(mlist->size());

		//kopieren der markierten Stellen
		for (size_t i = 0; i < markers->size(); i++) {
			if (markers->at(i)) {
				markers->at(i) = mlist->at(i);
			}
		}

		//neu Sortieren
		sort(markers->begin(), markers->end());
		Refresh(false, NULL);
	} else {
		cerr << "marker list must not be NULL! (list copy func)" << endl;
	}
}

GUITimeline::~GUITimeline() {

}

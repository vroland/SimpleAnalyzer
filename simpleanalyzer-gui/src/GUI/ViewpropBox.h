/*
 * ViewpropBox.h
 *
 *  Created on: 17.09.2013
 *      Author: Valentin Roland
 */

#ifndef VIEWPROPBOX_H_
#define VIEWPROPBOX_H_
#include <wx/wx.h>
#include <wx/spinctrl.h>

/**
 * @brief Oberfläche zum Verändern/Anzeigen der Visualisierungsoptionen.
 *
 * Diese Klasse verwaltet nur das Layout des Visualisierungsoptionen-Bereichs. Die Funktionalität
 * wird in GUIMainWindow behandelt.
 */
class ViewpropBox: public wxStaticBox {
public:
	/**
	 * Der Konstruktor.
	 * @param parent Die übergeordnete Komponente.
	 */
	ViewpropBox(wxWindow *parent);

	/**
	 * Behandelt Größenänderungen und passt die Positionen der Komponenten an.
	 */
	void resize();

	/**
	 * Gibt das Eingabefeld für die maximal Visualisierte Temperatur (entspricht der Farbe Rot) zurück.
	 */
	wxSpinCtrl* getColorRangeMaxEdit();

	/**
	 * Gibt das Eingabefeld für die minimal Visualisierte Temperatur (entspricht der Farbe Blau) zurück.
	 */
	wxSpinCtrl* getColorRangeMinEdit();

	/**
	 * Gibt das Auswahlfeld für den Darstellungsmodus von Kanten zurück.
	 */
	wxRadioBox* getEdgesCheckBox();

	/**
	 * Gibt das Auswahlfeld für den Darstellungsmodus von Flächen zurück.
	 */
	wxRadioBox* getFacesCheckBox();

	/**
	 * Gibt das Auswahlfeld für die Sichtbarkeit von Materialien zurück.
	 */
	wxCheckListBox* getMatVisibilityListBox();

	/**
	 * Gibt das Auswahlfeld für den Darstellungsmodus von Punkten zurück.
	 */
	wxRadioBox* getPointsCheckBox();

	/**
	 * Gibt die Checkbox zum Anzeigen Extrapolierter Elemente zurück.
	 */
	wxCheckBox* getShowExtrapolatedCheckBox();

	/**
	 * Gibt die Checkbox zum Anzeigen der Sensordaten als Punkte zurück.
	 */
	wxCheckBox* getShowShowSensorData();

	/**
	 * Gibt das Eingabefeld für einen Skalierungsfaktor für das 3D-Objekt zurück. Diese Skalierung ist rein optisch.
	 */
	wxTextCtrl* getViewScaleEdit();

	/**
	 * Der Destruktor.
	 */
	virtual ~ViewpropBox();

private:
	/**
	 * Auswahlfeld für den Darstellungsmodus von Punkten.
	 */
	wxRadioBox* pointsCheckBox;

	/**
	 * Auswahlfeld für den Darstellungsmodus von Kanten.
	 */
	wxRadioBox* edgesCheckBox;

	/**
	 * Auswahlfeld für den Darstellungsmodus von Flächen.
	 */
	wxRadioBox* facesCheckBox;

	/**
	 * Beschriftung für das Auswahlfeld für die Sichtbarkeit von Materialien.
	 */
	wxStaticText* matVisualizationLbl;

	/**
	 * Auswahlfeld für die Sichtbarkeit von Materialien.
	 */
	wxCheckListBox* matVisibilityListBox;

	/**
	 * Checkbox zum Anzeigen Extrapolierter Elemente.
	 */
	wxCheckBox* showExtrapolatedCheckBox;

	/**
	 * Checkbox zum Anzeigen der Sensordaten als Punkte.
	 */
	wxCheckBox* showShowSensorData;

	/**
	 * Beschriftung für die Eingabefelder des zur Visualisierung verwendeten Temperaturbereichs.
	 */
	wxStaticText* colorRangeLbl;

	/**
	 * Eingabefeld für die minimal Visualisierte Temperatur (entspricht der Farbe Blau).
	 */
	wxSpinCtrl* colorRangeMinEdit;

	/**
	 * Eingabefeld für die maximal Visualisierte Temperatur (entspricht der Farbe Rot).
	 */
	wxSpinCtrl* colorRangeMaxEdit;

	/**
	 * Beschriftung für das Eingabefeld eines Skalierungsfaktors für das 3D-Objekt.
	 */
	wxStaticText* viewScaleLbl;

	/**
	 * Eingabefeld für einen Skalierungsfaktor für das 3D-Objekt. Diese Skalierung ist rein optisch.
	 */
	wxTextCtrl* viewScaleEdit;
};

#endif /* VIEWPROPBOX_H_ */

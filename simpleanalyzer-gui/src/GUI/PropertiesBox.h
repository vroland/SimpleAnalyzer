/*
 * PropertiesBox.h
 *
 *  Created on: 07.09.2013
 *      Author: valentin
 */

#ifndef PROPERTIESBOX_H_
#define PROPERTIESBOX_H_

#include <wx/wx.h>
#include "GUITimeline.h"
/**
 * @brief Oberfläche zum Verändern/Anzeigen der Eigenschaften eines Objekts.
 *
 * Diese Klasse verwaltet nur das Layout des Objekteigenschaften-Bereichs. Die Funktionalität
 * wird in GUIMainWindow behandelt.
 */
class PropertiesBox: public wxStaticBox {
public:
	/**
	 * Der Konstruktor.
	 * @param parent Die übergeordnete Komponente.
	 */
	PropertiesBox(wxWindow *parent);

	/**
	 * Behandelt Größenänderungen und passt die Positionen der Komponenten an.
	 */
	void resize();

	/**
	 * Gibt die Checkbox zum markieren des auf der Zeitleiste ausgewählten Zeitpunkts zurück.
	 */
	wxCheckBox* getAnalyzeMarkerCheckBox();

	/**
	 * Gibt die Checkbox zum automatischen Neuberechnen der Temperaturverteilung nach einer Änderung
	 * an den Objekteigenschaften zurück.
	 */
	wxCheckBox* getAutoUpdateCeckBox();

	/**
	 * Gibt den Button zum Löschen aller Markierungen (s. GUITimeline) zurück.
	 */
	wxButton* getClearAnalyzeMarkerBt();

	/**
	 * Gibt das Eingabefeld für die spezifische Wärmekapazität zurück.
	 */
	wxTextCtrl* getSpecificHeatCapEdit();

	/**
	 * Gibt den Index des aktuell ausgewählten Materials zurück.
	 */
	int getCurrentMaterial();

	/**
	 * Setzt den Index des aktuell ausgewählten Materials.
	 * @param index Index des auszuwählenden Materials.
	 */
	void setCurrentMaterial(int index);

	/**
	 * Gibt das Eingabefeld für die Dichte des Materials zurück.
	 */
	wxTextCtrl* getDensityEdit();

	/**
	 * Gibt den Button zum Suchen des maximums zwischen zwei markierten Zeitpunkten (s. GUITimeline) zurück.
	 */
	wxButton* getFindMaxBt();

	/**
	 * Gibt das Auswahlfeld für den zu verwendenden Interpolationsmodus zurück.
	 */
	wxComboBox* getInterpolationModeList();

	/**
	 * Gibt die Auswahlbox für das Material, dessen Eigenschaften angezeigt werden sollen, zurück.
	 */
	wxListBox* getMatListBox();

	/**
	 * Gibt das Eingabefeld für den Materialnamen zurück.
	 */
	wxTextCtrl* getMatNameEdit();

	/**
	 * Gibt den Bereich, der die Materialeigenschaften enthält zurück.
	 */
	wxStaticBox* getMatPropBox();

	/**
	 * Gibt das Eingabefeld für das maximale Tetraedervolumen zurück.
	 */
	wxTextCtrl* getMaxVolumeEdit();

	/**
	 * Gibt den Button zum Auswählen der nächsten Markierung (s. GUITimeline) zurück.
	 */
	wxButton* getNextMarkerBt();

	/**
	 * Gibt das Eingeabefeld für den Objektnamen. zurück.
	 */
	wxTextCtrl* getObjNameEdit();

	/**
	 * Gibt den Button zum Auswählen der vorherigen Markierung (s. GUITimeline) zurück.
	 */
	wxButton* getPrevMarkerBt();

	/**
	 * Gibt das Eingabefeld für die Zerlegungsqualität des Modells (s. Tetgen-dokumentation für weitere Informationen) zurück.
	 */
	wxTextCtrl* getQualityEdit();

	/**
	 * Gibt den Button zum Neuberechnen der Temperaturverteilung zurück.
	 */
	wxButton* getRecalcButton();

	/**
	 * Gibt die Die Zeitleiste für zeitbezogene Sensordaten zurück.
	 */
	GUITimeline* getSdTimeline();

	/**
	 * Gibt das Auswahlfeld für den zu verwendenden Sensordatensatz zurück.
	 */
	wxComboBox* getSensorDataList();

	/**
	 * Gibt die Beschiftungskomponente für die Warnung bei geänderten Objekteigenschaften zurück.
	 */
	wxStaticText* getUpToDateLbl();

	/**
	 * Der Destruktor.
	 */
	virtual ~PropertiesBox();
private:
	/**
	 * Button zum Neuberechnen der Temperaturverteilung.
	 */
	wxButton* recalcButton;

	/**
	 * Beschriftung für das Objektnamen-Eingabefeld.
	 */
	wxStaticText* objNameLbl;

	/**
	 * Eingeabefeld für den Objektnamen.
	 */
	wxTextCtrl* objNameEdit;

	/**
	 * Beschriftung für das Materialnamen-Eingabefeld.
	 */
	wxStaticText* matNameLbl;

	/**
	 * Eingabefeld für den Materialnamen.
	 */
	wxTextCtrl* matNameEdit;

	/**
	 * Beschiftung für die Warnung bei geänderten Objekteigenschaften.
	 */
	wxStaticText* upToDateLbl;

	/**
	 * Beschriftung für das max. Tetraedervolumen-Eingabefeld.
	 */
	wxStaticText* maxVolumeLbl;

	/**
	 * Eingabefeld für das maximale Tetraedervolumen.
	 */
	wxTextCtrl* maxVolumeEdit;

	/**
	 * Beschriftung für das Zerlegungsqualität-Eingabefeld.
	 */
	wxStaticText* qualityLbl;

	/**
	 * Eingabefeld für die Zerlegungsqualität des Modells (s. Tetgen-dokumentation für weitere Informationen).
	 */
	wxTextCtrl* qualityEdit;

	/**
	 * Beschriftung für das Sensordatensatz-Auswahlfeld.
	 */
	wxStaticText* sensorDataLbl;

	/**
	 * Auswahlfeld für den zu verwendenden Sensordatensatz.
	 */
	wxComboBox* sensorDataList;

	/**
	 * Auswahlbox für das Material, dessen Eigenschaften angezeigt werden sollen.
	 */
	wxListBox* matListBox;

	/**
	 * Beschriftung für die Materialauswahl-Box.
	 */
	wxStaticText* matListBoxLbl;

	/**
	 * Bereich, der die Materialeigenschaften enthält.
	 */
	wxStaticBox* matPropBox;

	/**
	 * Auswahlfeld für den zu verwendenden Interpolationsmodus.
	 */
	wxComboBox* interpolationModeList;

	/**
	 * Beschriftung für das Interpolationsmodus-Auswahlfeld.
	 */
	wxStaticText* interpolationModeLbl;

	/**
	 * Eingabefeld für die Dichte des Materials.
	 */
	wxTextCtrl* densityEdit;

	/**
	 * Beschriftung für das Dichte-Eingabefeld.
	 */
	wxStaticText* densityLbl;

	/**
	 * Eingabefeld für die spezifische Wärmekapazität.
	 */
	wxTextCtrl* specificHeatCapEdit;

	/**
	 * Beschriftung für das Wärmekapazitäts-Eingabefeld.
	 */
	wxStaticText* specificHeatCapLbl;

	/**
	 * Die Zeitleiste für zeitbezogene Sensordaten.
	 */
	GUITimeline* sdTimeline;

	/**
	 * Checkbox zum markieren des auf der Zeitleiste ausgewählten Zeitpunkts.
	 * Dieser Zeitpunkt wird dann im Analysedaten-Übersichtsfenster (GUIAnalyzeOutputWindow) angezeigt.
	 */
	wxCheckBox* analyzeMarkerCheckBox;

	/**
	 * Button zum Suchen des maximums zwischen zwei markierten Zeitpunkten (s. GUITimeline).
	 */
	wxButton* findMaxBt;

	/**
	 * Button zum Löschen aller Markierungen (s. GUITimeline).
	 */
	wxButton* clearAnalyzeMarkerBt;

	/**
	 * Button zum Auswählen der nächsten Markierung (s. GUITimeline).
	 */
	wxButton* nextMarkerBt;

	/**
	 * Button zum Auswählen der vorherigen Markierung (s. GUITimeline).
	 */
	wxButton* prevMarkerBt;

	/**
	 * Checkbox zum automatischen Neuberechnen der Temperaturverteilung nach einer Änderung an den Objekteigenschaften.
	 */
	wxCheckBox* autoUpdateCeckBox;

	/**
	 * Index des aktuell ausgewählten Materials.
	 */
	int current_material;
};

#endif /* PROPERTIESBOX_H_ */

/*
 * SimpleAnalyzerApp.h
 *
 *  Created on: 04.09.2013
 *      Author: Valentin Roland
 */

#ifndef GUIAPPLICATION_H_
#define GUIAPPLICATION_H_
#include <wx/wx.h>
#include <vector>
#include "processing/utils.h"
#include "processing/ObjectData.h"
#include "DefaultValues.h"

using namespace std;

/**
 * @brief Regelt den allgemeinen Ablauf des Programms.
 *
 * Eine eigene Anwendungsklasse wird von wxWidgets gefordert. Das zugrunde liegende
 * System organisiert über diese Klasse den Programmablauf (MainLoop) und Events.
 */
class SimpleAnalyzerApp: public wxApp {
public:

	/**
	 * Gibt den Index des aktiven Objekts zurück.
	 * @return Der Index des aktiven Objetks.
	 */
	int getCurrentDataObjectIndex();

	/**
	 * Setzt den Index des aktiven Objekts.
	 * @param currentDataObjectIndex Index des auszuwählenden Objekts.
	 */
	void setCurrentDataObjectIndex(int currentDataObjectIndex);

	/**
	 * Gibt einen Verweis auf die Liste der geladenen Objekte zurück.
	 * @return Pointer zur Liste der geladenen Objekte.
	 */
	vector<ObjectData*>* getDataObjects();
	/**
	 * Gibt einen Verweis auf verwendeten Visualisierungsoptionen zurück.
	 * @return Pointer zu den verwendeten Visualisierungsoptionen.
	 */
	Utils::Visualization_info* getVisualizationInfo();

	/**
	 * Gibt einen Verweis auf das aktuell aktive Objekt zurück.
	 * @return Pointer auf das aktuell aktive Objekt.
	 */
	ObjectData* getActiveObject();

	/**
	 * Fügt ein Objekt zur Objektliste hinzu.
	 * @param obj Das hinzuzufügende Objekt.
	 */
	void addObject(ObjectData* obj);

	/**
	 * Gibt einen Verweis auf die geladenen Standardwerte zurück.
	 * @return Pointer auf das DefaultsValues-Objekt
	 */
	DefaultValues* getDefaults();

	/**
	 * Löscht das aktuelle Objekt aus der Objektliste.
	 */
	void removeCurrentObject();
	/**
	 * Der Destruktor.
	 */
	virtual ~SimpleAnalyzerApp();

private:

	/**
	 * Liste aller geladenen Objekte.
	 */
	vector<ObjectData*> data_objects;

	/**
	 * Index des aktuellen Objekts.
	 */
	int current_data_object_index = -1;

	/**
	 * Die allgemein verwendeten Visualisierungsoptionen.
	 */
	Utils::Visualization_info visualization_info;

	/**
	 * Standardwertobjekt
	 */
	DefaultValues* defaults;

	/**
	 * Wird beim Start der Anwendung ausgeführt und öffnet das Hauptfenster.
	 */
	virtual bool OnInit();
};

//Makro zur Deklaration der Anwendung.
DECLARE_APP(SimpleAnalyzerApp)

#endif /* GUIAPPLICATION_H_ */

/*
 * Importer.h
 *
 *  Created on: 31.08.2013
 *      Author: valentin
 */

#ifndef IMPORTER_H_
#define IMPORTER_H_
#include "../libraries/tetgen/tetgen.h"
#include "../processing/ObjectData.h"
#include <vector>

using namespace std;
/**
 * @brief Importieren von 3D-Modell (.obj) und Sensordaten (.tsd oder .sd).
 */
class Importer {
public:
	/**
	 * Der Konstruktor.
	 */
	Importer();
	/**
	 * Lädt von einfache Sensordaten (ohne Zeit) und Verknüpft
	 * diese mit dem Objekt.
	 */
	ObjectDataStatus LoadSensorData(const char* filename,ObjectData* data);
	/**
	 * Lädt zeitgesteuerte Sensordaten und Verknüpft
	 * diese mit dem Objekt.
	 */
	ObjectDataStatus LoadTimedData(const char* filename,ObjectData* data);
	/**
	 * Lädt Objektdaten aus einer .obj-Datei. Das Objekt
	 * ist zwar schon im Speicher erstellt, wird aber erst durch diese Methode mit Daten gefüllt.
	 */
	ObjectDataStatus ImportObj(const char* filename,ObjectData* data);
	/**
	 * Der Destruktor.
	 */
	virtual ~Importer();
};

#endif /* IMPORTER_H_ */

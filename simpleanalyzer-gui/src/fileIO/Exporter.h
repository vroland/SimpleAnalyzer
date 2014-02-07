/*
 * Exporter.h
 *
 *  Created on: 18.12.2013
 *      Author: valentin
 */

#ifndef EXPORTER_H_
#define EXPORTER_H_

#include "../libraries/tetgen/tetgen.h"
#include "../processing/ObjectData.h"
#include "../processing/utils.h"
using namespace Utils;

/**
 *  @brief Export der gewonnenen Daten
 *
 *  Klasse zum Export der dreidimensionalen Temperaturverteilung als VTK-Datei und
 *  der zweidimensionalen Temperaturverteilung (Schnitt durch das Modell) als .csv-Datei.
 */
class Exporter {
public:
	/**
	 * Der Konstruktor.
	 */
	Exporter();
	/**
	 * Exportiert die aktuell berechnete dreidimensionale Temperaturverteilung und das Modell als VTK-Datei.
	 * @return Der Fehlercode.
	 */
	ObjectData::ObjectDataStatus ExportLegacyVTK(string filename,ObjectData* data);
	/**
	 * Exportiert die zweidimensionale Temperaturverteilung (Schnitt durch das Modell) als csv-Datei.
	 * @return Der Fehlercode.
	 */
	ObjectData::ObjectDataStatus ExportCutCSV(string filename,float* values,CutRender_info* info);
	/**
	 * Der Destruktor.
	 */
	virtual ~Exporter();
};

#endif /* EXPORTER_H_ */

/*
 * Exporter.cpp
 *
 *  Created on: 18.12.2013
 *      Author: Valentin Roland
 */

#include "Exporter.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Exporter::Exporter() {
	CSV_SEPARATOR = ";";
}

//Indices für die Punkte der Flächen eines Tetraeders aus einer Punktliste
const int tetface_indices[4][3] = { { 0, 1, 2 }, { 0, 1, 3 }, { 0, 2, 3 }, { 1,
		2, 3 } };

ObjectData::ObjectDataStatus Exporter::ExportLegacyVTK(string filename,
		ObjectData* data) {

	ofstream file;
	//Öffnen der Ausgabedatei
	file.open(filename);

	//Öffnen erfolgreich?
	if (!file.is_open()) {
		cerr << "could not open file: " << filename << endl;
		//Schreiben der vtk-Datei fehlgeschlagen
		return ObjectData::OD_FAILURE;
	}

	//vtk-Header
	file << "# vtk DataFile Version 3.0" << endl;
	file << "exported from SimpleAnalyzer" << endl;
	file << "ASCII" << endl;
	file << "DATASET POLYDATA" << endl;

	//Variable für die Anzahl der Punkte
	int point_count = 0;
	//Variable für die Anzahl der Tetraeder
	int tetrahedra_count = 0;
	//Anzahl der Materialien
	int material_count = data->getMaterials()->size();

	//Anzahlen der Punkte und Tetraeder ermitteln
	for (int m = 0; m < material_count; m++) {
		tetgenio* io = data->getMaterials()->at(m).tetgenoutput;
		point_count += io->numberofpoints;
		tetrahedra_count += io->numberoftetrahedra;
	}

	//Alle Punkte mit Koordinaten schreiben
	file << "POINTS " << point_count << " float" << endl;
	for (int m = 0; m < material_count; m++) {
		tetgenio* io = data->getMaterials()->at(m).tetgenoutput;
		for (int p = 0; p < io->numberofpoints; p++) {
			file << io->pointlist[p * 3] << " " << io->pointlist[p * 3 + 1]
					<< " " << io->pointlist[p * 3 + 2] << " " << endl;
		}
	}

	//Alle Flächen schreiben
	file << "POLYGONS " << tetrahedra_count * 4 << " "
			<< tetrahedra_count * 4 * 4 << endl;
	//Zähler, der auf die Punktindices addiert wird um Überschneidung zu verhindern
	int material_point_counter = 0;

	//Schreiben der Flächen für alle Materialien
	for (int m = 0; m < material_count; m++) {
		tetgenio* io = data->getMaterials()->at(m).tetgenoutput;

		for (int t = 0; t < io->numberoftetrahedra; t++) {
			for (int k = 0; k < 4; k++) {
				file << 3 << " "
						<< material_point_counter
								+ io->tetrahedronlist[4 * t
										+ tetface_indices[k][0]] << " "
						<< material_point_counter
								+ io->tetrahedronlist[4 * t
										+ tetface_indices[k][1]] << " "
						<< material_point_counter
								+ io->tetrahedronlist[4 * t
										+ tetface_indices[k][2]] << endl;
			}
		}

		//Indexbereich für das nächste Material
		material_point_counter += io->numberofpoints;
	}

	//Schreiben der Temperaturwerte für alle Punkte
	file << "POINT_DATA " << point_count << endl;
	file << "SCALARS temperature float" << endl;
	file << "LOOKUP_TABLE default" << endl;
	for (int m = 0; m < material_count; m++) {
		tetgenio* io = data->getMaterials()->at(m).tetgenoutput;
		for (int p = 0; p < io->numberofpoints; p++) {
			file << io->pointattributelist[p * io->numberofpointattributes]
					<< endl;
		}
	}

	//Schreiben der Materialzugehörigkeit für alle Punkte
	file << "SCALARS material_index int" << endl;
	file << "LOOKUP_TABLE default" << endl;
	for (int m = 0; m < material_count; m++) {
		tetgenio* io = data->getMaterials()->at(m).tetgenoutput;
		for (int p = 0; p < io->numberofpoints; p++) {
			file << m << endl;
		}
	}

	file.close();

	//Schreiben der vtk-Datei erfolgreich.
	return ObjectData::OD_SUCCESS;
}

ObjectData::ObjectDataStatus Exporter::ExportCutCSV(string filename,
		float* values, CutRender_info* info) {

	ofstream file;
	//Öffnen der Ausgabedatei
	file.open(filename);

	//Öffnen erfolgreich?
	if (!file.is_open()) {
		cerr << "could not open file: " << filename << endl;
		//Schreiben der vtk-Datei fehlgeschlagen
		return ObjectData::OD_FAILURE;
	}

	//Allgemeine Informationen bzw. Header
	file << "Schnittebene: " << endl;
	file << "V1: (Mittelpunkt)" << CSV_SEPARATOR << *info->tri->getV1() << endl;
	file << "V2: " << CSV_SEPARATOR << *info->tri->getV2() << endl;
	file << "V3: " << CSV_SEPARATOR << *info->tri->getV3() << endl;
	file << "Schrittweite in mm: " << CSV_SEPARATOR << info->mmperpixel << endl;
	file << "Breite:" << CSV_SEPARATOR << info->img_width << CSV_SEPARATOR
			<< "Höhe:" << CSV_SEPARATOR << info->img_height << endl;
	file << "Temperaturen in K, -1 ist außerhalb des Objekts" << endl;
	file << endl;

	//Schreiben der Werte als Tabelle
	for (int y = 0; y < info->img_height; y++) {
		for (int x = 0; x < info->img_width; x++) {
			cout << values[info->img_width * y + x] << endl;
			if (abs(values[info->img_width * y + x] + 300) > .0001) {
				file << values[info->img_width * y + x] + 273.15
						<< CSV_SEPARATOR;
			} else {
				file << -1 << CSV_SEPARATOR;
			}
		}
		file << endl;
	}

	file.close();

	//.csv-Datei erfolgreich geschrieben
	return ObjectData::OD_SUCCESS;
}

Exporter::~Exporter() {
}


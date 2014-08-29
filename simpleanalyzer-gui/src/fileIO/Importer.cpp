/*
 * Importer.cpp
 *
 *  Created on: 31.08.2013
 *      Author: valentin
 */

#include "Importer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "../processing/ObjectData.h"
using namespace std;

//Pfadseparatorzeichen
#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

Importer::Importer() {

}

/**
 * Extrahiert den Index einer Fläche aus einem Textblock einer Zeile der .obj-Datei.
 * @param data Der zu untersuchende Block.
 * @param withUV Enthält die obj-Datei auch Texturdatenindices?
 * @return Der Flächenindex.
 */
int getFaceIndex(string data,bool withUV) {

	//Enthält die obj-Datei auch Texturdatenindices?
	if (withUV) {
		//Nur den Fächenindex, nicht den UV-Index auslesen
		return int(atof(data.substr(0,data.find("/")).c_str()));
	} else {
		return int(atof(data.c_str()));
	}
}

ObjectData::ObjectDataStatus Importer::ImportObj(string filename,
		ObjectData* data) {

	//Punkte als je 3 Koordinaten
	vector<double> points;
	//Flächen als Punktindices
	vector<vector<int> > faces;
	ifstream file;

	//.obj-Datei öffnen
	file.open(filename);

	//Öffnen erfolgreich?
	if (!file.is_open()) {
		cerr << "could not open file: " << filename << endl;
		//Lesen der Modelldatei fehlgeschlagen
		return ObjectData::OD_FAILURE;
	}

	//aktuelle Zeile
	string line;
	//index des aktuell verarbeiteten Materials (und des zugehörigen Teilobjekts)
	int currentMatIndex = -1;
	//Anzahl der Punkte im Material
	int currentMatPointCount = 0;
	/*
	 * Index des ersten Punkts des aktuellen Materialobjekts bezogen auf die Gesamtliste.
	 * Normalerweise unnötig, da die Punkte pro Teilobjekt lokal definiert werden sollten,
	 * sorgt hier aber notfalls für größere Kompatibilität.
	 */
	int currentMatFirstPoint = 0;

	//Für alle Zeilen...
	while (file.good()) {

		getline(file, line);

		//Position des ersten Leerzeichens
		int firstspace = line.find(" ");
		//String, der den Typ des Inhalts der Zeile enthält
		string type = line.substr(0, firstspace);
		//Daten der Zeile
		string line_data = line.substr(firstspace + 1);

		//Zeilentyp: Vertex?
		if (type == "v") {
			//Vergrößern der Punktliste
			points.resize(points.size() + 3);
			//Positionen der Leerzeichen in den Daten
			int space1 = line_data.find(" ");
			int space2 = line_data.rfind(" ");
			//Auslesen der Punktkoordinaten
			points.at(points.size() - 3) = atof(
					line_data.substr(0, space1).c_str());
			points.at(points.size() - 2) = atof(
					line_data.substr(space1, space2).c_str());
			points.at(points.size() - 1) = atof(
					line_data.substr(space2).c_str());
			currentMatPointCount++;
			continue;
		}

		//Zeilentyp: Face (Fläche)?
		if (type == "f") {

			//ist ein Material festgelegt?
			if (currentMatIndex < 0) {
				cerr << "no material set!" << endl;
			}

			//Anzahl der Leerzeichen zur Bestimmung der Anzahl der Eckpunkte der Flächen
			int spaces = count(line_data.begin(), line_data.end(), ' ');
			//Enthalten die Flächendaten auch Texturkoordnaten-indices?
			bool withUV = (line_data.find("/") < line_data.npos);

			faces.resize(faces.size() + 1);
			//Liste der Eckpunktindices der Fläche
			vector<int>* facepoints = &faces.at(faces.size() - 1);
			facepoints->resize(spaces + 1);

			//Position des letzten Leerzeichens
			int lastpos = 0;

			//Extrahieren der Eckpunktindices aus der Zeile
			for (int i = 0; i <= spaces; i++) {
				//Position des nächsten Leerzeichens
				int pos = line_data.find(" ", lastpos);
				//Extrahieren des Eckpunktindex für den aktuellen Textblock
				facepoints->at(i) = getFaceIndex(line_data.substr(lastpos, pos),
						withUV) - currentMatFirstPoint;
				lastpos = pos + 1;
			}
			continue;
		}

		//Zeilentyp: Materialbibliothek?
		if (type == "mtllib") {
			//Pfad der Materialbibliothek auslesen
			string mtlpath = getTextBlock(line, 1);

			/*
			 * Der Pfad der Materialbibliothek ist relativ zu .obj-Datei
			 * -> Extrahieren des Ordners, in dem die .obj-Datei liegt
			 */
			string filedir = filename;
			//Position des letzten Pfadseparatorzeichens
			size_t lastps = filedir.find_last_of(PATH_SEPARATOR);

			//Wurde ein Pfadseparatorzeichen gefunden?
			if (lastps != string::npos) {
				//Extrahieren des Ordners der .obj-Datei
				filedir = filedir.substr(0, filedir.find_last_of(PATH_SEPARATOR) + 1);
			}

			ifstream mtlfile;
			//Öffnen der Materialbibliothek
			mtlfile.open((filedir + mtlpath).c_str());

			//Öffnen erfolgreich?
			if (!mtlfile.is_open()) {
				cerr << "could not open file: " << mtlpath << endl;
				//Laden der Materialbibliothek und .obj-Datei fehlgeschlagen
				return ObjectData::OD_FAILURE;
			}

			//Verweis auf die Materialdaten des aktuellen Teilobjekts
			ObjectData::MaterialData* currentMat = NULL;

			//Für alle Zeilen
			while (mtlfile.good()) {

				//aktuelle Zeile
				string mtlline;
				getline(mtlfile, mtlline);
				int mtlfirstspace = mtlline.find(" ");
				//String, der den Typ des Inhalts der Zeile enthält
				string mtltype = mtlline.substr(0, mtlfirstspace);
				//Daten der Zeile
				string mtldata = mtlline.substr(mtlfirstspace + 1);

				//Zeilentyp: neues Material?
				if (mtltype == "newmtl") {
					//Vergrößern der Materialliste des Objekts
					data->getMaterials()->resize(
							data->getMaterials()->size() + 1);

					//Setzen der Standardeigenschaften für das Material
					currentMat = &data->getMaterials()->at(
							data->getMaterials()->size() - 1);
					currentMat->name = mtldata;
					currentMat->interpolation_mode = Interpolator::LINEAR;
					currentMat->visible = true;
					currentMat->extrapolated = NULL;
				}

				//Zeilentyp: Materialfarbe
				if (mtltype == "Kd") {

					//Materialfarbe auslesen
					for (int i = 0; i < 3; i++) {
						currentMat->color[i] = atof(
								getTextBlock(mtldata, i).c_str());
					}
				}

				//Zeilentyp: Materialdichte
				if (mtltype == "density") {
					//Materialdichte auslesen
					currentMat->density = atof(
							getTextBlock(mtlline, 1).c_str());
				}

				//Zeilentyp: spezifische Wärmekapazität
				if (mtltype == "spezcap") {
					//spezifische Wärmekapazität auslesen
					currentMat->specificheatcapacity = atof(
							getTextBlock(mtlline, 1).c_str());
				}
			}
			continue;
		}

		//Zeilentyp: use Material? -> Setzen des aktuellen Materials
		if (type == "usemtl") {
			//Name des gesuchten Materials
			string mat_str = getTextBlock(line, 1);

			//Finden des Index des gesuchten Materials
			for (unsigned int i = 0; i < data->getMaterials()->size(); i++) {
				//Material gefunden?
				if (data->getMaterials()->at(i).name == mat_str) {
					//Setzen des aktuellen Materials
					currentMatIndex = i;
					break;
				}

				//Material nicht gefunden?
				if (i == data->getMaterials()->size()) {
					cerr << "No material with name \"" << mat_str << "\" found!"
							<< endl;
				}
			}
			continue;
		}

		//Zeilentyp: Teilobjekt? bzw. Abschluss der Datei
		if (type == "o" || !file.good()) {

			//Enthält das aktuelle Teilobjekt Punkte? -> Datenstuktur für Tetgen erzeugen
			if (currentMatPointCount > 0) {
				//Neue Tetgen-Datenstukur für das Teilobjekt
				tetgenio* io = new tetgenio();

				//Schreiben der Punkte des Teilobjekts
				io->firstnumber = 0;
				io->numberofpoints = currentMatPointCount;
				io->pointlist = new REAL[currentMatPointCount * 3];
				for (int i = 0; i < currentMatPointCount * 3; i++) {
					io->pointlist[i] = points.at(3 * currentMatFirstPoint + i);
				}

				//Schreiben der Flächen des Teilobjekts
				io->numberoffacets = faces.size();
				io->facetlist = new tetgenio::facet[io->numberoffacets];
				for (int i = 0; i < io->numberoffacets; i++) {
					tetgenio::facet* face = &io->facetlist[i];
					face->numberofpolygons = 1;
					face->polygonlist =
							new tetgenio::polygon[face->numberofpolygons];
					face->numberofholes = 0;
					face->holelist = NULL;
					tetgenio::polygon* poly = &face->polygonlist[0];
					vector<int>* subvec = &faces.at(i);
					poly->numberofvertices = subvec->size();
					poly->vertexlist = new int[poly->numberofvertices];
					for (int j = 0; j < poly->numberofvertices; j++) {
						poly->vertexlist[j] = subvec->at(j) - 1;
					}
				}

				//Verknüpfen der Geometriedaten mit dem Objekteigenschaften
				data->getMaterials()->at(currentMatIndex).tetgeninput = io;
			}

			//mitzählen des Anfangspunktindex für das nächste Objekt
			currentMatFirstPoint += currentMatPointCount;
			//Zurücksetzen der Punktanzahl
			currentMatPointCount = 0;
			//Zurücksetzen der Eckpunktindexliste für die Flächen
			faces.clear();
			continue;
		}
	}

	file.close();

	//Objekt und Materialdaten erfolgreich geladen
	return ObjectData::OD_SUCCESS;
}

ObjectData::ObjectDataStatus Importer::LoadSensorData(string filename,
		ObjectData* data) {

	ifstream file;
	//Öffnen der Sensordatendatei
	file.open(filename);

	//Öffnen erfolgreich?
	if (!file.is_open()) {
		cerr << "could not open file: " << filename << endl;
		//Laden der Sensordatendatei fehlgeschalgen
		return ObjectData::OD_FAILURE;
	}

	//Neuen Sensordatensatz anlegen
	data->getSensorDataList()->resize(data->getSensorDataList()->size() + 1);
	//Referenz auf den agelegten Sensordatensatz
	SensorData* sd = &data->getSensorDataList()->at(
			data->getSensorDataList()->size() - 1);

	//Standardeinstellungen für nicht-zeigbezogene Datensätze
	sd->timed = false;
	sd->current_time_index = 0;

	//Position des letzten Pfadseparatorzeichens im Dateipfad
	size_t lastps = filename.find_last_of(PATH_SEPARATOR);

	//Extrahieren des Datensatznamens aus dem Dateinamen
	if (lastps != string::npos) {
		sd->name = filename.substr(filename.find_last_of(PATH_SEPARATOR) + 1,
				filename.size() - filename.find_last_of(PATH_SEPARATOR));
	} else {
		sd->name = filename;
	}

	//Der Datensatz hat nur einen (bzw. keinen bestimmten) Zeitpunkt
	sd->data.resize(1);

	//Referenz auf die Messpunktliste des Datensatzes
	vector<SensorPoint> *points = &sd->data.at(0);

	//Für alle Zeilen der Eingabedatei...
	while (file.good()) {
		//aktuell Zeile
		string line;
		getline(file, line);

		//Typ der aktuellen Zeile
		string type = line.substr(0, 1);

		//Zeilentyp: Sensordaten?
		if (type == "s") {
			points->resize(points->size() + 1);
			SensorPoint* newpoint = &points->at(points->size() - 1);

			//Koordinaten des Messpunkts auslesen
			for (int i = 0; i < 3; i++) {
				newpoint->coords[i] = atof(getTextBlock(line, i + 1).c_str());
			}

			//Wert des Messpunkts auslesen
			newpoint->temperature = atof(getTextBlock(line, 4).c_str());
		}
	}

	//Ist der gerade geladene Datensatz der Erste des Objetks?
	if (data->getCurrentSensorIndex() < 0) {
		//Auswählen als aktiven Datensatz des Objekts
		data->setCurrentSensorIndex(0);
	}

	//Laden der Sensordatendatei erfolgreich
	return ObjectData::OD_SUCCESS;
}
ObjectData::ObjectDataStatus Importer::LoadTimedData(string filename,
		ObjectData* data) {

	ifstream file;
	//Öffnen der Sensordatendatei
	file.open(filename);

	//Öffnen erfolgreich?
	if (!file.is_open()) {
		//Laden der Sensordatendatei fehlgeschalgen
		cerr << "could not open file: " << filename << endl;
		return ObjectData::OD_FAILURE;
	}

	//Neuen Sensordatensatz anlegen
	data->getSensorDataList()->resize(data->getSensorDataList()->size() + 1);
	//Referenz auf den agelegten Sensordatensatz
	SensorData* sd = &data->getSensorDataList()->at(
			data->getSensorDataList()->size() - 1);

	//Standardeinstellungen für zeigbezogene Datensätze
	sd->timed = true;
	sd->current_time_index = 0;

	//Position des letzten Pfadseparatorzeichens im Dateipfad
	size_t lastps = filename.find_last_of(PATH_SEPARATOR);

	//Extrahieren des Datensatznamens aus dem Dateinamen
	if (lastps != string::npos) {
		sd->name = filename.substr(filename.find_last_of(PATH_SEPARATOR) + 1,
				filename.size() - filename.find_last_of(PATH_SEPARATOR));
	} else {
		sd->name = filename;
	}

	//Referenz auf die Messpunktliste des Datensatzes zum aktuellen Zeitpunkt
	vector<SensorPoint>* currentData = NULL;

	//Für alle Zeilen der Eingabedatei...
	while (file.good()) {
		//aktuelle Zeile
		string line;
		getline(file, line);

		//Typ der aktuellen Zeile
		string type = line.substr(0, 1);

		//Zeilentyp: Zeitstempel?
		if (type == "t") {
			//Anlegen eines neuen Unterdatensatzes
			sd->data.resize(sd->data.size() + 1);
			currentData = &sd->data.at(sd->data.size() - 1);
			//Speichern des entsprechenden Zeitstempels
			sd->timestamps.resize(sd->timestamps.size() + 1,
					atoi(getTextBlock(line, 1).c_str()));
		}

		//Zeilentyp: Unterdatensatzname?
		if (type == "n") {
			//Namen für den Datensatz zum aktuellen Zeitpuntk speichern
			sd->subnames.resize(sd->subnames.size() + 1,
					getTextBlock(line, 1).c_str());
		}

		//Zeilentyp: Sensordaten?
		if (type == "s") {
			currentData->resize(currentData->size() + 1);
			SensorPoint* newpoint = &currentData->at(currentData->size() - 1);

			//Koordinaten des Messpunkts auslesen
			for (int i = 0; i < 3; i++) {
				newpoint->coords[i] = atof(getTextBlock(line, i + 1).c_str());
			}

			//Wert des Messpunkts auslesen
			newpoint->temperature = atof(getTextBlock(line, 4).c_str());
		}
	}

	//Ist der gerade geladene Datensatz der Erste des Objetks?
	if (data->getCurrentSensorIndex() < 0) {
		//Auswählen als aktiven Datensatz des Objekts
		data->setCurrentSensorIndex(0);
	}

	//Laden der Sensordatendatei erfolgreich
	return ObjectData::OD_SUCCESS;
}
Importer::~Importer() {
}


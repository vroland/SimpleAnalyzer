/*
 * ObjectData.cpp
 *
 *  Created on: 07.09.2013
 *      Author: Valentin Roland
 */

#include "ObjectData.h"
#include "../SimpleAnalyzerApp.h"
#include "../fileIO/Importer.h"
#include "../processing/MeshProcessor.h"
#include <iostream>
#include <sstream>

using namespace std;

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

ObjectData::ObjectData() {
	/*
	 * Objekteigenschaften, die keine Daten sind initialisieren.
	 * Die Daten werden erst durch das Laden einer Datei (loadFromFile) hinzugefügt.
	 */
	current_sensor_index = -1;
	maxvolume = wxGetApp().getDefaults()->obj_maxvolume;
	quality = wxGetApp().getDefaults()->obj_quality;
}

int ObjectData::loadFromFile(wxString &path) {

	//sind schon Daten mit dem Objekt verknüpft?
	if (materials.size() != 0) {
		return OD_LOAD_ALREADY_LOADED;
	}

	//Ist die angegebene Datei eine .obj-Datei?
	wxString end = _T("obj");
	wxString path_without_name = path.BeforeLast('.');
	if (path.AfterLast('.') != end) {
		return OD_LOAD_INVALID_FILE;
	}

	//Dezimaltrennzeichen auf Punkt setzen
	setlocale(LC_NUMERIC, "C");

	//Objekt importieren
	Importer importer;
	importer.ImportObj(string((path_without_name + _T(".obj")).ToUTF8().data()),
			this);

	//Sensordaten importieren
	int status = importer.LoadSensorData(
			string((path_without_name + _T(".sd")).ToUTF8().data()), this);
	//Fehlschlag? -> Versuchen, zeitgesteuerte Sensordaten zu importieren
	if (status == OD_FAILURE) {
		int status = importer.LoadTimedData(
				string((path_without_name + _T(".tsd")).ToUTF8().data()), this);
		if (status == OD_FAILURE) {
			//Sensordaten konnten nicht geladen werden
			return OD_LOAD_INVALID_SENSOR_FILE;
		}
	}

	//Namen für das Objekt aus dem Dateinamen ermitteln
	wxString wx_filename = path_without_name.AfterLast(PATH_SEPARATOR);
	string obj_name = string(wx_filename.ToAscii());
	string temp_name = obj_name;

	bool unique = false;
	//Anzahl der Durchläufe
	int count = 0;
	//Solange ein Objekt mit dem selben Namen existiert...
	while (!unique) {

		//Existiert bereits ein Objekt mit diesem Namen?
		bool found = false;
		for (unsigned int i = 0; i < wxGetApp().getDataObjects()->size(); i++) {
			if (wxGetApp().getDataObjects()->at(i)->name == temp_name) {
				found = true;
			}
		}

		//Objektnamen einzigartig?
		if (!found) {
			unique = true;
		} else {
			//Objekt eine Nummer zu Identifizierung an den Namen geben
			count++;
			stringstream ss(stringstream::in | stringstream::out);
			ss << obj_name;
			ss << " (" << count << ")";
			temp_name = ss.str();
		}
	}

	name = temp_name;

	//Geometrie und Temperaturverteilung für das Objekt berechnen
	calculateIO();

	return OD_SUCCESS;
}

int ObjectData::addSensorData(wxString &path) {
	Importer importer;
	importer.LoadSensorData(string(path.ToUTF8().data()), this);
	return OD_SUCCESS;
}

int ObjectData::addTimedData(wxString &path) {
	Importer importer;
	importer.LoadTimedData(string(path.ToUTF8().data()), this);
	return OD_SUCCESS;
}

int ObjectData::calculateIO() {

	//Dezimaltrennzeichen auf Punkt setzen
	setlocale(LC_NUMERIC, "C");

	//Für alle Materialien...
	for (unsigned int i = 0; i < materials.size(); i++) {
		MaterialData* data = &materials.at(i);

		//Muss erst alte Geometrie gelöscht werden?
		if (data->tetgenoutput != NULL) {
			delete data->tetgenoutput;
		}

		//Objekteigenschaften gültig?
		if (quality == 0) {
			cerr << "Error: quality is 0! Are object properties set properly?"
					<< endl;
			return OD_FAILURE;
		}

		//Speicherstruktur für die neue Geometrie erzeugen
		data->tetgenoutput = new tetgenio();

		//Die neue Geometrie mit tetgen erzeugen
		stringstream ss(stringstream::in | stringstream::out);
		ss << "Qpq";
		ss << quality;
		ss << "a";
		ss << maxvolume;
		string args = ss.str(); //"Qpq1.414a0.001";
		cout << args << endl;
		tetrahedralize(const_cast<char*>(args.c_str()), data->tetgeninput,
				data->tetgenoutput, NULL, NULL);
	}

	//Die Temperaturverteilung für die neue Geometrie berechnen
	MeshProcessor meshprocessor;
	meshprocessor.process(this);

	return OD_SUCCESS;
}

vector<ObjectData::MaterialData>* ObjectData::getMaterials() {
	return &materials;
}

double ObjectData::getMaxvolume() {
	return maxvolume;
}

void ObjectData::setMaxvolume(double maxvolume) {
	this->maxvolume = maxvolume;
}

string ObjectData::getName() {
	return name;
}

void ObjectData::setName(string name) {
	this->name = name;
}

double ObjectData::getQuality() {
	return quality;
}

void ObjectData::setQuality(double quality) {
	this->quality = quality;
}

vector<SensorData>* ObjectData::getSensorDataList() {
	return &sensorDataList;
}

int ObjectData::getCurrentSensorIndex() {
	return current_sensor_index;
}

void ObjectData::setCurrentSensorIndex(int currentSensorIndex) {
	current_sensor_index = currentSensorIndex;
}

ObjectData::~ObjectData() {
	//für alle Materialien...
	for (unsigned int i = 0; i < materials.size(); i++) {
		//Geometriedaten löschen
		MaterialData* data = &materials.at(i);
		delete data->tetgeninput;
		delete data->tetgenoutput;
	}
}


/*
 * ObjectData.cpp
 *
 *  Created on: 07.09.2013
 *      Author: valentin
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
	current_sensor_index = -1;
	maxvolume = .001;
	quality = 2;
}
int ObjectData::loadFromFile(wxString &path) {
	if (materials.size()!=0) {
		return OD_LOAD_ALREADY_LOADED;
	}
	wxString end = _T("obj");
	wxString path_without_name = path.BeforeLast('.');
	if (path.AfterLast('.')!=end) {
		return OD_LOAD_INVALID_FILE;
	}
	setlocale(LC_NUMERIC, "C");
	Importer importer;
	importer.ImportObj(string((path_without_name+_T(".obj")).ToUTF8().data()),this);
	int status = importer.LoadSensorData(string((path_without_name+_T(".sd")).ToUTF8().data()),this);
	if (status==OD_FAILURE) {
		int status = importer.LoadTimedData(string((path_without_name+_T(".tsd")).ToUTF8().data()),this);
		if (status==OD_FAILURE) {
			return OD_LOAD_INVALID_SENSOR_FILE;
		}
	}
	wxString wxfn = path_without_name.AfterLast(PATH_SEPARATOR);
	string oname = string(wxfn.ToAscii());
	string tname = oname;
	bool unique = false;
	int count = 0;
	while (!unique) {
		bool found = false;
		for (unsigned int i=0;i<wxGetApp().getDataObjects()->size();i++) {
			if (wxGetApp().getDataObjects()->at(i)->name==tname) {
				found = true;
			}
		}
		if (!found) {
			unique = true;
		} else {
			count++;
			stringstream ss (stringstream::in | stringstream::out);
			ss <<oname;
			ss <<" ("<<count<<")";
			tname =ss.str();
		}
	}
	name = tname;
	calculateIO();
	return OD_SUCCESS;
}
int ObjectData::addSensorData(wxString &path) {
	Importer importer;
	importer.LoadSensorData(string(path.ToUTF8().data()),this);
	return OD_SUCCESS;
}
int ObjectData::addTimedData(wxString &path) {
	Importer importer;
	importer.LoadTimedData(string(path.ToUTF8().data()),this);
	return OD_SUCCESS;
}
int ObjectData::calculateIO() {
	setlocale(LC_NUMERIC, "C");
	for (unsigned int i=0;i<materials.size();i++) {
		MaterialData* data = &materials.at(i);
		if (data->tetgenoutput!=NULL) {
			delete data->tetgenoutput;
		}
		if (quality==0) {
			cerr << "Error: quality is 0! Are object properties set properly?" << endl;
			return OD_FAILURE;
		}
		data->tetgenoutput = new tetgenio();
		stringstream ss (stringstream::in | stringstream::out);
		ss <<"Qpq1.414a";
		ss << maxvolume;
		string args =ss.str();//"Qpq1.414a0.001";
		tetrahedralize(const_cast<char*> (args.c_str()), data->tetgeninput, data->tetgenoutput,NULL,NULL);
	}
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
	for (unsigned int i=0;i<materials.size();i++) {
			MaterialData* data = &materials.at(i);
			delete data->tetgeninput;
			delete data->tetgenoutput;
	}
}


/*
 * ObjectData.cpp
 *
 *  Created on: 07.09.2013
 *      Author: valentin
 */

#include "ObjectData.h"
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
extern std::vector<ObjectData*> data_objects;

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
	importer.ImportObj((path_without_name+_T(".obj")).ToAscii(),&materials);
	int status = importer.LoadSensorData((path_without_name+_T(".sd")).ToAscii(),this);
	if (status==OD_FAILURE) {
		int status = importer.LoadTimedData((path_without_name+_T(".tsd")).ToAscii(),this);
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
		for (unsigned int i=0;i<data_objects.size();i++) {
			if (data_objects.at(i)->name==tname) {
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
	importer.LoadSensorData(path.ToAscii(),this);
	return OD_SUCCESS;
}
int ObjectData::addTimedData(wxString &path) {
	Importer importer;
	importer.LoadTimedData(path.ToAscii(),this);
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
		cout << data->tetgenoutput->numberoftrifaces << endl;
	}
	MeshProcessor meshprocessor;
	meshprocessor.process(this);
	return OD_SUCCESS;
}
ObjectData::~ObjectData() {
	for (unsigned int i=0;i<materials.size();i++) {
			MaterialData* data = &materials.at(i);
			delete data->tetgeninput;
			delete data->tetgenoutput;
	}
}


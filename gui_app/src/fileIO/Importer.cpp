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

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

Importer::Importer() {

}
int getFaceIndex(string data,bool withUV) {
	if (withUV) {
		return int(atof(data.substr(0,data.find("/")).c_str()));
	} else {
		return int(atof(data.c_str()));
	}
}
string getTextBlock(string data,int n) { //Gibt den n-ten durch Leerzeichen abgetrennten Block zurück
	int pos = 0;
	int prevPos = 0;
	size_t curPos = 0;
	curPos = data.find(" ");
	if (curPos==data.npos) {
		return "";
	}
	pos = curPos;
	for (int i=0;i<n;i++) {
		curPos = data.find(" ",pos+1);
		if (curPos==data.npos) {
			if (n-i>1) {	//Mehr Blöcke als vorhanden nachgefragt
				return "";
			} else {
				prevPos = pos+1;
				pos = data.size();
				break;
			}
		}
		prevPos = pos+1;
		pos = curPos;

	}
	return data.substr(prevPos,pos-(prevPos));
}

int Importer::ImportObj(const char* filename,vector<MaterialData>* matDataList) {
	// read data //

	vector<double> points;
	vector<vector<int> > faces;
	ifstream file;
	file.open(filename);
	if (!file.is_open()) {
		cerr << "could not open file: "<<filename<<endl;
		return OD_FAILURE;
	}
	string line;
	int currentMatIndex = -1;
	int currentMatPointCount = 0;
	int currentMatFirstPoint = 0;
	while (file.good()) {
		getline(file,line);
		int firstspace = line.find(" ");
		string type = line.substr(0,firstspace);
		string data = line.substr(firstspace+1);
		if (type=="v") {
			points.resize(points.size()+3);
			int space1 = data.find(" ");
			int space2 = data.rfind(" ");
			points.at(points.size()-3) = atof(data.substr(0,space1).c_str());
			points.at(points.size()-2) = atof(data.substr(space1,space2).c_str());
			points.at(points.size()-1) = atof(data.substr(space2).c_str());
			currentMatPointCount++;
			continue;
		}
		if (type=="f") {
			if (currentMatIndex<0) {
				cout << "no material set!" << endl;
			}
			int spaces = count(data.begin(),data.end(),' ');
			bool withUV = (data.find("/")<data.npos);
			faces.resize(faces.size()+1);
			vector<int>* subvec = &faces.at(faces.size()-1);
			subvec->resize(spaces+1);
			int lastpos = 0;
			for (int i=0;i<=spaces;i++) {
				int pos = data.find(" ",lastpos);
				subvec->at(i) = getFaceIndex(data.substr(lastpos,pos),withUV)-currentMatFirstPoint;
				lastpos = pos+1;
			}
			continue;
		}
		if (type=="mtllib") {
			string mtlpath = getTextBlock(line,1);
			ifstream mtlfile;
			string fn = string(filename);
			string filedir = "";
			size_t lastps = fn.find_last_of(PATH_SEPARATOR);
			if (lastps!=fn.npos) {
				filedir = fn.substr(0,fn.find_last_of(PATH_SEPARATOR)+1);
			}
			cout << (filedir+mtlpath).c_str() << endl;
			mtlfile.open((filedir+mtlpath).c_str());
			if (!mtlfile.is_open()) {
				cerr << "could not open file: "<<mtlpath<<endl;
				return OD_FAILURE;
			}
			MaterialData* currentMat = NULL;
			while (mtlfile.good()) {
				string mtlline;
				getline(mtlfile,mtlline);
				int mtlfirstspace = mtlline.find(" ");
				string mtltype = mtlline.substr(0,mtlfirstspace);
				string mtldata = mtlline.substr(mtlfirstspace+1);
				if (mtltype=="newmtl") {
					matDataList->resize(matDataList->size()+1);
					currentMat = &matDataList->at(matDataList->size()-1);
					currentMat->name = mtldata;
					currentMat->interpolation_mode = LINEAR;
					currentMat->visible = true;
					currentMat->extrapolated = NULL;
				}
				if (mtltype=="Kd") {
					for (int i=0;i<3;i++) {
						currentMat->color[i] = atof(getTextBlock(mtldata,i).c_str());
					}
				}
				if (mtltype=="density") {
					currentMat->density = atof(getTextBlock(mtlline,1).c_str());
				}
				if (mtltype=="spezcap") {
					currentMat->specificheatcapacity = atof(getTextBlock(mtlline,1).c_str());
				}
			}
			continue;
		}
		if (type=="usemtl") {
			string mat_str = getTextBlock(line,1);
			for (unsigned int i=0;i<matDataList->size();i++) {
				if (matDataList->at(i).name==mat_str) {
					currentMatIndex = i;
					break;
				}
			}
			continue;
		}
		if (type=="o" || !file.good()) {	// Zeichen "o" oder Dateiende
			if (currentMatPointCount>0) { //Objekt beenden
				// Build tetgenio //
				tetgenio* io = new tetgenio();
				io->firstnumber = 0;
				io->numberofpoints = currentMatPointCount;
				io->pointlist = new REAL[currentMatPointCount*3];
				for (int i=0;i<currentMatPointCount*3;i++) {
					io->pointlist[i] = points.at(3*currentMatFirstPoint+i);
				}
				io->numberoffacets = faces.size();
				io->facetlist = new tetgenio::facet[io->numberoffacets];
				for (int i=0;i<io->numberoffacets;i++) {
					tetgenio::facet* face = &io->facetlist[i];
					face->numberofpolygons = 1;
					face->polygonlist = new tetgenio::polygon[face->numberofpolygons];
					face->numberofholes = 0;
					face->holelist = NULL;
					tetgenio::polygon* poly = &face->polygonlist[0];
					vector<int>* subvec = &faces.at(i);
					poly->numberofvertices = subvec->size();
					poly->vertexlist = new int[poly->numberofvertices];
					for (int j=0;j<poly->numberofvertices;j++) {
						poly->vertexlist[j] = subvec->at(j)-1;
					}
				}
				matDataList->at(currentMatIndex).tetgeninput = io;
			}

			currentMatFirstPoint += currentMatPointCount;
			currentMatPointCount = 0;
			faces.clear();
			continue;
		}
	}
	file.close();
	return OD_SUCCESS;
}
int Importer::LoadSensorData(const char* filename,ObjectData* data) {
	ifstream file;
	file.open(filename);
	if (!file.is_open()) {
		cerr << "could not open file: "<<filename<<endl;
		return OD_FAILURE;
	}
	string line;

	data->sensordatalist.resize(data->sensordatalist.size()+1);
	SensorData* sd = &data->sensordatalist.at(data->sensordatalist.size()-1);
	sd->timed = false;
	sd->current_time_index = 0;
	string fn = string(filename);
	size_t lastps = fn.find_last_of(PATH_SEPARATOR);
	if (lastps!=fn.npos) {
		sd->name = fn.substr(fn.find_last_of(PATH_SEPARATOR)+1,fn.size()-fn.find_last_of(PATH_SEPARATOR));
	} else {
		sd->name = fn;
	}
	sd->data.resize(1);
	vector<SensorPoint> *points = &sd->data.at(0);
	while (file.good()) {
		getline(file,line);
		string type = line.substr(0,1);
		if (type=="s") {	//Sensordaten
			points->resize(points->size()+1);
			SensorPoint* newpoint = &points->at(points->size()-1);
			for (int i=0;i<3;i++) {
				newpoint->coords[i]= atof(getTextBlock(line,i+1).c_str());
			}
			newpoint->temperature = atof(getTextBlock(line,4).c_str());
		}
	}
	if (data->current_sensor_index<0) {
		data->current_sensor_index = 0;
	}
	return OD_SUCCESS;
}
int Importer::LoadTimedData(const char* filename,ObjectData* data) {
	ifstream file;
	file.open(filename);
	if (!file.is_open()) {
		cout << "could not open file: "<<filename<<endl;
		return OD_FAILURE;
	}
	string line;
	data->sensordatalist.resize(data->sensordatalist.size()+1);
	SensorData* sd = &data->sensordatalist.at(data->sensordatalist.size()-1);
	sd->timed = true;
	sd->current_time_index = 0;
	string fn = string(filename);
	size_t lastps = fn.find_last_of(PATH_SEPARATOR);
	if (lastps!=fn.npos) {
		sd->name = fn.substr(fn.find_last_of(PATH_SEPARATOR)+1,fn.size()-fn.find_last_of(PATH_SEPARATOR));
	} else {
		sd->name = fn;
	}
	vector<SensorPoint>* currentData = NULL;
	while (file.good()) {
		getline(file,line);
		string type = line.substr(0,1);
		if (type=="t") { 	//Zeitstempel
			sd->data.resize(sd->data.size()+1);
			currentData = &sd->data.at(sd->data.size()-1);
			sd->timestamps.resize(sd->timestamps.size()+1,atoi(getTextBlock(line,1).c_str()));
		}
		if (type=="n") { 	//Zeitstempel
			sd->subnames.resize(sd->subnames.size()+1,getTextBlock(line,1).c_str());
		}
		if (type=="s") {	//Sensordaten
			currentData->resize(currentData->size()+1);
			SensorPoint* newpoint = &currentData->at(currentData->size()-1);
			for (int i=0;i<3;i++) {
				newpoint->coords[i]= atof(getTextBlock(line,i+1).c_str());
			}
			newpoint->temperature = atof(getTextBlock(line,4).c_str());
		}
	}
	if (data->current_sensor_index<0) {
		data->current_sensor_index = 0;
	}
	return OD_SUCCESS;
}
Importer::~Importer() {
}


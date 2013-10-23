/*
 * MeshProcessor.cpp
 *
 *  Created on: 31.08.2013
 *      Author: valentin
 */

#include "MeshProcessor.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "../libraries/interpolate/Interpolator.h"

using namespace std;

MeshProcessor::MeshProcessor() {

}
void interpolatePoint(MaterialData* data,vector<SensorPoint>* sensorpoints,int pointIndex,Interpolator* interpolator) {
	int found = 0;
	double interval = getPointValue(found,sensorpoints,&data->tetgenoutput->pointlist[3*pointIndex],interpolator);
	data->tetgenoutput->pointattributelist[data->tetgenoutput->numberofpointattributes*pointIndex] = interval;
	data->extrapolated[pointIndex] = (found<1);
}
void MeshProcessor::process(ObjectData* object) {
	for (unsigned int i=0;i<object->materials.size();i++) {
		MaterialData* mat = &object->materials.at(i);
		tetgenio* io = mat->tetgenoutput;

		if (mat->tetgenoutput->numberofpointattributes!=0) {
			delete mat->tetgenoutput->pointattributelist;
		}
		if (mat->extrapolated!=NULL) {
			delete[] mat->extrapolated;
		}
		mat->extrapolated = new bool[mat->tetgenoutput->numberofpoints];
		mat->tetgenoutput->numberofpointattributes = NUMBEROFSENSORATTRIBUTES;
		mat->tetgenoutput->pointattributelist = new REAL[NUMBEROFSENSORATTRIBUTES*mat->tetgenoutput->numberofpoints];
		//Interpolieren
		Interpolator interpolator;
		interpolator.setMode(mat->interpolation_mode);
		for (int i=0;i<io->numberofpoints;i++) {
			interpolatePoint(mat,&object->sensordatalist.at(object->current_sensor_index).points,i,&interpolator);
		}
	}
}
MeshProcessor::~MeshProcessor() {
}


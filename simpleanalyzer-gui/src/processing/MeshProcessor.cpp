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

void interpolatePoint(ObjectData::MaterialData* data,
		vector<SensorPoint>* sensorpoints, int pointIndex,
		Interpolator* interpolator) {

	//Wert des Punktes ermitteln
	int found = 0;
	double interval = getPointValue(found, sensorpoints,
			&data->tetgenoutput->pointlist[3 * pointIndex], interpolator);

	//Wert und Status als Punktattribute ablegen
	data->tetgenoutput->pointattributelist[data->tetgenoutput->numberofpointattributes
			* pointIndex] = interval;
	data->extrapolated[pointIndex] = (found < 1);
}

void MeshProcessor::process(ObjectData* object) {

	//Statusinformationen ausgeben
	cout << "calculating temperature distribution for \"" << object->getName()
			<< "..." << endl;

	//Für alle Materialien...
	for (unsigned int i = 0; i < object->getMaterials()->size(); i++) {
		ObjectData::MaterialData* mat = &object->getMaterials()->at(i);
		tetgenio* io = mat->tetgenoutput;

		//ist es nötig, die Punktattributliste freizugeben?
		if (mat->tetgenoutput->numberofpointattributes != 0) {
			delete mat->tetgenoutput->pointattributelist;
		}

		//ist es nötig, die Extrapolationsstatusliste freizugeben?
		if (mat->extrapolated != NULL) {
			delete[] mat->extrapolated;
		}

		//Extrapolationsstatusliste neu erstellen
		mat->extrapolated = new bool[mat->tetgenoutput->numberofpoints];
		//Punktattributliste neu erstellen
		mat->tetgenoutput->numberofpointattributes = NUMBEROFSENSORATTRIBUTES;
		mat->tetgenoutput->pointattributelist =
				new REAL[NUMBEROFSENSORATTRIBUTES
						* mat->tetgenoutput->numberofpoints];


		Interpolator interpolator;
		interpolator.setMode(mat->interpolation_mode);

		//Ermitteln der Werte für alle Punkte...
		for (int j = 0; j < io->numberofpoints; j++) {

			//Statusinformationen ausgeben
			cout << "\rmaterial " << i + 1 << ": " << j + 1 << "/"
					<< io->numberofpoints << endl;

			SensorData* sd = &object->getSensorDataList()->at(
					object->getCurrentSensorIndex());
			interpolatePoint(mat, &sd->data.at(sd->current_time_index), j,
					&interpolator);
		}

		cout << endl;
	}
}

MeshProcessor::~MeshProcessor() {
}


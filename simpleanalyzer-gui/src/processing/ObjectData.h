/*
 * ObjectData.h
 *
 *  Created on: 07.09.2013
 *      Author: valentin
 */

#ifndef OBJECTDATA_H_
#define OBJECTDATA_H_

#include "../libraries/tetgen/tetgen.h"
#include <vector>
#include "../libraries/interpolate/Interpolator.h"
#include <wx/string.h>
#include "utils.h"
#define NUMBEROFSENSORATTRIBUTES 1
using namespace std;
using namespace Utils;
enum ObjectDataStatus {
	OD_SUCCESS = 1,
	OD_FAILURE,
	OD_LOAD_ALREADY_LOADED,
	OD_LOAD_INVALID_FILE,
	OD_LOAD_INVALID_SENSOR_FILE,
};

struct MaterialData {
	string name;
	InterpolationMode interpolation_mode;
	tetgenio* tetgeninput;
	tetgenio* tetgenoutput;
	bool* extrapolated;
	float color[3];
	double density;
	double specificheatcapacity;
	bool visible;
};

class ObjectData {
public:
	ObjectData();
	int loadFromFile(wxString &path);
	int addSensorData(wxString &path);
	int addTimedData(wxString &path);
	int calculateIO();
	int current_sensor_index;
	string name;
	//Properties:
	double maxvolume;
	double quality;
	vector<MaterialData> materials;
	vector<SensorData> sensordatalist;
	virtual ~ObjectData();
};

#endif /* OBJECTDATA_H_ */

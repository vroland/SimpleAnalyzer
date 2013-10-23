/*
 * Importer.h
 *
 *  Created on: 31.08.2013
 *      Author: valentin
 */

#ifndef IMPORTER_H_
#define IMPORTER_H_
#include "../libraries/tetgen/tetgen.h"
#include "../processing/ObjectData.h"
#include <vector>

using namespace std;

class Importer {
public:
	Importer();
	int LoadSensorData(const char* filename,ObjectData* data);
	int ImportObj(const char* filename,vector<MaterialData>* matDataList);
	virtual ~Importer();
};

#endif /* IMPORTER_H_ */

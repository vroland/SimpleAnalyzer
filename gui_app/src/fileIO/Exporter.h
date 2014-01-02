/*
 * Exporter.h
 *
 *  Created on: 18.12.2013
 *      Author: valentin
 */

#ifndef EXPORTER_H_
#define EXPORTER_H_

#include "../libraries/tetgen/tetgen.h"
#include "../processing/ObjectData.h"
#include "../processing/utils.h"
using namespace Utils;
class Exporter {
public:
	Exporter();
	int ExportLegacyVTK(string filename,ObjectData* data);
	int ExportCutCSV(string filename,float* values,CutRender_info* info);
	virtual ~Exporter();
};

#endif /* EXPORTER_H_ */

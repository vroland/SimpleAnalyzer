/*
 * Analyzer.h
 *
 *  Created on: 11.09.2013
 *      Author: valentin
 */

#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <vector>
#include <string>
#include <iostream>
#include "ObjectData.h"

using namespace std;


struct AnalyzerData_material {
	string name;
	double volume;
	double heat_energy;
};
struct AnalyzerData_dataset {
	string name;
	double heat_energy;
	vector<AnalyzerData_material> mat_data;
};
struct AnalyzerData_object {
	double volume;
	vector<AnalyzerData_dataset> data_sets;
};
struct AnalyzerData_point {
	double value;
	bool extrapolated;
};
std::ostream &operator<< (std::ostream &out, const AnalyzerData_object &data);

class Analyzer {
public:
	Analyzer();
	void analyzeObject(ObjectData* obj,AnalyzerData_object* out,bool use_markers = true,int sdindex = -1);
	void analyzePoint(ObjectData* obj,Vector3D* point,AnalyzerData_point* point_data,Interpolator* interpolator);
	virtual ~Analyzer();
};

#endif /* ANALYZER_H_ */

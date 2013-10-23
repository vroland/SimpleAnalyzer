/*
 * Analyzer.h
 *
 *  Created on: 11.09.2013
 *      Author: valentin
 */

#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <vector>
#include <iostream>
#include "ObjectData.h"

using namespace std;

struct AnalyzerData_object {
	double volume;
	vector<double> heat_energys;		//different sensor data sets
	vector<vector<double> > material_volumes;
	vector<vector<double> > material_heat_energies;
};
struct AnalyzerData_point {
	double value;
	bool extrapolated;
};
std::ostream &operator<< (std::ostream &out, const AnalyzerData_object &data);

class Analyzer {
public:
	Analyzer();
	void analyzeObject(ObjectData* obj,AnalyzerData_object* out);
	void analyzePoint(ObjectData* obj,Vector3D* point,AnalyzerData_point* point_data,Interpolator* interpolator);
	virtual ~Analyzer();
};

#endif /* ANALYZER_H_ */

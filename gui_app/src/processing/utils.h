/*
 * utils.h
 *
 *  Created on: 21.09.2013
 *      Author: valentin
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <wx-2.8/wx/wx.h>
#include "../libraries/tetgen/tetgen.h"
#include "../libraries/interpolate/GeometryClasses.h"
#include "../libraries/interpolate/Interpolator.h"
using namespace std;

namespace Utils {

	inline double sqr(double d) {
		return d*d;
	}
	inline double getDistance_d(double* p1,double* p2) {
		return sqrt(sqr(p1[0]-p2[0])+sqr(p1[1]-p2[1])+sqr(p1[2]-p2[2]));
	}
	struct SortStruct {
		double distance;
		int pointIndex;
	};
	struct SensorPoint {
		double coords[3];
		//Attribute
		double temperature;
	};
	struct SensorData {
		vector<vector<SensorPoint> > data;
		vector<string> subnames;
		vector<int> timestamps;
		bool timed;
		int current_time_index;
		string name;
	};
	struct SensorPointComparator {
		double meshpoint[3];
		bool operator() (SensorPoint p1,SensorPoint p2) {return (getDistance_d(meshpoint,p1.coords)<getDistance_d(meshpoint,p2.coords));}
	};
	wxString floattowxstr(double val);
	wxString floattowxstr(double val,int digits);
	int rayIntersectsTriangle(Vector3D* p,Vector3D* direction,Triangle* tri);
	int pointInsideMesh(Vector3D* p,tetgenio* io);
	int pointInsideTetrahedron(Vector3D* pges, Vector3D* v1, Vector3D* v2, Vector3D* v3, Vector3D* v4);
	int pointInsideTetrahedron(double* pges, double* v1, double* v2, double* v3, double* v4);
	int pointInsideTetrahedron(double* p,vector<SensorPoint*>* tet);
	void nextCombination(vector<int>* indices,int depth,int dataPointCount);
	double getPointValue(int &status,vector<SensorPoint>* sensorpoints,double* p,Interpolator* interpolator);
	float* hsvToRgb(float h, float s, float v);
}

#endif /* UTILS_H_ */

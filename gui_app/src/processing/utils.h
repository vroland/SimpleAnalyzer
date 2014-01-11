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
#include <wx/wx.h>
#include "../libraries/tetgen/tetgen.h"
#include "../libraries/interpolate/GeometryClasses.h"
#include "../libraries/interpolate/Interpolator.h"
using namespace std;

namespace Utils {

	enum PIM_algorithm {
		ALGORITHM_TETRAHEDRONS = 0,
		ALGORITHM_RAY,
	};
	inline double sqr(double d) {
		return d*d;
	}
	inline double getDistance_d(double* p1,double* p2) {
		return sqrt(sqr(p1[0]-p2[0])+sqr(p1[1]-p2[1])+sqr(p1[2]-p2[2]));
	}
	struct Visualization_info {
		int max_visualisation_temp = 100;
		int min_visualisation_temp = 0;
	};

	struct SortStruct {
		double distance;
		int pointIndex;
	};
	float clampHue(float h);
	struct SensorPoint {
		double coords[3];
		//Attribute
		double temperature;
	};
	struct CutRender_info {
		Triangle* tri;
		float mmperpixel;
		int img_width;
		int img_height;
		PIM_algorithm in_volume_algorithm;
	};
	struct SensorData {
		vector<vector<SensorPoint> > data;
		vector<string> subnames;
		vector<int> timestamps;
		vector<int> markers;
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
	int rayIntersectsTriangle(Vector3D* p,Vector3D* direction,Triangle* tri,double* depth);
	int pointInsideMesh(Vector3D* p,tetgenio* io,PIM_algorithm algorithm);
	int pointInsideTetrahedron(Vector3D* pges, Vector3D* v1, Vector3D* v2, Vector3D* v3, Vector3D* v4);
	int pointInsideTetrahedron(double* pges, double* v1, double* v2, double* v3, double* v4);
	int pointInsideTetrahedron(double* p,vector<SensorPoint*>* tet);
	void nextCombination(vector<int>* indices,int depth,int dataPointCount);
	double getPointValue(int &status,vector<SensorPoint>* sensorpoints,double* p,Interpolator* interpolator,vector<SensorPoint*>* prev_tet = NULL,vector<SensorPoint*>* current_tet = NULL);
	float* hsvToRgb(float h, float s, float v);
	void copySensorPoint(SensorPoint* from,SensorPoint* to);
}

#endif /* UTILS_H_ */

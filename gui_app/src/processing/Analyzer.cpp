/*
 * Analyzer.cpp
 *
 *  Created on: 11.09.2013
 *      Author: valentin
 */

#include "Analyzer.h"
#include <cmath>
#include "MeshProcessor.h"
#include "../libraries/interpolate/Interpolator.h"
#include "utils.h"
using namespace Utils;
Analyzer::Analyzer() {

}
SensorPointComparator spcomparator;
void Analyzer::analyzeObject(ObjectData* obj,AnalyzerData_object* out) {
	out->volume = 0;
	out->heat_energys.resize(obj->sensordatalist.size());
	for (unsigned int i=0;i<obj->sensordatalist.size();i++) {
		out->heat_energys.at(i) = 0;
	}
	out->material_volumes.resize(obj->sensordatalist.size());
	out->material_heat_energies.resize(obj->sensordatalist.size());
	for (unsigned int s=0;s<obj->sensordatalist.size();s++) {
		out->material_volumes.at(s).resize(obj->materials.size());
		for (unsigned int i=0;i<obj->materials.size();i++) {
			out->material_volumes.at(s).at(i) = 0;
		}
		out->material_heat_energies.at(s).resize(obj->materials.size());
		for (unsigned int i=0;i<obj->materials.size();i++) {
			out->material_heat_energies.at(s).at(i) = 0;
		}
	}
	int original_sd_index = obj->current_sensor_index;
	MeshProcessor processor;
	for (unsigned int s=0;s<obj->sensordatalist.size();s++) {
		out->volume = 0;
		obj->current_sensor_index = s;
		processor.process(obj);
		for (unsigned int i=0;i<obj->materials.size();i++) {
			MaterialData* mat = &obj->materials.at(i);
			double matvolume = 0;
			double matenergy = 0;
			for (int j=0;j<mat->tetgenoutput->numberoftetrahedra;j++) {
				int* indices = &mat->tetgenoutput->tetrahedronlist[4*j];
				Vector3D v1 = Vector3D(&mat->tetgenoutput->pointlist[3*indices[0]]);
				Vector3D v2 = Vector3D(&mat->tetgenoutput->pointlist[3*indices[1]]);
				Vector3D v3 = Vector3D(&mat->tetgenoutput->pointlist[3*indices[2]]);
				Vector3D v4 = Vector3D(&mat->tetgenoutput->pointlist[3*indices[3]]);
				v2.sub(&v1);
				v3.sub(&v1);
				v4.sub(&v1);
				Vector3D* vvec = v2.crossProduct(&v3);
				double volume = 1./6.*abs(vvec->dotProduct(&v4));
				matvolume +=volume;
				delete vvec;
				double averagetemp = 0;
				for (int k=0;k<4;k++) {
					averagetemp+=mat->tetgenoutput->pointattributelist[mat->tetgenoutput->numberofpointattributes*indices[k]];
				}
				averagetemp /= 4.;
				matenergy += averagetemp*volume*mat->density*mat->specificheatcapacity;

			}
			out->material_volumes.at(s).at(i) = matvolume;
			out->material_heat_energies.at(s).at(i) = matenergy;
			out->volume += matvolume;
			out->heat_energys.at(s) += matenergy;
		}
	}
	if (obj->current_sensor_index!=original_sd_index) {
		obj->current_sensor_index = original_sd_index;
		processor.process(obj);
	}
}

void Analyzer::analyzePoint(ObjectData* obj,Vector3D* point,AnalyzerData_point* point_data,Interpolator* interpolator) {
	vector<SensorPoint>* sensorpoints = &obj->sensordatalist.at(obj->current_sensor_index).points;
	int found = 0;
	double interval = getPointValue(found,sensorpoints,point->getXYZ(),interpolator);
	point_data->value = interval;
	point_data->extrapolated = (found<1);
}
std::ostream &operator<< (std::ostream &out, const AnalyzerData_object &data) {
    out << "object analysis:"<<endl;
    out << "object volume: "<<data.volume<<" (m³)"<<endl;
    for (unsigned int s=0;s<data.material_heat_energies.size();s++) {
    	out << "sensor data set "<<s<<": "<<endl;
		out << "object energy: "<<data.heat_energys.at(s)<<" (J)"<<endl;
		for (unsigned int i=0;i<data.material_heat_energies.size();i++) {
			out << "Material "<<i<<":"<<endl;
			out << "volume: "<<data.material_volumes.at(s).at(i)<<" (m³)"<<endl;
			out << "energy: "<<data.material_heat_energies.at(s).at(i)<<" (kJ)"<<endl;
		}
		out << endl;
    }
    cout << endl;
    return out;
}
Analyzer::~Analyzer() {
}


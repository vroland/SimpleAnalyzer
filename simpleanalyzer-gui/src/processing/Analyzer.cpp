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
void Analyzer::analyzeObject(ObjectData* obj,AnalyzerData_object* out,bool use_markers,int sdindex) {
	out->volume = 0;
	int original_sd_index = obj->getCurrentSensorIndex();
	bool sd_time_changed = false;
	MeshProcessor processor;
	for (unsigned int s=(sdindex>-1)?sdindex:0;s<((sdindex>-1)?sdindex+1:obj->getSensorDataList()->size());s++) {
		obj->setCurrentSensorIndex(s);
		SensorData* sd = &obj->getSensorDataList()->at(s);
		int subsets = sd->timed?sd->markers.size():1;
		subsets = (sd->timed && !use_markers)?1:subsets;
		int prev_set_count = out->data_sets.size();
		out->data_sets.resize(prev_set_count+subsets);
		int original_time_index = sd->current_time_index;
		for (int ts=0;ts<subsets;ts++) {
			cout << "subset "<<ts<<endl;
			out->volume = 0;
			sd->current_time_index = sd->timed?sd->markers.at(ts):0;
			sd->current_time_index = (sd->timed && !use_markers)?0:sd->current_time_index;
			processor.process(obj);
			AnalyzerData_dataset* data_set = &out->data_sets.at(prev_set_count+ts);
			data_set->name = sd->timed?sd->subnames.at(sd->current_time_index):sd->name;
			data_set->heat_energy = 0;
			data_set->mat_data.resize(obj->getMaterials()->size());
			for (unsigned int i=0;i<obj->getMaterials()->size();i++) {
				AnalyzerData_material* mat_data = &data_set->mat_data.at(i);
				ObjectData::MaterialData* mat = &obj->getMaterials()->at(i);
				mat_data->name = mat->name;
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
				mat_data->volume = matvolume;
				mat_data->heat_energy = matenergy;
				out->volume += matvolume;
				data_set->heat_energy += matenergy;
			}
		}
		sd_time_changed |= (sd->current_time_index != original_time_index);
		sd->current_time_index = original_time_index;
	}
	if (obj->getCurrentSensorIndex()!=original_sd_index || sd_time_changed) {
		obj->setCurrentSensorIndex(original_sd_index);
		processor.process(obj);
	}
}

void Analyzer::analyzePoint(ObjectData* obj,Vector3D* point,AnalyzerData_point* point_data,Interpolator* interpolator) {
	SensorData* sd = &obj->getSensorDataList()->at(obj->getCurrentSensorIndex());
	vector<SensorPoint>* sensorpoints = &sd->data.at(sd->current_time_index);
	int found = 0;
	double interval = getPointValue(found,sensorpoints,point->getXYZ(),interpolator);
	point_data->value = interval;
	point_data->extrapolated = (found<1);
}
std::ostream &operator<< (std::ostream &out, const Analyzer::AnalyzerData_object &data) {
    out << "object analysis:"<<endl;
    out << "object volume: "<<data.volume<<" (m³)"<<endl;
    for (unsigned int s=0;s<data.data_sets.size();s++) {
    	const Analyzer::AnalyzerData_dataset* data_set = &data.data_sets.at(s);
    	out << "sensor data set "<<data_set->name<<": "<<endl;
		out << "object energy: "<<data_set->heat_energy<<" (J)"<<endl;
		for (unsigned int i=0;i<data_set->mat_data.size();i++) {
			out << "Material "<<data_set->mat_data.at(i).name<<":"<<endl;
			out << "volume: "<<data_set->mat_data.at(i).volume<<" (m³)"<<endl;
			out << "energy: "<<data_set->mat_data.at(i).heat_energy<<" (kJ)"<<endl;
		}
		out << endl;
    }
    cout << "end" << endl;
    return out;
}
Analyzer::~Analyzer() {
}


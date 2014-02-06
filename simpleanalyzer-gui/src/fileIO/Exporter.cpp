/*
 * Exporter.cpp
 *
 *  Created on: 18.12.2013
 *      Author: valentin
 */

#include "Exporter.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
#define GET_IO(i) data->materials.at(i).tetgenoutput
#define CSV_SEP ";"


Exporter::Exporter() {
	// TODO Auto-generated constructor stub

}
const int tetface_indices[4][3] = {{0,1,2},{0,1,3},{0,2,3},{1,2,3}};
ObjectDataStatus Exporter::ExportLegacyVTK(string filename,ObjectData* data) {
	ofstream file;
	file.open(filename);
	if (!file.is_open()) {
		cerr << "could not open file: "<<filename<<endl;
		return OD_FAILURE;
	}
	file << "# vtk DataFile Version 3.0" << endl;
	file << "exported from SimpleAnalyzer" << endl;
	file << "ASCII" << endl;
	file << "DATASET POLYDATA" << endl;
	int point_count = 0;
	int tetrahedra_count = 0;
	int material_count = data->materials.size();
	for (int m=0;m<material_count;m++) {
		tetgenio* io = GET_IO(m);
		point_count		 += io->numberofpoints;
		tetrahedra_count += io->numberoftetrahedra;
	}

	file << "POINTS "<< point_count << " float" << endl;
	for (int m=0;m<material_count;m++) {
		tetgenio* io = GET_IO(m);
		for (int p=0;p<io->numberofpoints;p++) {
			file << io->pointlist[p*3] << " " << io->pointlist[p*3+1] << " " << io->pointlist[p*3+2] << " " << endl;
		}
	}
	file << "POLYGONS " << tetrahedra_count*4 << " "<< tetrahedra_count*4*4 << endl;
	int counter = 0;
	for (int m=0;m<material_count;m++) {
		tetgenio* io = GET_IO(m);
		for (int t=0;t<io->numberoftetrahedra;t++) {
			for (int k=0;k<4;k++) {
				file << 3 << " "<< counter+io->tetrahedronlist[4*t+tetface_indices[k][0]] << " " << counter+io->tetrahedronlist[4*t+tetface_indices[k][1]] << " " << counter+io->tetrahedronlist[4*t+tetface_indices[k][2]] << endl;
			}
		}
		counter+=io->numberofpoints;
	}
	file << "POINT_DATA " << point_count << endl;
	file << "SCALARS temperature float" << endl;
	file << "LOOKUP_TABLE default" << endl;
	for (int m=0;m<material_count;m++) {
		tetgenio* io = GET_IO(m);
		for (int p=0;p<io->numberofpoints;p++) {
			file << io->pointattributelist[p*io->numberofpointattributes]<< endl;
		}
	}
	file << "SCALARS material_index int" << endl;
	file << "LOOKUP_TABLE default" << endl;
	for (int m=0;m<material_count;m++) {
		tetgenio* io = GET_IO(m);
		for (int p=0;p<io->numberofpoints;p++) {
			file << m<< endl;
		}
	}
	file.close();
	return OD_SUCCESS;
}
ObjectDataStatus Exporter::ExportCutCSV(string filename,float* values,CutRender_info* info) {
	ofstream file;
	file.open(filename);
	if (!file.is_open()) {
		cerr << "could not open file: "<<filename<<endl;
		return OD_FAILURE;
	}
	file << "Schnittebene: " << endl;
	file << "V1: (Mittelpunkt)"<<CSV_SEP<<*info->tri->getV1() << endl;
	file << "V2: "<<CSV_SEP<<*info->tri->getV2() << endl;
	file << "V3: "<<CSV_SEP<<*info->tri->getV3() << endl;
	file << "Schrittweite in mm: "<<CSV_SEP<<info->mmperpixel << endl;
	file << "Breite:"<<CSV_SEP<<info->img_width<<CSV_SEP<<"Höhe:"<<CSV_SEP<<info->img_height<<endl;
	file << "Temperaturen in K, -1 ist außerhalb des Objekts" << endl;
	file << endl;
	for (int y=0;y<info->img_height;y++) {
		for (int x=0;x<info->img_width;x++) {
			cout << values[info->img_width*y+x] << endl;
			if (abs(values[info->img_width*y+x]+300)>.0001) {
				file << values[info->img_width*y+x]+273.15 << CSV_SEP;
			} else {
				file << -1 << CSV_SEP;
			}
		}
		file << endl;
	}
	file.close();
	return OD_SUCCESS;
}
Exporter::~Exporter() {
	// TODO Auto-generated destructor stub
}


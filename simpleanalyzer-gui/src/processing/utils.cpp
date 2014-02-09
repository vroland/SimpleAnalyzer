/*
 * utils.cpp
 *
 *  Created on: 21.09.2013
 *      Author: valentin
 */

#include "utils.h"
#include <cmath>
using namespace std;


void Utils::nextCombination(vector<int>* indices,int depth,int dataPointCount) {
	if (depth<0) {
		return;
	}
	int beforeNext = indices->at(depth)<dataPointCount-4+depth;
	if (beforeNext) {
		indices->at(depth)++;
	} else {
		nextCombination(indices,depth-1,dataPointCount);
		indices->at(depth) = indices->at(depth-1)+1;
	}

}
float Utils::clampHue(float h) {
	if (h>1.0) {
		h = 1.0;
	}
	if (h<0.0) {
		h = 0.0;
	}
	return h;
}
wxString Utils::floattowxstr(double val) {
	ostringstream ss;
	ss << val;
	return wxString::FromAscii(ss.str().c_str());
}
wxString Utils::floattowxstr(double val,int digits)  {
	ostringstream ss;
	ss << int(val*exp10(digits))/(float) exp10(digits);
	return wxString::FromAscii(ss.str().c_str());
}
float* Utils::hsvToRgb(float h, float s, float v) {
	float r, g, b;

	int i = floor(h * 6.);
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

    switch(i % 6){
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
    float* col = new float[3];
    col[0] = r;
    col[1] = g;
    col[2] = b;
    return col;
}
void Utils::copySensorPoint(SensorPoint* from,SensorPoint* to) {
	to->coords[0] = from->coords[0];
	to->coords[1] = from->coords[1];
	to->coords[2] = from->coords[2];
	to->temperature = from->temperature;
}
#define EPSILON 0.000001

int Utils::rayIntersectsTriangle(Vector3D* point,Vector3D* direction,Triangle* tri,double* depth) {
	Vector3D p0 = Vector3D(tri->getV1());
	Vector3D p1 = Vector3D(tri->getV2());
	Vector3D p2 = Vector3D(tri->getV3());

	double det, inv_det, u, v;
	double t;

	//Find vectors for two edges sharing V0
	Vector3D e1 = p1.copy();
	Vector3D e2 = p2.copy();
	e1.sub(&p0);
	e2.sub(&p0);
	//Begin calculating determinant - also used to calculate u parameter
	Vector3D* P = direction->crossProduct(&e2);
	//if determinant is near zero, ray lies in plane of triangle
	det = e1.dotProduct(P);
	//NOT CULLING
	if(det > -EPSILON && det < EPSILON) {
		delete P;
		return 0;
	}
	inv_det = 1.f / det;

	//calculate distance from V0 to ray origin
	Vector3D* T = point->copy();
	T->sub(&p0);

	//Calculate u parameter and test bound
	u = T->dotProduct(P) * inv_det;
	//The intersection lies outside of the triangle
	if(u < 0.f || u > 1.f) {
		delete T;
		delete P;
		return 0;
	}

	//Prepare to test v parameter
	Vector3D* Q = T->crossProduct(&e1);

	//Calculate V parameter and test bound
	v = direction->dotProduct(Q) * inv_det;
	//The intersection lies outside of the triangle
	if(v < 0.f || u + v  > 1.f) {
		delete T;
		delete P;
		delete Q;
		return 0;
	}

	t = e2.dotProduct(Q) * inv_det;
	delete T;
	delete P;
	delete Q;
	if(t > EPSILON) { //ray intersection
		*depth = t;
		return 1;
	}

	// No hit, no win
	return 0;
}
int Utils::pointInsideMesh(Vector3D* p,tetgenio* io,PIM_algorithm algorithm) {
	switch (algorithm) {
		case ALGORITHM_RAY: {
			if (io->numberoftrifaces==0) {
				cerr << "mesh doesn't have tri faces. Triangulate it first!" << endl;
			}
			Vector3D dir = Vector3D(1,0,0);
			vector<double> depths;
			int intersections = 0;
			for (int f=0;f<io->numberoftrifaces;f++) {
				Vector3D v1 = Vector3D(&io->pointlist[3*io->trifacelist[3*f]]);
				Vector3D v2 = Vector3D(&io->pointlist[3*io->trifacelist[3*f+1]]);
				Vector3D v3 = Vector3D(&io->pointlist[3*io->trifacelist[3*f+2]]);
				Triangle tri = Triangle(&v1,&v2,&v3);
				double z = 0;
				int found = rayIntersectsTriangle(p,&dir,&tri,&z);
				if (found) {
					intersections++;
					depths.resize(intersections,z);
				}
			}
			sort(depths.begin(),depths.end());
			for (size_t i=1;i<depths.size();i++) {
				if (abs(depths.at(i)-depths.at(i-1))<EPSILON) {
					intersections--;
				}
			}
			return (intersections%2);
		}
		case ALGORITHM_TETRAHEDRONS: {
			if (io->numberoftetrahedra==0) {
				cerr << "mesh doesn't have tetrahedra. Reconstruct it first!" << endl;
			}
			for (int t=0;t<io->numberoftetrahedra;t++) {
				int found = pointInsideTetrahedron(p->getXYZ(),
					 &io->pointlist[3*io->tetrahedronlist[4*t]],
					 &io->pointlist[3*io->tetrahedronlist[4*t+1]],
					 &io->pointlist[3*io->tetrahedronlist[4*t+2]],
					 &io->pointlist[3*io->tetrahedronlist[4*t+3]]);
				if (found>0) {
					return 1;
				}
			}
			return 0;
		}
	}
	cerr << "pointInsideMesh(): invalid algorithm!" << endl;
	return -1;
}
int Utils::pointInsideTetrahedron(Vector3D* pges, Vector3D* v1, Vector3D* v2, Vector3D* v3, Vector3D* v4) {
	v2->sub(v1);
	v3->sub(v1);
	v4->sub(v1);
	pges->sub(v1);
	v1->sub(v1);
	Vector3D* vvec = v2->crossProduct(v3);
	double V = 1./6.*abs(vvec->dotProduct(v4));
	delete vvec;
	if (V<.0001) { // Tetraeder ist koplanar
		return -1;
	}




	Vector3D* n1 = v2->crossProduct(v3);
	if (v4->dotProduct(n1)>0) {
		n1->mult(-1);
	}
	Vector3D* n2 = v4->crossProduct(v2);
	if (v3->dotProduct(n2)>0) {
		n2->mult(-1);
	}
	Vector3D* n3 = v3->crossProduct(v4);
	if (v2->dotProduct(n3)>0) {
		n3->mult(-1);
	}
	Vector3D* backedge1 = v2->copy();
	Vector3D* backedge2 = v3->copy();
	backedge1->sub(v4);
	backedge2->sub(v4);
	Vector3D* n4 = backedge1->crossProduct(backedge2);
	if (v4->dotProduct(n4)<0) {
		n4->mult(-1);
	}
	Vector3D* pges_back = pges->copy();
	pges_back->sub(v3); //Punkt relativ zu v3
	int inside;
	if (pges->dotProduct(n1)<=0 && pges->dotProduct(n2)<=0 && pges->dotProduct(n3)<=0 && pges_back->dotProduct(n4)<=0) {
		inside= 1;
	} else {
		inside= 0;
	}

	delete backedge1;
	delete backedge2;
	delete n1;
	delete n2;
	delete n3;
	delete n4;
	delete pges_back;
	return inside;
}
int Utils::pointInsideTetrahedron(double* p,vector<SensorPoint*>* tet) {
	Vector3D v1 = Vector3D(tet->at(0)->coords);
	Vector3D v2 = Vector3D(tet->at(1)->coords);
	Vector3D v3 = Vector3D(tet->at(2)->coords);
	Vector3D v4 = Vector3D(tet->at(3)->coords);
	Vector3D pges = Vector3D(p);
	return pointInsideTetrahedron(&pges,&v1,&v2,&v3,&v4);
}
int Utils::pointInsideTetrahedron(double* p, double* pv1, double* pv2, double* pv3, double* pv4) {
	Vector3D v1 = Vector3D(pv1);
	Vector3D v2 = Vector3D(pv2);
	Vector3D v3 = Vector3D(pv3);
	Vector3D v4 = Vector3D(pv4);
	Vector3D pges = Vector3D(p);
	return pointInsideTetrahedron(&pges,&v1,&v2,&v3,&v4);
}
double Utils::getPointValue(int &status,vector<SensorPoint>* sensorpoints,double* p,Interpolator* interpolator,vector<SensorPoint*>* prev_tet,vector<SensorPoint*>* current_tet) {
	SensorPointComparator spcomparator;
	int sensorpointcount = sensorpoints->size();
	for (int i=0;i<3;i++) {
		spcomparator.meshpoint[i] = p[i];
	}
	sort(sensorpoints->begin(),sensorpoints->end(),spcomparator);
	vector<SensorPoint*> tet = vector<SensorPoint*>(4);
	vector<int> indices = vector<int>(4);
	for (int i=0;i<4;i++) {
		tet.at(i) = &sensorpoints->at(i);
		indices.at(i) = i;
	}
	vector<int>* extrapolationIndices = NULL;
	bool pretest_successful = (prev_tet!=NULL) && (pointInsideTetrahedron(p,prev_tet)==1);
	status = pretest_successful?1:0;	// Vorherigen Tetraeder testen, wenn gegeben
	//cout << "prev_stat: "<<status<<endl;
	while (status<1) {
		status = pointInsideTetrahedron(p,&tet);
	    if (status>-1 && extrapolationIndices==NULL) {	//Erster nicht coplanarer Tetraeder
			extrapolationIndices = new vector<int>(4);
			for (int i=0;i<4;i++) {
				extrapolationIndices->at(i)= indices.at(i);
			}
		}
	    if (indices.at(0)==sensorpointcount-4 && indices.at(1)==sensorpointcount-3 && indices.at(2)==sensorpointcount-2 && indices.at(3)==sensorpointcount-1) {
			break;
		}
	    if (status<1) {
			nextCombination(&indices,3,sensorpointcount);
			for (int j=0;j<4;j++) {
				tet.at(j) = &sensorpoints->at(indices.at(j));
			}
	    }
	}
	if (extrapolationIndices!=NULL || pretest_successful) {
		if (status<1) { //Extrapolation
			for (int i=0;i<4;i++) {
				tet.at(i) = &sensorpoints->at(extrapolationIndices->at(i));
			}
		}
		if (pretest_successful) {
			for (int i=0;i<4;i++) {
				tet.at(i) = prev_tet->at(i);
			}
		}
		if (current_tet!=NULL) {	//aktuellen Tetraeder speichern, wenn gewünscht
			current_tet->resize(4);
			for (int i=0;i<4;i++) {
				current_tet->at(i) = tet.at(i);
			}
		}
		Vector3D v1 = Vector3D(tet.at(0)->coords);
		Vector3D v2 = Vector3D(tet.at(1)->coords);
		Vector3D v3 = Vector3D(tet.at(2)->coords);
		Vector3D v4 = Vector3D(tet.at(3)->coords);
		Vector3D pg = Vector3D(p);
		Tetrahedron tetrahedron = Tetrahedron(&v1,&v2,&v3,&v4);
		double* values = new double[4];
		for (int i=0;i<4;i++) {
			values[i] = tet.at(i)->temperature;
		}
		double interval =  interpolator->interpolateTet(&tetrahedron,&pg,values);
		delete[] values;
		delete extrapolationIndices;
		return interval;
	} else {
		cerr << "Alle Messwerte sind komplanar -> keine Inter/Extrapolation möglich!" << endl;
		status = -1;
	}
	return -1;
}

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
int Utils::rayIntersectsTriangle(Vector3D* p,Vector3D* direction,Triangle* tri) {
	Vector3D* n;
	double     r, a, b;              // params to calc ray-plane intersect

	// get triangle edge vectors and plane normal
	Vector3D u = Vector3D(tri->getV2());
	u.sub(tri->getV1());
	Vector3D v = Vector3D(tri->getV3());
	v.sub(tri->getV1());
	n = u.crossProduct(&v);              // cross product
	if (n->getLength()==0) {             // triangle is degenerate
		delete n;
		return -1;                  // do not deal with this case
	}
	//dir = R.P1 - R.P0;              // ray direction vector
	Vector3D w0 = Vector3D(p);
	w0.sub(tri->getV1());
	a = -n->dotProduct(&w0);
	b = n->dotProduct(direction);
	delete n;
	if (abs(b) < .000000000001) {     // ray is  parallel to triangle plane
		if (a == 0)                 // ray lies in triangle plane
			return 2;
		else return 0;              // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0) {                   // ray goes away from triangle
		return 0;                   // => no intersect
	}
	// for a segment, also test if (r > 1.0) => no intersect

	Vector3D ip = Vector3D(direction);
	ip.mult(r);
	ip.add(p);            // intersect point of ray and plane

	// is I inside T?
	double    uu, uv, vv, wu, wv, D;
	uu = u.dotProduct(&u);
	uv = u.dotProduct(&v);
	vv = v.dotProduct(&v);
	Vector3D w = Vector3D(ip);
	w.sub(tri->getV1());
	wu = w.dotProduct(&u);
	wv = w.dotProduct(&v);
	D = uv * uv - uu * vv;

	// get and test parametric coords

	double s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0)         // I is outside T
		return 0;
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0)  // I is outside T
		return 0;

	return 1;                       // I is in T
}
int Utils::pointInsideMesh(Vector3D* p,tetgenio* io) {
	if (io->numberoftrifaces==0) {
		cerr << "mesh doesn't have tri faces. Triangulate ist using tetgen first!" << endl;
	}
	Vector3D dir = Vector3D(1,0,0);
	int intersections = 0;
	for (int f=0;f<io->numberoftrifaces;f++) {
		Vector3D v1 = Vector3D(&io->pointlist[3*io->trifacelist[3*f]]);
		Vector3D v2 = Vector3D(&io->pointlist[3*io->trifacelist[3*f+1]]);
		Vector3D v3 = Vector3D(&io->pointlist[3*io->trifacelist[3*f+2]]);
		Triangle tri = Triangle(&v1,&v2,&v3);
		int found = rayIntersectsTriangle(p,&dir,&tri);
		if (found) intersections++;
	}
	return intersections%2;
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
	if (V<.0001) { // Tetraeder ist coplanar
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
double Utils::getPointValue(int &status,vector<SensorPoint>* sensorpoints,double* p,Interpolator* interpolator) {
	SensorPointComparator spcomparator;
	int sensorpointcount = sensorpoints->size();
	for (int i=0;i<3;i++) {
		spcomparator.meshpoint[i] = p[i];
	}
	sort(sensorpoints->begin(),sensorpoints->end(),spcomparator);
	vector<SensorPoint*> tet = vector<SensorPoint*>(4);
	tet.at(0) = &sensorpoints->at(0);
	tet.at(1) = &sensorpoints->at(1);
	tet.at(2) = &sensorpoints->at(2);
	tet.at(3) = &sensorpoints->at(3);
	vector<int> indices = vector<int>(4);
	indices.at(0) = 0;
	indices.at(1) = 1;
	indices.at(2) = 2;
	indices.at(3) = 3;
	vector<int>* extrapolationIndices = NULL;
	status = 0;
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
	if (extrapolationIndices!=NULL) {
		if (status<1) { //Extrapolation
			for (int i=0;i<4;i++) {
				tet.at(i) = &sensorpoints->at(extrapolationIndices->at(i));
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
		cout << "Alle Messwerte sind coplanar -> keine Inter/Extrapolation möglich!" << endl;
		status = -1;
	}
	return -1;
}

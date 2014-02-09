/*
 * GeometryClasses.cpp
 *
 *  Created on: 24.07.2013
 *      Author: valentin
 */

#include "GeometryClasses.h"
#include  <iostream>
#include <cmath>
using namespace std;

#define EPSILON 0.0000001
inline double sqr(double v) {
	return v*v;
}

// Vector3D
Vector3D::Vector3D(double x,double y,double z) {
	coords[0] = x;
	coords[1] = y;
	coords[2] = z;
}
Vector3D::Vector3D(const double* v) {
	coords[0] = v[0];
	coords[1] = v[1];
	coords[2] = v[2];
}
Vector3D::Vector3D(Vector3D* other) {
	coords[0] = other->getX();
	coords[1] = other->getY();
	coords[2] = other->getZ();
}
Vector3D* Vector3D::copy() {
	Vector3D* res = new Vector3D(coords[0],coords[1],coords[2]);
	return res;
}
bool Vector3D::equals(Vector3D* other) {
	return ((abs(getX()-other->getX())<EPSILON) && (abs(getY()-other->getY())<EPSILON) && (abs(getZ()-other->getZ())<EPSILON));
}
double Vector3D::getX() {
	return coords[0];
}
double Vector3D::getY() {
	return coords[1];
}
double Vector3D::getZ() {
	return coords[2];
}
double Vector3D::getLength() {
	return sqrt(sqr(coords[0])+sqr(coords[1])+sqr(coords[2]));
}
double Vector3D::getAngleTo(Vector3D* other) {
	double dp = (getX()*other->getX()+getY()*other->getY()+getZ()*other->getZ());
	double multLen = (getLength()*other->getLength());
	double x = dp/multLen;
	if (x < -1.0) x = -1.0 ;	//Rundungsfehler korrigieren
	  else if (x > 1.0) x = 1.0 ;
	return acos(x);
}
double Vector3D::dotProduct(Vector3D* other) {
	return (getX()*other->getX()+getY()*other->getY()+getZ()*other->getZ());
}
Vector3D* Vector3D::crossProduct(Vector3D* other) {
	double x = getY()*other->getZ()-getZ()*other->getY();
	double y = getZ()*other->getX()-getX()*other->getZ();
	double z = getX()*other->getY()-getY()*other->getX();
	return new Vector3D(x,y,z);
}
void Vector3D::add(Vector3D* other) {
	coords[0]+=other->getX();
	coords[1]+=other->getY();
	coords[2]+=other->getZ();
}
void Vector3D::sub(Vector3D* other) {
	coords[0]-=other->getX();
	coords[1]-=other->getY();
	coords[2]-=other->getZ();
}
void Vector3D::mult(double scalar) {
	coords[0]*=scalar;
	coords[1]*=scalar;
	coords[2]*=scalar;
}
void Vector3D::normalize() {
	double l = getLength();
	coords[0]/=l;
	coords[1]/=l;
	coords[2]/=l;
}

double* Vector3D::getXYZ() {
	return coords;
}
double Vector3D::getDistanceTo(Vector3D* other) {
	return sqrt(sqr(getX()-other->getX())+sqr(getY()-other->getY())+sqr(getZ()-other->getZ()));
}
void Vector3D::print() {
	cout <<"Vector3D ("<< getX() << " "<<getY()<<" " << getZ() <<")"<< endl;
}
void Vector3D::printTo(std::ostream &stream) const {
	stream << "("<<coords[0] << " " << coords[1] << " "<< coords[2]<<")";
}
std::ostream &operator<< (std::ostream &out, const Vector3D &vec) {
    vec.printTo(out);
    return out;
}
Vector3D::~Vector3D() {
}

//Matrix3D
Matrix3D::Matrix3D() {
	elements[0] = 1.0;
	elements[1] = 0.0;
	elements[2] = 0.0;
	elements[3] = 0.0;
	elements[4] = 1.0;
	elements[5] = 0.0;
	elements[6] = 0.0;
	elements[7] = 0.0;
	elements[8] = 1.0;
}
Matrix3D::Matrix3D(double x1, double y1, double z1,
					double x2, double y2, double z2,
					double x3, double y3, double z3)
{
	elements[0] = x1;
	elements[1] = y1;
	elements[2] = z1;
	elements[3] = x2;
	elements[4] = y2;
	elements[5] = z2;
	elements[6] = x3;
	elements[7] = y3;
	elements[8] = z3;
}
void Matrix3D::mult(Matrix3D* other) {
	double elemcopy[9];
	for (int i=0;i<9;i++) {
		elemcopy[i] = elements[i];
	}
	for (int i=0;i<3;i++) {
		for (int j=0;j<3;j++) {
			elements[3*i+j] = elemcopy[3*i]*other->elements[j]+
							  elemcopy[3*i+1]*other->elements[3+j]+
							  elemcopy[3*i+2]*other->elements[6+j];

		}
	}
}
Vector3D* Matrix3D::mult(Vector3D* other) {
	Vector3D* nvec = new Vector3D(0,0,0);
	for (int i=0;i<3;i++) {
		for (int j=0;j<3;j++) {
			nvec->getXYZ()[i] += other->getXYZ()[j]*elements[3*i+j];
		}
	}
	return nvec;
}
void Matrix3D::rotateX(double angle) {
	Matrix3D mat = Matrix3D(1,0,0,
							0,cos(angle),-sin(angle),
							0,sin(angle),cos(angle));
	this->mult(&mat);
}
void Matrix3D::rotateY(double angle) {
	Matrix3D mat = Matrix3D(cos(angle),0,sin(angle),
							0,1,0,
							-sin(angle),0,cos(angle));
	this->mult(&mat);
}
void Matrix3D::rotateZ(double angle) {
	Matrix3D mat = Matrix3D(cos(angle),-sin(angle),0,
							sin(angle),cos(angle),0,
							0,0,1);
	this->mult(&mat);
}
void Matrix3D::transpose() {
	double elemcopy[9];
	for (int i=0;i<9;i++) {
		elemcopy[i] = elements[i];
	}
	for (int i=0;i<3;i++) {
		for (int j=0;j<3;j++) {
			elements[3*j+i] = elemcopy[3*i+j];
		}
	}
}
void Matrix3D::print() {
		cout <<"(" << elements[0] << " " << elements[1] << " " << elements[2] << ")" << endl;
		cout <<"(" << elements[3] << " " << elements[4] << " " << elements[5] << ")" << endl;
		cout <<"(" << elements[6] << " " << elements[7] << " " << elements[8] << ")" << endl;
}
// Triangle
Triangle::Triangle(Vector3D* v1,Vector3D* v2, Vector3D* v3) {
	verts[0] = v1;
	verts[1] = v2;
	verts[2] = v3;
}
Vector3D* Triangle::getV1() {
	return verts[0];
}
Vector3D* Triangle::getV2() {
	return verts[1];
}
Vector3D* Triangle::getV3() {
	return verts[2];
}
Vector3D* Triangle::getVert(int index) {
	return verts[index];
}
Vector3D* Triangle::getNormal() {
	Vector3D* e1 = verts[1]->copy();
	Vector3D* e2 = verts[2]->copy();
	e1->sub(verts[0]);
	e2->sub(verts[0]);
	Vector3D* normal = e1->crossProduct(e2);
	delete e1;
	delete e2;
	normal->normalize();
	return normal;
}
void Triangle::print() {
	cout << "Triangle:"<<endl;
	cout <<"(" << verts[0]->getX() << " " << verts[0]->getY() << " " << verts[0]->getZ() << ")" << endl;
	cout <<"(" << verts[1]->getX() << " " << verts[1]->getY() << " " << verts[1]->getZ() << ")" << endl;
	cout <<"(" << verts[2]->getX() << " " << verts[2]->getY() << " " << verts[2]->getZ() << ")" << endl;
}
Triangle::~Triangle() {
}

Tetrahedron::Tetrahedron(Vector3D* v1,Vector3D* v2,Vector3D* v3,Vector3D* v4) {
	verts[0] = v1;
	verts[1] = v2;
	verts[2] = v3;
	verts[3] = v4;
}
Vector3D* Tetrahedron::getV1() {
	return verts[0];
}
Vector3D* Tetrahedron::getV2() {
	return verts[1];
}
Vector3D* Tetrahedron::getV3() {
	return verts[2];
}
Vector3D* Tetrahedron::getV4() {
	return verts[3];
}
Vector3D* Tetrahedron::getVert(int i) {
	return verts[i];
}

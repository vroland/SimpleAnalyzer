/*
 * GeometryClasses.h
 *
 *  Created on: 24.07.2013
 *      Author: valentin
 */
#include <iostream>
#ifndef GEOMETRYCLASSES_H_
#define GEOMETRYCLASSES_H_

class Vector3D {
	public:
		Vector3D(double x,double y,double z);
		Vector3D(const double* v);
		Vector3D(Vector3D* other);
		Vector3D* copy();
		double getX();
		double getY();
		double getZ();
		double getLength();
		double getAngleTo(Vector3D* other);
		double dotProduct(Vector3D* other);
		Vector3D* crossProduct(Vector3D* other);
		void add(Vector3D* other);
		void sub(Vector3D* other);
		void mult(double scalar);
		void normalize();
		virtual ~Vector3D();
		bool equals(Vector3D* other);
		double getDistanceTo(Vector3D* other);
		double* getXYZ();
		void print();
		void printTo(std::ostream &stream) const;
	private:
		double coords[3];
};
std::ostream &operator<< (std::ostream &out, const Vector3D &vec);
class Matrix3D {
	private:
		double elements[9];
	public:
		Matrix3D();
		Matrix3D(double x1, double y1, double z1,
			     double x2, double y2, double z2,
			     double x3, double y3, double z3);
		void mult(Matrix3D* other);
		Vector3D* mult(Vector3D* other);
		void rotateX(double angle);
		void rotateY(double angle);
		void rotateZ(double angle);
		void transpose();
		void print();
};
class Triangle {
	public:
		Triangle(Vector3D* v1,Vector3D* v2, Vector3D* v3);
		~Triangle();
		Vector3D* getV1();
		Vector3D* getV2();
		Vector3D* getV3();
		Vector3D* getVert(int index);
		void print();
		Vector3D* getNormal();
	private:
		Vector3D* verts[3];
};
class Tetrahedron {
public:
	Tetrahedron(Vector3D* v1,Vector3D* v2,Vector3D* v3,Vector3D* v4);
	Vector3D* getV1();
	Vector3D* getV2();
	Vector3D* getV3();
	Vector3D* getV4();
	Vector3D* getVert(int i);
private:
	Vector3D* verts[4];
};

#endif /* GEOMETRYCLASSES_H_ */

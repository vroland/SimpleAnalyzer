/*
 * Interpolator.h
 *
 *  Created on: 22.07.2013
 *      Author: valentin
 */

#ifndef INTERPOLATOR_H_
#define INTERPOLATOR_H_
#include "GeometryClasses.h"

using namespace std;

extern "C"
{
enum InterpolationMode {LINEAR,LOGARITHMIC};


class Interpolator {
	public:
		Interpolator();
		double interpolateTri(Triangle* tri,Vector3D* pos,double* values);
		double interpolateTet(Tetrahedron* tet,Vector3D* pos,double* values);
		void setMode(InterpolationMode mode);
		virtual ~Interpolator();
	private:
		InterpolationMode mode;
};
}

#endif /* INTERPOLATOR_H_ */

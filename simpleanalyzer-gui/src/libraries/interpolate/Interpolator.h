/*
 * Interpolator.h
 *
 *  Created on: 22.07.2013
 *      Author: Valentin Roland
 */

#ifndef INTERPOLATOR_H_
#define INTERPOLATOR_H_
#include "GeometryClasses.h"

using namespace std;

/**
 * @brief 2- und 3-dimensionale Inter-/Extrapolation
 *
 * Klasse zur Bi- und Trilinearen Inter-/Extrapolation, wobei die Interpolationsfunktion
 * zwischen zwei Werten entweder linear oder logarithmisch sein kann (InterpolationMode).
 */
class Interpolator {
public:
	/**
	 * Der Typ der verwendeten Interpolationsfunktion.
	 */
	enum InterpolationMode {
		LINEAR, LOGARITHMIC
	};

public:
	/**
	 * Der Konstruktor.
	 */
	Interpolator();

	/**
	 * Ermittelt den Wert für einen beliebigen Punkt in einer Ebene.
	 * Dabei wird wie bei der bilinearen Interpolation (http://en.wikipedia.org/wiki/Bilinear_interpolation)
	 * vorgegangen, es kann jedoch auch eine logarithmische Interpolationsfunktion verwendet werden.
	 * @param tri Dreieck, durch das die Ebene beschrieben wird.
	 * @param pos Position des Punktes, für den der Wert ermittelt werden soll.
	 * @param values Die Werte, die den Punkten des Dreiecks entsprechen.
	 * Dabei ist values[0] der Wert des ersten Punktes des Dreiecks, values[0] der Zweite usw.
	 * @return Der Wert für den angegebenen Punkt (pos).
	 */
	double interpolateTri(Triangle* tri, Vector3D* pos, double* values);

	/**
	 * Ermittelt den Wert für einen beliebigen Punkt im Raum.
	 * Dabei wird wie bei der bilinearen Interpolation (http://en.wikipedia.org/wiki/Trilinear_interpolation)
	 * vorgegangen, es kann jedoch auch eine logarithmische Interpolationsfunktion verwendet werden.
	 * @param tet Tetraeder, durch den die Punkte für die gegebenen Werte gegeben sind.
	 * @param pos Position des Punktes, für den der Wert ermittelt werden soll.
	 * @param values Die Werte, die den Punkten des Tetraeders entsprechen.
	 * Dabei ist values[0] der Wert des ersten Punktes des Tetraeders, values[0] der Zweite usw.
	 * @return Der Wert für den angegebenen Punkt (pos).
	 */
	double interpolateTet(Tetrahedron* tet, Vector3D* pos, double* values);

	/**
	 * Setzt den verwendeten Interpolationsmodus (die Interpolationsfunktion).
	 */
	void setMode(InterpolationMode mode);

	/**
	 * Der Destruktor.
	 */
	virtual ~Interpolator();

private:
	/**
	 * Der verwendete Interpolationsmodus bzw. die Interpolationsfunktion.
	 */
	InterpolationMode mode;
};

#endif /* INTERPOLATOR_H_ */

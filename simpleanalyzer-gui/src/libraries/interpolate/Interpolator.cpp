#include "Interpolator.h"
#include  <iostream>
#include <cmath>
#define PI 3.14159265358979323846
using namespace std;

inline double sqr(double v) {
	return v * v;
}

// Interpolator
Interpolator::Interpolator() {
	mode = LINEAR;
}

void Interpolator::setMode(InterpolationMode newmode) {
	mode = newmode;
}

inline double getSign(double x) {
	return (x < 0) ? -1 : 1;
}

double Interpolator::interpolateTri(Triangle* tri, Vector3D* pos,
		double* values) {

	//Test auf pos == Eckpunkt
	for (int i = 0; i < 3; i++) {
		if (pos->equals(tri->getVert(i))) {
			return values[i];
		}
	}

	// Kanten und Winkel
	Vector3D* e1 = tri->getV2()->copy();
	e1->sub(tri->getV1());
	Vector3D* e2 = tri->getV3()->copy();
	e2->sub(tri->getV1());
	Vector3D* e3 = tri->getV3()->copy();
	e3->sub(tri->getV2());
	e3->mult(-1);
	double alpha = e1->getAngleTo(e2);
	double beta = e1->getAngleTo(e3);

	/*Dreieck in 2D-Ebene (Bezeichnungen im lokalen Dreieck):
	 * => Das Dreieck wird so transformiert, dass A auf O(0|0|), Vektor AB auf der X-Achse und C in XY-Ebene liegt.
	 * Dieses Dreieck wird im folgenden als "lokal" bezeichnet:
	 *
	 *				  C			CAB = alpha
	 * 				 /\			ABC = beta
	 * 				/  \		P = pos
	 * 			e2 /    \ e3	PAB = delta
	 * 			  /	     \		e1..e3 im code nicht lokal, aber Innenwinkel und Längen äquivalent zu lokalen Kanten
	 * 			 /	.P    \			   (da die zwei gleiche Winkel und in allen Verhältnissen der Seiten übereinstimmen).
	 * 			/	       \
	 * 		   /------------\
	 * 		  A		e1  	 B
	 */

	// Lokales Dreieck
	Vector3D localV1 = Vector3D(0, 0, 0);
	Vector3D localV2 = Vector3D(e1->getLength(), 0, 0);
	Vector3D localV3 = Vector3D(cos(alpha) * e2->getLength(),
			sin(alpha) * e2->getLength(), 0);
	Vector3D* v1ToPos = pos->copy();
	v1ToPos->sub(tri->getV1());
	double delta = e1->getAngleTo(v1ToPos);
	Vector3D localUp = Vector3D(0, 1, 0);
	int ysign = (localUp.dotProduct(v1ToPos) > 0) ? 1 : -1;
	Vector3D localPos = Vector3D(cos(delta) * v1ToPos->getLength(),
			ysign * sin(delta) * v1ToPos->getLength(), 0);
	Vector3D* v2ToPos = pos->copy();
	v2ToPos->sub(tri->getV2());

	// Bilineare Interpolation:

	// Punkte auf Kanten 1 und 3:
	Vector3D iPointX1 = Vector3D(localPos.getY() / tan(alpha), localPos.getY(),
			0);
	Vector3D iPointX2 = Vector3D(localV2.getX() - localPos.getY() / tan(beta),
			localPos.getY(), 0);
	Vector3D* x2ToV2 = iPointX2.copy();
	x2ToV2->sub(&localV2);
	x2ToV2->mult(1. / x2ToV2->getX());
	// Werte für Punkte:

	double value = 0;
	double xVal1 = 0;
	double xVal2 = 0;

	switch (mode) {
	case LINEAR: {
		// getSign(localPos.y) zeigt Richtung der Strecke auf Dreieckskante e2 (bzw. linearer Funktion)
		// Länge von iPointX1: auf Kante "gegangene" Strecke x bis Pgesucht.Y == f(x)
		xVal1 = values[0]
				+ (values[2] - values[0]) * getSign(localPos.getY())
						* iPointX1.getLength() / e2->getLength();
		// analog für Kante e3:
		xVal2 = values[1]
				+ (values[2] - values[1]) * getSign(localPos.getY())
						* sqrt(
								sqr(iPointX2.getX() - localV2.getX())
										+ sqr(localPos.getY()))
						/ e3->getLength();

		// Interpolieren zwischen Punkten auf Kanten 1 und 3
		value = xVal1
				+ (xVal2 - xVal1) * (localPos.getX() - iPointX1.getX())
						/ (iPointX2.getX() - iPointX1.getX());
		break;
	}
	case LOGARITHMIC:
		// siehe Linear
		xVal1 = values[0]
				* exp(
						(log(values[2]) - log(values[0]))
								* getSign(localPos.getY())
								* iPointX1.getLength() / e2->getLength());
		xVal2 = values[1]
				* exp(
						(log(values[2]) - log(values[1]))
								* getSign(localPos.getY())
								* sqrt(
										sqr(iPointX2.getX() - localV2.getX())
												+ sqr(localPos.getY()))
								/ e3->getLength());
		// Interpolieren zwischen Punkten auf Kanten 1 und 3
		value = xVal1
				* exp(
						(log(xVal2) - log(xVal1))
								* (localPos.getX() - iPointX1.getX())
								/ (iPointX2.getX() - iPointX1.getX()));
		break;
	default:
		cout << "Kein Gültiges Interpolationsverfahren angegeben!" << endl;
		break;
	}

	// Speicher freigeben
	delete e1;
	delete e2;
	delete e3;
	delete x2ToV2;
	delete v1ToPos;
	delete v2ToPos;
	return value;
}

double Interpolator::interpolateTet(Tetrahedron* tet, Vector3D* pos,
		double* values) {

	//Test auf pos == Eckpunkt
	for (int i = 0; i < 3; i++) {
		if (pos->equals(tet->getVert(i))) {
			return values[i];
		}
	}

	Triangle groundTri = Triangle(tet->getV1(), tet->getV2(), tet->getV3());

	// Bodendreieck in XY-Ebene legen
	Vector3D* e1 = groundTri.getV2()->copy();
	e1->sub(groundTri.getV1());
	Vector3D* e2 = groundTri.getV3()->copy();
	e2->sub(groundTri.getV1());
	double alpha = e1->getAngleTo(e2);

	//Kanten von Punkt 4
	Vector3D* gs1 = tet->getV4()->copy();
	gs1->sub(tet->getV1());
	Vector3D* up = groundTri.getNormal();

	if (gs1->dotProduct(up) < 0) {	//Normale in Richtung der Tetraederspitze?
		up->mult(-1);
	}

	double delta = gs1->getAngleTo(up);
	double h = cos(delta) * gs1->getLength();
	Vector3D* v1ToV4InGround = tet->getV4()->copy();
	Vector3D* tUp = up->copy();
	tUp->mult(h);
	v1ToV4InGround->sub(tUp);
	v1ToV4InGround->sub(tet->getV1());						//relativ zu Vert0
	double beta = e1->getAngleTo(v1ToV4InGround);

	Vector3D* lot = e1->crossProduct(up);	// Richtung des Lots von V3 auf e1
	if (lot->dotProduct(e2) < 0) {
		lot->mult(-1);
	}

	int sign = (lot->dotProduct(v1ToV4InGround) > 0) ? 1 : -1;//Spitze des Tetraeders über/unter Dreiecksebene
	if (v1ToV4InGround->getLength() == 0) { 					//Spitze über V1
		beta = 0;
	}

	//Lokaler Tetraeder:
	Vector3D* groundVerts[3];
	groundVerts[0] = new Vector3D(0, 0, 0);
	groundVerts[1] = new Vector3D(
			groundTri.getV1()->getDistanceTo(groundTri.getV2()), 0, 0);
	groundVerts[2] = new Vector3D(cos(alpha) * e2->getLength(),
			sin(alpha) * e2->getLength(), 0);
	Triangle localGround = Triangle(groundVerts[0], groundVerts[1],
			groundVerts[2]);
	Vector3D* localTip = new Vector3D(cos(beta) * v1ToV4InGround->getLength(),
			sign * sin(beta) * v1ToV4InGround->getLength(), h);

	//Lokale Kanten:
	Vector3D* s1 = localTip->copy();
	Vector3D* s2 = localTip->copy();
	Vector3D* s3 = localTip->copy();
	s1->sub(localGround.getV1());
	s2->sub(localGround.getV2());
	s3->sub(localGround.getV3());

	// Lokaler Interpolationspunkt
	Vector3D* v1ToPos = pos->copy();
	v1ToPos->sub(tet->getV1());
	double pAlpha = v1ToPos->getAngleTo(up);

	double ph = cos(pAlpha) * v1ToPos->getLength();
	Vector3D* posInGround = pos->copy();
	Vector3D* tempUp = up->copy();
	tempUp->normalize();
	tempUp->mult(ph);
	posInGround->sub(tempUp);
	posInGround->sub(tet->getV1()); //rel. zu Vert0
	delete tempUp;
	double pBeta = e1->getAngleTo(posInGround);
	sign = (lot->dotProduct(v1ToPos) > 0) ? 1 : -1;
	Vector3D* localPos = new Vector3D(cos(pBeta) * posInGround->getLength(),
			sign * sin(pBeta) * posInGround->getLength(), ph);
	delete lot;

	//Punkte auf 3 Tetraederkanten ermitteln, auf deren Ebene der gesuchte Punkt liegt
	Vector3D* localUp = localGround.getNormal();
	double sAlpha = s1->getAngleTo(localUp);
	double sBeta = s2->getAngleTo(localUp);
	double sGamma = s3->getAngleTo(localUp);
	double lengths[3] = { s1->getLength(), s2->getLength(), s3->getLength() };
	s1->normalize();
	s2->normalize();
	s3->normalize();
	s1->mult(ph / cos(sAlpha));
	s2->mult(ph / cos(sBeta));
	s3->mult(ph / cos(sGamma));
	Vector3D* s1Point = s1->copy();
	Vector3D* s2Point = s2->copy();
	s2Point->add(localGround.getV2());
	Vector3D* s3Point = s3->copy();
	s3Point->add(localGround.getV3());

	double interValues[3];

	//Werte für die Punkte auf den 3 Kanten entsprechend der Interpolationsfunktion ermitteln
	switch (mode) {
	case LINEAR:
		interValues[0] = values[0]
				+ (values[3] - values[0]) * ph / cos(sAlpha) / lengths[0];
		interValues[1] = values[1]
				+ (values[3] - values[1]) * ph / cos(sBeta) / lengths[1];
		interValues[2] = values[2]
				+ (values[3] - values[2]) * ph / cos(sGamma) / lengths[2];
		break;
	case LOGARITHMIC:
		interValues[0] = values[0]
				* exp(
						(log(values[3]) - log(values[0])) * ph / cos(sAlpha)
								/ lengths[0]);
		interValues[1] = values[1]
				* exp(
						(log(values[3]) - log(values[1])) * ph / cos(sBeta)
								/ lengths[1]);
		interValues[2] = values[2]
				* exp(
						(log(values[3]) - log(values[2])) * ph / cos(sGamma)
								/ lengths[2]);
		break;
	default:
		cerr << "Kein Gültiges Interpolationsverfahren angegeben!" << endl;
		break;
	}

	/*
	 * Durch die 3 Punkte auf den Kanten stellen ein Dreieck dar, dass in der selben Ebene wie der gesuchte Punkt liegt.
	 * => Bilineare Interpolation über dieses Dreieck
	 */
	Triangle newTri = Triangle(s1Point, s2Point, s3Point);
	double val = interpolateTri(&newTri, localPos, interValues);

	//Speicher freigeben
	for (int v = 0; v < 3; v++) {
		delete groundVerts[v];
	}

	delete up;
	delete tUp;
	delete localUp;
	delete v1ToV4InGround;
	delete posInGround;
	delete localTip;
	delete localPos;
	delete e1;
	delete e2;
	delete gs1;
	delete s1;
	delete s2;
	delete s3;
	delete s1Point;
	delete s2Point;
	delete s3Point;
	delete v1ToPos;
	return val;
}
Interpolator::~Interpolator() {
}


/*
 * utils.cpp
 *
 *  Created on: 21.09.2013
 *      Author: Valentin Roland
 */

#include "utils.h"
#include <cmath>

using namespace std;

//Epsilon für die Epsilonumgebung zum Test auf Gleichheit.
#define EPSILON 0.000001

string Utils::getTextBlock(string data, int n) {

	//Tabulatoren mit Leerzeichen ersetzen
	for (size_t c = 0; c < data.size(); c++) {
		if (data.at(c) == '\t') {
			data.at(c) = ' ';
		}
	}

	//Position des abschließenden Leerzeichens für den ersten Block
	size_t position = data.find(" ");
	//Speicher für das Ende des vorherigen Blocks
	size_t previousPos = 0;

	//Ist der String Leerzeichenlos?
	if (position == data.npos) {
		return "";
	}

	//Durchlaufen aller Textblöcke bis zum gewünschten Index
	for (int i = 0; i < n; i++) {

		//aus Ende des verherigen Blocks den Anfang des aktuellen Berechnen.
		previousPos = position + 1;
		//Position des nächsten Blocks suchen
		position = data.find(" ", position + 1);
		//Ende der Zeile erreicht?
		if (position == data.npos) {
			//Ist der Index außerhalb des möglichen Bereichs?
			if (n - 1 > i) {
				return "";
			}
		}
	}

	//Zurückgeben des aktuellen Textblocks
	return data.substr(previousPos, position - (previousPos));
}

void Utils::nextCombination(vector<int>* indices, int depth,
		int dataPointCount) {

	//sind bereits alle Indices maximal?
	if (depth < 0) {
		return;
	}

	//Kann der aktuelle Index noch weiter erhöht werden ohne ungültig zu werden?
	int beforeNext = indices->at(depth) < dataPointCount - 4 + depth;
	if (beforeNext) {
		//erhöhen des Index
		indices->at(depth)++;}
	else {
		//es muss der vorhergehende index (depth - 1) verändert werden, um eine neue Kombination zu generieren
		nextCombination(indices, depth - 1, dataPointCount);
		//Index auf den um eins erhöhten Wert der vorhergehenden stelle setzen
		indices->at(depth) = indices->at(depth - 1) + 1;
	}

	//neue Kombination erstellt
}

float Utils::clampHue(float h) {
	if (h > 1.0) {
		h = 1.0;
	}
	if (h < 0.0) {
		h = 0.0;
	}
	return h;
}

wxString Utils::floattowxstr(double val) {
	ostringstream ss;
	ss << val;
	return wxString::FromAscii(ss.str().c_str());
}

wxString Utils::floattowxstr(double val, int digits) {
	ostringstream ss;
	ss << int(val * pow(10, digits)) / (float) pow(10, digits);
	return wxString::FromAscii(ss.str().c_str());
}

float* Utils::hsvToRgb(float h, float s, float v) {
	float r, g, b;

	int i = floor(h * 6.);
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (i % 6) {
	case 0:
		r = v, g = t, b = p;
		break;
	case 1:
		r = q, g = v, b = p;
		break;
	case 2:
		r = p, g = v, b = t;
		break;
	case 3:
		r = p, g = q, b = v;
		break;
	case 4:
		r = t, g = p, b = v;
		break;
	case 5:
		r = v, g = p, b = q;
		break;
	}
	float* col = new float[3];
	col[0] = r;
	col[1] = g;
	col[2] = b;
	return col;
}

void Utils::copySensorPoint(SensorPoint* from, SensorPoint* to) {
	to->coords[0] = from->coords[0];
	to->coords[1] = from->coords[1];
	to->coords[2] = from->coords[2];
	to->temperature = from->temperature;
}

int Utils::rayIntersectsTriangle(Vector3D* point, Vector3D* direction,
		Triangle* tri, double* depth) {

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
	if (det > -EPSILON && det < EPSILON) {
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
	if (u < 0.f || u > 1.f) {
		delete T;
		delete P;
		return 0;
	}

	//Prepare to test v parameter
	Vector3D* Q = T->crossProduct(&e1);

	//Calculate V parameter and test bound
	v = direction->dotProduct(Q) * inv_det;
	//The intersection lies outside of the triangle
	if (v < 0.f || u + v > 1.f) {
		delete T;
		delete P;
		delete Q;
		return 0;
	}

	t = e2.dotProduct(Q) * inv_det;
	delete T;
	delete P;
	delete Q;
	if (t > EPSILON) { //ray intersection
		*depth = t;
		return 1;
	}

	// No hit, no win
	return 0;
}

int Utils::pointInsideMesh(Vector3D* p, tetgenio* io, PIM_algorithm algorithm) {

	//zu verwendender Algorithmus?
	switch (algorithm) {
	case ALGORITHM_RAY: {

		//Haben die Geometriedaten die nötige Struktur (sind die Außenflächen berechnet)?
		if (io->numberoftrifaces == 0) {
			cerr << "mesh doesn't have tri faces. Triangulate it first!"
					<< endl;
		}

		Vector3D dir = Vector3D(1, 0, 0);
		vector<double> depths;

		//Anzahl der Schnittpunkte eines Strahls mit dem Objekt
		int intersections = 0;

		//Schnittpunkte mit der Geometrie zählen
		for (int f = 0; f < io->numberoftrifaces; f++) {
			Vector3D v1 = Vector3D(&io->pointlist[3 * io->trifacelist[3 * f]]);
			Vector3D v2 = Vector3D(
					&io->pointlist[3 * io->trifacelist[3 * f + 1]]);
			Vector3D v3 = Vector3D(
					&io->pointlist[3 * io->trifacelist[3 * f + 2]]);
			Triangle tri = Triangle(&v1, &v2, &v3);

			//Tiefe zum ausschließen doppelter Schnittpunkte speichern
			double z = 0;
			int found = rayIntersectsTriangle(p, &dir, &tri, &z);
			if (found) {
				intersections++;
				depths.resize(intersections, z);
			}
		}

		//doppelte Schnittpunkte aussortieren
		sort(depths.begin(), depths.end());
		for (size_t i = 1; i < depths.size(); i++) {
			if (abs(depths.at(i) - depths.at(i - 1)) < EPSILON) {
				intersections--;
			}
		}

		//Bei gerader Anzahl an Schnittpunkten ist der Punkt außerhalb
		return (intersections % 2);
	}
	case ALGORITHM_TETRAHEDRONS: {

		//Haben die Geometriedaten die nötige Struktur (sind die Tetraeder berechnet)?
		if (io->numberoftetrahedra == 0) {
			cerr << "mesh doesn't have tetrahedra. Reconstruct it first!"
					<< endl;
		}

		//Ist der Punkt in einem Tetraeder des Objekts?
		for (int t = 0; t < io->numberoftetrahedra; t++) {
			int found = pointInsideTetrahedron(p->getXYZ(),
					&io->pointlist[3 * io->tetrahedronlist[4 * t]],
					&io->pointlist[3 * io->tetrahedronlist[4 * t + 1]],
					&io->pointlist[3 * io->tetrahedronlist[4 * t + 2]],
					&io->pointlist[3 * io->tetrahedronlist[4 * t + 3]]);

			//Punkt liegt in einem Tetraeder
			if (found > 0) {
				return 1;
			}
		}

		//Der Punkt wurde in keinem Tetraeder gefunden.
		return 0;
	}
	}

	//Kein gültiger Modus übergeben
	cerr << "pointInsideMesh(): invalid algorithm!" << endl;
	return -1;
}

int Utils::pointInsideTetrahedron(Vector3D* pges, Vector3D* v1, Vector3D* v2,
		Vector3D* v3, Vector3D* v4) {

	//Kanten berechnen
	Vector3D e1(v2);
	Vector3D e2(v3);
	Vector3D e3(v4);
	e1.sub(v1);
	e2.sub(v1);
	e3.sub(v1);

	//gesuchter Punkt relativ zu v1
	Vector3D p_loc(pges);
	p_loc.sub(v1);

	//Normale der Grundfläche
	Vector3D* vvec = e1.crossProduct(&e2);
	//Volumen des Tetraeders
	double V = 1. / 6. * abs(vvec->dotProduct(&e3));
	delete vvec;

	//Ist der Tetraeder komplanar?
	if (V < EPSILON) {
		return -1;
	}

	//Normalen des Tetraeders berechnen und nach innen ausrichten
	Vector3D* n1 = e1.crossProduct(&e2);

	if (e3.dotProduct(n1) > 0) {
		n1->mult(-1);
	}

	Vector3D* n2 = e3.crossProduct(&e1);

	if (e2.dotProduct(n2) > 0) {
		n2->mult(-1);
	}

	Vector3D* n3 = e2.crossProduct(&e3);

	if (e1.dotProduct(n3) > 0) {
		n3->mult(-1);
	}

	Vector3D* backedge1 = e1.copy();
	Vector3D* backedge2 = e2.copy();
	backedge1->sub(&e3);
	backedge2->sub(&e3);
	Vector3D* n4 = backedge1->crossProduct(backedge2);

	if (e3.dotProduct(n4) < 0) {
		n4->mult(-1);
	}

	//gesuchter Punkt relativ zu e2, zum Test mit der Normale der v1 gegenüberliegenden Fläche
	Vector3D* p_loc_e2 = p_loc.copy();
	p_loc_e2->sub(&e2);

	int inside = 0;

	/*
	 * Befindet sich der Punkt innerhalb des Tetraeders.
	 * (durch Vorzeichen des Punktprodukts der Normalen der einzelnen Flächen mit dem Vektor zum gesuchten Punkt)
	 */
	if (p_loc.dotProduct(n1) <= 0 && p_loc.dotProduct(n2) <= 0
			&& p_loc.dotProduct(n3) <= 0 && p_loc_e2->dotProduct(n4) <= 0) {
		inside = 1;
	} else {
		inside = 0;
	}

	//Speicher freigeben
	delete backedge1;
	delete backedge2;
	delete n1;
	delete n2;
	delete n3;
	delete n4;
	delete p_loc_e2;

	return inside;
}

int Utils::pointInsideTetrahedron(double* p, vector<SensorPoint*>* tet) {

	Vector3D v1 = Vector3D(tet->at(0)->coords);
	Vector3D v2 = Vector3D(tet->at(1)->coords);
	Vector3D v3 = Vector3D(tet->at(2)->coords);
	Vector3D v4 = Vector3D(tet->at(3)->coords);
	Vector3D pges = Vector3D(p);
	return pointInsideTetrahedron(&pges, &v1, &v2, &v3, &v4);
}

int Utils::pointInsideTetrahedron(double* p, double* pv1, double* pv2,
		double* pv3, double* pv4) {

	Vector3D v1 = Vector3D(pv1);
	Vector3D v2 = Vector3D(pv2);
	Vector3D v3 = Vector3D(pv3);
	Vector3D v4 = Vector3D(pv4);
	Vector3D pges = Vector3D(p);
	return pointInsideTetrahedron(&pges, &v1, &v2, &v3, &v4);
}

double Utils::getPointValue(int &status, vector<SensorPoint>* sensorpoints,
		double* p, Interpolator* interpolator, vector<SensorPoint*>* prev_tet,
		vector<SensorPoint*>* current_tet) {

	//Objekt zum Vergleichen des Abstands zweier Messpunkte von einem best. Punkt in der std::sort-Funktion
	SensorPointComparator spcomparator;
	int sensorpointcount = sensorpoints->size();

	//Speichern der Koordinaten des gesuchten Punkten im Vergleichsobjekt
	for (int i = 0; i < 3; i++) {
		spcomparator.meshpoint[i] = p[i];
	}

	//Sortieren der Messpunkte nach Abstand zum gesuchten Punkt
	sort(sensorpoints->begin(), sensorpoints->end(), spcomparator);

	//Die 4 Messpunkte, zwischen denen interpoliert werden soll
	vector<SensorPoint*> interpolation_tet = vector<SensorPoint*>(4);
	//Die Indices für die zu verwendenden Messpunkte.
	vector<int> indices = vector<int>(4);

	//Messpunktliste und Indices mit den am nächsten befindlichen Punkten initialisieren
	for (int i = 0; i < 4; i++) {
		interpolation_tet.at(i) = &sensorpoints->at(i);
		indices.at(i) = i;
	}

	//Die für die Extrapolation verwendeten Messpunktindices, wenn keine Interpolation möglich ist
	vector<int>* extrapolationIndices = NULL;
	//Testen, ob der eventuell übergebene Tetraeder bereits geeignet ist
	bool pretest_successful = (prev_tet != NULL)
			&& (pointInsideTetrahedron(p, prev_tet) == 1);

	//Status des Punkt-in-Tetraedertests für den aktuellen Messpunkttetraeder
	status = pretest_successful ? 1 : -1;

	//Bis ein geeigneter Messpunkttetraeder gefunden ist...
	while (status < 1) {
		//Ist der gesuchte Punkt im aktuellen Messpunkttetraeder? (-1 = komplanarer Tetraeder, 0 = Nein, 1 = Ja)
		status = pointInsideTetrahedron(p, &interpolation_tet);

		//der erste (also lokal nächste) nicht komplanare tetraeder wird für die Extrapolation vorgemerkt
		if (status > -1 && extrapolationIndices == NULL) {
			extrapolationIndices = new vector<int>(4);
			for (int i = 0; i < 4; i++) {
				extrapolationIndices->at(i) = indices.at(i);
			}
		}

		//Sind alle Möglichkeiten erfolglos getestet worden? -> Abbruch
		if (indices.at(0) == sensorpointcount - 4
				&& indices.at(1) == sensorpointcount - 3
				&& indices.at(2) == sensorpointcount - 2
				&& indices.at(3) == sensorpointcount - 1) {
			break;
		}

		//ist der gesuchte Punkt nicht im aktuellen Messpunkttetraeder? -> nächste Indexkombination
		if (status < 1) {
			nextCombination(&indices, 3, sensorpointcount);
			for (int j = 0; j < 4; j++) {
				interpolation_tet.at(j) = &sensorpoints->at(indices.at(j));
			}
		}
	}

	//Ist eine Inter- oder Extrapolation möglich?
	if (extrapolationIndices != NULL || pretest_successful) {

		//Muss extrapoliert werden?
		if (status < 1) {
			//vorgemerkte Indices in den aktuellen Messwerttetraeder übertragen
			for (int i = 0; i < 4; i++) {
				interpolation_tet.at(i) = &sensorpoints->at(
						extrapolationIndices->at(i));
			}
		}

		//War der Test mit dem vorher übergebenen Tetraeder erfolgreich??
		if (pretest_successful) {
			//vorher übergebenen Tetraeder in den aktuellen Messwerttetraeder übertragen
			for (int i = 0; i < 4; i++) {
				interpolation_tet.at(i) = prev_tet->at(i);
			}
		}

		//Soll der verwendete Tetraeder gespeichert werden?
		if (current_tet != NULL) {
			//in übergebener Variable speichern
			current_tet->resize(4);
			for (int i = 0; i < 4; i++) {
				current_tet->at(i) = interpolation_tet.at(i);
			}
		}

		//Vorbereiten für die Inter/Extrapoltion
		Vector3D v1 = Vector3D(interpolation_tet.at(0)->coords);
		Vector3D v2 = Vector3D(interpolation_tet.at(1)->coords);
		Vector3D v3 = Vector3D(interpolation_tet.at(2)->coords);
		Vector3D v4 = Vector3D(interpolation_tet.at(3)->coords);
		Vector3D pg = Vector3D(p);
		Tetrahedron tetrahedron = Tetrahedron(&v1, &v2, &v3, &v4);

		double* values = new double[4];
		for (int i = 0; i < 4; i++) {
			values[i] = interpolation_tet.at(i)->temperature;
		}

		//Inter/Extrapolation mit dem ermittelten Tetraeder
		double interval = interpolator->interpolateTet(&tetrahedron, &pg,
				values);

		//Speicher freigeben
		delete[] values;
		delete extrapolationIndices;

		return interval;
	} else {
		cerr
				<< "Alle Messwerte sind komplanar -> keine Inter/Extrapolation möglich!"
				<< endl;
		status = -1;
	}

	//Ermitteln eines Wertes nicht möglich
	return -1;
}

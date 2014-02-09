/*
 * utils.h
 *
 *  Created on: 21.09.2013
 *      Author: valentin
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <wx/wx.h>
#include "../libraries/tetgen/tetgen.h"
#include "../libraries/interpolate/GeometryClasses.h"
#include "../libraries/interpolate/Interpolator.h"
using namespace std;

/**
 * @brief allgemeine Funktionen und Typen.
 */
namespace Utils {
	/**
	 * @brief Zum Punkt-in-Volumen Testen verwendeter Algorithmus.
	 * Dies wird bei ALGORITHM_TETRAHEDRONS über alle Tetraeder des Objekts und deren Flächennormalen ermittelt.
	 * Bei ALGORITHM_RAY werden die Schnittpunkte aller Außenflächen mit einem Strahl gezählt (Aktuell nicht verwendet).
	 */
	enum PIM_algorithm {
		ALGORITHM_TETRAHEDRONS = 0,
		ALGORITHM_RAY,
	};
	/**
	 * Quadriert eine Zahl.
	 * @param d Die zu quadrierende Zahl.
	 * @return \f$d^2\f$.
	 */
	inline double sqr(double d) {
		return d*d;
	}
	/**
	 * @brief Informationen über die Farbgebung bei der Visualisierung.
	 */
	struct Visualization_info {
		int max_visualisation_temp = 100; /**< maximal Visualisierte Temperatur (entspricht der Farbe Rot). */
		int min_visualisation_temp = 0;	  /**< minimal Visualisierte Temperatur (entspricht der Farbe Blau). */
	};
	/**
	 * @brief Hilfsstruktur zum Sortieren von Punkten nach dem Abstand zu einem anderen Punkt.
	 */
	struct SortStruct {
		double distance;	/**< Abstand des Punktes. */
		int pointIndex;		/**< Index des entsprechenden Sensordatenpuntkes. */
	};
	/**
	 * @brief Begrenzt einen Wert auf den Bereich 0..1.
	 * @param h Die zu begrenzende Zahl.
	 * @return Der den Grenzen entsprechende Wert.
	 */
	float clampHue(float h);
	/**
	 * @brief Daten eines Sensordatenpunktes.
	 */
	struct SensorPoint {
		double coords[3]; 	/**< Koordinaten des Punktes. */
		double temperature; /**< Temperatur des Punktes. */
	};
	/**
	 * @brief Daten zur Darstellung einer 2D-Temperaturverteilungs-Ebene.
	 */
	struct CutRender_info {
		Triangle* tri;						/**< Das die Ebene beschreibende Dreieck. Der erste Punkt ist dabei das Zentrum der später ermittelten Temperaturverteilung. */
		float mmperpixel;					/**< Maßstab der Darstellung der Temperaturverteilung in \f$\frac{mm}{Pixel}\f$. */
		int img_width;						/**< Breite der Darstellung der Temperaturverteilung. */
		int img_height;						/**< Höhe der Darstellung der Temperaturverteilung. */
		PIM_algorithm in_volume_algorithm;	/**< Der zu verwendende Punkt-in-Volumen-Testalgorithmus. Immer ALGORITHM_TETRAHEDRONS. */
	};
	/**
	 * @brief Ein Sensordatensatz.
	 */
	struct SensorData {
		vector<vector<SensorPoint> > data;	/**< Daten des Datensatzes (Sensorpunkte zu versch. Zeitpunkten).*/
		vector<string> subnames;			/**< Namen der einzelnen Zeitpunkte. */
		vector<int> timestamps;				/**< Zeitstempel der einzelnen Zeitpunkte. */
		vector<int> markers;				/**< Markierte Zeitpunkte. */
		bool timed;							/**< Sind die Sensordaten zeitbezogen? Wenn nein, ist die Länge von data 1. */
		int current_time_index;				/**< Index des aktuell ausgewählten Zeitpunkts. */
		string name;						/**< Name des Sensordatensatzes. */
	};
	/**
	 * @brief Hilfsstruktur zum Vergleichen des Abstands von Sensordaten.
	 *
	 * Wird für Sortieralgorithmen der Standardbibliothek benötigt.
	 */
	struct SensorPointComparator {
		/**
		 * Berechnet den Abstand zwischen zwei Punkten mithilfe des Satzes des Pythagoras.
		 * @param p1 Koordinaten des ersten Punkten als Liste dreier Koordinaten.
		 * @param p2 Koordinaten des zweiten Punkten als Liste dreier Koordinaten.
		 */
		inline double getDistance_d(double* p1,double* p2) {
			return sqrt(sqr(p1[0]-p2[0])+sqr(p1[1]-p2[1])+sqr(p1[2]-p2[2]));
		};
		double meshpoint[3];		/**< Punkt, zu dem der Abstand ermittelt werden soll. */
		bool operator() (SensorPoint p1,SensorPoint p2) {
			return (getDistance_d(meshpoint,p1.coords)<getDistance_d(meshpoint,p2.coords));
		} /**<  Vergleichsoperator für den Abstand zum Punkt meshpoint. */
	};
	/**
	 * Wandelt eine Fließkommazahl in einen wxWidgets-String um.
	 * @param val Die umzuwandelnde Zahl.
	 * @return Der entstandene String.
	 */
	wxString floattowxstr(double val);
	/**
	 * Wandelt eine Fließkommazahl in einen wxWidgets-String um.
	 * @param val Die umzuwandelnde Zahl.
	 * @param digits Anzahl der zu übernehmenden Stellen.
	 * @return Der entstandene String.
	 */
	wxString floattowxstr(double val,int digits);
	/**
	 * Testet, ob ein Strahl ein Dreieck schneidet.
	 * @param p Ortsvektor zum Ausganspunkt des Strahls.
	 * @param direction Richtung des Strahls.
	 * @param tri Das zu testende Dreieck.
	 * @param depth Ausgabevariablie, ein Maß für den Abstand von Ausganspunkt zu Schnittpunkt.
	 * @return Gibt 1 zurück, wenn es einen Schnittpunkt gibt, ansonsten 0.
	 */
	int rayIntersectsTriangle(Vector3D* p,Vector3D* direction,Triangle* tri,double* depth);
	/**
	 * Testet, ob sich ein Punkt innerhalb eines Körpers befindet.
	 * @param p Der zu testende Punkt.
	 * @param io Der zu testende Körper als Tetgen-Daten (s. Tetgen Dokumentation).
	 * @param algorithm Der zu verwendende Testalgorithmus (Empfohlen und ausschließlich verwendet: ALGORITHM_TETRAHEDRONS).
	 * @return 1 Wenn innerhalb, 0 wenn außerhalb. Bei einer falschen Algorithmuskonstante -1.
	 */
	int pointInsideMesh(Vector3D* p,tetgenio* io,PIM_algorithm algorithm);
	/**
	 * Testet, ob sich ein Punkt innerhalb eines Tetraeders befindet.
	 * @param pges Der zu testende Punkt.
	 * @param v1 Der 1. Punkt des Tetraeders.
	 * @param v2 Der 2. Punkt des Tetraeders.
	 * @param v3 Der 3. Punkt des Tetraeders.
	 * @param v4 Der 4. Punkt des Tetraeders.
	 * @return 1 Wenn innerhalb, 0 wenn außerhalb. -1, wenn der Tetraeder Komplanar ist.
	 */
	int pointInsideTetrahedron(Vector3D* pges, Vector3D* v1, Vector3D* v2, Vector3D* v3, Vector3D* v4);
	/**
	 * Testet, ob sich ein Punkt innerhalb eines Tetraeders befindet.
	 * @param pges Koordinaten des zu testenden Punktes.
	 * @param v1 Koordinaten des 1. Punktes des Tetraeders.
	 * @param v2 Koordinaten des 2. Punktes des Tetraeders.
	 * @param v3 Koordinaten des 3. Punktes des Tetraeders.
	 * @param v4 Koordinaten des 4. Punktes des Tetraeders.
	 * @return 1 Wenn innerhalb, 0 wenn außerhalb. -1, wenn der Tetraeder Komplanar ist.
	 */
	int pointInsideTetrahedron(double* pges, double* v1, double* v2, double* v3, double* v4);
	/**
	 * Testet, ob sich ein Punkt innerhalb eines Tetraeders befindet.
	 * @param p Koordinaten des zu testenden Punktes.
	 * @param tet Der zu untersuchende Tetraeder als Liste von Sensordaten.
	 * @return 1 Wenn innerhalb, 0 wenn außerhalb. -1, wenn der Tetraeder Komplanar ist.
	 */
	int pointInsideTetrahedron(double* p,vector<SensorPoint*>* tet);
	/**
	 * Ermöglicht das generieren aller möglichen Verteilungen von depth+1 Elementen auf dataPointCount Plätze.
	 * Die Indices der Plätze, die die Elemente jeweils besetzten stehen in indices. Diese Funktion generiert aus
	 * der vorherigen Anordnung die Nächste.
	 * @param indices Liste der Indices der Elemente.
	 * @param depth Anzahl der Elemente-1.
	 * @param dataPointCount Anzahl der Plätze.
	 */
	void nextCombination(vector<int>* indices,int depth,int dataPointCount);
	/**
	 * Gibt den inter/extrapolierten Wert eines Punktes zurück.
	 * @param status Rückgabevariable. 1: Punkt wurde extrapoliert 0: Punkt wurde interpoliert. -1: Alle Sensorpunkte sind komplanar.
	 * @param sensorpoints Die zu verwendenden Senosorpunkte.
	 * @param p Die Koordinaten des gesuchten Punktes.
	 * @param interpolator Das zu verwendende Interpolatorobjekt.
	 * @param prev_tet Zuerst zu Testender Tetraeder (optional, NULL zum Nichtverwenden).
	 * @param current_tet Rückgabevariable für den zuletzt verwendeten Tetraeder (optional, NULL zum Nichtverwenden).
	 * @return Temperatur des gesuchten Punktes.
	 */
	double getPointValue(int &status,vector<SensorPoint>* sensorpoints,double* p,Interpolator* interpolator,vector<SensorPoint*>* prev_tet = NULL,vector<SensorPoint*>* current_tet = NULL);
	/**
	 * Wandelt eine Farbe im HSV-Format ins RGB-Format um.
	 * @param h H-Komponente der Farbe.
	 * @param s S-Komponente der Farbe.
	 * @param v V-Komponente der Farbe.
	 * @return RGB-Farbe als Liste mit 3 Werten im Bereich 0..1. Muss manuell mit delete[] freigegeben werden!
	 */
	float* hsvToRgb(float h, float s, float v);
	/**
	 * Kopiert die Eigenschaften eines Sensorpunktes in einen Anderen.
	 * @param from Quelle.
	 * @param to Ziel.
	 */
	void copySensorPoint(SensorPoint* from,SensorPoint* to);
}

#endif /* UTILS_H_ */

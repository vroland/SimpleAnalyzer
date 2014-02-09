/*
 * Analyzer.h
 *
 *  Created on: 11.09.2013
 *      Author: valentin
 */

#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <vector>
#include <string>
#include <iostream>
#include "ObjectData.h"

using namespace std;

/**
 * @brief Ermittelt Daten aus der Temperaturverteilung.
 */
class Analyzer {
public:
	/**
	 * @brief Analyseergebnisse für ein Material.
	 */
	struct AnalyzerData_material {
		string name; 		/**< Der Name des Material.< */
		double volume;		/**< Das Volumen, das dem Material zugeordnet ist.< */
		double heat_energy; /**< Die Wärmeenergie, die das dem Material zugeordnete Volumen enthält.< */
	};
	/**
	 * @brief Analyseergebnisse für einen Sensordatensatz.
	 */
	struct AnalyzerData_dataset {
		string name;							/**< Der Name des Sensordatensatzes< */
		double heat_energy;						/**< Die Wärmeenergie, die das Objekt für diesen Datensatz enthält.*/
		vector<AnalyzerData_material> mat_data; /**< Die Analyseergebnisse für die Einzelnen Materialien. */
	};
	/**
	 * @brief Analyseergebnisse für ein Objekt.
	 */
	struct AnalyzerData_object {
		double volume;							/**< Das Volumen des Objekts.< */
		vector<AnalyzerData_dataset> data_sets; /**< Die Analyseergebisse für die Sensordatensätze.< */
	};
	/**
	 * @brief Analyseergebnisse für einen Punkt.
	 */
	struct AnalyzerData_point {
		double value;		/**< Die Temperatur an diesem Punkt. */
		bool extrapolated;	/**< Ist der Punkt extrapoliert? */
	};
	/**
	 * Operator zum Ausgeben der Analysedaten für ein Objekt im cout-Stream.
	 */
	friend std::ostream &operator<< (std::ostream &out, const AnalyzerData_object &data);
public:
	/**
	 * Der Konstruktor.
	 */
	Analyzer();
	/**
	 * Ermittelt Daten für ein Objekt.
	 * @param obj Das zu analysierende Objekt.
	 * @param out Referenz auf die AnalyzerData_object -Struktur in der die Analyseergebnisse gespeichert werden sollen.
	 * @param use_markers Die markierten Zeitpunkte eines Sensordatensatzes analysieren.
	 * Wenn false wird nur der aktuell ausgewählte Zeitpunkt analysiert.
	 * @param sdindex Nur den Sensordatensatz mit diesem Index analysieren.
	 */
	void analyzeObject(ObjectData* obj,AnalyzerData_object* out,bool use_markers = true,int sdindex = -1);
	/**
	 * Ermittelt Daten für einen Punkt am aktuell ausgewählten Zeitpunkt.
	 * @param obj Das zu analysierende Objekt.
	 * @param point Der Ortsvektor zum zu analysierenden Punkt.
	 * @param point_data Referenz auf die AnalyzerData_point -Struktur in der die Analyseergebnisse gespeichert werden sollen.
	 * @param interpolator Das zu verwendende Interpolatorobjekt.
	 */
	void analyzePoint(ObjectData* obj,Vector3D* point,AnalyzerData_point* point_data,Interpolator* interpolator);
	/**
	 * Der Destruktor.
	 */
	virtual ~Analyzer();
};

#endif /* ANALYZER_H_ */

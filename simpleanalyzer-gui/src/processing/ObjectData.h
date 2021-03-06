/*
 * ObjectData.h
 *
 *  Created on: 07.09.2013
 *      Author: Valentin Roland
 */

#ifndef OBJECTDATA_H_
#define OBJECTDATA_H_

#include "../libraries/tetgen/tetgen.h"
#include <vector>
#include "../libraries/interpolate/Interpolator.h"
#include <wx/string.h>
#include "utils.h"
#define NUMBEROFSENSORATTRIBUTES 1
using namespace std;
using namespace Utils;

/**
 * @brief Die Daten eines Versuchsobjekts.
 *
 * Diese Klasse hält Objekteigenschaften, Materialien und Sensordaten eines untersuchten Objekts.
 * Desweiteren stellt sie die Schnittstelle zur Tetgen-Bibliothek (http://wias-berlin.de/software/tetgen/) zum
 * zerlegen des Objekts dar.
 */
class ObjectData {
public:
	/**
	 * @brief Die Daten eines Materials.
	 */
	struct MaterialData {
		string name; /**< Der Name des Materials. */
		Interpolator::InterpolationMode interpolation_mode; /**< Der zu verwendende Interpolationsmodus.*/
		tetgenio* tetgeninput; /**< Originalgeometrie im Tetgen-Format (s. Tetgen Dokumentation)*/
		tetgenio* tetgenoutput; /**< Durch Zerlegung erstellte Geometrie im Tetgen-Format (s. Tetgen Dokumentation)*/
		bool* extrapolated; /**< Liste, die für jeden Punkt in der aktuellen Geometrie angibt, ob er extra- (true) oder interpoliert (false) ist.*/
		float color[3]; /**< Die Farbe des Materials im RGB-Format. */
		double density; /**< Die Dichte in \f$\frac{kg}{m^3}\f$ */
		double specificheatcapacity; /**< Spezifische Wärmekapazität in \f$\frac{kJ}{kg*K}\f$ */
		bool visible; /**< Soll das Material angezeigt werden? */
	};

	/**
	 * Status einer die Objektdaten betreffenden Aktion.
	 */
	enum ObjectDataStatus {
		OD_SUCCESS = 1,
		OD_FAILURE,
		OD_LOAD_ALREADY_LOADED,
		OD_LOAD_INVALID_FILE,
		OD_LOAD_INVALID_SENSOR_FILE,
	};

	/**
	 * Der Konstruktor.
	 */
	ObjectData();

	/**
	 * Lädt ein Objekt und erste Sensordaten.
	 * @param path Pfad zur 3D-Modell(.obj)-Datei.
	 */
	int loadFromFile(wxString &path);

	/**
	 * Lädt einfache Sensordaten und verknüpft sie mit dem Objekt.
	 * @param path Pfad zur .sd-Datei.
	 */
	int addSensorData(wxString &path);

	/**
	 * Lädt zeitbezogene Sensordaten und verknüpft sie mit dem Objekt.
	 * @param path Pfad zur .tsd-Datei.
	 */
	int addTimedData(wxString &path);

	/**
	 * Zerlegt das Objekt in Tetraeder (Schnittstelle zur Tetgen-Bibliothek) und Berechnet
	 * die Temperaturverteilung für die aktuell ausgewählten Sensordaten (und den aktuelle augewählten Zeitpunkt).
	 */
	int calculateIO();

	/**
	 * Gibt eine Referenz auf die Liste der Materialien (mit Materialdaten) des Objekts zurück.
	 */
	vector<MaterialData>* getMaterials();

	/**
	 * Gibt das maximale Tetraedervolumen für der Zerlegung zurück.
	 */
	double getMaxvolume();

	/**
	 * Setzt das maximale Tetraedervolumen für der Zerlegung.
	 * @param maxvolume Maximales Tetraedervolumen.
	 */
	void setMaxvolume(double maxvolume);

	/**
	 * Gibt den Namen des Objekts zurück.
	 */
	string getName();

	/**
	 * Setzt den Namen des Objekts.
	 * @param name Der neue Name des Objekts.
	 */
	void setName(string name);

	/**
	 * Gibt die Qualisätseinstellung für die Tetraeder bei der Zerlegung (s. Tetgen Dokumentation) zurück.
	 */
	double getQuality();

	/**
	 * Sezt die Qualisätseinstellung für die Tetraeder bei der Zerlegung (s. Tetgen Dokumentation).
	 */
	void setQuality(double quality);

	/**
	 * Gibt eine Referenz auf die Sensordaten des Objekts zurück.
	 */
	vector<SensorData>* getSensorDataList();

	/**
	 * Gibt den Index des aktuell verwendeten Sensordatensatzes zurück.
	 */
	int getCurrentSensorIndex();

	/**
	 * Setzt den Index des aktuell verwendeten Sensordatensatzes.
	 */
	void setCurrentSensorIndex(int currentSensorIndex);

	/**
	 * Der Destruktor.
	 */
	virtual ~ObjectData();

private:
	/**
	 * Index des aktuell verwendeten Sensordatensatzes.
	 */
	int current_sensor_index;

	/**
	 * Name des Objekts.
	 */
	string name;

	/**
	 * Maximales Volumen für Tetraeder bei der Zerlegung.
	 */
	double maxvolume;

	/**
	 * Qualität der Tetraeder bei der Zerlegung (s. Tetgen Dokumentation).
	 */
	double quality;

	/**
	 * Liste der Materialien des Objekts.
	 */
	vector<MaterialData> materials;

	/**
	 * Liste der Sensordaten des Objekts.
	 */
	vector<SensorData> sensorDataList;
};

#endif /* OBJECTDATA_H_ */

/*
 * DefaultValues.h
 *
 *  Created on: 29.08.2014
 *      Author: Valentin Roland
 */

#ifndef DEFAULTVALUES_H_
#define DEFAULTVALUES_H_

#include <string>
#include "GUI/Renderer.h"

using namespace std;

class DefaultValues {
public:
	/**
	 * Lädt die Standardwerte für verschiedene Parameter
	 * @return Ein DefaultsValues-Objekt mit den geladenen Standardwerten
	 */
	DefaultValues();

	/**
	 * Anzahl der Suchpfade für die Standardwertdatei.
	 */
	static const int NUMBEROFPATHS = 3;

	/**
	 * Suchpfade für die Konfigurationsdatei.
	 * Das Verzeichnis der ausführbaren Datei wird immer geprüft.
	 */
	string configpaths[NUMBEROFPATHS] {
			"/etc/simpleanalyzer/simpleanalyzer-gui.conf",
			"/usr/local/share/simpleanalyzer/simpleanalyzer-gui.conf",
			"/usr/share/simpleanalyzer/simpleanalyzer-gui.conf" };

	//object defaults
	double obj_maxvolume = .001;
	double obj_quality = 2;

	//visualization defaults
	float max_visualisation_temp = 100;
	float min_visualisation_temp = 0;

	//renderer defaults
	Renderer::RenderMode viewport_showPoints = Renderer::RM_NONE;
	Renderer::RenderMode viewport_showEdges = Renderer::RM_VALUECOLOR;
	Renderer::RenderMode viewport_showFaces = Renderer::RM_NONE;
	bool viewport_show_extrapolated = true;
	bool viewport_show_sensordata = true;
	float viewport_scale = 1;
};

#endif /* DEFAULTVALUES_H_ */

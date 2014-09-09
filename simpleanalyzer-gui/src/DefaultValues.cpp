/*
 * DefaultValues.cpp
 *
 *  Created on: 29.08.2014
 *      Author: Valentin Roland
 */

#include "DefaultValues.h"
#include "SimpleAnalyzerApp.h"
#include <fstream>
#include "processing/utils.h"

using namespace Utils;

DefaultValues::DefaultValues() {
	//Punkt als Dezimaltrennzeichen verwenden
	setlocale(LC_NUMERIC, "C");

	ifstream cfgfile;

	//Pfad der ausführbaren Datei
	wstring wpath(wxGetApp().argv[0]);
	string path(wpath.begin(), wpath.end());
	cout << path << endl;

	//Versuch, die Standardwertdatei im Verzeichnis der Binärdatei zu öffnen

	cfgfile.open(string(path + ".conf"));

	//Erfolgreich geöffnet?
	if (!cfgfile.is_open()) {

		//Testen der Standardpfade für die Konfigurationsdatei
		for (int i = 0; i < NUMBEROFPATHS; i++) {
			cfgfile.open(configpaths[i]);
			//Erfolgreich geöffnet?
			if (cfgfile.is_open()) {
				break;
			}
		}

		//Konfigurationsdatei nirgendwo gefunden?
		if (!cfgfile.is_open()) {
			cout << "defaults file file simpleanalyzer-gui.conf not found!"
					<< endl;
			//Lesen der Konfiguration fehlgeschlagen
			return;
		}
	}

	string line;

	//einlesen der Standardwerte
	getline(cfgfile, line);
	cout << getTextBlock(line, 0).c_str() << endl;
	obj_maxvolume = atof(getTextBlock(line, 0).c_str());
	getline(cfgfile, line);
	obj_quality = atof(getTextBlock(line, 0).c_str());

	//direkt in globale visualisierungsinformationen schreiben, da diese schon erstellt wurden
	getline(cfgfile, line);
	wxGetApp().getVisualizationInfo()->max_visualisation_temp = atof(
			getTextBlock(line, 0).c_str());
	getline(cfgfile, line);
	wxGetApp().getVisualizationInfo()->min_visualisation_temp = atof(
			getTextBlock(line, 0).c_str());

	getline(cfgfile, line);
	viewport_showPoints = (Renderer::RenderMode) atoi(
			getTextBlock(line, 0).c_str());
	getline(cfgfile, line);
	viewport_showEdges = (Renderer::RenderMode) atoi(
			getTextBlock(line, 0).c_str());
	getline(cfgfile, line);
	viewport_showFaces = (Renderer::RenderMode) atoi(
			getTextBlock(line, 0).c_str());

	getline(cfgfile, line);
	viewport_show_extrapolated = atoi(getTextBlock(line, 0).c_str());
	getline(cfgfile, line);
	viewport_show_sensordata = atoi(getTextBlock(line, 0).c_str());
	getline(cfgfile, line);
	viewport_scale = atof(getTextBlock(line, 0).c_str());

	cfgfile.close();

}

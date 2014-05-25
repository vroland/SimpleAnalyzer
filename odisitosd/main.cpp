/*
 * main.cpp
 *
 *  Created on: 26.10.2013
 *      Author: valentin
 */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>

using namespace std;

/**
 * @brief Konverter von ODiSI zu .tsd.
 *
 * Zusätzlich können Ausreißerwerte erkannt und eliminiert werden.
 */
class OdisiToSdConverter {
protected:
	/**
	 * Anzahl der Suchpfade für die Konfigurationsdatei.
	 */
	static const int NUMBEROFPATHS = 3;

	/**
	 * Suchpfade für die Konfigurationsdatei.
	 * Das Verzeichnis der ausführbaren Datei wird immer geprüft.
	 */
	string configpaths[NUMBEROFPATHS] {
			"/etc/simpleanalyzer/odisitosd.conf",
			"/usr/local/share/simpleanalyzer/odisitosd.conf",
			"/usr/share/simpleanalyzer/odisitosd.conf" };

	/**
	 * @brief Strunktur für die Programmeinstellungen.
	 */
	struct Options {
		size_t startrow; /**< Index der ersten Zeile in der Odisi-Datei, die Sensordaten enthält */
		char separator; /**< Das verwendete Separatorzeichen. (Hier Leerzeichen) */
		bool replace_comma_with_point; /**< Sollen Kommata durch Punkte ersetzt werrden? */
		size_t timecol; /**< Index der Spalte, die die Zeitstempel enthält.*/
		float error_threshold; /**< Maximal zulässige Differenz zum Vorgängerwert für einen gültigen Messwert bei der Fehlerkorrektur.*/
		int maxfwcount; /**< Maximale Schrittanzahl zum finden eines gültigen Messwertes bei der Fehlerkorrektur.*/
		int tab_space_count; /**< Anzahl der Leerzeichen für TAB (Für die Positionsangabe in der Log-Datei).*/
		float height; /**< Höhe der Faserebene in \f$m\f$.*/
		float basetemp; /**< Temperatur zu Beginn des Versuches (Die Odisi-Daten sind Differenzen zu dieser Anfangstemperatur). */
		float objwidth; /**< Position der Messwerte auf der X-Achse um diesen Wert verschieben. */
		bool flipobj; /**< Position auf der X-Achse spiegeln? */
		int fiber_step_delta; /**< Schrittweite beim Auslesen der Sensordaten (nur jeder fiber_step_delta Messpunkt auf der Faser wird verwendet).*/
		int time_step_delta; /**< Schrittweite beim Auslesen der Sensordaten (nur jeder time_step_delta Zeitpunkt wird verwendet).*/
		double max_time; /**< Nur bis maximal zu diesem Zeitstempel auslesen. */
		double min_time; /**< Ab diesem Zeitstempel auslesen. */
	} opts; /**< Hält die verwendeten Programmeinstellungen. */

	/**
	 * Testet, ob sich ein String in einer Liste von Strings befindet.
	 * @param Vec Liste der Strings.
	 * @param Element Der zu suchende String.
	 * @return true, wenn das Element gefunden wurde, sonst false.
	 */
	bool contains(std::vector<string>& Vec, const string& Element) {

		//Ist das gesuchte Element in der Liste?
		if (find(Vec.begin(), Vec.end(), Element) != Vec.end()) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Testet, ob sich eine Ganzzahl in einer Liste von Ganzzahlen befindet.
	 * @param Vec Liste der Ganzzahlen.
	 * @param Element Die zu suchende Ganzzahl.
	 * @return true, wenn das Element gefunden wurde, sonst false.
	 */
	bool contains(std::vector<int>& Vec, const int& Element) {

		//Ist das gesuchte Element in der Liste?
		if (find(Vec.begin(), Vec.end(), Element) != Vec.end()) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Ersetzt in einem String alle Vorkommen eines Teilstrings durch einen Anderen.
	 * @param str Der zu durchsuchende String.
	 * @param from Der zu ersetzende Teilstring.
	 * @param to Der Teilstring, durch den ersetzt werden soll.
	 */
	void replaceAll(string &str, const string from, const string to) {

		//Position des zu ersetzenden Teilstrings
		size_t replace_pos = str.find(from, 0);

		//Bis Stringende
		while (replace_pos != str.npos) {

			//Ersetzen des Teilstrings bei start_pos
			str.replace(replace_pos, from.length(), to);

			//Suchen des nächsten zu ersetzenden Teilstrings
			replace_pos += to.length();
			replace_pos = str.find(from, replace_pos);
		}
	}

	/**
	 * Wandelt eine Zeichenkette (String) um.
	 * @param val Die umzuwandelnde Zahl.
	 * @return Die Entsprechung der Zahl als String.
	 */
	string floattostr(float val) {

		ostringstream ss;
		ss << val;
		return ss.str();
	}

	/**
	 * Gibt den n-ten durch Leerzeichen abgetrennten Block aus einem String zurück.
	 * @param data Der Ausgansstring.
	 * @param n Index des zu findenden Blocks.
	 * @return Der n-te durch Leerzeichen getrennte Teilstring. "" Bei ungültigem Index.
	 */
	string getTextBlock(string data, int n) {

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

	/**
	 * Sammelt Daten aus einer Textzeile (string).
	 * @param line Die zu untersuchende Textzeile.
	 * @param out Ausgabevariable für die Sensordaten der Zeile. Alle Spalten nach opts.start_col werden als Sensordatenspalten betrachtet.
	 * Wenn row_count == opts.startrow ist, werden statt der Sensordaten die Faserpositionen eingelesen!
	 * @param times Wenn nicht NULL, Ausgabevariable für den Zeitstempel der Zeile (opts.timecol). Der Zeitstempel wird an die übergebene Liste angehängt.
	 * @param debug_positions Wenn nicht NULL, Ausgabevariable für die Position der einzelnen Messwerte in der Datei.
	 * Diese Positionen werden in der Logdatei zur Fehlerkorrektur angegeben, um ein Wiederfinden der Werte für den Nutzer einfacher zu machen.
	 * @param row_count Nummer der aktuellen Zeile (Index+1).
	 */
	void parseLine(string line, vector<float>* out, vector<float>* times,
			vector<int>* debug_positions, size_t row_count) {

		//Ist die untersuchte Zeile die Kopfzeile der Tabelle?
		if (row_count == opts.startrow) {
			//Alles vor dem ersten TAB löschen
			size_t pos = line.find('\t');
			line = line.substr(pos + 1, line.length() - pos - 1);
		};

		out->clear();

		//Erstellen eines Strings mit der Anzahl der für TAB gewünschten Leerzeichen
		string tab_to_space = "";
		for (int i = 0; i < opts.tab_space_count; i++) {
			tab_to_space += " ";
		}

		//Ersetzen aller TABs durch die gewünschte Anzahl an Leerzeichen
		replaceAll(line, "\t", tab_to_space);

		//Position des ersten Separatorzeichens im String
		size_t sep_pos = line.find(opts.separator);
		//aktuelle Position im String
		size_t position = 0;
		//Index der aktuellen Spalte
		size_t col_index = 0;

		//Für alle Spalten...
		while (sep_pos != string::npos) {

			//Inhalt der aktuellen Spalte
			string substr;

			//Anfang der neuen Spalte suchen, die Breiter als 0 Zeichen ist
			do {
				//Position des ersten Separatorzeichens
				sep_pos = line.find(opts.separator);
				//Inhalt der Zeile bis zum Separatorzeichen
				substr = line.substr(0, sep_pos);

				//Ist die "Spalte" breiter als 0 Zeichen?
				if (substr == "") {
					position++;
				}

				//Löschen des Zeileninhalts vor dem Separatorzeichen und des Separatorzeichens aus der Zeile
				line.erase(0, sep_pos + 1);

			} while (sep_pos == 0);

			///Komma mit Punkt als Dezimaltrennzeichen ersetzen?
			if (opts.replace_comma_with_point) {
				replaceAll(substr, ",", ".");
			}

			//Enthält die Spalte Sensordaten?
			if (row_count > opts.startrow && col_index > opts.timecol) {

				//Speichern der Daten aus der Spalte in die Ausgabeliste
				debug_positions->resize(debug_positions->size() + 1,
						position + 2);

				//Speichern der Position in der Datei
				out->resize(out->size() + 1, atof(substr.c_str()));
			}

			//Enthält die Spalte Zeitstempel?
			if (col_index == opts.timecol && times != NULL) {

				//Speichern der Zeitstempel in die Ausgabeliste
				times->resize(times->size() + 1, atof(substr.c_str()));
			}

			//Faserpositionen auslesen?
			if (row_count == opts.startrow) {

				//Speichern der Faserpositionen in der Ausgabeliste
				out->resize(out->size() + 1, atof(substr.c_str()));
			}

			col_index++;
			//Mitführen der Position in der Zeile
			position += sep_pos + 1;
		}
	}

	/**
	 * Liest und setzt die Programmkonfiguration aus der Konfigurationsdatei.
	 * @param binary_path Pfad zur Binärdatei.
	 * @return War das Einlesen erfolgreich?
	 */
	bool readConfiguration(string binary_path) {

		ifstream cfgfile;

		//Versuch, die Konfiguration im Verzeichnis der Binärdatei zu öffnen
		cfgfile.open(string(string(binary_path) + ".conf"));

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
				cout << "configuration file odisitosd.conf not found!" << endl;
				//Lesen der Konfiguration fehlgeschlagen
				return false;
			}
		}

		string line;

		getline(cfgfile, line);
		//Lesen des Index für die erste Sensordatenzeile
		opts.startrow = atoi(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Lesen, ob Komma durch Punkt ersetzt werden soll
		opts.replace_comma_with_point = atoi(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Maximal zulässige Differenz zum Vorgängerwert für einen gültigen Messwert bei der Fehlerkorrektur
		opts.error_threshold = atof(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Maximale Schrittanzahl zum finden eines gültigen Messwertes bei der Fehlerkorrektur
		opts.maxfwcount = atoi(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Anzahl der Leerzeichen für TAB (Für die Positionsangabe in der Log-Datei)
		opts.tab_space_count = atoi(getTextBlock(line, 0).c_str()) - 1;
		getline(cfgfile, line);
		//Temperatur zu Beginn des Versuches (Die Odisi-Daten sind Differenzen zu dieser Anfangstemperatur)
		opts.basetemp = atof(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Position auf der X-Achse spiegeln?
		opts.flipobj = atoi(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Höhe der Faserebene in m
		opts.height = atof(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Position der Messwerte auf der X-Achse um diesen Wert verschieben
		opts.objwidth = atof(getTextBlock(line, 0).c_str());

		cfgfile.close();

		//Lesen der Konfiguration erfolgreich
		return true;
	}

	/**
	 * Wertet die Programmargumente aus.
	 * @param argc Anzahl der Programmargumente.
	 * @param argv Die Programmargumente.
	 * @param def_filename Ausgabe für den Pfad zur Sensordefinitionsdatei.
	 * @param data_filename Ausgabe für den Pfad zur Eingabedatei.
	 * @param out_filename Ausgabe für den Pfad zur Ausgabedatei.
	 * @param err_filename Ausgabe für den Pfad zur Logdatei.
	 * @return Soll das Programm weiter ablaufen?
	 */
	bool parseArguments(int argc, char* argv[], string& def_filename,
			string& data_filename, string& out_filename, string& err_filename) {

		//Für alle Argumente...
		for (int i = 1; i < argc; i++) {

			//aktuelles Argument
			string arg = string(argv[i]);

			//Eingabedatei?
			if (arg == "-in" || arg == "-i") {
				data_filename = string(argv[i + 1]);
				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Ausgabedatei?
			if (arg == "-out" || arg == "-o") {
				out_filename = string(argv[i + 1]);
				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Sensordefinitionsdatei?
			if (arg == "-sensor-def" || arg == "-s") {
				def_filename = string(argv[i + 1]);
				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Logdatei?
			if (arg == "-log" || arg == "-l") {
				err_filename = string(argv[i + 1]);
				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Zeitl. Schrittweite?
			if (arg == "-step-time") {
				opts.time_step_delta = atoi(argv[i + 1]);

				//Gültige Schrittweite?
				if (opts.time_step_delta < 1) {
					cerr << "-step-time must be >0!" << endl;
					return false;
				}

				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Örtliche Schrittweite auf der Faser?
			if (arg == "-step-fiber") {
				opts.fiber_step_delta = atoi(argv[i + 1]);

				//Gültige Schrittweite?
				if (opts.fiber_step_delta < 1) {
					cerr << "-step-fiber must be >0!" << endl;
					return false;
				}

				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Höhe der Faserebene?
			if (arg == "-height") {
				opts.height = atof(argv[i + 1]);
				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Maximaler Zeitstempel?
			if (arg == "-max-time") {
				opts.max_time = atof(argv[i + 1]);

				//Standard bei Fehleingabe
				if (opts.max_time < 0) {
					opts.max_time = -1;
				}
				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Minimaler Zeitstempel?
			if (arg == "-min-time") {
				opts.min_time = atof(argv[i + 1]);

				//Standard bei Fehleingabe
				if (opts.min_time < 0) {
					opts.min_time = -1;
				}

				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Hilfetext?
			if (arg == "-h") {
				cout
						<< "odisitosd converts data files from the odisi sensor into timed sensor data (tsd) for SimpleAnalyzer.\n"
						<< "usage: mergetsd ARGUMENT1 VALUE1 ARGUMENT2 VALUE2...\n"
						<< "program arguments: \n"
						<< "\t-i,\t-in\t\tpath to csv input file\n\n"
						<< "\t-o,\t-out\t\tpath to tsd output file\n\n"
						<< "\t-s,\t-sensor-def\tpath to sensor definition file\n\n"
						<< "\t-l,\t-log\t\tpath to log file (optional)\n\n"
						<< "\t\t-height\t\theight of the fiber level (optional, default 0.0)\n\n"
						<< "\t\t-step-time\tstepwidth in time (take every n-th data set) (optional, default 1)\n\n"
						<< "\t\t-step-fiber\tstepwidth on fiber (take every n-th measuring point) (optional, default 1)\n\n"
						<< "\t\t-min-time\tread only from this time on (optional, default -1 (means no restriction))\n\n"
						<< "\t\t-max-time\tread only until this time (optional, default -1 (means no restriction))\n\n"
						<< "\t-h,\t-help\t\tprint this help\n\n"
						<< "configuration details can be set in configuration file: odisitosd.conf\n";

				//Kein weiterer Programmablauf nach Hilfetext.
				return false;
			}
			cout << "invalid argument: " << argv[i] << endl;
		}
		return true;
	}

	/**
	 * Liest die Daten aus der Sensordefinitionsdatei.
	 * @param path Pfad zur Binärdatei.
	 * @param inlist Liste für die Positionen der Fasereingänge auf der Faser.
	 * @param outlist Liste für die Positionen der Faserausgänge auf der Faser.
	 * @param in_x Liste für die X-Positionen der Fasereingänge.
	 * @param out_x Liste für die X-Positionen der Faserausgänge.
	 * @param dirlist Liste für die Richtungen der Faser zwischen Ein-und Ausgang bezüglich der Z-Richtung.
	 * @return War das Einlesen erfolgreich?
	 */
	bool readSensorDefinitions(string path, vector<float>& inlist,
			vector<float>& outlist, vector<float>& in_x, vector<float>& out_x,
			vector<bool>& dirlist) {

		ifstream deffile;
		//Öffnen der Sensordefinitionsdatei
		deffile.open(path);

		//Öffnen der Datei fehlgeschlagen?
		if (!deffile.is_open()) {
			cerr << "sensor definition file \"" << path << "\" not found!"
					<< endl;
			//Laden der Sensordefinitionen fehlgeschlagen
			return false;
		}

		string line;

		//Alle Zeilen...
		while (deffile.good()) {

			getline(deffile, line);

			//Leerzeilen überspringen
			if (line.length() == 0) {
				continue;
			}

			//Kommentarzeilen überspringen
			if (line.at(0) == '#') {
				continue;
			}

			//Fasereingang
			if (line.at(0) == 'i') {
				//Position auf der Faser speichern
				inlist.resize(inlist.size() + 1,
						atof(getTextBlock(line, 1).c_str()));
				//X-Position speichern
				in_x.resize(in_x.size() + 1,
						atof(getTextBlock(line, 2).c_str()));

				//Soll das Objekt gespiegelt werden?
				if (opts.flipobj) {
					in_x.at(in_x.size() - 1) = opts.objwidth
							- in_x.at(in_x.size() - 1);
				}
			}

			//Faserausgang
			if (line.at(0) == 'o') {
				//Position auf der Faser speichern
				outlist.resize(outlist.size() + 1,
						atof(getTextBlock(line, 1).c_str()));
				//X-Position speichern
				out_x.resize(out_x.size() + 1,
						atof(getTextBlock(line, 2).c_str()));

				//Soll das Objekt gespiegelt werden?
				if (opts.flipobj) {
					out_x.at(out_x.size() - 1) = opts.objwidth
							- out_x.at(out_x.size() - 1);
				}
			}

			//Faserrichtungen
			if (line.at(0) == 'd') {
				//Gesamtwert auslesen
				string dirs = getTextBlock(line, 1);

				cout << "Faserrichtungen: " << endl;

				//Einzelne Richtungen auslesen
				for (int i = 0; i < int(dirs.size()); i++) {
					dirlist.resize(dirlist.size() + 1,
							(dirs.at(i) == '+') ? false : true);
					cout << ((dirs.at(i) == '+') ? "►" : "◄") << endl;
				}

				cout << endl;
			}
		}

		//Gleiche Anzahl an Faser-Ein- und Ausgang
		if (outlist.size() != inlist.size()) {
			cerr << "number of fiber in and outs not equal! (in: "
					<< inlist.size() << " out: " << outlist.size() << ")"
					<< endl;
			return false;
		}

		deffile.close();

		//Laden der Sensordefinitionen erfolgreich
		return true;
	}

	/**
	 * Liest die Daten aus der Eingabedatei.
	 * @param path Der Pfad zur Eingabedatei.
	 * @param values Liste für die extrahierten Sensorwerte.
	 * @param times Liste für die Zeitstempel der Messwerte.
	 * @param debug_positions Liste für die Positionen der Messwerte in der Datei.
	 * @param lin_positions Liste für die Positionen auf der Faser.
	 * @return War das Einlesen erfolgreich?
	 */
	bool readInputFile(string path, vector<vector<float> >& values,
			vector<vector<int> >& debug_positions, vector<float>& times,
			vector<float>& lin_positions) {

		ifstream file;
		//Öffnen der Sensordefinitionsdatei
		file.open(path);

		//Öffnen der Datei fehlgeschlagen?
		if (!file.is_open()) {
			cerr << "input file \"" << path << "\" not found!" << endl;
			//Laden der Sensordefinitionen fehlgeschlagen
			return false;
		}

		//Index der aktuellen Zeile
		size_t row_index = 0;
		//aktuelle Zeile
		string line;
		//Sind die Positionen auf der Faser für die Spalten eingelesen?
		bool positions_read = false;

		//Für die gesamte Datei...
		while (file.good()) {

			getline(file, line);

			//Zeile im relevanten Bereich?
			if (row_index >= opts.startrow) {

				//Leerzeilen ignorieren
				if (line == ("")) {
					continue;
				}

				//Die Positionen auf der Faser für die Spalten auslesen?
				if (!positions_read) {
					//Einlesen der Positionen auf der Faser
					parseLine(line, &lin_positions, NULL, NULL, row_index);
					positions_read = true;
				} else {

					values.resize(values.size() + 1);
					debug_positions.resize(debug_positions.size() + 1);
					//Einlesen der aktuellen Zeile
					parseLine(line, &values.at(values.size() - 1), &times,
							&debug_positions.at(debug_positions.size() - 1),
							row_index);
				}

				//Ist der Zeitstempel kleiner als der minimale Zeitstempel?
				if (times.size() > 0
						&& times.at(times.size() - 1) < opts.min_time
						&& opts.min_time != -1) {

					//Datensatz löschen
					values.resize(times.size() - 1);
					debug_positions.resize(times.size() - 1);
					times.resize(times.size() - 1);
				}

				//Ist der Zeitstempel kleiner als der maximale Zeitstempel?
				if (times.size() > 0
						&& times.at(times.size() - 1) > opts.max_time
						&& opts.max_time != -1) {

					//Datensatz löschen
					values.resize(times.size() - 1);
					debug_positions.resize(times.size() - 1);
					times.resize(times.size() - 1);

					//Mit dem Einlesen aufhören
					break;
				}
			}

			row_index++;
		}

		file.close();

		//Laden der Eingabedatei erfolgreich
		return true;
	}

	/**
	 * Schreibt die Ausgabedatei.
	 * @param path Der Pfad zur Ausgabedatei.
	 * @param logpath Der Pfad zur Logdatei. Bei "" wird keine Logdatei angelegt.
	 * @param debug_positions Liste der Positionen der Sensorwerte in der Eingabedatei.
	 * @param values Die extrahierten Sensorwerte.
	 * @param times Zeitstempel der Datensätze.
	 * @param lin_positions Die Position der Messstellen auf der Faser.
	 * @param inlist Positionen der Fasereingänge auf der Faser.
	 * @param outlist Positionen der Faserausgänge auf der Faser.
	 * @param in_x X-Positionen der Fasereingänge.
	 * @param out_x X-Positionen der Faserausgänge.
	 * @param dirlist Liste für die Richtungen der Faser zwischen Ein-und Ausgang bezüglich der Z-Richtung.
	 * @return War das Schreiben erfolgreich?
	 */
	bool writeOutputFile(string path, string logpath,
			vector<vector<float> >& values,
			vector<vector<int> >& debug_positions, vector<float>& times,
			vector<float>& lin_positions, vector<float>& inlist,
			vector<float>& outlist, vector<float>& in_x, vector<float>& out_x,
			vector<bool>& dirlist) {

		ofstream errfile;
		ofstream outfile;
		//Öffnen der Ausgabedatei
		outfile.open(path);

		//Öffnen erfolgreich?
		if (!outfile.is_open()) {
			cerr << "output file path \"" << path << "\" is invalid!" << endl;
			//Schreiben der Ausgabedatei fehlgeschlagen
			return false;
		}

		//Eine Logdatei anlegen?
		if (logpath != "") {
			errfile.open(logpath);
			//Öffnen erfolgreich?
			if (!errfile.is_open()) {
				cerr << "log file path \"" << logpath << "\" is invalid!"
						<< endl;
				//Schreiben der Logdatei fehlgeschlagen
				return false;
			}
		}

		//Statistikvariablen für innen/außen liegende Punkte und deren Wert
		int insidecount = 0;
		int outsidecount = 0;
		double insideval = 0;
		double outsideval = 0;

		//Für alle Datensätze
		for (size_t i = 0; i < values.size(); i++) {

			//Fällt die akuelle Zeile durch die zeitliche Schrittweite?
			if (!((i + 1) % opts.time_step_delta == 0)) {
				continue;
			}

			//Aller hundert Datensätze den prozentualen Fortschritt ausgeben
			if (i % 100) {
				cout << "\r" << int((float) i / values.size() * 100.) << "%";
			}

			//Zeitstempel schreiben
			outfile << "t " << int(times.at(i)) << endl;
			//Namen schreiben
			outfile << "n " << times.at(i) << endl;

			//Für alle Messwerte
			for (size_t j = 0; j < values.at(i).size(); j++) {

				//aktuelle Faserposition
				float l_pos = lin_positions.at(j);
				//Position des Fasereingangs auf der Faser
				float l_in = 0;
				//Position des Faserausgangs auf der Faser
				float l_out = 0;
				//Position des Fasereingangs
				float x_in = 0;
				//Position des Faserausgangs
				float x_out = 0;
				//Ist der Punkt außerhalb des Objekts?
				bool outside = false;
				//Verläuft die Faserrichtung entgegen der Z-Achse?
				bool flip_z = false;

				//Suchen des eingrenzenden Faser-Ein- und Ausgangs
				for (size_t li = 0; li < inlist.size(); li++) {

					if (inlist.at(li) <= l_pos && outlist.at(li) >= l_pos) {
						l_in = inlist.at(li);
						l_out = outlist.at(li);
						x_in = in_x.at(li);
						x_out = out_x.at(li);
						flip_z = dirlist.at(li);
						break;
					}

					//Nichts gefunden? -> Punkt liegt außerhalb des Objekts
					if (li == inlist.size() - 1) {
						outside = true;
					}
				}

				//Ist der Punkt innerhalb des Objekts?
				if (!outside) {

					//Fällt die akuelle Zeile durch die Schrittweite auf der Faser?
					if (!(j % opts.fiber_step_delta == 0)) {
						continue;
					}

					//Punkte innerhalb des Objekts mitzählen
					insidecount++;

					//Kann der Wert korrigiert werden? -> Testen auf Fehler und Versuch der Korrektur
					if (i > 0) {
						//Wert des vorherigen Datensatzes
						float prev_val = values.at(i - 1).at(j);
						//Originalwert
						float original_val = values.at(i).at(j);

						//Suchschritte bei der Messwertkorrektur
						int search_step_count = 0;

						//Solange der Wert nicht innerhalb der zugelassenen Grenzen liegt...
						while ((i + search_step_count + 1 < values.size())
								&& (abs(
										values.at(i + search_step_count).at(j)
												- prev_val)
										> (search_step_count + 1)
												* opts.error_threshold)) {

							search_step_count++;

							//Maximale Suchweite erreicht?
							if (search_step_count == opts.maxfwcount) {

								//Logdatei verwenden?
								if (logpath != "")
									errfile
											<< "count of invalid values too big -> nothing changed!"
											<< " (l. " << i + opts.startrow + 2
											<< ", c. "
											<< debug_positions.at(i).at(j)
											<< ")" << endl;
								//weitere Suche abbrechen
								break;
							}
						}

						//Suche nach validem Wert erfolgreich?
						if (search_step_count > 0) {
							//Korrektur durch lineare Interpolation
							values.at(i).at(j) = prev_val
									+ (values.at(i + search_step_count).at(j)
											- prev_val)
											/ (search_step_count + 1);

							//Logdatei verwenden?
							if (logpath != "")
								errfile << "changed " << original_val << " (l. "
										<< i + opts.startrow + 2 << ", c. "
										<< debug_positions.at(i).at(j)
										<< ") to " << values.at(i).at(j)
										<< endl;
						}
					}

					//Position des Messpunktes Berechnen
					float x = x_in
							+ ((x_out - x_in) / (l_out - l_in))
									* (l_pos - l_in);

					float y = opts.height;
					//Winkel der Faserrichtung zur Z-Achse
					float z_angle = cos(
							asin(((x_out - x_in) / (l_out - l_in))));
					float z = z_angle * (l_pos - l_in);

					//Richtung der Faser umkehren?
					if (flip_z) {
						z = z_angle * (l_out - l_in) - z;
					}

					//Schreiben des Messpunktes
					outfile << "s " << x << " " << y << " " << z << " "
							<< opts.basetemp + values.at(i).at(j) << endl;
					//Mitführen der Statistikdaten
					insideval += values.at(i).at(j);
				} else {
					//Statistik für die ausßerhalb gelegenen Messpunkte
					outsidecount++;
					outsideval += values.at(i).at(j);
				}
			}

			outfile << endl;
		}

		//Ausgeben der statistischen Daten.
		cout << "\r" << endl;
		cout << insidecount << " values inside object, " << outsidecount
				<< " outside object " << endl;
		cout << "Inside average: " << insideval / insidecount
				<< " Outside average: " << outsideval / outsidecount
				<< " (uncorrected)" << endl;

		outfile.close();
		if (logpath != "")
			errfile.close();

		//Schreiben der Ausgabe- und Logdatei erfolgreich
		return true;
	}

public:

	/**
	 * Liest die Programmargumente, die Konfiguration, die Sensordefinitionsdatei und die ODiSI-Datei um eine
	 * .tsd-Datei zu generieren, bzw. die Daten der ODiSI-Datei ein eine .tsd-Datei umzuwandeln.
	 * Wird duch die Funktion main() von außerhalb des Namespaces aufgerufen.
	 * @param argc Anzahl der Programmargumente.
	 * @param argv Die Programmargumente.
	 */
	int convert(int argc, char *argv[]) {

		//Standardwerte für die Progammoptionen
		opts.startrow = 4;
		opts.separator = ' ';
		opts.replace_comma_with_point = true;
		opts.timecol = 0;
		opts.error_threshold = 5;
		opts.maxfwcount = 10;
		opts.tab_space_count = 8 - 1;
		opts.height = .0;
		opts.basetemp = 20;
		opts.objwidth = .5;
		opts.flipobj = true;
		opts.fiber_step_delta = 1;
		opts.time_step_delta = 1;
		opts.max_time = -1;
		opts.min_time = -1;

		//Pfad zur Sensordefinitionsdatei
		string def_filename;
		//Pfad zur Eingabedatei
		string data_filename;
		//Pfad zur Logdatei
		string err_filename;
		//Pfad zur Ausgabedatei
		string out_filename;

		//Einlesen der Konfiguration aus der Konfigurationsdatei erfolgreich?
		if (!readConfiguration(string(argv[0]))) {
			return 1;
		}

		//Alle Programmargumente auslesen, weiterer Programmablauf?
		if (!parseArguments(argc, argv, def_filename, data_filename,
				out_filename, err_filename)) {
			return 1;
		}

		//Sind die Zeitbeschränkungen sinnvoll?
		if (opts.min_time > opts.max_time and opts.max_time != -1) {
			cerr << "-min-time can't be greater than -max-time!" << endl;
			return 1;
		}

		//Liste für die Positionen der Ein- und Ausgänge auf der Faser
		vector<float> inlist;
		vector<float> outlist;
		//Liste für die X-Positionen der Ein- und Ausgänge
		vector<float> in_x;
		vector<float> out_x;
		//Liste für die Richtungen der Faser zwischen Ein-und Ausgang bezüglich der Z-Richtung.
		vector<bool> dirlist;

		//Einlesen der Sensordefinitionen erfolgreich?
		if (!readSensorDefinitions(def_filename, inlist, outlist, in_x,
				out_x, dirlist)) {
			return 1;
		}

		//Liste für die Messwerte
		vector<vector<float> > dataset_values;
		//Liste für die Position der Werte in der Datei
		vector<vector<int> > debug_positions;
		//Liste für die Zeitstempel der Datensätze
		vector<float> dataset_times;
		//Liste für die Position der Messpunkte auf der Faser
		vector<float> lin_positions;

		//Einlesen der Messwerte erfolgreich?
		if (!readInputFile(data_filename, dataset_values, debug_positions,
				dataset_times, lin_positions)) {
			return 1;
		}

		//Schreiben der Ausgabe- und Logdatei erfolgreich?
		if (!writeOutputFile(out_filename, err_filename, dataset_values,
				debug_positions, dataset_times, lin_positions, inlist, outlist,
				in_x, out_x, dirlist)) {
			return 1;
		}

		cout << "files successfully created." << endl;
		return 0;
	};

};

int main(int argc, char *argv[]) {
	OdisiToSdConverter converter;
	return converter.convert(argc, argv);
};

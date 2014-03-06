/*
 * main.cpp
 *
 *  Created on: 17.10.2013
 *      Author: valentin
 */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string.h>

using namespace std;

/**
 * @brief Konverter von .csv zu .tsd.
 */
class CsvToSdConverter {
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
			"/etc/simpleanalyzer/csvtosd.conf",
			"/usr/local/share/simpleanalyzer/csvtosd.conf",
			"/usr/share/simpleanalyzer/csvtosd.conf" };

	/**
	 * @brief Strunktur für die Programmeinstellungen.
	 */
	struct Options {
		size_t start_col; /**< Index der Spalte, in der die ersten Sensordaten stehen. */
		char separator; /**< Das verwendete Separatorzeichen. */
		bool replace_comma_with_point; /**< Sollen Kommata durch Punkte ersetzt werrden? */
		size_t timecol; /**< Index der Spalte, die die Zeitstempel enthält. */
		size_t namecol; /**< Index der Spalte, die die Namen für die Datensätze enthält. */
		int time_step_delta; /**< Schrittweite beim Auslesen der Sensordaten (nur jeder time_step_delta Zeitpunkt wird verwendet).*/
		long max_time; /**< Nur bis maximal zu diesem Zeitstempel auslesen. */
		long min_time; /**< Ab diesem Zeitstempel auslesen. */
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
	 * @param timestamps Wenn nicht NULL, Ausgabevariable für den Zeitstempel der Zeile (opts.timecol). Der Zeitstempel wird an die übergebene Liste angehängt.
	 * @param names Wenn nicht NULL, Ausgabevariable für den Namen der Zeile (opts.namecol). Der Name wird an die übergebene Liste angehängt.
	 * @param valid_cols Wenn nicht NULL, werden nur die Sensordaten-Spalten mit den Indices dieser Liste ausgewertet.
	 */
	void parseLine(string line, vector<string> &out, vector<string>* timestamps,
			vector<string>* names, vector<int>* valid_cols) {

		out.clear();
		//Position des ersten Separatorzeichens
		size_t separator_pos = line.find(opts.separator);
		//aktueller Spaltenindex
		size_t col_index = 0;

		//Bis Zeilenende
		while (separator_pos != line.npos) {

			//Position des Separatorzeichens
			separator_pos = line.find(opts.separator);
			//Inhalt der aktuellen Spalte
			string substr = line.substr(0, separator_pos);
			//Löschen der Spalte aus der Zeile
			line.erase(0, separator_pos + 1);

			//Ist die Spalte leer?
			if (substr == "") {
				continue;
			}

			//Enthält die Spalte Sensordaten?
			if (col_index >= opts.start_col) {

				//Darf die Spalte ausgelesen werden?
				if (valid_cols == NULL
						|| contains(*valid_cols, col_index - opts.start_col)) {

					//Speichern der Daten aus der Spalte in die Ausgabeliste
					out.resize(out.size() + 1, substr);
				}
			}

			//Enthält die Spalte den Zeitstempel und soll dieser ausgelesen werden?
			if (col_index == opts.timecol && timestamps != NULL) {

				//Speichern des Zeitstempels in die entsprechende Ausgabeliste
				timestamps->resize(timestamps->size() + 1, substr);
			}

			//Enthält die Spalte den Namen des Datensatzes?
			if (col_index == opts.namecol && names != NULL) {

				//Speichern des Namens in die entsprechende Ausgabeliste
				names->resize(names->size() + 1, substr);
			}

			col_index++;
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
	 * Liest und setzt die Programmkonfiguration aus der Konfigurationsdatei.
	 * @param binary_path Pfad zur Binärdatei.
	 * @return War das Einlesen erfolgreich?
	 */
	bool readConfiguration(string binary_path) {

		ifstream cfgfile;

		//Versuch, die Konfiguration im Verzeichnis der Binärdatei zu öffnen
		cfgfile.open(string(binary_path + ".conf"));
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
				cout << "configuration file csvtosd.conf not found!" << endl;
				//Lesen der Konfiguration fehlgeschlagen
				return false;
			}
		}

		string line;
		getline(cfgfile, line);
		//Lesen des Index für die erste Sensordatenspalte
		opts.start_col = atoi(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Lesen des Separatorzeichens
		opts.separator = getTextBlock(line, 0).c_str()[0];
		getline(cfgfile, line);
		//Lesen, ob Komma durch Punkt ersetzt werden soll
		opts.replace_comma_with_point = atoi(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Lesen des Index der Zeitstempelspalte
		opts.timecol = atoi(getTextBlock(line, 0).c_str());
		getline(cfgfile, line);
		//Lesen des Index der Namensspalte
		opts.namecol = atoi(getTextBlock(line, 0).c_str());

		cfgfile.close();

		//Lesen der Konfiguration erfolgreich
		return true;
	}

	/**
	 * Liest die Daten aus der Sensordefinitionsdatei.
	 * @param path Pfad zur Binärdatei.
	 * @param sensor_names Liste für die Namen der Sensoren.
	 * @param sensor_data Liste für die Daten der Sensorden (Koordinaten).
	 * @return War das Einlesen erfolgreich?
	 */
	bool readSensorDefinitions(string path, vector<string>* sensor_names,
			vector<string>* sensor_data) {

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

		//Alles aus der Datei auslesen
		while (deffile.good()) {

			getline(deffile, line);

			//Leerzeilen überspringen
			if (line == "") {
				continue;
			}

			//Kommentarzeilen überspringen
			if (line.at(0) == '#') {
				continue;
			}

			//Diesen Block nicht an Leerzeichen aufsplitten
			bool dont_split = false;

			for (size_t i = 0; i < line.length(); i++) {

				//aktuelles Zeichen der Zeile
				char c = line.at(i);

				//Startet/Beendet einen Abschnitt, der nicht getrennt werden darf (in Anführungszeichen)
				if (c == '"') {
					dont_split = !dont_split;
				}

				//An Leerzeichen in Sensorname und -daten trennen
				if (c == ' ' && !dont_split) {

					string name = line.substr(0, i);
					//Herausfiltern der Anführungszeichen aus dem Namen
					replaceAll(name, "\"", "");

					string data = line.substr(i + 1, line.length() - i);
					//Herausfiltern der Anführungszeichen aus den Daten
					replaceAll(data, "\"", "");

					//Speichern von Namen und Daten in den Listen
					sensor_names->resize(sensor_names->size() + 1, name);
					sensor_data->resize(sensor_data->size() + 1, data);

					//nächste Zeile
					break;
				}
			}
		}

		deffile.close();

		//Laden der Sensordefinitionen erfolgreich
		return true;
	}

	/**
	 * Wertet die Programmargumente aus.
	 * @param argc Anzahl der Programmargumente.
	 * @param argv Die Programmargumente.
	 * @param sdef_file Ausgabe für den Pfad zur Sensordefinitionsdatei.
	 * @param input_file Ausgabe für den Pfad zur Eingabedatei.
	 * @param output_file Ausgabe für den Pfad zur Ausgabedatei.
	 * @return Soll das Programm weiter ablaufen?
	 */
	bool parseArguments(int argc, char* argv[], string& sdef_file,
			string& input_file, string& output_file) {

		//Für alle Argumente...
		for (int i = 1; i < argc; i++) {

			//aktuelles Argument
			string arg = string(argv[i]);

			//Eingabedatei?
			if (arg == "-in" || arg == "-i") {
				input_file = string(argv[i + 1]);
				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Ausgabedatei?
			if (arg == "-out" || arg == "-o") {
				output_file = string(argv[i + 1]);
				//Den Argumentwert nicht als Argument einlesen
				i++;
				continue;
			}

			//Sensordefinitionsdatei?
			if (arg == "-sensor-def" || arg == "-s") {
				sdef_file = string(argv[i + 1]);
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
						<< "csvtosd converts character-separated values (csv) into timed sensor data (tsd) for SimpleAnalyzer.\n"
						<< "usage: mergetsd ARGUMENT1 VALUE1 ARGUMENT2 VALUE2...\n"
						<< "program arguments: \n"
						<< "\t-i\t-in\t\tpath to csv input file\n\n"
						<< "\t-o\t-out\t\tpath to tsd output file\n\n"
						<< "\t-s\t-sensor-def\tpath to sensor definition file\n\n"
						<< "\t\t-step-time\tstepwidth in time (take every n-th data set) (optional, default 1)\n\n"
						<< "\t\t-min-time\tread only from this time on (optional, default -1 (means no restriction))\n\n"
						<< "\t\t-max-time\tread only until this time (optional, default -1 (means no restriction))\n\n"
						<< "\t-h\t-help\t\tprint this help\n\n"
						<< "configuration details can be set in configuration file: csvtosd.conf\n";

				//Kein weiterer Programmablauf nach Hilfetext.
				return false;
			}

			cerr << "invalid argument: " << argv[i] << endl;
		}
		return true;
	}

	/**
	 * Liest die Daten aus der Eingabedatei.
	 * @param path Der Pfad zur Eingabedatei.
	 * @param sensor_names Liste der Namen der verwendeten Sensoren.
	 * @param values Liste für die extrahierten Sensorwerte.
	 * @param timestamps Liste für die Zeitstempel der Messwerte.
	 * @param names Liste für die Namen der Datensätze.
	 * @return War das Einlesen erfolgreich?
	 */
	bool readInputFile(string path, vector<string>& sensor_names,
			vector<vector<string> >& values, vector<string>& timestamps,
			vector<string>& names) {

		ifstream file;
		//Öffnen der Eingabedatei
		file.open(path);

		//Öffnen erfolgreich?
		if (!file.is_open()) {
			cerr << "input file \"" << path << "\" not found!" << endl;
			//Laden der Eingabedatei fehlgeschlagen
			return false;
		}

		//Liste für die auszulesenden Sensoren bzw. Spalten
		vector<int> valid_cols;
		//Index der aktuellen mit Messdaten gefüllten Zeile
		int data_row_count = 0;
		//Sind die Namen der Sensoren eingelesen?
		bool names_read = false;
		//aktuelle Zeile
		string line;

		//Für die gesamte Datei...
		while (file.good()) {

			getline(file, line);

			//Leerzeilen ignorieren
			if (line == ("")) {
				continue;
			}

			//Die Spaltennamen auslesen?
			if (!names_read) {
				vector<string> col_names;
				//Einlesen der Spaltennamen
				parseLine(line, col_names, NULL, NULL, NULL);

				//Testen, ob die Spalten in der Sensordefinitionsdatei beschrieben sind
				for (size_t i = 0; i < col_names.size(); i++) {

					//Existiert ein Sensor mit dem aktuellen Spaltennamen?
					if (contains(sensor_names, col_names.at(i))) {

						//Zu den untersuchten Spalten hinzufügen
						valid_cols.resize(valid_cols.size() + 1, i);
					} else {
						cerr << "HINT: sensor \"" << col_names.at(i)
								<< "\" from csv not found in sensor definition file!"
								<< endl;
					}
				}

				names_read = true;
			} else {

				data_row_count++;

				//Fällt die akuelle Zeile durch die festgelegte Schrittweite?
				if (!(data_row_count % opts.time_step_delta == 0)) {
					continue;
				}

				//Komma mit Punkt als Dezimaltrennzeichen ersetzen?
				if (opts.replace_comma_with_point) {
					replaceAll(line, ",", ".");
				}

				values.resize(values.size() + 1);
				//Einlesen der aktuellen Zeile
				parseLine(line, values.at(values.size() - 1), &timestamps,
						&names, &valid_cols);

				//Ist der Zeitstempel kleiner als der minimale Zeitstempel?
				if (timestamps.size() > 0 && opts.min_time != -1
						&& atoi(timestamps.at(timestamps.size() - 1).c_str())
								< opts.min_time) {

					//Datensatz löschen
					values.resize(timestamps.size() - 1);
					names.resize(timestamps.size() - 1);
					timestamps.resize(timestamps.size() - 1);
				}

				//Ist der Zeitstempel größer als der maximale Zeitstempel?
				if (timestamps.size() > 0 && opts.max_time != -1
						&& atoi(timestamps.at(timestamps.size() - 1).c_str())
								> opts.max_time) {

					//Datensatz löschen
					values.resize(timestamps.size() - 1);
					names.resize(timestamps.size() - 1);
					timestamps.resize(timestamps.size() - 1);

					//Mit dem Einlesen aufhören
					break;
				}
			}
		}

		file.close();

		//Laden der Eingabedatei erfolgreich
		return true;
	}

	/**
	 * Schreibt die Ausgabedatei.
	 * @param path Der Pfad zur Ausgabedatei.
	 * @param sensor_names Liste der Namen der verwendeten Sensoren.
	 * @param sensor_data Liste der Koordinaten der verwendeten Sensoren.
	 * @param values Liste für die extrahierten Sensorwerte.
	 * @param timestamps Liste für die Zeitstempel der Messwerte.
	 * @param names Liste für die Namen der Datensätze.
	 * @return War das Schreiben erfolgreich?
	 */
	bool writeOutputFile(string path, vector<string>& sensor_names,
			vector<string>& sensor_data, vector<vector<string> >& values,
			vector<string>& timestamps, vector<string>& names) {

		ofstream outfile;
		//Öffnen der Ausgabedatei
		outfile.open(path);
		//Öffnen erfolgreich?
		if (!outfile.is_open()) {
			cerr << "output file \"" << path << "\" is invalid!" << endl;
			//Schreiben der Ausgabedatei fehlgeschlagen
			return false;
		}

		//Schreiben der Sensornamen als Kommentar
		outfile << "#sensors:\n";
		for (size_t i = 0; i < sensor_names.size(); i++) {
			outfile << "#" << sensor_names.at(i) << "\n";
		}

		//Schreiben der Datensätze
		for (size_t j = 0; j < values.size(); j++) {

			//Zeitstempel des Datensatzes
			outfile << "t " << timestamps.at(j) << endl;
			//Name des Datensatzes
			outfile << "n " << names.at(j) << endl;

			//Werte des Datensatzes
			for (size_t i = 0; i < sensor_names.size(); i++) {
				outfile << "s " << sensor_data.at(i) << " "
						<< values.at(j).at(i) << "\n";
			}
		}

		outfile.close();

		//Schreiben der Ausgabedatei erfolgreich
		return true;
	}

public:

	/**
	 * Wandelt die Daten der .csv-Datei ein eine .tsd-Datei um.
	 * Wird duch die Funktion main() von außerhalb des Namespaces aufgerufen.
	 * @param argc Anzahl der Programmargumente.
	 * @param argv Die Programmargumente.
	 */
	int convert(int argc, char *argv[]) {

		//Standardwerte für die Progammoptionen
		opts.start_col = 4;
		opts.separator = ';';
		opts.replace_comma_with_point = true;
		opts.timecol = 2;
		opts.namecol = 1;
		opts.max_time = -1;
		opts.min_time = -1;
		opts.time_step_delta = 1;

		//Pfad zur Sensordefinitionsdatei
		string sdef_file = "";
		//Pfad zur Eingabedatei
		string input_file = "";
		//Pfad zur Ausgabedate
		string output_file = "";

		//Einlesen der Konfiguration aus der Konfigurationsdatei erfolgreich?
		if (!readConfiguration(string(argv[0]))) {
			return 1;
		}

		//Alle Programmargumente auslesen, weiterer Programmablauf?
		if (!parseArguments(argc, argv, sdef_file, input_file, output_file)) {
			return 1;
		}

		//Sind die Zeitbeschränkungen sinnvoll?
		if (opts.min_time > opts.max_time and opts.max_time != -1) {
			cerr << "-min-time can't be greater than -max-time!" << endl;
			return 1;
		}

		//Liste für die Namen der Sensoren
		vector<string> sensor_names;
		//Liste für die Daten der Sensoren
		vector<string> sensor_data;

		//Einlesen der Sensordefinitionen erfolgreich?
		if (!readSensorDefinitions(sdef_file, &sensor_names, &sensor_data)) {
			return 1;
		}

		//Liste für die Werte der Datensätze
		vector<vector<string> > dataset_values(0);
		//Liste für die Zeitstempel der Datensätze
		vector<string> dataset_timestamps(0);
		//Liste für die Namen der Datensätze
		vector<string> dataset_names(0);

		//Einlesen der Messwerte erfolgreich?
		if (!readInputFile(input_file, sensor_names, dataset_values,
				dataset_timestamps, dataset_names)) {
			return 1;
		}

		//Schreiben der Ausgabedatei erfolgreich?
		if (!writeOutputFile(output_file, sensor_names, sensor_data,
				dataset_values, dataset_timestamps, dataset_names)) {
			return 1;
		}

		cout << "files successfully created." << endl;

		//Konvertieren der Daten erfolgreich.
		return 0;
	}
	;

};

int main(int argc, char *argv[]) {
	CsvToSdConverter converter;
	return converter.convert(argc, argv);
}

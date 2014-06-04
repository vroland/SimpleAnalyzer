//============================================================================
// Name        : mergetsd.cpp
// Author      : Valentin Roland
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
using namespace std;

/**
 * @brief Zusammenführen zweier .tsd-Dateien.
 */
class TsdMerger {
protected:

	/**
	 * @brief Strunktur für die Programmeinstellungen.
	 */
	struct Options {
		int offset; 		/**< Ein zusätzlicher Versatz, der zu den Zeitstempeln der zweiten Datei addiert wird. */
		unsigned int max_dt; 	/**< Maximale Zeitdifferenz zwischen den Zeitstempeln um die Datensätze zusammenführen zu können. */
		long int delta; 	/**< Ein Versatz, der zu den Zeitstempeln der zweiten Datei addiert wird. */
		bool auto_delta; 	/**< Delta automatisch aus der Differenz der jeweils ersten Zeitstempel der Eingabedateien ermitteln. */
	} opts; /**< Hält die verwendeten Programmeinstellungen. */

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
	 * Sammelt Daten aus einer .tsd-Datei.
	 * @param filename Der Pfad zur .tsd-Datei.
	 * @param timestamps Ausgabevariable für die Zeitstempel der Datensätze.
	 * @param names Ausgabevariable für den Namen der Datensätze.
	 * @param data Ausgabevariable für die Sensordaten der Datensätze.
	 * @return Gibt 0 bei Erfolg zurück, 1, wenn die Datei nicht gefunden werden konnte.
	 */
	int parseFile(string filename, vector<long> &timestamps,
			vector<string> &names, vector<string> &data) {

		ifstream file;
		//Öffnen der Eingabedatei
		file.open(filename.c_str());

		//Öffnen erfolgreich?
		if (!file.is_open()) {
			cerr << "file \"" << filename << "\" not found!" << endl;
			//Datei nicht gefunden
			return 1;
		}

		string line;

		//Alle Zeilen...
		while (file.good()) {
			getline(file, line);

			//Leerzeilen ignorieren
			if (line == "") {
				continue;
			}

			//Zeitstempel eines Datensatzes lesen
			if (line.at(0) == 't') {
				timestamps.resize(timestamps.size() + 1,
						atoi(getTextBlock(line, 1).c_str()));
				data.resize(data.size() + 1, "");
			}

			//Namen eines Datensatzes lesen
			if (line.at(0) == 'n') {
				names.resize(names.size() + 1, getTextBlock(line, 1));
			}

			//Daten des Datensatzes lesen
			if (line.at(0) == 's') {
				data.at(data.size() - 1) += line + "\n";
			}
		}

		file.close();

		//Lesen der Datei erfolgreich
		return 0;
	}

	/**
	 * Wertet die Programmargumente aus.
	 * @param argc Anzahl der Programmargumente.
	 * @param argv Die Programmargumente.
	 * @param input1 Ausgabe für den Pfad zur Eingabedatei 1.
	 * @param input2 Ausgabe für den Pfad zur Eingabedatei 2.
	 * @param output_file Ausgabe für den Pfad zur Ausgabedatei.
	 * @return Soll das Programm weiter ablaufen?
	 */
	bool parseArguments(int argc, char* argv[], string& input1,
			string& input2, string& output_file) {

		//Für alle Argumente...
		for (int i = 1; i < argc; i++) {

			//aktuelles Argument
			string arg = string(argv[i]);

			//Eingabedatei 1?
			if (arg == "-i1" || arg == "in1") {
				input1 = string(argv[i + 1]);
				i++;
				continue;
			}

			//Eingabedatei 2?
			if (arg == "-i2" || arg == "in2") {
				input2 = string(argv[i + 1]);
				i++;
				continue;
			}

			//Ausgabedatei?
			if (arg == "-out" || arg == "-o") {
				output_file = string(argv[i + 1]);
				i++;
				continue;
			}

			//Versatz für die Zeitstempel der 2. Datei
			if (arg == "-offset") {
				opts.offset = atoi(argv[i + 1]);
				i++;
				continue;
			}

			//Maximale Zeitdifferenz zwischen den Zeitstempeln um die Datensätze zusammenführen zu können
			if (arg == "-max-dt") {
				opts.max_dt = atoi(argv[i + 1]);

				//Warnung bei Fehleingabe
				if (opts.max_dt < 0) {
					cerr << "max-dt (max time difference) must be positive!"
							<< endl;
					//Programmausgührung stoppen
					return false;
				}

				i++;
				continue;
			}

			//Einen Versatz automatisch aus der Differenz der jeweils ersten Zeitstempel der Eingabedateien ermitteln
			if (arg == "-auto-offset") {
				//übergebener Wert
				string val = string(argv[i + 1]);
				//Eingaben in Kleinbuchstaben umwandeln
				transform(val.begin(), val.end(), val.begin(), ::tolower);

				//auto-offset verwenden?
				if (val == "true" || val == "1") {
					opts.auto_delta = true;
				} else {

					//auto-offset nicht verwenden?
					if (val == "false" || val == "0") {
						opts.auto_delta = false;
					} else {
						//Warnung bei Fehleingabe
						cerr
								<< "invalid value for -auto-offset! possible values are true/false or 1/0!"
								<< endl;
						//Programmausgührung stoppen
						return false;
					}
				}
				i++;
				continue;
			}

			//Hilfetext ausgeben
			if (arg == "-h") {
				cout
						<< "mergetsd merges two timed sensor data (tsd) files into one.\n"
						<< "usage: mergetsd ARGUMENT1 VALUE1 ARGUMENT2 VALUE2...\n"
						<< "program arguments: \n"
						<< "\t-i1,\t-in1\t\tpath to first tsd input file (defines dataset names)\n\n"
						<< "\t-i2,\t-in2\t\tpath to second tsd input file\n\n"
						<< "\t-o,\t-out\t\tpath to tsd output file\n\n"
						<< "\t\t-max-dt\t\tmaximum absolute value of the time difference between\n"
						<< "\t\t\t\ttwo datasets to be merged (optional, default 3) in seconds\n\n"
						<< "\t\t-offset\t\toffset added to time values of second input file in seconds\n"
						<< "\t\t\t\t(if -auto-offset is true additionally to the auto-offset, can be negative, optional, default 0)\n\n"
						<< "\t\t-auto-offset\tadd an offset of the difference of the first time value to the second time value\n"
						<< "\t\t\t\tto the second time value (can be true/false or 1/0, optional, default true)\n\n"
						<< "\t-h,\t-help\t\tprint this help\n\n";

				//Programmausgührung stoppen
				return false;
			}
			cout << "invalid argument: " << argv[i] << endl;
		}

		return true;
	}

	/**
	 * Schreibt die Ausgabedatei.
	 * @param path Der Pfad zur Ausgabedatei.
	 * @param timestamps1 Zeitstempel der Datensätze der ersten Datei.
	 * @param timestamps2 Zeitstempel der Datensätze der zweiten Datei.
	 * @param names1 Namen der Datensätze der ersten Datei.
	 * @param names2 Namen der Datensätze der zweiten Datei.
	 * @param data1 Daten der Datensätze der ersten Datei.
	 * @param data2 Daten der Datensätze der zweiten Datei.
	 * @return War das Schreiben erfolgreich?
	 */
	bool writeOutputFile(string path, vector<long>& timestamps1,
			vector<string>& names1, vector<string>& data1,
			vector<long>& timestamps2, vector<string>& names2,
			vector<string>& data2) {

		//Anzahl der zusammengeführten Datensätze
		int matched_datasets = 0;
		ofstream outfile;
		//Öffnen der Ausgabedatei
		outfile.open(path.c_str());

		//Öffnen erfolgreich?
		if (!outfile.good()) {
			cerr << "file \"" << path << "\" could not be created!"
					<< endl;
			//Schreiben fehlgeschlagen
			return false;
		}

		//Alle Datensätze miteinander Vergleichen
		for (size_t i = 0; i < timestamps1.size(); i++) {
			for (size_t j = 0; j < timestamps2.size(); j++) {

				//Zeitdifferenz zwischen den Datensätzen
				int delta_t = timestamps1.at(i) - (timestamps2.at(j) 
					+ opts.offset + opts.delta);

				//kleiner als das maximum?
				if ((unsigned int) abs(delta_t) <= opts.max_dt) {
					//Zusammenführen und schreiben
					outfile << "t " << timestamps1.at(i) << endl;
					outfile << "n " << names1.at(i) << endl;
					outfile << data1.at(i);
					outfile << data2.at(j);
					outfile << endl;
					matched_datasets++;
					//nächster Datensatz
					break;
				}
			}
		}
		outfile.close();
		//Ausgeben der Statistik
		cout << "merged " << matched_datasets << " data sets" << endl;

		//Schreieb erfolgreich
		return true;
	}

public:

	/**
	 * Liest die Programmargumente um die Eingabedateien anhand der Zeitstempel in eine .tsd-Datei zusammen zu führen.
	 * Wird duch die Funktion main() von außerhalb des Namespaces aufgerufen.
	 * @param argc Anzahl der Programmargumente.
	 * @param argv Die Programmargumente.
	 */
	int merge(int argc, char *argv[]) {

		//Standardwerte für die Progammoptionen
		opts.auto_delta = 1;
		opts.offset = 0;
		opts.max_dt = 3;

		//Pfad zur ersten Eingabedatei
		string input1;
		//Pfad zur zweiten Eingabedatei
		string input2;
		//Pfad zur ersten Ausgabedatei
		string output_file;

		//Alle Programmargumente auslesen, weiterer Programmablauf?
		if (!parseArguments(argc, argv, input1, input2, output_file)) {
			return 1;
		}

		//Zeitstempel, Namen und Daten der 1. Datei
		vector<long> timestamps1;
		vector<string> names1;
		vector<string> data1;
		//Zeitstempel, Namen und Daten der 2. Datei
		vector<long> timestamps2;
		vector<string> names2;
		vector<string> data2;

		//Einlesen der 1. Datei
		if (parseFile(input1, timestamps1, names1, data1)) {
			return 1;
		}

		//Einlesen der 2. Datei
		if (parseFile(input2, timestamps2, names2, data2)) {
			return 1;
		}

		//Automatischen Versatz berechnen?
		if (opts.auto_delta) {
			opts.delta = timestamps1.at(0) - (timestamps2.at(0));
		}

		//Schreiben der Ausgabedatei erfolgreich?
		if (!writeOutputFile(output_file, timestamps1, names1, data1,
				timestamps2, names2, data2)) {
			return 1;
		}

		return 0;
	}
};

int main(int argc, char *argv[]) {
	TsdMerger merger;
	return merger.merge(argc, argv);
}

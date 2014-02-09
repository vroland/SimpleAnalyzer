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
 * @brief Namespace des ODiSI-zu-.tsd-Converterprogramms.
 *
 * Zusätzlich können Ausreißerwerte erkannt und eliminiert werden.
 */
namespace ODiSIToSD {
/**
	 * Anzahl der Suchpfade für die Konfigurationsdatei.
	 */
	const int NUMBEROFPATHS = 2;
	/**
	 * Suchpfade für die Konfigurationsdatei.
	 * Das Verzeichnis der ausführbaren Datei wird immer geprüft.
	 */
	string configpaths[NUMBEROFPATHS] {
		"/usr/local/share/simpleanalyzer/odisitosd.conf",
		"/usr/share/simpleanalyzer/odisitosd.conf"
	};
	/**
	 * @brief Strunktur für die Programmeinstellungen.
	 */
	struct Options {
		size_t startrow;				/**< Index der ersten Zeile in der Odisi-Datei, die Sensordaten enthält */
		char separator;					/**< Das verwendete Separatorzeichen. (Hier Leerzeichen) */
		bool replace_comma_with_point;	/**< Sollen Kommata durch Punkte ersetzt werrden? */
		size_t timecol;					/**< Index der Spalte, die die Zeitstempel enthält.*/
		float error_threshold;			/**< Maximal zulässige Differenz zum Vorgängerwert für einen gültigen Messwert bei der Fehlerkorrektur.*/
		int maxfwcount;					/**< Maximale Schrittanzahl zum finden eines gültigen Messwertes bei der Fehlerkorrektur.*/
		int tab_space_count;			/**< Anzahl der Leerzeichen für TAB (Für die Positionsangabe in der Log-Datei).*/
		float height;					/**< Höhe der Faserebene in \f$m\f$.*/
		float basetemp;					/**< Temperatur zu Beginn des Versuches (Die Odisi-Daten sind Differenzen zu dieser Anfangstemperatur). */
		float objwidth;					/**< Position der Messwerte auf der X-Achse um diesen Wert verschieben. */
		bool flipobj;					/**< Position auf der X-Achse spiegeln? */
		int fiber_step_delta;			/**< Schrittweite beim Auslesen der Sensordaten (nur jeder fiber_step_delta Messpunkt auf der Faser wird verwendet).*/
		int time_step_delta;			/**< Schrittweite beim Auslesen der Sensordaten (nur jeder time_step_delta Zeitpunkt wird verwendet).*/
		double max_time;				/**< Nur bis maximal zu diesem Zeitstempel auslesen. */
		double min_time;				/**< Ab diesem Zeitstempel auslesen. */
	} opts;		/**< Hält die verwendeten Programmeinstellungen. */
	/**
	 * Testet, ob sich ein String in einer Liste von Strings befindet.
	 * @param Vec Liste der Strings.
	 * @param Element Der zu suchende String.
	 * @return true, wenn das Element gefunden wurde, sonst false.
	 */
	bool contains( std::vector<string>& Vec, const string& Element )
	{
		if (find(Vec.begin(), Vec.end(), Element) != Vec.end())
			return true;

		return false;
	}
	/**
	 * Testet, ob sich eine Ganzzahl in einer Liste von Ganzzahlen befindet.
	 * @param Vec Liste der Ganzzahlen.
	 * @param Element Die zu suchende Ganzzahl.
	 * @return true, wenn das Element gefunden wurde, sonst false.
	 */
	bool contains( std::vector<int>& Vec, const int& Element )
	{
		if (find(Vec.begin(), Vec.end(), Element) != Vec.end())
			return true;

		return false;
	}
	/**
	 * Ersetzt in einem String alle Vorkommen eines Teilstrings durch einen Anderen.
	 * @param str Der zu durchsuchende String.
	 * @param from Der zu ersetzende Teilstring.
	 * @param to Der Teilstring, durch den ersetzt werden soll.
	 */
	void replaceAll(string &str, const string &from, const string &to) {
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
	}
	/**
	 * Gibt die Position des ersten Vorkommens eines Zeichens in einem String zurück.
	 * @param str Der zu durchsuchende String.
	 * @param chr Das zu findende Zeichen.
	 * @return Gibt den Index des ersten Vorkommens des Zeichens zurück. Wenn das Zeichen nicht gefunden wird -1.
	 */
	int find(string str,char chr) {
		for (int i=0;i<int(str.length());i++) {
			if (str.at(i)==chr) {
				return i;
			}
		}
		return -1;
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
	string getTextBlock(string data,int n) { //Gibt den n-ten durch Leerzeichen abgetrennten Block zurück
		int pos = 0;
		int prevPos = 0;
		size_t curPos = 0;
		curPos = data.find(" ");
		if (curPos==data.npos) {
			return "";
		}
		pos = curPos;
		for (int i=0;i<n;i++) {
			curPos = data.find(" ",pos+1);
			if (curPos==data.npos) {
				if (n-i>1) {	//Mehr Blöcke als vorhanden nachgefragt
					return "";
				} else {
					prevPos = pos+1;
					pos = data.size();
					break;
				}
			}
			prevPos = pos+1;
			pos = curPos;

		}
		return data.substr(prevPos,pos-(prevPos));
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
	void parseLine(string line,vector<float>* out,vector<float>* times,vector<int>* debug_positions,size_t row_count) {
		string workstr;
		if (row_count==opts.startrow) {
			int pos = find(line,'\t');
			workstr = line.substr(pos+1,line.length()-pos-1);
		} else {
			workstr = line.substr(0,line.length());
		}
		out->clear();
		size_t count = 0;
		string tab_to_space = "";
		for (int i=0;i<opts.tab_space_count;i++) {
			tab_to_space+=" ";
		}
		replaceAll(workstr,"\t",tab_to_space);
		int sep_pos = find(workstr,opts.separator);
		int position = 0;
		while (sep_pos>-1) {
			string substr;
			do {
				sep_pos = find(workstr,opts.separator);
				substr= workstr.substr(0,sep_pos);
				if (substr=="") {
					position++;
				}
				workstr.erase(0,sep_pos+1);
			} while (sep_pos==0);
			if (row_count>opts.startrow && count>opts.timecol) {
				debug_positions->resize(debug_positions->size()+1,position+2);
				out->resize(out->size()+1,atof(substr.c_str()));
			}
			if (count==opts.timecol && times!=NULL) {
				times->resize(times->size()+1,atof(substr.c_str()));
			}
			// Auslesen der Faserpositionen
			if (row_count==opts.startrow) {
				out->resize(out->size()+1,atof(substr.c_str()));
			}
			count++;
			position += sep_pos+1;
		}
	}
	/**
	 * Liest die Programmargumente, die Konfiguration, die Sensordefinitionsdatei und die ODiSI-Datei um eine
	 * .tsd-Datei zu generieren, bzw. die Daten der ODiSI-Datei ein eine .tsd-Datei umzuwandeln.
	 * Wird duch die Funktion main() von außerhalb des Namespaces aufgerufen.
	 * @param argc Anzahl der Programmargumente.
	 * @param argv Die Programmargumente.
	 */
	int convert(int argc, char *argv[]) {

		opts.startrow = 4;
		opts.separator = ' ';
		opts.replace_comma_with_point = true;
		opts.timecol = 0;
		opts.error_threshold = 5;
		opts.maxfwcount = 10;
		opts.tab_space_count = 8-1;
		opts.height = .0;
		opts.basetemp = 20;
		opts.objwidth = .5;
		opts.flipobj  = true;
		opts.fiber_step_delta = 1;
		opts.time_step_delta = 1;
		opts.max_time = -1;
		opts.min_time = -1;
		string def_filename;
		string data_filename;
		string err_filename;
		string out_filename;

		ifstream cfgfile;			//Config file
		cfgfile.open(string(string(argv[0])+".conf").c_str());
		if (!cfgfile.is_open()) {
			for (int i=0;i<NUMBEROFPATHS;i++) {
				cfgfile.open(configpaths[i].c_str());
				if (cfgfile.is_open()) break;
			}
			if (!cfgfile.is_open()) {
				cout << "configuration file odisitosd.conf not found!"<<endl;
				return 1;
			}
		}
		string line;
		getline(cfgfile,line);
		opts.startrow = atoi(getTextBlock(line,0).c_str());
		getline(cfgfile,line);
		opts.replace_comma_with_point = atoi(getTextBlock(line,0).c_str());
		getline(cfgfile,line);
		opts.timecol = atoi(getTextBlock(line,0).c_str());
		getline(cfgfile,line);
		opts.error_threshold = atof(getTextBlock(line,0).c_str());
		getline(cfgfile,line);
		opts.maxfwcount = atoi(getTextBlock(line,0).c_str());
		getline(cfgfile,line);
		opts.tab_space_count = atoi(getTextBlock(line,0).c_str())-1;
		getline(cfgfile,line);
		opts.basetemp = atof(getTextBlock(line,0).c_str());
		getline(cfgfile,line);
		opts.flipobj = atoi(getTextBlock(line,0).c_str());
		getline(cfgfile,line);
		opts.height = atof(getTextBlock(line,0).c_str());
		getline(cfgfile,line);
		opts.objwidth = atof(getTextBlock(line,0).c_str());
		cfgfile.close();

		int i=0;
		while (i<argc-1) {
			i++;
			string arg = string(argv[i]);
			if (arg=="-in" || arg=="-i") {
				data_filename = string(argv[i+1]);
				i++;
				continue;
			}
			if (arg=="-out" || arg=="-o") {
				out_filename = string(argv[i+1]);
				i++;
				continue;
			}
			if (arg=="-sensor-def" || arg=="-s") {
				def_filename = string(argv[i+1]);
				i++;
				continue;
			}
			if (arg=="-log" || arg=="-l") {
				err_filename = string(argv[i+1]);
				i++;
				continue;
			}
			if (arg=="-step-time") {
				opts.time_step_delta = atoi(argv[i+1]);
				if (opts.time_step_delta<1) {
					cerr << "-step-time must be >0!" << endl;
					return 1;
				}
				i++;
				continue;
			}
			if (arg=="-step-fiber") {
				opts.fiber_step_delta = atoi(argv[i+1]);
				if (opts.fiber_step_delta<1) {
					cerr << "-step-fiber must be >0!" << endl;
					return 1;
				}
				i++;
				continue;
			}
			if (arg=="-height") {
				opts.height = atof(argv[i+1]);
				i++;
				continue;
			}
			if (arg=="-max-time") {
				opts.max_time = atof(argv[i+1]);
				if (opts.max_time<0) {
					opts.max_time = -1;
				}
				i++;
				continue;
			}
			if (arg=="-min-time") {
				opts.min_time = atof(argv[i+1]);
				if (opts.min_time<0) {
					opts.min_time = -1;
				}
				i++;
				continue;
			}
			if (arg=="-h") {
				cout << "odisitosd converts data files from the odisi sensor into timed sensor data (tsd) for SimpleAnalyzer.\n"
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
				return 0;
			}
			cout << "invalid argument: " << argv[i] << endl;
		}
		if (opts.min_time>opts.max_time and opts.max_time!=-1) {
			cerr << "-min-time can't be greater than -max-time!" << endl;
			return 1;
		}

		bool names_read = false;
		ifstream deffile;			//Sensor definitions
		deffile.open(def_filename.c_str());
		if (!deffile.is_open()) {
			cerr << "sensor definition file \""<<def_filename<< "\" not found!"<<endl;
			return 1;
		}
		vector<float> inlist;
		vector<float> outlist;
		vector<float> in_x;
		vector<float> out_x;
		while (deffile.good()) {
			getline(deffile,line);
			if (line.length()==0) continue;
			if (line.at(0)=='#') continue;
			if (line.at(0)=='i') {
				inlist.resize(inlist.size()+1,atof(getTextBlock(line,1).c_str()));
				in_x.resize(in_x.size()+1,atof(getTextBlock(line,2).c_str()));
				if (opts.flipobj) {
					in_x.at(in_x.size()-1) = opts.objwidth-in_x.at(in_x.size()-1);
				}
				//in_y.resize(in_y.size()+1,atof(getTextBlock(line,3).c_str()));
			}
			if (line.at(0)=='o') {
				outlist.resize(outlist.size()+1,atof(getTextBlock(line,1).c_str()));
				out_x.resize(out_x.size()+1,atof(getTextBlock(line,2).c_str()));
				if (opts.flipobj) {
					out_x.at(out_x.size()-1) = opts.objwidth-out_x.at(out_x.size()-1);
				}

				//out_y.resize(in_y.size()+1,atof(getTextBlock(line,3).c_str()));
			}
		}
		if (outlist.size()!=inlist.size()) {
			cerr << "number of fiber in and outs not equal! (in: "<<inlist.size()<<" out: "<<outlist.size()<<")" << endl;
			return 1;
		}
		deffile.close();
		ifstream file;					// data file
		file.open(data_filename.c_str());
		if (!file.is_open()) {
			cerr << "input file \""<<data_filename<< "\" not found!"<<endl;
			return 1;
		}
		vector<vector<float> > values;
		vector<vector<int> > debug_positions;
		vector<float> times;
		vector<float> lin_positions;
		size_t row = 0;
		while (file.good()) {
			getline(file,line);
			if (row>=opts.startrow) {
				if (line==("")) continue;
				if (!names_read) {
					parseLine(line,&lin_positions,NULL,NULL,row);
					names_read = true;
				} else {
					if (opts.replace_comma_with_point) {
						replaceAll(line,",", ".");
					}
					values.resize(values.size()+1);
					debug_positions.resize(debug_positions.size()+1);
					parseLine(line,&values.at(values.size()-1),&times,&debug_positions.at(debug_positions.size()-1),row);	//&valid_cols
				}
				if (times.size()>0 && times.at(times.size()-1)<opts.min_time && opts.min_time!=-1) {
					values.resize(times.size()-1);
					debug_positions.resize(times.size()-1);
					times.resize(times.size()-1);
				}
				if (times.size()>0 && times.at(times.size()-1)>opts.max_time && opts.max_time!=-1) {
					values.resize(times.size()-1);
					debug_positions.resize(times.size()-1);
					times.resize(times.size()-1);
					break;
				}
			}
			row++;
		}
		ofstream errfile;					// error output
		ofstream outfile;					// output
		outfile.open(out_filename.c_str());
		if (!outfile.is_open()) {
			cerr << "output file path \""<<out_filename<< "\" is invalid!"<<endl;
		}
		if (err_filename!="") {
			errfile.open(err_filename.c_str());
			if (!errfile.is_open()) {
				cerr << "log file path \""<<err_filename<< "\" is invalid!"<<endl;
			}
		}
		int insidecount = 0;
		int outsidecount = 0;
		double insideval = 0;
		double outsideval= 0;
		for (size_t i=0;i<values.size();i++) {
			if (!((i+1)%opts.time_step_delta==0)) continue;
			if (i%100) cout << "\r"<<int((float)i/values.size()*100.) << "%";
			vector<float>* curr_delta = &values.at(i);
			values.at(i).resize(curr_delta->size());
			outfile << "t "<<int(times.at(i))<<endl;
			outfile << "n "<<times.at(i)<<endl;
			for (size_t j=0;j<values.at(i).size();j++) {

				// Calculate position
				float l_pos = lin_positions.at(j);
				float l_in  = 0;
				float l_out = 0;
				float x_in  = 0;
				float x_out = 0;
				//float y_in  = 0;
				//float y_out = 0;
				bool outside = false;
				for (size_t li=0;li<inlist.size();li++) {
					if (inlist.at(li)<=l_pos && outlist.at(li)>=l_pos) {
						l_in = inlist.at(li);
						l_out= outlist.at(li);
						x_in = in_x.at(li);
						x_out= out_x.at(li);
						//y_in = in_y.at(li);
						//y_out= out_y.at(li);
						break;
					}
					if (li==inlist.size()-1) {
						outside = true;
					}
				}
				if (!outside) {
					if (!(j%opts.fiber_step_delta==0)) continue;
					insidecount++;
					// calc value
					int fwcount = 0;
					if (i>0) {
						float prev_val = values.at(i-1).at(j);
						float original_val = values.at(i).at(j);
						while ((i+fwcount+1<values.size()) && (abs(values.at(i+fwcount).at(j)-prev_val)>(fwcount+1)*opts.error_threshold)) {
							fwcount++;
							if (fwcount==opts.maxfwcount) {
								if (err_filename!="") errfile << "count of invalid values too big -> nothing changed!" <<" (l. "<<i+opts.startrow+2<<", c. "<<debug_positions.at(i).at(j)<<")"<< endl;
								break;
							}
						}
						if (fwcount>0) {
							values.at(i).at(j) = prev_val+(values.at(i+fwcount).at(j)-prev_val)/(fwcount+1);
							if (err_filename!="") errfile << "changed " <<original_val<<" (l. "<<i+opts.startrow+2<<", c. "<<debug_positions.at(i).at(j)<<") to "<<values.at(i).at(j)<< endl;
						}
					}
					float x = x_in+((x_out-x_in)/(l_out-l_in))*(l_pos-l_in);
					float y = opts.height;
					float z = cos(asin(((x_out-x_in)/(l_out-l_in))))*(l_pos-l_in);
					outfile <<"s "<<x<<" "<<y<<" "<<z<< " "<<opts.basetemp+values.at(i).at(j) << endl;
					insideval+=values.at(i).at(j);
				} else {
					outsidecount++;
					outsideval+=values.at(i).at(j);
				}
			}
			outfile << endl;

		}
		cout << "\r"<<endl;
		cout << insidecount << " values inside object, "<<outsidecount << " outside object "<<endl;
		cout << "Inside average: " << insideval/insidecount << " Outside average: " << outsideval/outsidecount << endl;
		outfile.close();
		if (err_filename!="") errfile.close();
		cout << "files successfully created." << endl;
		return 0;
	};
}

int main(int argc, char *argv[]) {
	return ODiSIToSD::convert(argc,argv);
};

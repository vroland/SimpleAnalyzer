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

struct Options {
	size_t startcol;
	char separator;
	bool replace_comma_with_point;
	size_t timecol;
	size_t namecol;
	int time_step_delta;
	long max_time;
	long min_time;
} opts;

bool contains( std::vector<string>& Vec, const string& Element )
{
    if (find(Vec.begin(), Vec.end(), Element) != Vec.end())
        return true;

    return false;
}
bool contains( std::vector<int>& Vec, const int& Element )
{
    if (find(Vec.begin(), Vec.end(), Element) != Vec.end())
        return true;

    return false;
}
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

int parseLine(string line,vector<string> &out,vector<string>* timestamps,vector<string>* names,vector<int>* valid_cols) {
	string workstr = line.substr(0,line.length());
	out.clear();
	size_t sep_pos = workstr.find(opts.separator);
	size_t count = 0;
	while (sep_pos!=workstr.npos) {
		sep_pos = workstr.find(opts.separator);
		string substr = workstr.substr(0,sep_pos);
		workstr.erase(0,sep_pos+1);
		if (substr=="") continue;
		if (count>=opts.startcol) {
			if (valid_cols==NULL || contains(*valid_cols,count-opts.startcol)) {
				out.resize(out.size()+1,substr);
			}
		}
		if (count==opts.timecol && timestamps!=NULL) {
			timestamps->resize(timestamps->size()+1,substr);
		}
		if (count==opts.namecol && names!=NULL) {
			names->resize(names->size()+1,substr);
		}
		count++;
	}
	return 0;
}

void replaceAll(string &str, const string &from, const string &to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
int main(int argc, char *argv[]) {

	opts.startcol = 4;
	opts.separator = ';';
	opts.replace_comma_with_point = true;
	opts.timecol = 2;
	opts.namecol = 1;
	opts.max_time = -1;
	opts.min_time = -1;
	opts.time_step_delta = 1;
	bool names_read = false;
	string sdef_file = "";
	string input_file = "";
	string output_file = "";
	int i=0;
	while (i<argc-1) {
		i++;
		string arg = string(argv[i]);
		if (arg=="-in" || arg=="-i") {
			input_file = string(argv[i+1]);
			i++;
			continue;
		}
		if (arg=="-out" || arg=="-o") {
			output_file = string(argv[i+1]);
			i++;
			continue;
		}
		if (arg=="-sensor-def" || arg=="-s") {
			sdef_file = string(argv[i+1]);
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
			cout << "csvtosd converts character-separated values (csv) into timed sensor data (tsd) for SimpleAnalyzer.\n"
				 << "program arguments: \n"
				 << "\t-i\t-in\t\tpath to csv input file\n\n"
				 << "\t-o\t-out\t\tpath to tsd output file\n\n"
				 << "\t-s\t-sensor-def\tpath to sensor definition file\n\n"
				 << "\t\t-step-time\tstepwidth in time (take every n-th data set) (optional)\n\n"
				 << "\t\t-min-time\tread only from this time on (optional)\n\n"
				 << "\t\t-max-time\tread only until this time (optional)\n\n"
				 << "\t-h\t-help\t\tprint this help\n\n"
				 << "configuration details can be set in configuration file: csvtosd.conf\n";
			return 0;
		}
		cout << "invalid argument: " << argv[i] << endl;
	}
	if (opts.min_time>opts.max_time and opts.max_time!=-1) {
		cerr << "-min-time can't be greater than -max-time!" << endl;
		return 1;
	}

	ifstream cfgfile;			//Sensor definitions
	cfgfile.open("csvtosd.conf");
	if (!cfgfile.is_open()) {
		cout << "configuration file csvtosd.cfg not found!"<<endl;
		return 1;
	}
	string line;
	getline(cfgfile,line);
	opts.startcol = atoi(getTextBlock(line,0).c_str());
	getline(cfgfile,line);
	opts.separator = getTextBlock(line,0).c_str()[0];
	getline(cfgfile,line);
	opts.replace_comma_with_point = atoi(getTextBlock(line,0).c_str());
	getline(cfgfile,line);
	opts.timecol = atoi(getTextBlock(line,0).c_str());
	getline(cfgfile,line);
	opts.namecol = atoi(getTextBlock(line,0).c_str());
	cfgfile.close();

	ifstream deffile;			//Sensor definitions
	deffile.open(sdef_file.c_str());
	if (!deffile.is_open()) {
		cerr << "sensor definition file \""<<sdef_file<< "\" not found!"<<endl;
		return 1;
	}
	vector<string> sensor_names;
	vector<string> sensor_data;
	while (deffile.good()) {
		getline(deffile,line);
		if (line=="") continue;
		bool instr = false;
		if (line.at(0)=='#') continue;
		for (size_t i=0;i<line.length();i++) {
			char c = line.at(i);
			if (c=='"') instr=!instr;
			if (c==' ' && !instr) {
				string name = line.substr(0,i);
				replaceAll(name,"\"","");
				string data = line.substr(i+1,line.length()-i);
				replaceAll(data,"\"","");
				sensor_names.resize(sensor_names.size()+1,name);
				sensor_data.resize(sensor_data.size()+1,data);
				break;
			}
		}
	}
	deffile.close();

	ifstream file;					// data file
	file.open(input_file.c_str());
	if (!file.is_open()) {
		cerr << "input file \""<<input_file<< "\" not found!"<<endl;
		return 1;
	}
	vector<int> valid_cols;
	vector<vector<string> > values(0);
	vector<string> timestamps;
	vector<string> names;
	int data_row_count = 0;
	while (file.good()) {
		getline(file,line);
		if (line==("")) continue;
		if (!names_read) {
			vector<string> col_names;
			parseLine(line,col_names,NULL,NULL,NULL);
			for (size_t i=0;i<col_names.size();i++) {
				if (contains(sensor_names,col_names.at(i))) {
					valid_cols.resize(valid_cols.size()+1,i);
				} else {
					cerr << "HINT: sensor \""<<col_names.at(i) << "\" from csv not found in sensor definition file!" << endl;
				}
			}
			names_read = true;
		} else {
			data_row_count++;
			if (!(data_row_count%opts.time_step_delta==0)) continue;
			if (opts.replace_comma_with_point) {
				replaceAll(line,",", ".");
			}
			values.resize(values.size()+1);
			parseLine(line,values.at(values.size()-1),&timestamps,&names,&valid_cols);
			if (timestamps.size()>0 && atoi(timestamps.at(timestamps.size()-1).c_str())<opts.min_time && opts.min_time!=-1) {
				values.resize(timestamps.size()-1);
				names.resize(timestamps.size()-1);
				timestamps.resize(timestamps.size()-1);
			}
			if (timestamps.size()>0 && atoi(timestamps.at(timestamps.size()-1).c_str())>opts.max_time && opts.max_time!=-1) {
				values.resize(timestamps.size()-1);
				names.resize(timestamps.size()-1);
				timestamps.resize(timestamps.size()-1);
				break;
			}
		}
	}
	file.close();
	ofstream outfile;					// output
	outfile.open(output_file.c_str());
	if (!outfile.is_open()) {
		cerr << "output file \""<<output_file<< "\" is invalid!"<<endl;
		return 1;
	}
	outfile << "#sensors:\n";
	for (size_t i=0;i<sensor_names.size();i++) {
		outfile << "#" << sensor_names.at(i) <<"\n";
	}
	for (size_t j=0;j<values.size();j++) {
		outfile << "t " << timestamps.at(j) << endl;
		outfile << "n " << names.at(j) << endl;
		for (size_t i=0;i<sensor_names.size();i++) {
			outfile << "s "<< sensor_data.at(i) << " " << values.at(j).at(i) << "\n";
		}
	}
	outfile.close();
	cout << "files successfully created." << endl;
	return 0;
};



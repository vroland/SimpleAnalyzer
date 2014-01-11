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

struct Options {
	int offset;
	unsigned int max_dt;
	long int delta;
	bool auto_delta;
} opts;

bool contains( std::vector<string>& Vec, const string& Element )
{
    if (find(Vec.begin(), Vec.end(), Element) != Vec.end())
        return true;

    return false;
}
bool contains( std::vector<int>& Vec, const long& Element )
{
    if (find(Vec.begin(), Vec.end(), Element) != Vec.end())
        return true;

    return false;
}
void replaceAll(string &str, const string &from, const string &to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
int find(string str,char chr) {
	for (int i=0;i<int(str.length());i++) {
		if (str.at(i)==chr) {
			return i;
		}
	}
	return -1;
}
string floattostr(float val) {
	ostringstream ss;
	ss << val;
	return ss.str();
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
int parseFile(string filename,vector<long> &timestamps,vector<string> &names,vector<string> &data) {
	ifstream file;					// data file
	file.open(filename.c_str());
	if (!file.is_open()) {
		cerr << "file \""<<filename<<"\" not found!"<<endl;
		return 1;
	}
	string line;
	while (file.good()) {
		getline(file,line);
		if (line=="") continue;
		if (line.at(0)=='t') {
			timestamps.resize(timestamps.size()+1,atoi(getTextBlock(line,1).c_str()));
			data.resize(data.size()+1,"");
		}
		if (line.at(0)=='n') {
			names.resize(names.size()+1,getTextBlock(line,1));
		}
		if (line.at(0)=='s') {
			data.at(data.size()-1)+=line+"\n";
		}
	}
	file.close();
	return 0;
}


int main(int argc, char *argv[]) {
	opts.offset			   = 0;
	opts.max_dt			   = 3;
	int i=0;
	string input1;
	string input2;
	string output_file;
	while (i<argc-1) {
		i++;
		string arg = string(argv[i]);
		if (arg=="-i1" || arg=="in1") {
			input1 = string(argv[i+1]);
			i++;
			continue;
		}
		if (arg=="-i2" || arg=="in2") {
			input2 = string(argv[i+1]);
			i++;
			continue;
		}
		if (arg=="-out" || arg=="-o") {
			output_file = string(argv[i+1]);
			i++;
			continue;
		}
		if (arg=="-offset") {
			opts.offset = atoi(argv[i+1]);
			i++;
			continue;
		}
		if (arg=="-max-dt") {
			opts.max_dt = atoi(argv[i+1]);
			if (opts.max_dt<0) {
				cerr << "max-dt (max time difference) must be positive!" << endl;
			}
			i++;
			continue;
		}
		if (arg=="-auto-offset") {
			string val = string(argv[i+1]);
			transform(val.begin(), val.end(), val.begin(), ::tolower);
			if (val=="true" || val=="1") {
				opts.auto_delta = true;
			} else {
				if (val=="false" || val=="0") {
					opts.auto_delta = false;
				} else {
					cerr << "invalid value for -auto-offset! possible values are true/false or 1/0!"<<endl;
				}
			}
			i++;
			continue;
		}
		if (arg=="-h") {
			cout << "mergetsd merges two timed sensor data (tsd) files into one.\n"
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
			return 0;
		}
		cout << "invalid argument: " << argv[i] << endl;
	}
	vector<long> timestamps1;
	vector<string> names1;
	vector<string> data1;
	vector<long> timestamps2;
	vector<string> names2;
	vector<string> data2;
	if (parseFile(input1,timestamps1,names1,data1)) return 1;
	if (parseFile(input2,timestamps2,names2,data2)) return 1;
	if (opts.auto_delta) opts.delta = timestamps1.at(0)-(timestamps2.at(0));

	int matched_datasets = 0;
	ofstream outfile;
	outfile.open(output_file.c_str());
	if (!outfile.good()) {
		cerr << "file \""<<output_file<<"\" could not be created!" << endl;
	}
	for (size_t i = 0;i<timestamps1.size();i++) {
		for (size_t j = 0;j<timestamps2.size();j++) {
			int delta_t = (timestamps1.at(i)-(timestamps2.at(j))+opts.offset)-opts.delta;
			if ((unsigned int)abs(delta_t)<=opts.max_dt) {
				outfile << "t "<<timestamps1.at(i) << endl;
				outfile << "n "<<names1.at(i) << endl;
				outfile << data1.at(i);
				outfile << data2.at(j);
				outfile << endl;
				matched_datasets++;
				break;
			}
		}
	}
	outfile.close();
	cout << "merged "<<matched_datasets<<" data sets"<<endl;
	return 0;
}

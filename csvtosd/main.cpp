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

using namespace std;

struct Options {
	size_t startcol;
	char separator;
	bool replace_comma_with_point;
	size_t timecol;
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
int parseLine(string line,vector<string> &out,vector<string>* times,vector<int>* valid_cols) {
	string workstr = line.substr(0,line.length());
	out.clear();
	size_t sep_pos = workstr.find(opts.separator);
	size_t count = 0;
	while (sep_pos!=workstr.npos) {
		sep_pos = workstr.find(opts.separator);
		string substr = workstr.substr(0,sep_pos);
		workstr.erase(0,sep_pos+1);
		if (count>=opts.startcol) {
			if (valid_cols==NULL || contains(*valid_cols,count-opts.startcol)) {
				out.resize(out.size()+1,substr);
			}
		}
		if (count==opts.timecol && times!=NULL) {
			times->resize(times->size()+1,substr);
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
	opts.timecol = 1;
	bool names_read = false;

	ifstream deffile;			//Sensor definitions
	deffile.open("temperatur.sdef");
	if (!deffile.is_open()) {
		cout << "file not found!"<<endl;
		return 1;
	}
	vector<string> sensor_names;
	vector<string> sensor_data;
	string line;
	while (deffile.good()) {
		getline(deffile,line);
		bool instr = false;
		if (line.length()>0 && line.at(0)=='#') continue;
		for (size_t i=0;i<line.length();i++) {
			char c = line.at(i);
			if (c=='"') instr=!instr;
			if (c==' ' && !instr) {
				string name = line.substr(0,i);
				replaceAll(name,"\"","");
				string data = line.substr(i+1,line.length()-i);
				replaceAll(data,"\"","");
				cout << "Found sensor: " << name << endl;
				sensor_names.resize(sensor_names.size()+1,name);
				sensor_data.resize(sensor_data.size()+1,data);
				break;
			}
		}
	}
	deffile.close();

	ifstream file;					// data file
	file.open("temperatur.csv");
	if (!file.is_open()) {
		cout << "file not found!"<<endl;
		return 1;
	}
	vector<int> valid_cols;
	vector<vector<string> > values(0);
	vector<string> times;
	while (file.good()) {
		getline(file,line);
		if (line==("")) continue;
		if (!names_read) {
			vector<string> col_names;
			parseLine(line,col_names,NULL,NULL);
			for (size_t i=0;i<col_names.size();i++) {
				if (contains(sensor_names,col_names.at(i))) {
					valid_cols.resize(valid_cols.size()+1,i);
				} else {
					cout << "no match with sensor found for column "<<col_names.at(i) << "!" << endl;
				}
			}
			names_read = true;
		} else {
			if (opts.replace_comma_with_point) {
				replaceAll(line,",", ".");
			}
			values.resize(values.size()+1);
			parseLine(line,values.at(values.size()-1),&times,&valid_cols);
		}
	}
	file.close();

	for (size_t j=0;j<values.size();j++) {
		ofstream outfile;					// output
		outfile.open((string("data_files/")+string("temperatur_")+times.at(j)+string(".sd")).c_str());
		for (size_t i=0;i<sensor_names.size();i++) {
			outfile << "#" << sensor_names.at(i) <<"\n";
			outfile << "s "<< sensor_data.at(i) << " " << values.at(j).at(i) << "\n";
		}
		outfile.close();
	}
	cout << "files successfully created." << endl;
	return 0;
};


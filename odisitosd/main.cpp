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

struct Options {
	size_t startrow;
	char separator;
	bool replace_comma_with_point;
	size_t timecol;
	double error_threshold;
	int maxfwcount;
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
string doubletostr(double val) {
	ostringstream ss;
	ss << val;
	return ss.str();
}
int parseLine(string line,vector<double>* out,vector<double>* times,vector<int>* valid_cols,size_t row_count) {
	string workstr;
	if (row_count==opts.startrow) {
		int pos = find(line,'\t');
		cout << pos << endl;
		workstr = line.substr(pos+1,line.length()-pos-1);
	} else {
		workstr = line.substr(0,line.length());
	}
	out->clear();
	int sep_pos = find(workstr,opts.separator);
	size_t count = 0;
	replaceAll(workstr," ","\t");
	while (sep_pos>-1) {
		string substr;
		do {
			sep_pos = find(workstr,opts.separator);
			substr= workstr.substr(0,sep_pos);
			workstr.erase(0,sep_pos+1);
		} while (sep_pos==0);

		if (row_count>opts.startrow && count>opts.timecol) {
			if (valid_cols==NULL || contains(*valid_cols,count-opts.startrow)) {
				out->resize(out->size()+1,atof(substr.c_str()));
			}
		}
		if (count==opts.timecol && times!=NULL) {
			times->resize(times->size()+1,atof(substr.c_str()));
		}
		if (row_count==opts.startrow) {
			out->resize(out->size()+1,atof(substr.c_str()));
		}
		count++;
	}
	return 0;
}

int main(int argc, char *argv[]) {

	opts.startrow = 4;
	opts.separator = '\t';
	opts.replace_comma_with_point = true;
	opts.timecol = 0;
	opts.error_threshold = 5;
	opts.maxfwcount = 10;

	bool names_read = false;

	/*ifstream deffile;			//Sensor definitions
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
	*/
	string line;
	ifstream file;					// data file
	file.open("FS02012LUNA000231.txt");
	if (!file.is_open()) {
		cout << "file not found!"<<endl;
		return 1;
	}
	vector<int> valid_cols;
	vector<vector<double> > delta_values(0);
	vector<double> times;
	vector<double> lin_positions;
	size_t row = 0;
	while (file.good()) {
		getline(file,line);
		if (row>=opts.startrow) {
			if (line==("")) continue;
			if (!names_read) {
				cout << "read names..." << endl;
				parseLine(line,&lin_positions,NULL,NULL,row);
				names_read = true;
			} else {
				if (opts.replace_comma_with_point) {
					replaceAll(line,",", ".");
				}
				delta_values.resize(delta_values.size()+1);
				parseLine(line,&delta_values.at(delta_values.size()-1),&times,NULL,row);	//&valid_cols
			}
		}
		row++;
	}
	vector<vector<double> > values;
	values.resize(delta_values.size());
	for (size_t i=0;i<values.size();i++) {
		values.at(i).resize(delta_values.at(i).size());
		for (size_t j=0;j<values.at(i).size();j++) {
			values.at(i).at(j) = delta_values.at(i).at(j);
		}
	}
	ofstream errfile;					// output
	ofstream outfile;					// output
	outfile.open("temperatur_odisi.txt");
	errfile.open("temperatur_odisi_err.log");
	for (size_t i=1;i<values.size();i++) {
		vector<double>* curr_delta = &values.at(i);
		values.at(i).resize(curr_delta->size());
		for (size_t j=0;j<values.at(i).size();j++) {
			int fwcount = 0;
			double prev_val = values.at(i-1).at(j);
			while ((i+fwcount<values.size()) && (abs(values.at(i+fwcount).at(j)-prev_val)>(fwcount+1)*opts.error_threshold)) {
				fwcount++;
				if (fwcount==opts.maxfwcount) {
					errfile << "count of invalid values too big -> nothing changed!" <<" (Z. "<<i<<", S. "<<j<<")"<< endl;
					break;
				}
			}
			if (fwcount>0) {
				values.at(i).at(j) = prev_val+(values.at(i+fwcount).at(j)-prev_val)/(fwcount+1)*1;
				errfile << "changed " <<delta_values.at(i).at(j)<<" (Z. "<<i<<", S. "<<j<<") to "<<values.at(i).at(j)<< endl;
			}
			outfile << " "<<values.at(i).at(j);
			for (int l=0;l<10-int(doubletostr(values.at(i).at(j)).length());l++) {
				outfile<< " ";
			}
		}
		outfile << endl;

	}
	outfile.close();
	errfile.close();
	/*file.close();

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
	outfile.close();*/
	cout << "files successfully created." << endl;
	return 0;
};



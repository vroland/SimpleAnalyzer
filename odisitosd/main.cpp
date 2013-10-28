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
	float error_threshold;
	int maxfwcount;
	int tab_space_count;
	float height;
	float basetemp;
	float objwidth;
	bool flipobj;
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
int parseLine(string line,vector<float>* out,vector<float>* times,vector<int>* debug_positions,size_t row_count) {
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
		if (row_count==opts.startrow) {
			out->resize(out->size()+1,atof(substr.c_str()));
		}
		count++;
		position += sep_pos+1;
	}
	return 0;
}

int main(int argc, char *argv[]) {

	opts.startrow = 4;
	opts.separator = ' ';
	opts.replace_comma_with_point = true;
	opts.timecol = 0;
	opts.error_threshold = 5;
	opts.maxfwcount = 10;
	opts.tab_space_count = 7;
	opts.height = .065;
	opts.basetemp = 20;
	opts.objwidth = .5;
	opts.flipobj  = true;

	bool names_read = false;

	ifstream deffile;			//Sensor definitions
	deffile.open("odisi.sdef");
	if (!deffile.is_open()) {
		cout << "file not found!"<<endl;
		return 1;
	}
	string line;
	vector<float> inlist;
	vector<float> outlist;
	vector<float> in_x;
	vector<float> out_x;
	//vector<float> in_y;
	//vector<float> out_y;
	while (deffile.good()) {
		getline(deffile,line);
		if (line.length()==0) continue;
		if (line.at(0)=='#') continue;
		if (line.at(0)=='i') {
			inlist.resize(inlist.size()+1,atof(getTextBlock(line,1).c_str()));
			in_x.resize(in_x.size()+1,atof(getTextBlock(line,2).c_str()));
			//in_y.resize(in_y.size()+1,atof(getTextBlock(line,3).c_str()));
		}
		if (line.at(0)=='o') {
			outlist.resize(outlist.size()+1,atof(getTextBlock(line,1).c_str()));
			out_x.resize(out_x.size()+1,atof(getTextBlock(line,2).c_str()));
			//out_y.resize(in_y.size()+1,atof(getTextBlock(line,3).c_str()));
		}
	}
	if (outlist.size()!=inlist.size()) {
		cerr << "number of fiber in and outs not equal! (in: "<<inlist.size()<<" out: "<<outlist.size()<<")" << endl;
		return 1;
	}
	deffile.close();
	ifstream file;					// data file
	file.open("FS02012LUNA000231.txt");
	if (!file.is_open()) {
		cout << "file not found!"<<endl;
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
		}
		row++;
	}
	ofstream errfile;					// output
	ofstream outfile;					// output
	outfile.open("temperatur_odisi.tsd");
	errfile.open("temperatur_odisi_err.log");
	for (size_t i=1;i<values.size();i++) {
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
				// calc value
				int fwcount = 0;
				float prev_val = values.at(i-1).at(j);
				float original_val = values.at(i).at(j);
				while ((i+fwcount+1<values.size()) && (abs(values.at(i+fwcount).at(j)-prev_val)>(fwcount+1)*opts.error_threshold)) {
					fwcount++;
					if (fwcount==opts.maxfwcount) {
						errfile << "count of invalid values too big -> nothing changed!" <<" (l. "<<i+opts.startrow+2<<", c. "<<debug_positions.at(i).at(j)<<")"<< endl;
						break;
					}
				}
				if (fwcount>0) {
					values.at(i).at(j) = prev_val+(values.at(i+fwcount).at(j)-prev_val)/(fwcount+1);
					errfile << "changed " <<original_val<<" (l. "<<i+opts.startrow+2<<", c. "<<debug_positions.at(i).at(j)<<") to "<<values.at(i).at(j)<< endl;
				}
				float x = x_in+((x_out-x_in)/(l_out-l_in))*(l_pos-l_in);
				float y = opts.height;
				float z = cos(asin(((x_out-x_in)/(l_out-l_in))))*(l_pos-l_in);
				if (opts.flipobj) {
					x = opts.objwidth-x;
				}
				outfile <<"s "<<x<<" "<<y<<" "<<z<< " "<<opts.basetemp+values.at(i).at(j) << endl;
			}
		}
		outfile << endl;

	}
	outfile.close();
	errfile.close();
	cout << "files successfully created." << endl;
	return 0;
};



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
	int warning_threshold;
	int offset;
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
int parseFile(string filename,vector<int> &timestamps,vector<string> &names,vector<string> &data) {
	ifstream file;					// data file
	file.open(filename.c_str());
	if (!file.is_open()) {
		cout << "file not found!"<<endl;
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
int main() {
	opts.warning_threshold = 1;
	opts.offset			   = 0;
	vector<int> timestamps1;
	vector<string> names1;
	vector<string> data1;
	vector<int> timestamps2;
	vector<string> names2;
	vector<string> data2;
	if (parseFile("temperatur.tsd",timestamps1,names1,data1)) return 1;
	if (parseFile("temperatur_odisi.tsd",timestamps2,names2,data2)) return 1;
	int delta = timestamps1.at(0)-timestamps2.at(opts.offset);
	int start_delta = delta;
	int changes = 0;
	int bigger_changes = 0;
	int matched_datasets = 0;
	int last_dataset = 0;
	ofstream outfile;
	outfile.open("temperatur_gesamt.tsd");
	for (size_t i = 0;i<timestamps1.size();i++) {
		if (i+opts.offset<timestamps2.size() && (i+opts.offset>0)) {
			if (timestamps2.at(i+opts.offset)!=timestamps1.at(i)-delta) {
				int dd = (timestamps1.at(i)-timestamps2.at(i+opts.offset))-delta;
				if (abs(dd)>opts.warning_threshold) {
					cout << "WARNING: time stamp difference changed by: "<<dd<<" at data set "<<i<<endl;
					bigger_changes++;
				}
				delta = timestamps1.at(i)-timestamps2.at(i+opts.offset);
				changes++;
			}
			outfile << "t "<<timestamps1.at(i) << endl;
			outfile << "n "<<names1.at(i) << endl;
			outfile << data1.at(i) << endl,
			outfile << data2.at(i) << endl;
			matched_datasets++;
		}
		last_dataset++;
	}
	outfile.close();
	cout << "matched "<<matched_datasets<<" data sets"<<endl;
	cout << "time stamp difference changed "<<changes<<" times. ("<<bigger_changes<<"x more than "<<opts.warning_threshold<<")"<<endl;
	cout << "time difference at last data set ("<<last_dataset<<") to first is "<<delta-start_delta << endl;
	return 0;
}

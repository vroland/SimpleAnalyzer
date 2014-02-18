/*
 * SimpleAnalyzerApp.cpp
 *
 *  Created on: 04.09.2013
 *      Author: valentin
 */

/**
* @mainpage SimpleAnalyzer
*
* Dies ist die Dokumentation der Programmquellen.
* Für Informationen über die Verwendung der Programme
* konsultieren Sie bitte das Handbuch der Software.
*
* @author Valentin Roland
*/
#include <iostream>
#include "SimpleAnalyzerApp.h"
#include "GUI/GUIMainWindow.h"
#include "processing/ObjectData.h"
#include "processing/utils.h"
#include <vector>
using namespace std;

IMPLEMENT_APP(SimpleAnalyzerApp)

int SimpleAnalyzerApp::getCurrentDataObjectIndex() {
	return current_data_object_index;
}

void SimpleAnalyzerApp::setCurrentDataObjectIndex(int currentDataObjectIndex) {
	current_data_object_index = currentDataObjectIndex;
}

vector<ObjectData*>* SimpleAnalyzerApp::getDataObjects() {
	return &data_objects;
}

Utils::Visualization_info* SimpleAnalyzerApp::getVisualizationInfo() {
	return &visualization_info;
}

ObjectData* SimpleAnalyzerApp::getActiveObject() {
	return data_objects.at(current_data_object_index);
}

void SimpleAnalyzerApp::addObject(ObjectData* obj) {
	data_objects.resize(data_objects.size()+1,obj);
}

void SimpleAnalyzerApp::removeCurrentObject() {
	if (data_objects.size()>1) {
		delete data_objects.at(current_data_object_index);
		data_objects.erase(data_objects.begin()+current_data_object_index);
		current_data_object_index--;
		if (current_data_object_index<0) {
			current_data_object_index = 0;
		}
	}
}
bool SimpleAnalyzerApp::OnInit()
{
	wxMilliSleep(100);	//Workaround für eventuelle Timingprobleme
	GUIMainWindow *frame = new GUIMainWindow(wxT("Simple Analyzer"), 100, 100, 1200, 700);
	frame->Show(TRUE);
	SetTopWindow(frame);
	return true;
}

SimpleAnalyzerApp::~SimpleAnalyzerApp() {
	for (unsigned int i=0;i<data_objects.size();i++) {
		delete data_objects.at(i);
	}
	cout << "exit!" << endl;
}

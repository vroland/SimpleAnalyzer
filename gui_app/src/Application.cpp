/*
 * GUIApplication.cpp
 *
 *  Created on: 04.09.2013
 *      Author: valentin
 */

#include <iostream>
#include "Application.h"
#include "GUI/GUIMainWindow.h"
#include "processing/ObjectData.h"
#include <vector>
using namespace std;

vector<ObjectData*> data_objects;
int current_data_object_index = -1;

IMPLEMENT_APP(Application)



bool Application::OnInit()
{

	GUIMainWindow *frame = new GUIMainWindow(wxT("Simple Analyzer"), 100, 100, 1200, 700);
	frame->Show(TRUE);
	SetTopWindow(frame);

	return true;
}

Application::~Application() {
	for (unsigned int i=0;i<data_objects.size();i++) {
		delete data_objects.at(i);
	}
	cout << "exit!" << endl;
}

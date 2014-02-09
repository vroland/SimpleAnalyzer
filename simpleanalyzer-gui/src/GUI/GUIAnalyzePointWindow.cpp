/*
 * GUIAnalyzePoint.cpp
 *
 *  Created on: 21.09.2013
 *      Author: valentin
 */

#include "GUIAnalyzePointWindow.h"
#include <iostream>
#include <sstream>
#include "constants.h"
#include "../processing/Analyzer.h"
using namespace std;

BEGIN_EVENT_TABLE(GUIAnalyzePointWindow, wxDialog)
	EVT_BUTTON(ID_ANALYZE_POINT_BT, GUIAnalyzePointWindow::analyzePoint)
END_EVENT_TABLE()
extern std::vector<ObjectData*> data_objects;
extern int current_data_object_index;

GUIAnalyzePointWindow::GUIAnalyzePointWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height):
		wxDialog(parent, -1, title, wxPoint(xpos, ypos), wxSize(width, height), wxCAPTION | wxCLOSE_BOX) {
	label = new wxStaticText(this,wxID_ANY,wxT("Punktkoordinaten:"));
	xedit = new wxTextCtrl(this,wxID_ANY,wxT("0.0"));
	yedit = new wxTextCtrl(this,wxID_ANY,wxT("0.0"));
	zedit = new wxTextCtrl(this,wxID_ANY,wxT("0.0"));
	calcbt= new wxButton(this,ID_ANALYZE_POINT_BT,wxT("analysieren"));
	interpolationModeList = new wxComboBox(this,wxID_ANY,wxT(""),wxDefaultPosition,wxDefaultSize,NUMBER_OF_INTERPOLATION_MODES,INTERPOLATION_MODE_STRINGS,wxCB_READONLY | wxCB_DROPDOWN);
	interpolationModeLabel = new wxStaticText(this,wxID_ANY,wxT("Interpolationsmodus:"));
	label->SetSize(10,10,300,20);
	xedit->SetSize(20,30,100,20);
	yedit->SetSize(120,30,100,20);
	zedit->SetSize(220,30,100,20);
	interpolationModeLabel->SetSize(20,60,200,20);
	interpolationModeList->SetSize(180,60,140,20);
	interpolationModeList->SetSelection(0);
	calcbt->SetSize(20,80,100,30);
	xedit->SetValue(wxT("0"));
	yedit->SetValue(wxT(".3"));
	zedit->SetValue(wxT("1"));
	Centre();
}

void GUIAnalyzePointWindow::analyzePoint(wxCommandEvent &event) {
	Analyzer analyzer;
	Analyzer::AnalyzerData_point data;
	Interpolator interpolator;
	interpolator.setMode((Interpolator::InterpolationMode) interpolationModeList->GetSelection());
	setlocale(LC_NUMERIC, "C");
	Vector3D point = Vector3D(atof(xedit->GetValue().ToAscii()),atof(yedit->GetValue().ToAscii()),atof(zedit->GetValue().ToAscii()));
	analyzer.analyzePoint(data_objects.at(current_data_object_index),&point,&data,&interpolator);
	wxString message = wxT("Analysedaten des Punktes (");
	message.Append(floattowxstr(point.getX()));
	message.Append(wxT("|"));
	message.Append(floattowxstr(point.getY()));
	message.Append(wxT("|"));
	message.Append(floattowxstr(point.getZ()));
	message.Append(wxT("):\nTemperatur: "));
	message.Append(floattowxstr(data.value));
	message.Append(wxT("\nExtrapoliert: "));
	data.extrapolated?message.Append(wxT("ja")):message.Append(wxT("nein"));
	wxMessageBox( message, wxT("Punktanalyse"), wxICON_INFORMATION);
}

GUIAnalyzePointWindow::~GUIAnalyzePointWindow() {
}


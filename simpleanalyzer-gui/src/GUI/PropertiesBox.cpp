/*
 * PropertiesBox.cpp
 *
 *  Created on: 07.09.2013
 *      Author: valentin
 */

#include "PropertiesBox.h"
#include "constants.h"
#include <iostream>
#include "../processing/utils.h"
#include "../processing/ObjectData.h"

using namespace Utils;

wxString sdfilestring[] = {wxT("")};
extern std::vector<ObjectData*> data_objects;
extern int current_data_object_index;

PropertiesBox::PropertiesBox(wxWindow *parent):
	wxStaticBox(parent, wxID_ANY, wxT("Objekteigenschaften")) {
	recalcbutton  = new wxButton(parent,ID_RECALCBT,wxT("neu berechnen"));
	uptodatetext  = new wxStaticText(parent,wxID_ANY,wxT("Unangewendete\nÄnderungen!"));
	uptodatetext->SetForegroundColour(wxColour(200,0,0));
	uptodatetext->Hide();
	objnameedit	  = new wxTextCtrl(parent,ID_IMMEDIATE_UPDATE_PROP,wxT(""));
	objnametext   = new wxStaticText(parent,wxID_ANY,wxT("Objektname:"));
	maxvolumeedit = new wxTextCtrl(parent,ID_GENERAL_PROP,wxT(""));
	maxvolumetext = new wxStaticText(parent,wxID_ANY,wxT("Maximales Tetraedervolumen:"));
	qualityedit   = new wxTextCtrl(parent,ID_GENERAL_PROP,wxT(""));
	qualitytext	  = new wxStaticText(parent,wxID_ANY,wxT("Min. Radius-Kantenlänge-Verhältnis:"));
	sensordatalist = new wxComboBox(parent,ID_SD_BOX,wxT(""),wxDefaultPosition,wxDefaultSize,1,sdfilestring,wxCB_READONLY | wxCB_DROPDOWN);
	sensordatatext= new wxStaticText(parent,wxID_ANY,wxT("Sensordaten:"));
	matlistbox 	  = new wxListBox(parent,ID_MATERIALBOX);
	matlistboxtext= new wxStaticText(parent,wxID_ANY,wxT("Materialien:"));
	matpropbox    = new wxStaticBox(parent,wxID_ANY,wxT("Materialeigenschaften:"));
	matnameedit	  = new wxTextCtrl(parent,ID_IMMEDIATE_UPDATE_PROP,wxT(""));
	matnametext	  = new wxStaticText(parent,wxID_ANY,wxT("Materialname:"));
	interpolationmodelist = new wxComboBox(parent,ID_GENERAL_PROP,wxT(""),wxDefaultPosition,wxDefaultSize,NUMBEROFINTERPOLATIONMODES,imenumstrings,wxCB_READONLY | wxCB_DROPDOWN);
	interpolationmodetext = new wxStaticText(parent,wxID_ANY,wxT("Interpolationsmodus:"));
	densityedit	  = new wxTextCtrl(parent,ID_GENERAL_PROP,wxT(""));
	densitytext	  = new wxStaticText(parent,wxID_ANY,wxT("Dichte in kg/m³:"));
	cspecedit	  = new wxTextCtrl(parent,ID_GENERAL_PROP,wxT(""));
	cspectext	  = new wxStaticText(parent,wxID_ANY,wxT("Spez. Wärmekapazität in kJ/(kg*K):"));
	sdtimeline 	  = new GUITimeline(parent,ID_SD_TIMELINE,wxT("Zeitleiste"));
	analyzemarkercb = new wxCheckBox(parent,ID_ANALYZE_MARKER_CB,wxT("Analysieren"));
	findmaxbt	  = new wxButton(parent,ID_FIND_MAX_BT,wxT("Max"));
	clranalyzemarkerbt = new wxButton(parent,ID_CLEAR_MARKER_BT,wxT("Clear"));
	nextmarkerbt  = new wxButton(parent,ID_MARKER_NEXT_BT,wxT("►"));
	prevmarkerbt  = new wxButton(parent,ID_MARKER_PREV_BT,wxT("◄"));
	autoupdatecb  = new wxCheckBox(parent,ID_AUTO_UPDATE_CB,wxT("Automatisch neu berechnen"));
	autoupdatecb->SetValue(false);
	current_material = 0;
}
void PropertiesBox::resize() {
	int x,y,w,h;
	GetPosition(&x,&y);
	GetSize(&w,&h);
	recalcbutton->SetSize(x+10,y+25,150,30,0);
	autoupdatecb->SetSize(x+10,y+58,250,20,0);
	uptodatetext->SetSize(x+160,y+25,200,40,0);
	objnametext->SetSize(x+10,y+80,300,20,0);
	objnameedit->SetSize(x+10,y+100,150,20,0);
	maxvolumetext->SetSize(x+10,y+120,300,20,0);
	maxvolumeedit->SetSize(x+10,y+140,150,20,0);
	qualitytext->SetSize(x+10,y+160,300,20,0);
	qualityedit->SetSize(x+10,y+180,150,20,0);
	sensordatatext->SetSize(x+10,y+200,300,20,0);
	sensordatalist->SetSize(x+10,y+220,150,20,0);
	sdtimeline->Hide();
	analyzemarkercb->Hide();
	clranalyzemarkerbt->Hide();
	nextmarkerbt->Hide();
	prevmarkerbt->Hide();
	findmaxbt->Hide();
	int sdheight = 0;
	if (current_data_object_index>-1) {
		ObjectData* object = data_objects.at(current_data_object_index);
		SensorData* sd = &object->sensordatalist.at(sensordatalist->GetSelection());
		if (sd->timed) {
			sdtimeline->setMarkerList(&sd->markers);
			sdtimeline->SetSize(x+20,y+250,260,80,0);
			sdtimeline->setValue(sd->current_time_index);
			sdtimeline->setMinValue(0);
			sdtimeline->setMaxValue(sd->timestamps.size()-1);
			sdtimeline->setNameList(&sd->subnames);
			sdtimeline->Show();
			sdheight = 120;

			analyzemarkercb->Show();
			clranalyzemarkerbt->Show();
			nextmarkerbt->Show();
			prevmarkerbt->Show();
			findmaxbt->Show();
			prevmarkerbt->SetSize(x+20,y+330,25,25,0);
			nextmarkerbt->SetSize(x+45,y+330,25,25,0);
			findmaxbt->SetSize(x+70,y+330,40,25,0);
			analyzemarkercb->SetSize(x+110,y+330,110,25,0);
			clranalyzemarkerbt->SetSize(x+220,y+330,60,25,0);
		}
	}
	matlistboxtext->SetSize(x+10,y+240+sdheight,300,110,0);
	matlistbox->SetSize(x+10,y+260+sdheight,270,60,0);
	matpropbox->SetSize(x+10,y+330+sdheight,270,195,0);
	matnametext->SetSize(x+20,y+350+sdheight,300,20,0);
	matnameedit->SetSize(x+20,y+370+sdheight,150,20,0);
	interpolationmodetext->SetSize(x+20,y+390+sdheight,300,20,0);
	interpolationmodelist->SetSize(x+20,y+410+sdheight,150,20,0);
	densitytext->SetSize(x+20,y+430+sdheight,240,20,0);
	densityedit->SetSize(x+20,y+450+sdheight,150,20,0);
	cspectext->SetSize(x+20,y+470+sdheight,300,20,0);
	cspecedit->SetSize(x+20,y+490+sdheight,150,20,0);
	SetSize(x,y,w,530+sdheight);
}
PropertiesBox::~PropertiesBox() {
}

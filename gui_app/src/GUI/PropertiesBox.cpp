/*
 * PropertiesBox.cpp
 *
 *  Created on: 07.09.2013
 *      Author: valentin
 */

#include "PropertiesBox.h"
#include "constants.h"
#include <iostream>


wxString sdfilestring[] = {wxT("")};

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
	sensordatalist = new wxComboBox(parent,ID_GENERAL_PROP,wxT(""),wxDefaultPosition,wxDefaultSize,1,sdfilestring,wxCB_READONLY | wxCB_DROPDOWN);
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
	sdtimeline 	  = new GUITimeline(parent,wxID_ANY,wxT("Zeitleiste"));
	current_material = 0;
}
void PropertiesBox::resize() {
	int x,y,w,h;
	GetPosition(&x,&y);
	GetSize(&w,&h);
	recalcbutton->SetSize(x+10,y+25,150,30,0);
	uptodatetext->SetSize(x+160,y+25,200,40,0);
	objnametext->SetSize(x+10,y+60,300,20,0);
	objnameedit->SetSize(x+10,y+80,150,20,0);
	maxvolumetext->SetSize(x+10,y+100,300,20,0);
	maxvolumeedit->SetSize(x+10,y+120,150,20,0);
	qualitytext->SetSize(x+10,y+140,300,20,0);
	qualityedit->SetSize(x+10,y+160,150,20,0);
	sensordatatext->SetSize(x+10,y+180,300,20,0);
	sensordatalist->SetSize(x+10,y+200,150,20,0);
	matlistboxtext->SetSize(x+10,y+220,300,110,0);
	matlistbox->SetSize(x+10,y+240,270,60,0);
	matpropbox->SetSize(x+10,y+310,270,250,0);
	matnametext->SetSize(x+20,y+330,300,20,0);
	matnameedit->SetSize(x+20,y+350,150,20,0);
	interpolationmodetext->SetSize(x+20,y+370,300,20,0);
	interpolationmodelist->SetSize(x+20,y+390,150,20,0);
	densitytext->SetSize(x+20,y+410,240,20,0);
	densityedit->SetSize(x+20,y+430,150,20,0);
	cspectext->SetSize(x+20,y+450,300,20,0);
	cspecedit->SetSize(x+20,y+470,150,20,0);
	sdtimeline->SetSize(x+20,y+500,250,100,0);
}
PropertiesBox::~PropertiesBox() {
}


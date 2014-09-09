/*
 * PropertiesBox.cpp
 *
 *  Created on: 07.09.2013
 *      Author: Valentin Roland
 */

#include "PropertiesBox.h"
#include "constants.h"
#include <iostream>
#include "../SimpleAnalyzerApp.h"
#include "../processing/utils.h"
#include "../processing/ObjectData.h"

using namespace Utils;

wxString sdfilestring[] = { wxT("") };

PropertiesBox::PropertiesBox(wxWindow *parent) :
		wxStaticBox(parent, wxID_ANY, wxT("Objekteigenschaften")) {

	//Erstellen aller Komponenten für die Objekteigenschaften-Oberfläche
	recalcButton = new wxButton(parent, ID_RECALCBT, wxT("neu berechnen"));
	upToDateLbl = new wxStaticText(parent, wxID_ANY,
			wxT("Unangewendete\nÄnderungen!"));
	upToDateLbl->SetForegroundColour(wxColour(200, 0, 0));
	upToDateLbl->Hide();
	objNameEdit = new wxTextCtrl(parent, ID_IMMEDIATE_UPDATE_PROP, wxT(""));
	objNameLbl = new wxStaticText(parent, wxID_ANY, wxT("Objektname:"));
	maxVolumeEdit = new wxTextCtrl(parent, ID_GENERAL_PROP, wxT(""));
	maxVolumeLbl = new wxStaticText(parent, wxID_ANY,
			wxT("Maximales Tetraedervolumen:"));
	qualityEdit = new wxTextCtrl(parent, ID_GENERAL_PROP, wxT(""));
	qualityLbl = new wxStaticText(parent, wxID_ANY,
			wxT("Min. Radius-Kantenlänge-Verhältnis:"));
	sensorDataList = new wxComboBox(parent, ID_SD_BOX, wxT(""),
			wxDefaultPosition, wxDefaultSize, 1, sdfilestring,
			wxCB_READONLY | wxCB_DROPDOWN);
	sensorDataLbl = new wxStaticText(parent, wxID_ANY, wxT("Sensordaten:"));
	matListBox = new wxListBox(parent, ID_MATERIALBOX);
	matListBoxLbl = new wxStaticText(parent, wxID_ANY, wxT("Materialien:"));
	matPropBox = new wxStaticBox(parent, wxID_ANY,
			wxT("Materialeigenschaften:"));
	matNameEdit = new wxTextCtrl(parent, ID_IMMEDIATE_UPDATE_PROP, wxT(""));
	matNameLbl = new wxStaticText(parent, wxID_ANY, wxT("Materialname:"));
	interpolationModeList = new wxComboBox(parent, ID_GENERAL_PROP, wxT(""),
			wxDefaultPosition, wxDefaultSize, NUMBER_OF_INTERPOLATION_MODES,
			INTERPOLATION_MODE_STRINGS, wxCB_READONLY | wxCB_DROPDOWN);
	interpolationModeLbl = new wxStaticText(parent, wxID_ANY,
			wxT("Interpolationsmodus:"));
	densityEdit = new wxTextCtrl(parent, ID_GENERAL_PROP, wxT(""));
	densityLbl = new wxStaticText(parent, wxID_ANY, wxT("Dichte in kg/m³:"));
	specificHeatCapEdit = new wxTextCtrl(parent, ID_GENERAL_PROP, wxT(""));
	specificHeatCapLbl = new wxStaticText(parent, wxID_ANY,
			wxT("Spez. Wärmekapazität in kJ/(kg*K):"));
	sdTimeline = new GUITimeline(parent, ID_SD_TIMELINE);
	analyzeMarkerCheckBox = new wxCheckBox(parent, ID_ANALYZE_MARKER_CB,
			wxT("Analysieren"));
	findMaxBt = new wxButton(parent, ID_FIND_MAX_BT, wxT("Max"));
	clearAnalyzeMarkerBt = new wxButton(parent, ID_CLEAR_MARKER_BT,
			wxT("Clear"));
	nextMarkerBt = new wxButton(parent, ID_MARKER_NEXT_BT, wxT("►"));
	prevMarkerBt = new wxButton(parent, ID_MARKER_PREV_BT, wxT("◄"));
	autoUpdateCeckBox = new wxCheckBox(parent, ID_AUTO_UPDATE_CB,
			wxT("Automatisch neu berechnen"));
	autoUpdateCeckBox->SetValue(false);

	//Statusvariablen initialisieren
	current_material = 0;
}

void PropertiesBox::resize() {

	//Positionieren aller Komponenten
	int x, y, w, h;
	GetPosition(&x, &y);
	GetSize(&w, &h);
	recalcButton->SetSize(x + 10, y + 25, 150, 30, 0);
	autoUpdateCeckBox->SetSize(x + 10, y + 58, 250, 20, 0);
	upToDateLbl->SetSize(x + 160, y + 25, 200, 40, 0);
	objNameLbl->SetSize(x + 10, y + 80, 300, 20, 0);
	objNameEdit->SetSize(x + 10, y + 100, 150, 20, 0);
	maxVolumeLbl->SetSize(x + 10, y + 120, 300, 20, 0);
	maxVolumeEdit->SetSize(x + 10, y + 140, 150, 20, 0);
	qualityLbl->SetSize(x + 10, y + 160, 300, 20, 0);
	qualityEdit->SetSize(x + 10, y + 180, 150, 20, 0);
	sensorDataLbl->SetSize(x + 10, y + 200, 300, 20, 0);
	sensorDataList->SetSize(x + 10, y + 220, 150, 20, 0);
	sdTimeline->Hide();
	analyzeMarkerCheckBox->Hide();
	clearAnalyzeMarkerBt->Hide();
	nextMarkerBt->Hide();
	prevMarkerBt->Hide();
	findMaxBt->Hide();
	int sdheight = 0;

	//Ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		ObjectData* object = wxGetApp().getActiveObject();
		SensorData* sd = &object->getSensorDataList()->at(
				sensorDataList->GetSelection());

		//Sind die verwendeten Sensordaten zeitbezogen?
		if (sd->timed) {
			sdTimeline->setMarkerList(&sd->markers);
			sdTimeline->SetSize(x + 20, y + 250, 260, 80, 0);
			sdTimeline->setValue(sd->current_time_index);
			sdTimeline->setMinValue(0);
			sdTimeline->setMaxValue(sd->timestamps.size() - 1);
			sdTimeline->setNameList(&sd->subnames);
			sdTimeline->Show();
			sdheight = 120;

			analyzeMarkerCheckBox->Show();
			clearAnalyzeMarkerBt->Show();
			nextMarkerBt->Show();
			prevMarkerBt->Show();
			findMaxBt->Show();
			prevMarkerBt->SetSize(x + 20, y + 330, 25, 25, 0);
			nextMarkerBt->SetSize(x + 45, y + 330, 25, 25, 0);
			findMaxBt->SetSize(x + 70, y + 330, 40, 25, 0);
			analyzeMarkerCheckBox->SetSize(x + 110, y + 330, 110, 25, 0);
			clearAnalyzeMarkerBt->SetSize(x + 220, y + 330, 60, 25, 0);
		}
	}

	matListBoxLbl->SetSize(x + 10, y + 240 + sdheight, 300, 20, 0);
	matListBox->SetSize(x + 10, y + 260 + sdheight, 270, 60, 0);
	matPropBox->SetSize(x + 10, y + 330 + sdheight, 270, 195, 0);
	matNameLbl->SetSize(x + 20, y + 350 + sdheight, 300, 20, 0);
	matNameEdit->SetSize(x + 20, y + 370 + sdheight, 150, 20, 0);
	interpolationModeLbl->SetSize(x + 20, y + 390 + sdheight, 300, 20, 0);
	interpolationModeList->SetSize(x + 20, y + 410 + sdheight, 150, 20, 0);
	densityLbl->SetSize(x + 20, y + 430 + sdheight, 240, 20, 0);
	densityEdit->SetSize(x + 20, y + 450 + sdheight, 150, 20, 0);
	specificHeatCapLbl->SetSize(x + 20, y + 470 + sdheight, 300, 20, 0);
	specificHeatCapEdit->SetSize(x + 20, y + 490 + sdheight, 150, 20, 0);
	SetSize(x, y, w, 530 + sdheight);
}

wxCheckBox* PropertiesBox::getAnalyzeMarkerCheckBox() {
	return analyzeMarkerCheckBox;
}

wxCheckBox* PropertiesBox::getAutoUpdateCeckBox() {
	return autoUpdateCeckBox;
}

wxButton* PropertiesBox::getClearAnalyzeMarkerBt() {
	return clearAnalyzeMarkerBt;
}

wxTextCtrl* PropertiesBox::getSpecificHeatCapEdit() {
	return specificHeatCapEdit;
}

int PropertiesBox::getCurrentMaterial() {
	return current_material;
}

void PropertiesBox::setCurrentMaterial(int index) {
	current_material = index;
}

wxTextCtrl* PropertiesBox::getDensityEdit() {
	return densityEdit;
}

wxButton* PropertiesBox::getFindMaxBt() {
	return findMaxBt;
}

wxComboBox* PropertiesBox::getInterpolationModeList() {
	return interpolationModeList;
}

wxListBox* PropertiesBox::getMatListBox() {
	return matListBox;
}

wxTextCtrl* PropertiesBox::getMatNameEdit() {
	return matNameEdit;
}

wxStaticBox* PropertiesBox::getMatPropBox() {
	return matPropBox;
}

wxTextCtrl* PropertiesBox::getMaxVolumeEdit() {
	return maxVolumeEdit;
}

wxButton* PropertiesBox::getNextMarkerBt() {
	return nextMarkerBt;
}

wxTextCtrl* PropertiesBox::getObjNameEdit() {
	return objNameEdit;
}

wxButton* PropertiesBox::getPrevMarkerBt() {
	return prevMarkerBt;
}

wxTextCtrl* PropertiesBox::getQualityEdit() {
	return qualityEdit;
}

wxButton* PropertiesBox::getRecalcButton() {
	return recalcButton;
}

GUITimeline* PropertiesBox::getSdTimeline() {
	return sdTimeline;
}

wxComboBox* PropertiesBox::getSensorDataList() {
	return sensorDataList;
}

wxStaticText* PropertiesBox::getUpToDateLbl() {
	return upToDateLbl;
}

PropertiesBox::~PropertiesBox() {
}

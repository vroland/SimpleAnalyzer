/*
 * ViewpropBox.cpp
 *
 *  Created on: 17.09.2013
 *      Author: valentin
 */

#include "ViewpropBox.h"
#include "constants.h"
#include "Renderer.h"
#include <iostream>
using namespace std;

//Auswahlmöglichkeiten für die Darstellung eines Elementtyps
wxString renderchoices[] = { wxT("Kein"), wxT("Material"), wxT("Wert") };

ViewpropBox::ViewpropBox(wxWindow *parent) :
		wxStaticBox(parent, wxID_ANY, wxT("Anzeigeeigenschaften")) {

	//Erstellen und initialisieren aller Komponenten für die Visualisierungsoptionen-Oberfläche
	pointsCheckBox = new wxRadioBox(parent, ID_GENERAL_VIEW_PROP,
			wxT("Punkte:"), wxDefaultPosition, wxDefaultSize, 3,
			renderchoices);
	edgesCheckBox = new wxRadioBox(parent, ID_GENERAL_VIEW_PROP, wxT("Kanten:"),
			wxDefaultPosition, wxDefaultSize, 3, renderchoices);
	facesCheckBox = new wxRadioBox(parent, ID_GENERAL_VIEW_PROP,
			wxT("Flächen:"), wxDefaultPosition, wxDefaultSize, 3,
			renderchoices);
	matVisibilityListBox = new wxCheckListBox(parent, ID_GENERAL_VIEW_PROP);
	matVisualizationLbl = new wxStaticText(parent, wxID_ANY,
			wxT("sichtbare Materialien:"));
	showExtrapolatedCheckBox = new wxCheckBox(parent, ID_GENERAL_VIEW_PROP,
			wxT("zeige extrapolierte Punkte"));
	showShowSensorData = new wxCheckBox(parent, ID_GENERAL_VIEW_PROP,
			wxT("zeige Sensordaten"));
	colorRangeLbl = new wxStaticText(parent, wxID_ANY,
			wxT("Farbbereich (Blau-Rot) in °C:"));
	colorRangeMinEdit = new wxSpinCtrl(parent, ID_GENERAL_VIEW_PROP);
	colorRangeMaxEdit = new wxSpinCtrl(parent, ID_GENERAL_VIEW_PROP);
	viewScaleLbl = new wxStaticText(parent, ID_GENERAL_VIEW_PROP,
			wxT("Objektskallierung:"));
	viewScaleEdit = new wxTextCtrl(parent, ID_GENERAL_VIEW_PROP, wxT(""));

	colorRangeMinEdit->SetRange(-274, 10000000);
	colorRangeMaxEdit->SetRange(-274, 10000000);
}

void ViewpropBox::resize() {

	//Positionieren aller Komponenten
	int x, y, w, h;
	GetPosition(&x, &y);
	GetSize(&w, &h);
	pointsCheckBox->SetSize(x + 10, y + 20, 90, 100, 0);
	edgesCheckBox->SetSize(x + 100, y + 20, 90, 100, 0);
	facesCheckBox->SetSize(x + 190, y + 20, 90, 100, 0);
	showExtrapolatedCheckBox->SetSize(x + 10, y + 130, 250, 20, 0);
	showShowSensorData->SetSize(x + 10, y + 160, 250, 20, 0);
	matVisualizationLbl->SetSize(x + 10, y + 190, 200, 20, 0);
	matVisibilityListBox->SetSize(x + 10, y + 210, 200, 80, 0);
	colorRangeLbl->SetSize(x + 10, y + 300, 200, 20, 0);
	colorRangeMinEdit->SetSize(x + 20, y + 320, 100, 20, 0);
	colorRangeMaxEdit->SetSize(x + 120, y + 320, 100, 20, 0);
	viewScaleLbl->SetSize(x + 10, y + 340, 200, 20, 0);
	viewScaleEdit->SetSize(x + 20, y + 360, 100, 20, 0);
	SetSize(x, y, w, 390);
}

wxSpinCtrl* ViewpropBox::getColorRangeMaxEdit() {
	return colorRangeMaxEdit;
}

wxSpinCtrl* ViewpropBox::getColorRangeMinEdit() {
	return colorRangeMinEdit;
}

wxRadioBox* ViewpropBox::getEdgesCheckBox() {
	return edgesCheckBox;
}

wxRadioBox* ViewpropBox::getFacesCheckBox() {
	return facesCheckBox;
}

wxCheckListBox* ViewpropBox::getMatVisibilityListBox() {
	return matVisibilityListBox;
}

wxRadioBox* ViewpropBox::getPointsCheckBox() {
	return pointsCheckBox;
}

wxCheckBox* ViewpropBox::getShowExtrapolatedCheckBox() {
	return showExtrapolatedCheckBox;
}

wxCheckBox* ViewpropBox::getShowShowSensorData() {
	return showShowSensorData;
}

wxTextCtrl* ViewpropBox::getViewScaleEdit() {
	return viewScaleEdit;
}

ViewpropBox::~ViewpropBox() {

}

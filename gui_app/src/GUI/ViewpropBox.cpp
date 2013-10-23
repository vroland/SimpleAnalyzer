
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
wxString renderchoices[] = {wxT("Kein"),wxT("Material"),wxT("Wert")};
ViewpropBox::ViewpropBox(wxWindow *parent):
wxStaticBox(parent, wxID_ANY, wxT("Anzeigeeigenschaften")) {
	pointscb      = new wxRadioBox(parent,ID_GENERAL_VIEW_PROP,wxT("Punkte:"),wxDefaultPosition,wxDefaultSize,3,renderchoices);
	edgescb       = new wxRadioBox(parent,ID_GENERAL_VIEW_PROP,wxT("Kanten:"),wxDefaultPosition,wxDefaultSize,3,renderchoices);
	facescb       = new wxRadioBox(parent,ID_GENERAL_VIEW_PROP,wxT("Flächen:"),wxDefaultPosition,wxDefaultSize,3,renderchoices);
	matvisibility = new wxCheckListBox(parent,ID_GENERAL_VIEW_PROP);
	matvistext    = new wxStaticText(parent,wxID_ANY,wxT("sichtbare Materialien:"));
	show_extcb    = new wxCheckBox(parent,ID_GENERAL_VIEW_PROP,wxT("zeige Extrapolierte Punkte"));
	show_sdata    = new wxCheckBox(parent,ID_GENERAL_VIEW_PROP,wxT("zeige Sensordaten"));
}
void ViewpropBox::resize() {
	int x,y,w,h;
	GetPosition(&x,&y);
	GetSize(&w,&h);
	pointscb->SetSize(x+10,y+20,90,100,0);
	edgescb->SetSize(x+100,y+20,90,100,0);
	facescb->SetSize(x+190,y+20,90,100,0);
	show_extcb->SetSize(x+10,y+130,250,20,0);
	show_sdata->SetSize(x+10,y+160,250,20,0);
	matvistext->SetSize(x+10,y+190,200,20,0);
	matvisibility->SetSize(x+10,y+210,200,80,0);
}
ViewpropBox::~ViewpropBox() {

}


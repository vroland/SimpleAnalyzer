/*
 * PropertiesBox.h
 *
 *  Created on: 07.09.2013
 *      Author: valentin
 */

#ifndef PROPERTIESBOX_H_
#define PROPERTIESBOX_H_

#include <wx/wx.h>
#include "GUITimeline.h"
class PropertiesBox: public wxStaticBox {
public:
	PropertiesBox(wxWindow *parent);
	void resize();
	virtual ~PropertiesBox();
	wxButton* recalcbutton;
	wxStaticText* objnametext;
	wxTextCtrl* objnameedit;
	wxStaticText* matnametext;
	wxTextCtrl* matnameedit;
	wxStaticText* uptodatetext;
	wxStaticText* maxvolumetext;
	wxTextCtrl* maxvolumeedit;
	wxStaticText* qualitytext;
	wxTextCtrl* qualityedit;
	wxStaticText* sensordatatext;
	wxComboBox* sensordatalist;
	wxListBox* matlistbox;
	wxStaticText* matlistboxtext;
	wxStaticBox* matpropbox;
	wxComboBox* interpolationmodelist;
	wxStaticText* interpolationmodetext;
	wxTextCtrl* densityedit;
	wxStaticText* densitytext;
	wxTextCtrl* cspecedit;
	wxStaticText* cspectext;
	GUITimeline* sdtimeline;
	wxCheckBox* analyzemarkercb;
	wxButton* findmaxbt;
	wxButton* clranalyzemarkerbt;
	wxButton* nextmarkerbt;
	wxButton* prevmarkerbt;
	wxCheckBox* autoupdatecb;
	int current_material;
};

#endif /* PROPERTIESBOX_H_ */

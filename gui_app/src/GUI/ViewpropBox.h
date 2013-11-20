/*
 * ViewpropBox.h
 *
 *  Created on: 17.09.2013
 *      Author: valentin
 */

#ifndef VIEWPROPBOX_H_
#define VIEWPROPBOX_H_
#include <wx-2.8/wx/wx.h>
#include <wx/spinctrl.h>

class ViewpropBox: public wxStaticBox {
public:
	ViewpropBox(wxWindow *parent);
	void resize();
	virtual ~ViewpropBox();
	wxRadioBox* pointscb;
	wxRadioBox* edgescb;
	wxRadioBox* facescb;
	wxStaticText* matvistext;
	wxCheckListBox* matvisibility;
	wxCheckBox* show_extcb;
	wxCheckBox* show_sdata;
	wxStaticText* visdelta_lbl;
	wxSpinCtrl* min_visval;
	wxSpinCtrl* max_visval;
	wxStaticText* viewscale_lbl;
	wxTextCtrl* viewscale;
};

#endif /* VIEWPROPBOX_H_ */

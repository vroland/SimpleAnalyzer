/*
 * GUIAnalyzeOutputWindow.h
 *
 *  Created on: 12.09.2013
 *      Author: valentin
 */

#ifndef GUIANALYZEOUTPUTWINDOW_H_
#define GUIANALYZEOUTPUTWINDOW_H_

#include "PropertiesBox.h"
#include <wx/grid.h>
#include <wx/wx.h>

class GUIAnalyzeOutputWindow: public wxFrame {
public:
	GUIAnalyzeOutputWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height);
	void Update();
	virtual ~GUIAnalyzeOutputWindow();
private:
	wxGrid* table;
};

#endif /* GUIANALYZEOUTPUTWINDOW_H_ */

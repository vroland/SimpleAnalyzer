/*
 * GUIMainWindow.cpp
 *
 *  Created on: 04.09.2013
 *      Author: valentin
 */

#include "GUIMainWindow.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "../libraries/tetgen/tetgen.h"
#include "../processing/MeshProcessor.h"
#include "../fileIO/Importer.h"
#include "../processing/ObjectData.h"
#include "constants.h"
#include "../fileIO/Exporter.h"
#include <wx/stdpaths.h>
using namespace std;

extern std::vector<ObjectData*> data_objects;
extern int current_data_object_index;
extern Visualization_info visualization_info;

#define PROPBOXWIDTH 300
#define VIEWBOXWIDTH 300

extern const wxEventType wxEVT_TIMELINE_CHANGE;

BEGIN_EVENT_TABLE(GUIMainWindow, wxFrame)
	EVT_MENU(ID_IMPORT_OBJ, GUIMainWindow::OnMenuImportObj)
	EVT_MENU(ID_IMPORT_SD, GUIMainWindow::OnMenuImportSD)
	EVT_MENU(wxID_EXIT, GUIMainWindow::OnMenuFileQuit)
	EVT_MENU(ID_ABOUT, GUIMainWindow::OnMenuHelpAbout)
	EVT_MENU(ID_ANALYZE, GUIMainWindow::OnAnalyze)
	EVT_LISTBOX(ID_MATERIALBOX, GUIMainWindow::OnMaterialSelect)
	EVT_BUTTON(ID_RECALCBT,GUIMainWindow::OnRecalcBtClick)
	EVT_SIZE(GUIMainWindow::OnResize)
	EVT_TEXT(ID_IMMEDIATE_UPDATE_PROP,GUIMainWindow::OnImmediateUpdatePropChange)
	EVT_TEXT(ID_GENERAL_PROP,GUIMainWindow::OnGeneralPropChange)
	EVT_RADIOBOX(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropChange)
	EVT_TEXT(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropChange)
	EVT_CHECKLISTBOX(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropChange)
	EVT_CHECKBOX(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropChange)
	EVT_SPINCTRL(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropSpinChange)
	EVT_MENU(ID_CHANGE_ACTIVE_OBJ,GUIMainWindow::OnActiveObjectChange)
	EVT_MENU(ID_ANALYZE_POINT,GUIMainWindow::OnAnalyzePoint)
	EVT_MENU(ID_RENDER_CUT,GUIMainWindow::OnRenderCut)
	EVT_MENU(ID_DELETE_ACTIVE_OBJ,GUIMainWindow::OnActiveObjectDelete)
	EVT_MENU(ID_IMPORT_TSD,GUIMainWindow::OnMenuImportTSD)
	EVT_COMBOBOX(ID_SD_BOX,GUIMainWindow::OnSensorDataChange)
	EVT_COMMAND(ID_SD_TIMELINE, wxEVT_TIMELINE_CHANGE,GUIMainWindow::OnSDTimelineChange)
	EVT_CHECKBOX(ID_ANALYZE_MARKER_CB,GUIMainWindow::OnAnalyzeMarkerChange)
	EVT_BUTTON(ID_CLEAR_MARKER_BT,GUIMainWindow::OnSDTLMarkerClear)
	EVT_BUTTON(ID_MARKER_NEXT_BT,GUIMainWindow::OnSDTLNextMarker)
	EVT_BUTTON(ID_MARKER_PREV_BT,GUIMainWindow::OnSDTLPrevMarker)
	EVT_MENU(ID_EXPORT_VIEWPORT,GUIMainWindow::OnExportViewportImage)
	EVT_BUTTON(ID_FIND_MAX_BT,GUIMainWindow::OnFindMaxTSD)
	EVT_CHECKBOX(ID_AUTO_UPDATE_CB,GUIMainWindow::OnAutoUpdateChange)
	EVT_MENU(ID_EXPORT_VTK,GUIMainWindow::OnExportVTK)
END_EVENT_TABLE()

GUIMainWindow::GUIMainWindow(const wxChar *title, int xpos, int ypos, int width, int height):
			wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height))
{
	gl_context = new GUIGLCanvas(this);

	mwMenuBar = new wxMenuBar();
	// File Menu
	mwFileMenu = new wxMenu();
	//Import menu
	mwImportMenu = new wxMenu();
	mwImportMenu->Append(ID_IMPORT_OBJ, wxT("Modell und Sensordaten..."));
	mwImportMenu->Append(ID_IMPORT_SD, wxT("Sensordaten..."));
	mwImportMenu->Append(ID_IMPORT_TSD, wxT("Sensordaten-Paket..."));
	mwFileMenu->AppendSubMenu(mwImportMenu,wxT("Import"));
	//Export menu
	mwExportMenu = new wxMenu();
	mwExportMenu->Append(ID_EXPORT_VIEWPORT,wxT("Screenshot (Viewport)..."));
	mwExportMenu->Append(ID_EXPORT_VTK,wxT("Legacy VTK - Datei..."));
	mwFileMenu->AppendSubMenu(mwExportMenu,wxT("Export"));
	mwFileMenu->AppendSeparator();
	mwFileMenu->Append(wxID_EXIT, wxT("&Beenden"));
	mwMenuBar->Append(mwFileMenu, wxT("&Datei"));
	// Edit menu
	mwEditMenu = new wxMenu();
	mwEditMenu->Append(ID_DELETE_ACTIVE_OBJ,wxT("Aktives Objekt löschen"));
	mwMenuBar->Append(mwEditMenu,wxT("Bearbeiten"));
	// Analyze menu
	mwAnalyzeMenu = new wxMenu();
	mwAnalyzeMenu->Append(ID_ANALYZE,wxT("Übersicht..."));
	mwAnalyzeMenu->Append(ID_ANALYZE_POINT,wxT("Punkt..."));
	mwAnalyzeMenu->Append(ID_RENDER_CUT,wxT("Schnitt berechnen..."));
	mwMenuBar->Append(mwAnalyzeMenu,wxT("Analysieren"));
	// About menu
	mwHelpMenu = new wxMenu();
	mwHelpMenu->Append(ID_ABOUT, wxT("Über"));
	mwMenuBar->Append(mwHelpMenu, wxT("Hilfe"));

	prop_scroll_win = new wxScrolledWindow(this, wxID_ANY);
	view_scroll_win = new wxScrolledWindow(this, wxID_ANY);
	propbox =  new PropertiesBox(prop_scroll_win);
	viewbox = new ViewpropBox(view_scroll_win);
	prop_scroll_win->SetScrollRate(10,10);
	view_scroll_win->SetScrollRate(10,10);

	SetMenuBar(mwMenuBar);
	analyzerframe = NULL;
	analyze_window_valid = false;
	rendercutwindow = NULL;
	render_cut_window_valid = false;
	updating = false;
	wxImage::AddHandler( new wxJPEGHandler );

	#define NUMBEROFPATHS 4
	string configpaths[NUMBEROFPATHS] {
		string(wxStandardPaths::Get().GetExecutablePath().BeforeLast('/').ToUTF8().data())+"/",
		"/usr/local/share/simpleanalyzer/",
		"/usr/share/simpleanalyzer/",
	};
	string datadir = "";
	ifstream testfile;
	for (int i=0;i<NUMBEROFPATHS;i++) {
		datadir = configpaths[i];
		testfile.open(datadir+string("icons/analyze_point.png"));
		if (testfile.is_open()) break;
	}
	if (!testfile.is_open()) {
		cerr << "could not find application data! make sure the data is located in one of this paths:"<<endl;
		for (int i=0;i<NUMBEROFPATHS;i++) {
			cout << configpaths[i] << endl;
		}
		Close(true);
	}

	//Toolbar
	wxImage::AddHandler( new wxPNGHandler );
	toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT, _T("ID_TOOLBAR1"));
	toolbar->AddTool(ID_IMPORT_OBJ,wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_TOOLBAR), wxT("Objekt importieren"));
	toolbar->AddTool(ID_IMPORT_SD, wxArtProvider::GetBitmap(wxART_NORMAL_FILE , wxART_TOOLBAR), wxT("Sensordaten importieren"));
	toolbar->AddTool(ID_IMPORT_TSD, wxArtProvider::GetBitmap(wxART_FILE_OPEN , wxART_TOOLBAR), wxT("Sensordatenpaket importieren"));
	toolbar->AddSeparator();
	toolbar->AddTool(ID_CHANGE_ACTIVE_OBJ,wxT("aktives Objekt"),wxArtProvider::GetBitmap(wxART_LIST_VIEW  ,wxART_TOOLBAR) , wxT("Aktives Objekt wählen"));
	toolbar->AddTool(ID_DELETE_ACTIVE_OBJ,wxArtProvider::GetBitmap(wxART_DELETE, wxART_TOOLBAR),wxT("aktives Objekt löschen"));
	toolbar->AddSeparator();
	wxImage analyze_point(wxString::FromUTF8(datadir.c_str())+wxT("icons/analyze_point.png"), wxBITMAP_TYPE_PNG);
	analyze_point = analyze_point.Scale(toolbar->GetToolBitmapSize().x,toolbar->GetToolBitmapSize().y,wxIMAGE_QUALITY_HIGH);
	toolbar->AddTool(ID_ANALYZE_POINT,analyze_point,wxT("Punkt analysieren"));
	toolbar->Realize();
	SetToolBar(toolbar);
	/*//Load Testobject
	ObjectData* newobj = new ObjectData();
	wxString path2 = wxT("../examples/haus.obj");
	newobj->loadFromFile(path2);
	wxString sdpath = wxT("../../csvtosd/temperatur.tsd");
	newobj->addTimedData(sdpath);
	addObject(newobj);*/
	/*ObjectData*newobj = new ObjectData();
	wxString path = wxT("../examples/cylinder.obj");
	newobj->loadFromFile(path);
	addObject(newobj);*/

	SetIcon(wxIcon(wxString::FromUTF8(datadir.c_str())+wxT("icons/prgm-icon.png")));
	Centre();
}

void GUIMainWindow::setAnalyzeWindowStatus(bool isValid) {
	analyze_window_valid = isValid;
}
void GUIMainWindow::setCutRenderWindowStatus(bool isValid) {
	render_cut_window_valid = isValid;
}

GUIGLCanvas* GUIMainWindow::getGLCanvas() {
	return gl_context;
}

void GUIMainWindow::addObject(ObjectData* obj) {
	data_objects.resize(data_objects.size()+1,obj);
	setActiveObject(data_objects.size()-1);
}
void GUIMainWindow::setActiveObject(int index) {
	current_data_object_index = index;
	gl_context->setRenderObject(data_objects.at(current_data_object_index));
	propbox->setCurrentMaterial(0);
	updateObjectPropGUI();
	updateViewPropGUI();
}
void GUIMainWindow::OnActiveObjectDelete(wxCommandEvent &event) {
	if (data_objects.size()>1) {
		delete data_objects.at(current_data_object_index);
		data_objects.erase(data_objects.begin()+current_data_object_index);
		current_data_object_index--;
		if (current_data_object_index<0) {
			current_data_object_index = 0;
		}
		setActiveObject(current_data_object_index);
	} else {
		wxMessageBox( wxT("Das aktuelle Objekt ist das Einzige, kann also nicht gelöscht werden!"), wxT("Fehler"), wxICON_ERROR);
	}
}

void GUIMainWindow::OnResize(wxSizeEvent &event) {
	// 3d-view
	gl_context->SetSize(VIEWBOXWIDTH,0,GetSize().x-PROPBOXWIDTH-VIEWBOXWIDTH,GetSize().y,0);
	propbox->SetSize(propbox->GetPosition().x,propbox->GetPosition().y,PROPBOXWIDTH-10,0,0);
	propbox->resize();
	prop_scroll_win->SetSize(GetSize().x-PROPBOXWIDTH+5,0,PROPBOXWIDTH-10,GetSize().y-25,0);
	prop_scroll_win->SetVirtualSize(propbox->GetSize().x,propbox->GetSize().y+30);
	viewbox->SetSize(5,0,VIEWBOXWIDTH-10,GetSize().y-25,0);
	viewbox->resize();
	view_scroll_win->SetSize(5,0,VIEWBOXWIDTH-10,GetSize().y-25,0);
	view_scroll_win->SetVirtualSize(viewbox->GetSize().x,viewbox->GetSize().y+30);

}
string floattostr(double val) {
	ostringstream ss;
	ss << val;
	return ss.str();
}
void GUIMainWindow::OnSDTimelineChange(wxCommandEvent &event) {
	OnGeneralPropChange(event);
	updating = true;
	propbox->getAnalyzeMarkerCheckBox()->SetValue(propbox->getSdTimeline()->isMarked(propbox->getSdTimeline()->getValue()));
	updating = false;
}
void GUIMainWindow::OnSensorDataChange(wxCommandEvent &event) {
	OnGeneralPropChange(event);
	propbox->resize();
	prop_scroll_win->SetVirtualSize(propbox->GetSize().x,propbox->GetSize().y+50);
}
void GUIMainWindow::OnSDTLMarkerClear(wxCommandEvent &event) {
	propbox->getSdTimeline()->clearMarkers();
	updating = true;
	propbox->getAnalyzeMarkerCheckBox()->SetValue(false);
	updating = false;
}
void GUIMainWindow::OnAnalyzeMarkerChange(wxCommandEvent &event) {
	if (!updating) {
		int val = propbox->getSdTimeline()->getValue();
		propbox->getSdTimeline()->setMarked(val,propbox->getAnalyzeMarkerCheckBox()->GetValue());
	}
}
void GUIMainWindow::OnFindMaxTSD(wxCommandEvent &event) {
	if (current_data_object_index>-1) {
		ObjectData* obj = data_objects.at(current_data_object_index);
		if (obj->getCurrentSensorIndex()!=propbox->getSensorDataList()->GetSelection()) {
			wxMessageBox(wxT("Das Objekt wurde noch nicht neu berechnet!\nBitte berechnen sie das Objekt neu, um die Maximumssuche fortzusetzen!"),wxT("Fehler"));
			return;
		}
		wxMessageDialog dlg(this,wxT("Nur Messwertdurchschnitt verwenden? (schneller)"),wxT("Schnelle Methode verwenden?"),wxYES_NO|wxYES_DEFAULT| wxCANCEL);
		int res = dlg.ShowModal();
		if (res!=wxID_CANCEL) {
			propbox->getSdTimeline()->findMaxValue(obj,(res == wxID_YES));
		}
	}
}
void GUIMainWindow::OnSDTLNextMarker(wxCommandEvent &event) {
	vector<int>* markers = propbox->getSdTimeline()->getMarkers();
	if (markers->size()==0) {
		wxMessageBox(wxT("Es sind keine Analyze-Marker gesetzt!"),wxT("Hinweis"),wxICON_INFORMATION);
	} else {
		for (size_t i=0;i<markers->size();i++) {
			if (markers->at(i)>propbox->getSdTimeline()->getValue()) {
				propbox->getSdTimeline()->setValue(markers->at(i));
				break;
			}
		}
	}
}
void GUIMainWindow::OnSDTLPrevMarker(wxCommandEvent &event) {
	vector<int>* markers = propbox->getSdTimeline()->getMarkers();
	if (markers->size()==0) {
		wxMessageBox(wxT("Es sind keine Analyze-Marker gesetzt!"),wxT("Hinweis"),wxICON_INFORMATION);
	} else {
		for (size_t i=0;i<markers->size();i++) {
			if (markers->at(i)>=propbox->getSdTimeline()->getValue()) {
				if (i>0) {
					propbox->getSdTimeline()->setValue(markers->at(i-1));
				} else {
					propbox->getSdTimeline()->setValue(markers->at(0));
				}
				break;
			}
			if (i==markers->size()-1) {
				propbox->getSdTimeline()->setValue(markers->at(i));
			}
		}
	}
}
void GUIMainWindow::OnAutoUpdateChange(wxCommandEvent &event) {
	if (propbox->getAutoUpdateCeckBox()->IsChecked()) {
		OnRecalcBtClick(event);
	}
}
void GUIMainWindow::assignCurrentObjectProps() {	//GUI -> object
	if (current_data_object_index>-1) {
		ObjectData* obj = data_objects.at(current_data_object_index);
		setlocale(LC_NUMERIC, "C");
		obj->setName(string(propbox->getObjNameEdit()->GetValue().ToAscii()));
		obj->setMaxvolume(atof(propbox->getMaxVolumeEdit()->GetValue().ToAscii()));
		obj->setQuality(atof(propbox->getQualityEdit()->GetValue().ToAscii()));
		obj->setCurrentSensorIndex(propbox->getSensorDataList()->GetSelection());
		ObjectData* object = data_objects.at(current_data_object_index);
		SensorData* sd = &object->getSensorDataList()->at(propbox->getSensorDataList()->GetSelection());
		if (sd->timed) {
			sd->current_time_index = propbox->getSdTimeline()->getValue();
		}
		ObjectData::MaterialData* mat = &obj->getMaterials()->at(propbox->getCurrentMaterial());
		mat->name				  = propbox->getMatNameEdit()->GetValue().ToAscii();
		mat->interpolation_mode   = (Interpolator::InterpolationMode) propbox->getInterpolationModeList()->GetSelection();
		mat->density   			  = atof(propbox->getDensityEdit()->GetValue().ToAscii());
		mat->specificheatcapacity = atof(propbox->getSpecificHeatCapEdit()->GetValue().ToAscii());
	}
}
void GUIMainWindow::updateObjectPropGUI() {		//object -> GUI
	if (current_data_object_index>-1) {
		updating = true;
		ObjectData* obj = data_objects.at(current_data_object_index);
		setlocale(LC_NUMERIC, "C");
		propbox->SetLabel(wxString::FromAscii((obj->getName()+" - Objekteigenschaften:").c_str()));
		propbox->getObjNameEdit()->SetValue(wxString::FromAscii(obj->getName().c_str()));
		propbox->getMaxVolumeEdit()->SetValue(wxString::FromAscii(floattostr(obj->getMaxvolume()).c_str()));
		propbox->getQualityEdit()->SetValue(wxString::FromAscii(floattostr(obj->getQuality()).c_str()));
		propbox->getSensorDataList()->Clear();
		for (unsigned int i=0;i<obj->getSensorDataList()->size();i++) {
			propbox->getSensorDataList()->Insert(wxString::FromAscii(obj->getSensorDataList()->at(i).name.c_str()),i);
		}
		propbox->getSensorDataList()->SetSelection(obj->getCurrentSensorIndex());
		propbox->getMatListBox()->Clear();
		for (unsigned int i=0;i<obj->getMaterials()->size();i++) {
			propbox->getMatListBox()->Insert(wxString::FromAscii(obj->getMaterials()->at(i).name.c_str()),i);
		}
		propbox->getMatListBox()->SetSelection(propbox->getCurrentMaterial());
		ObjectData::MaterialData* mat = &obj->getMaterials()->at(propbox->getCurrentMaterial());
		propbox->getMatPropBox()->SetLabel(wxString::FromAscii((mat->name+" - Materialeigenschaften").c_str()));

		propbox->getInterpolationModeList()->SetSelection(mat->interpolation_mode);
		propbox->getDensityEdit()->SetValue(wxString::FromAscii(floattostr(mat->density).c_str()));
		propbox->getSpecificHeatCapEdit()->SetValue(wxString::FromAscii(floattostr(mat->specificheatcapacity).c_str()));
		propbox->getMatNameEdit()->SetValue(wxString::FromAscii(mat->name.c_str()));
		propbox->resize();
		updating = false;
		propbox->getUpToDateLbl()->Hide();


		int nPos = toolbar->GetToolPos(ID_CHANGE_ACTIVE_OBJ);
		wxToolBarToolBase* pTool = toolbar->RemoveTool(ID_CHANGE_ACTIVE_OBJ);
		pTool->SetLabel(wxString::FromAscii(("aktives Objekt: "+obj->getName()).c_str()));
		toolbar->InsertTool(nPos, pTool);
		toolbar->Realize();
	}
}
void GUIMainWindow::OnMaterialSelect(wxCommandEvent &event) {
	if (!updating) {
		propbox->setCurrentMaterial(propbox->getMatListBox()->GetSelection());
		updateObjectPropGUI();
	}
}
void GUIMainWindow::OnRecalcBtClick(wxCommandEvent& event) {
	if (current_data_object_index>-1) {
		ObjectData* obj = data_objects.at(current_data_object_index);
		assignCurrentObjectProps();
		obj->calculateIO();
		gl_context->refresh();
		propbox->getUpToDateLbl()->Hide();
		if (analyze_window_valid) {
			analyzerframe->Update();
		}
	}
}
void GUIMainWindow::OnAnalyze(wxCommandEvent &event) {
	if (current_data_object_index>-1) {
		if (!analyze_window_valid) {
			analyzerframe = new GUIAnalyzeOutputWindow(this,wxT("Analysedaten"), 100, 100, 800, 300);
			analyzerframe->Show(true);
			analyze_window_valid = true;
		}
	}
}
void GUIMainWindow::OnAnalyzePoint(wxCommandEvent &event) {
	if (current_data_object_index>-1) {
		GUIAnalyzePointWindow* analyzewin = new GUIAnalyzePointWindow(this,wxT("Analyse an Punkt"),100,100,350,120);
		analyzewin->Show();
	} else {
		wxMessageBox(wxT("Es ist kein Objekt geladen!"),wxT("Fehler"));
	}
}
void GUIMainWindow::OnRenderCut(wxCommandEvent &event) {
	if (current_data_object_index>-1) {
		if (!render_cut_window_valid) {
			rendercutwindow = new GUICutRenderWindow(this,wxT("Schnitt brechnen"), 100, 100, 800, 600);
			rendercutwindow->Show(true);
			render_cut_window_valid = true;
			gl_context->Refresh(false,NULL);
		}
	}
}
void GUIMainWindow::OnImmediateUpdatePropChange(wxCommandEvent &event) {
	if (!updating) {
		propbox->getUpToDateLbl()->Show();
		assignCurrentObjectProps();
		updateObjectPropGUI();
		updateViewPropGUI();
	}
}

void GUIMainWindow::OnGeneralPropChange(wxCommandEvent &event) {
	if (!updating) {
		propbox->getUpToDateLbl()->Show();
		if (propbox->getAutoUpdateCeckBox()->IsChecked()) {
			OnRecalcBtClick(event);
		}
	}
}

void GUIMainWindow::assignViewProps() {
	if (current_data_object_index>-1) {
		Renderer::Viewport_info* view = gl_context->getRenderer()->getViewport();

		view->showPoints 		= (Renderer::RenderMode) viewbox->getPointsCheckBox()->GetSelection();
		view->showEdges 		= (Renderer::RenderMode) viewbox->getEdgesCheckBox()->GetSelection();
		view->showFaces 		= (Renderer::RenderMode) viewbox->getFacesCheckBox()->GetSelection();
		view->show_extrapolated = viewbox->getShowExtrapolatedCheckBox()->IsChecked();
		view->show_sensordata	= viewbox->getShowShowSensorData()->IsChecked();
		visualization_info.min_visualisation_temp = viewbox->getColorRangeMinEdit()->GetValue();
		visualization_info.max_visualisation_temp = viewbox->getColorRangeMaxEdit()->GetValue();
		view->scale				= atof(viewbox->getViewScaleEdit()->GetValue().ToAscii());
		for (unsigned int i=0;i<data_objects.at(current_data_object_index)->getMaterials()->size();i++) {
			ObjectData::MaterialData* mat = &data_objects.at(current_data_object_index)->getMaterials()->at(i);
			mat->visible = viewbox->getMatVisibilityListBox()->IsChecked(i);
		}
	}
}
void GUIMainWindow::updateViewPropGUI() {
	if (current_data_object_index>-1) {
		updating = true;
		Renderer::Viewport_info* view = gl_context->getRenderer()->getViewport();
		viewbox->getShowExtrapolatedCheckBox()->SetValue(view->show_extrapolated);
		viewbox->getShowShowSensorData()->SetValue(view->show_sensordata);
		viewbox->getPointsCheckBox()->SetSelection(view->showPoints);
		viewbox->getEdgesCheckBox()->SetSelection(view->showEdges);
		viewbox->getFacesCheckBox()->SetSelection(view->showFaces);
		viewbox->getColorRangeMinEdit()->SetValue(visualization_info.min_visualisation_temp);
		viewbox->getColorRangeMaxEdit()->SetValue(visualization_info.max_visualisation_temp);
		viewbox->getViewScaleEdit()->SetValue(floattowxstr(view->scale));
		viewbox->getMatVisibilityListBox()->Clear();
		for (unsigned int i=0;i<data_objects.at(current_data_object_index)->getMaterials()->size();i++) {
			ObjectData::MaterialData* mat = &data_objects.at(current_data_object_index)->getMaterials()->at(i);
			viewbox->getMatVisibilityListBox()->Insert(wxString::FromAscii(mat->name.c_str()),i);
			viewbox->getMatVisibilityListBox()->Check(i,mat->visible);
		}
		updating = false;
	}
}
void GUIMainWindow::OnViewPropChange(wxCommandEvent &event) {
	if (!updating) {
		assignViewProps();
		gl_context->refresh();
	}
}
void GUIMainWindow::OnViewPropSpinChange(wxSpinEvent &event) {
	if (!updating) {
		assignViewProps();
		gl_context->refresh();
	}
}

void GUIMainWindow::OnMenuImportObj(wxCommandEvent &event)
{
	wxFileDialog *OpenDialog= new wxFileDialog(this, wxT("Datei öffnen..."), _(""), _(""), _("Wavefront-Objektdateien (*.obj)|*.obj"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if ( OpenDialog->ShowModal() == wxID_OK )
	{
		ObjectData* newobj = new ObjectData();
		wxString path = OpenDialog->GetPath();
		int status = newobj->loadFromFile(path);
		switch (status) {
		case ObjectData::OD_LOAD_ALREADY_LOADED:
			wxMessageBox(path+wxT(" ist bereits geöffnet!"));
			delete newobj;
			break;
		case ObjectData::OD_LOAD_INVALID_FILE:
			wxMessageBox(path+wxT(" ist keine Wavefront(.obj)-Datei."));
			delete newobj;
			break;
		case ObjectData::OD_LOAD_INVALID_SENSOR_FILE:
			wxMessageBox(wxT("Sensordaten konnten nicht geladen werden: ")+(path.BeforeLast('.')+wxT(".(t)sd")));
			delete newobj;
			break;
		case ObjectData::OD_SUCCESS:
			addObject(newobj);
			break;
		default:
			cout << "undefined status value!" << endl;
			delete newobj;
			break;
		}
		propbox->setCurrentMaterial(0);
		updateObjectPropGUI();
		updateViewPropGUI();
	}
	OpenDialog->Close();
	OpenDialog->Destroy();
	propbox->getUpToDateLbl()->Hide();
}
void GUIMainWindow::OnMenuImportSD(wxCommandEvent &event) {
	wxFileDialog *OpenDialog= new wxFileDialog(this, wxT("Datei öffnen..."), _(""), _(""), _("Sensordaten (*.sd)|*.sd"), wxFD_OPEN);
	if ( OpenDialog->ShowModal() == wxID_OK )
	{
		ObjectData* obj = data_objects.at(current_data_object_index);
		wxString path = OpenDialog->GetPath();
		obj->addSensorData(path);
		updateObjectPropGUI();
	}
	OpenDialog->Close();
	OpenDialog->Destroy();
}
void GUIMainWindow::OnMenuImportTSD(wxCommandEvent &event) {
	wxFileDialog *OpenDialog= new wxFileDialog(this, wxT("Datei öffnen..."), _(""), _(""), _("Sensordaten-Pakete (*.tsd)|*.tsd"), wxFD_OPEN);
	if ( OpenDialog->ShowModal() == wxID_OK )
	{
		ObjectData* obj = data_objects.at(current_data_object_index);
		wxString path = OpenDialog->GetPath();
		obj->addTimedData(path);
		updateObjectPropGUI();
	}
	OpenDialog->Close();
	OpenDialog->Destroy();
}
void GUIMainWindow::OnExportViewportImage(wxCommandEvent &event) {
	if (current_data_object_index==-1) {
		cerr << "no object loaded!" << endl;
		return;
	}
	wxFileDialog *SaveDialog= new wxFileDialog(this, wxT("Export nach..."), _(""), _(""), _("Portable Network Graphics (*.png)|*.png"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if ( SaveDialog->ShowModal() == wxID_OK )
	{
		cout << SaveDialog->GetPath().ToUTF8().data() << endl;
		gl_context->SetCurrent();
		wxImage* img = gl_context->getRenderer()->getViewportImage();
		img->SaveFile(SaveDialog->GetPath());
		img->Destroy();
	}
	SaveDialog->Close();
	SaveDialog->Destroy();
}
void GUIMainWindow::OnExportVTK(wxCommandEvent &event) {
	if (current_data_object_index==-1) {
		cerr << "no object loaded!" << endl;
		return;
	}
	wxFileDialog *SaveDialog= new wxFileDialog(this, wxT("Export nach..."), _(""), _(""), _("Visualization Toolkit file(*.vtk)|*.vtk"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if ( SaveDialog->ShowModal() == wxID_OK )
	{
		Exporter exporter;
		cout << string(SaveDialog->GetPath().ToUTF8().data()) << endl;
		exporter.ExportLegacyVTK(string(SaveDialog->GetPath().ToUTF8().data()),data_objects.at(current_data_object_index));
	}
	SaveDialog->Close();
	SaveDialog->Destroy();
}
void GUIMainWindow::OnActiveObjectChangePopup(wxCommandEvent &event) {
	if (event.GetId()!=0) {
		setActiveObject(event.GetId()-wxID_HIGHEST-1);
	}
}
void GUIMainWindow::OnActiveObjectChange(wxCommandEvent &event) {
	wxMenu mnu;
	mnu.Append(0,wxT("verfügbare Objekte:"));
	mnu.AppendSeparator();
	for (unsigned int i=0;i<data_objects.size();i++) {
		mnu.Append(wxID_HIGHEST+i+1,wxString::FromAscii(data_objects.at(i)->getName().c_str()));
	}
	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&GUIMainWindow::OnActiveObjectChangePopup, NULL, this);
	PopupMenu(&mnu);
}
void GUIMainWindow::OnMenuFileQuit(wxCommandEvent &event) {
	Close(false);
}

void GUIMainWindow::OnMenuHelpAbout(wxCommandEvent &event) {
	wxMessageBox(wxT("Simple Analyzer\n©2013-2014 by Valentin Roland"));
}

GUIMainWindow::~GUIMainWindow() {
}


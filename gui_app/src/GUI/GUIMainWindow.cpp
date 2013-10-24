/*
 * GUIMainWindow.cpp
 *
 *  Created on: 04.09.2013
 *      Author: valentin
 */

#include "GUIMainWindow.h"
#include <iostream>
#include <sstream>
#include <string>
#include "../libraries/tetgen/tetgen.h"
#include "../processing/MeshProcessor.h"
#include "../fileIO/Importer.h"
#include "../processing/ObjectData.h"
#include "constants.h"

using namespace std;

extern std::vector<ObjectData*> data_objects;
extern int current_data_object_index;

#define PROPBOXWIDTH 300
#define VIEWBOXWIDTH 300

BEGIN_EVENT_TABLE(GUIMainWindow, wxFrame)
	EVT_MENU(ID_IMPORT_OBJ, GUIMainWindow::OnMenuImportObj)
	EVT_MENU(ID_IMPORT_SD, GUIMainWindow::OnMenuImportSD)
	EVT_MENU(wxID_EXIT, GUIMainWindow::OnMenuFileQuit)
	EVT_MENU(ID_ABOUT, GUIMainWindow::OnMenuHelpAbout)
	EVT_MENU(ID_ANALYZE, GUIMainWindow::OnAnalyze)
	EVT_LISTBOX(ID_MATERIALBOX, GUIMainWindow::OnMaterialSelect)
	EVT_BUTTON(ID_RECALCBT,GUIMainWindow::OnRecalcBtClick)
	EVT_SIZE(GUIMainWindow::OnResize)
	EVT_TEXT(ID_IMMEDIATE_UPDATE_PROP,GUIMainWindow::OnImmUpPropChange)
	EVT_TEXT(ID_GENERAL_PROP,GUIMainWindow::OnGeneralPropChange)
	EVT_RADIOBOX(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropChange)
	EVT_CHECKLISTBOX(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropChange)
	EVT_CHECKBOX(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropChange)
	EVT_MENU(ID_CHANGE_ACTIVE_OBJ,GUIMainWindow::OnActiveObjectChange)
	EVT_MENU(ID_ANALYZE_POINT,GUIMainWindow::OnAnalyzePoint)
	EVT_MENU(ID_RENDER_CUT,GUIMainWindow::OnRenderCut)
	EVT_MENU(ID_DELETE_ACTIVE_OBJ,GUIMainWindow::OnActiveObjectDelete)
	EVT_MENU(ID_IMPORT_TSD,GUIMainWindow::OnMenuImportTSD)
	EVT_COMBOBOX(ID_SD_BOX,GUIMainWindow::OnSensorDataChange)
	EVT_COMMAND(ID_SD_TIMELINE, wxEVT_TIMELINE, GUIMainWindow::OnGeneralPropChange)
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
	mwImportMenu->Append(ID_IMPORT_OBJ, wxT("&Modell und Sensordaten..."));
	mwImportMenu->Append(ID_IMPORT_SD, wxT("&Sensordaten..."));
	mwImportMenu->Append(ID_IMPORT_TSD, wxT("&Sensordaten-Paket..."));
	mwFileMenu->AppendSubMenu(mwImportMenu,wxT("Import"));
	//Export menu
	mwExportMenu = new wxMenu();
	mwFileMenu->AppendSubMenu(mwExportMenu,wxT("Export"));
	mwFileMenu->AppendSeparator();
	mwFileMenu->Append(wxID_EXIT, wxT("&Beenden"));
	mwMenuBar->Append(mwFileMenu, wxT("&Datei"));
	// Edit menu
	mwEditMenu = new wxMenu();
	mwEditMenu->Append(ID_DELETE_ACTIVE_OBJ,wxT("&Aktives Objekt löschen"));
	// Analyze menu
	mwAnalyzeMenu = new wxMenu();
	mwAnalyzeMenu->Append(ID_ANALYZE,wxT("Übersicht..."));
	mwAnalyzeMenu->Append(ID_ANALYZE_POINT,wxT("Punkt..."));
	mwAnalyzeMenu->Append(ID_RENDER_CUT,wxT("Schnitt berechnen..."));
	mwMenuBar->Append(mwAnalyzeMenu,wxT("Analysieren"));
	// About menu
	mwHelpMenu = new wxMenu();
	mwHelpMenu->Append(ID_ABOUT, wxT("&Über"));
	mwMenuBar->Append(mwHelpMenu, wxT("&Hilfe"));
	propbox = new PropertiesBox(this);
	viewbox = new ViewpropBox(this);
	SetMenuBar(mwMenuBar);
	analyzerframe = NULL;
	analyze_window_valid = false;
	rendercutwindow = NULL;
	render_cut_window_valid = false;
	updating = false;
	wxImage::AddHandler( new wxJPEGHandler );

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
	wxImage analyze_point(wxT("icons/analyze_point.png"), wxBITMAP_TYPE_PNG);
	analyze_point = analyze_point.Scale(toolbar->GetToolBitmapSize().x,toolbar->GetToolBitmapSize().y,wxIMAGE_QUALITY_HIGH);
	toolbar->AddTool(ID_ANALYZE_POINT,analyze_point,wxT("Punkt analysieren"));
	toolbar->Realize();
	SetToolBar(toolbar);
	//Load Testobject
	ObjectData* newobj = new ObjectData();
	wxString path2 = wxT("../examples/haus.obj");
	newobj->loadFromFile(path2);
	wxString sdpath = wxT("../examples/haus_2.sd");
	newobj->addSensorData(sdpath);
	addObject(newobj);
	/*newobj = new ObjectData();
	wxString path = wxT("../examples/cylinder.obj");
	newobj->loadFromFile(path);
	addObject(newobj);*/

	SetIcon(wxIcon(wxT("icons/prgm-icon.png")));
	Centre();
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
	updateObjectPropGUI();
	updateViewPropGUI();
}
void GUIMainWindow::OnActiveObjectDelete(wxCommandEvent&event) {
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
	//opbox
	propbox->SetSize(GetSize().x-PROPBOXWIDTH+5,0,PROPBOXWIDTH-10,GetSize().y-25,0);
	propbox->resize();
	viewbox->SetSize(5,0,PROPBOXWIDTH-10,GetSize().y-25,0);
	viewbox->resize();
}
string floattostr(double val) {
	ostringstream ss;
	ss << val;
	return ss.str();
}
void GUIMainWindow::OnSensorDataChange(wxCommandEvent &event) {
	OnGeneralPropChange(event);
	propbox->resize();
	propbox->Update();
}
void GUIMainWindow::assignCurrentObjectProps() {	//GUI -> object
	if (current_data_object_index>-1) {
		ObjectData* obj = data_objects.at(current_data_object_index);
		setlocale(LC_NUMERIC, "C");
		obj->name	   = propbox->objnameedit->GetValue().ToAscii();
		obj->maxvolume = atof(propbox->maxvolumeedit->GetValue().ToAscii());
		obj->quality   = atof(propbox->qualityedit->GetValue().ToAscii());
		obj->current_sensor_index = propbox->sensordatalist->GetSelection();
		ObjectData* object = data_objects.at(current_data_object_index);
		SensorData* sd = &object->sensordatalist.at(propbox->sensordatalist->GetSelection());
		if (sd->timed) {
			sd->current_time_index = propbox->sdtimeline->getValue();
		}
		MaterialData* mat = &obj->materials.at(propbox->current_material);
		mat->name				  = propbox->matnameedit->GetValue().ToAscii();
		mat->interpolation_mode   = (InterpolationMode) propbox->interpolationmodelist->GetSelection();
		mat->density   			  = atof(propbox->densityedit->GetValue().ToAscii());
		mat->specificheatcapacity = atof(propbox->cspecedit->GetValue().ToAscii());
	}
}
void GUIMainWindow::updateObjectPropGUI() {		//object -> GUI
	if (current_data_object_index>-1) {
		updating = true;
		ObjectData* obj = data_objects.at(current_data_object_index);
		setlocale(LC_NUMERIC, "C");
		propbox->SetLabel(wxString::FromAscii((obj->name+" - Objekteigenschaften:").c_str()));
		propbox->objnameedit->SetValue(wxString::FromAscii(obj->name.c_str()));
		propbox->maxvolumeedit->SetValue(wxString::FromAscii(floattostr(obj->maxvolume).c_str()));
		propbox->qualityedit->SetValue(wxString::FromAscii(floattostr(obj->quality).c_str()));
		propbox->sensordatalist->Clear();
		for (unsigned int i=0;i<obj->sensordatalist.size();i++) {
			propbox->sensordatalist->Insert(wxString::FromAscii(obj->sensordatalist.at(i).name.c_str()),i);
		}
		propbox->sensordatalist->SetSelection(obj->current_sensor_index);
		propbox->matlistbox->Clear();
		for (unsigned int i=0;i<obj->materials.size();i++) {
			propbox->matlistbox->Insert(wxString::FromAscii(obj->materials.at(i).name.c_str()),i);
		}
		propbox->matlistbox->SetSelection(propbox->current_material);
		MaterialData* mat = &obj->materials.at(propbox->current_material);
		propbox->matpropbox->SetLabel(wxString::FromAscii((mat->name+" - Materialeigenschaften").c_str()));

		propbox->interpolationmodelist->SetSelection(mat->interpolation_mode);
		propbox->densityedit->SetValue(wxString::FromAscii(floattostr(mat->density).c_str()));
		propbox->cspecedit->SetValue(wxString::FromAscii(floattostr(mat->specificheatcapacity).c_str()));
		propbox->matnameedit->SetValue(wxString::FromAscii(mat->name.c_str()));
		propbox->resize();
		updating = false;
		propbox->uptodatetext->Hide();


		int nPos = toolbar->GetToolPos(ID_CHANGE_ACTIVE_OBJ);
		wxToolBarToolBase* pTool = toolbar->RemoveTool(ID_CHANGE_ACTIVE_OBJ);
		pTool->SetLabel(wxString::FromAscii(("aktives Objekt: "+obj->name).c_str()));
		toolbar->InsertTool(nPos, pTool);
		toolbar->Realize();
	}
}
void GUIMainWindow::OnMaterialSelect(wxCommandEvent &event) {
	if (!updating) {
		propbox->current_material = propbox->matlistbox->GetSelection();
		updateObjectPropGUI();
	}
}
void GUIMainWindow::OnRecalcBtClick(wxCommandEvent& event) {
	if (current_data_object_index>-1) {
		ObjectData* obj = data_objects.at(current_data_object_index);
		assignCurrentObjectProps();
		obj->calculateIO();
		gl_context->refreshRenderObject();
		propbox->uptodatetext->Hide();
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
	GUIAnalyzePointWindow* analyzewin = new GUIAnalyzePointWindow(this,wxT("Analyse an Punkt"),100,100,350,120);
	analyzewin->Show();
}
void GUIMainWindow::OnRenderCut(wxCommandEvent &event) {

	if (current_data_object_index>-1) {
		if (!render_cut_window_valid) {
			rendercutwindow = new GUICutRenderWindow(this,wxT("Schnitt brechnen"), 100, 100, 600, 500);
			rendercutwindow->Show(true);
			render_cut_window_valid = true;
			gl_context->Refresh(false,NULL);
		}
	}
}
void GUIMainWindow::OnImmUpPropChange(wxCommandEvent &event) {
	if (!updating) {
		propbox->uptodatetext->Show();
		assignCurrentObjectProps();
		updateObjectPropGUI();
		updateViewPropGUI();
	}
}
void GUIMainWindow::OnGeneralPropChange(wxCommandEvent &event) {
	if (!updating) {
		propbox->uptodatetext->Show();
	}
}
void GUIMainWindow::assignViewProps() {
	Viewport_info* view = &gl_context->renderer.viewport;

	view->showpoints 		= viewbox->pointscb->GetSelection();
	view->showedges 		= viewbox->edgescb->GetSelection();
	view->showfaces 		= viewbox->facescb->GetSelection();
	view->show_extrapolated = viewbox->show_extcb->IsChecked();
	view->show_sensordata	= viewbox->show_sdata->IsChecked();
	for (unsigned int i=0;i<data_objects.at(current_data_object_index)->materials.size();i++) {
		MaterialData* mat = &data_objects.at(current_data_object_index)->materials.at(i);
		mat->visible = viewbox->matvisibility->IsChecked(i);
	}
}
void GUIMainWindow::updateViewPropGUI() {
	updating = true;
	Viewport_info* view = &gl_context->renderer.viewport;
	viewbox->show_extcb->SetValue(view->show_extrapolated);
	viewbox->show_sdata->SetValue(view->show_sensordata);
	viewbox->pointscb->SetSelection(view->showpoints);
	viewbox->edgescb->SetSelection(view->showedges);
	viewbox->facescb->SetSelection(view->showfaces);
	viewbox->matvisibility->Clear();
	for (unsigned int i=0;i<data_objects.at(current_data_object_index)->materials.size();i++) {
		MaterialData* mat = &data_objects.at(current_data_object_index)->materials.at(i);
		viewbox->matvisibility->Insert(wxString::FromAscii(mat->name.c_str()),i);
		viewbox->matvisibility->Check(i,mat->visible);
	}
	updating = false;
}
void GUIMainWindow::OnViewPropChange(wxCommandEvent &event) {
	if (!updating) {
		assignViewProps();
		gl_context->refreshRenderObject();
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
		case OD_LOAD_ALREADY_LOADED:
			wxMessageBox(path+wxT(" ist bereits geöffnet!"));
			delete newobj;
			break;
		case OD_LOAD_INVALID_FILE:
			wxMessageBox(path+wxT(" ist keine Wavefront(.obj)-Datei."));
			delete newobj;
			break;
		case OD_LOAD_INVALID_SENSOR_FILE:
			wxMessageBox(wxT("Sensordaten konnten nicht geladen werden: ")+(path.BeforeLast('.')+wxT(".sd")));
			delete newobj;
			break;
		case OD_SUCCESS:
			addObject(newobj);
			break;
		default:
			cout << "undefined status value!" << endl;
			delete newobj;
			break;
		}
		propbox->current_material = 0;
		updateObjectPropGUI();
		updateViewPropGUI();
	}
	OpenDialog->Close();
	OpenDialog->Destroy();
	propbox->uptodatetext->Hide();
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
		mnu.Append(wxID_HIGHEST+i+1,wxString::FromAscii(data_objects.at(i)->name.c_str()));
	}
	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&GUIMainWindow::OnActiveObjectChangePopup, NULL, this);
	PopupMenu(&mnu);
}
void GUIMainWindow::OnMenuFileQuit(wxCommandEvent &event) {
	Close(false);
}

void GUIMainWindow::OnMenuHelpAbout(wxCommandEvent &event) {
	wxMessageBox(wxT("Simple Analyzer\n©2013-2013 by Valentin Roland"));
}

GUIMainWindow::~GUIMainWindow() {
}


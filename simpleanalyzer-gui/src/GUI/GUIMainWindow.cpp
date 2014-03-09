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
#include "../SimpleAnalyzerApp.h"
#include "../libraries/tetgen/tetgen.h"
#include "../processing/MeshProcessor.h"
#include "../fileIO/Importer.h"
#include "../processing/ObjectData.h"
#include "constants.h"
#include "../fileIO/Exporter.h"
#include "GUITimeline.h"
#include "../processing/utils.h"

#if defined (__WIN32__)
#include <Shellapi.h>
#endif

//Pfadseparatorzeichen
#if defined (__WIN32__)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

using namespace std;
using namespace Utils;

#define PROPBOXWIDTH 300
#define VIEWBOXWIDTH 300

//Eventtabelle zum Verknüpfen der Events von Menü, Objekteigenschaftenfenster und Visualisierungseigenschaftenfenster
BEGIN_EVENT_TABLE(GUIMainWindow, wxFrame)
	EVT_MENU(ID_IMPORT_OBJ, GUIMainWindow::OnMenuImportObj)
	EVT_MENU(ID_IMPORT_SD, GUIMainWindow::OnMenuImportSD)
	EVT_MENU(wxID_EXIT, GUIMainWindow::OnMenuFileQuit)
	EVT_MENU(ID_ABOUT, GUIMainWindow::OnMenuHelpAbout)
	EVT_MENU(ID_OPEN_MANUAL, GUIMainWindow::OnMenuOpenManual)
	EVT_MENU(ID_ANALYZE, GUIMainWindow::OnAnalyze)
	EVT_LISTBOX(ID_MATERIALBOX, GUIMainWindow::OnMaterialSelect)
	EVT_BUTTON(ID_RECALCBT,GUIMainWindow::OnRecalcBtClick)
	EVT_SIZE(GUIMainWindow::OnResize)
	EVT_TEXT(ID_IMMEDIATE_UPDATE_PROP,GUIMainWindow::OnImmediateUpdatePropChange)
	EVT_TEXT(ID_GENERAL_PROP,GUIMainWindow::OnGeneralPropChange)
	EVT_RADIOBOX(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropChange)
	EVT_TEXT(ID_GENERAL_VIEW_PROP,GUIMainWindow::OnViewPropChange)
	EVT_CHECKLISTBOX(ID_CHECKLISTBOX_VIEW_PROP,GUIMainWindow::OnViewPropChange)
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

GUIMainWindow::GUIMainWindow(const wxChar *title, int xpos, int ypos, int width,
		int height) : wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos),
				wxSize(width, height)) {

	/**
	 * Erstellen und initialisieren der Fensterkomponenten
	 */

	//Die 3D-Zeichenfläche
	gl_context = new GUIGLCanvas(this);

	//Das Haupmenü
	mwMenuBar = new wxMenuBar();

	// Das "Datei"-Untermenü
	mwFileMenu = new wxMenu();

	// Das "Import"-Untermenü
	mwImportMenu = new wxMenu();
	mwImportMenu->Append(ID_IMPORT_OBJ, wxT("Modell und Sensordaten..."));
	mwImportMenu->Append(ID_IMPORT_SD, wxT("Sensordaten..."));
	mwImportMenu->Append(ID_IMPORT_TSD, wxT("Sensordaten-Paket..."));
	mwFileMenu->AppendSubMenu(mwImportMenu, wxT("Import"));

	// Das "Export"-Untermenü
	mwExportMenu = new wxMenu();
	mwExportMenu->Append(ID_EXPORT_VIEWPORT, wxT("Screenshot (Viewport)..."));
	mwExportMenu->Append(ID_EXPORT_VTK, wxT("Legacy VTK - Datei..."));
	mwFileMenu->AppendSubMenu(mwExportMenu, wxT("Export"));
	mwFileMenu->AppendSeparator();
	mwFileMenu->Append(wxID_EXIT, wxT("&Beenden"));
	mwMenuBar->Append(mwFileMenu, wxT("&Datei"));

	// Das "Bearbeiten"-Untermenü
	mwEditMenu = new wxMenu();
	mwEditMenu->Append(ID_DELETE_ACTIVE_OBJ, wxT("Aktives Objekt löschen"));
	mwMenuBar->Append(mwEditMenu, wxT("Bearbeiten"));

	// Das "Analysieren"-Untermenü
	mwAnalyzeMenu = new wxMenu();
	mwAnalyzeMenu->Append(ID_ANALYZE, wxT("Übersicht..."));
	mwAnalyzeMenu->Append(ID_ANALYZE_POINT, wxT("Punkt..."));
	mwAnalyzeMenu->Append(ID_RENDER_CUT, wxT("2D-Temperaturverteilung berechnen..."));
	mwMenuBar->Append(mwAnalyzeMenu, wxT("Analysieren"));

	// Das "Hilfe"-Untermenü
	mwHelpMenu = new wxMenu();
	mwHelpMenu->Append(ID_OPEN_MANUAL, wxT("Handbuch öffnen..."));
	mwHelpMenu->Append(ID_ABOUT, wxT("Über"));
	mwMenuBar->Append(mwHelpMenu, wxT("Hilfe"));

	//Scrollbares Unterfenster für die Objekteigenschaften
	prop_scroll_win = new wxScrolledWindow(this, wxID_ANY);
	//Scrollbares Unterfenster für die Visualisierungsoptionen
	view_scroll_win = new wxScrolledWindow(this, wxID_ANY);


	prop_scroll_win->SetScrollRate(10, 10);
	view_scroll_win->SetScrollRate(10, 10);

	//Die Objekteigenschaften-Oberfläche
	propbox = new PropertiesBox(prop_scroll_win);
	//Die Visualisierungsoptionen-Oberfläche
	viewbox = new ViewpropBox(view_scroll_win);

	SetMenuBar(mwMenuBar);

	//Initialisieren der Verknüpfungen für die Unterfenster und ihrer Statusvariablen
	analyzerframe = NULL;
	analyze_window_valid = false;
	rendercutwindow = NULL;
	render_cut_window_valid = false;

	//Wird die Oberfläche gerade aktualisiert? -> Unterdrückt endloses aktualisieren durch Änderungen an der Oberfläche
	updating = false;

	//Laden der Verarbeitungsroutinen für die wxImage-Klasse
	wxInitAllImageHandlers();

	/*
	 * Finden des Datenverzeichnisses
	 */

	data_directory = string(wxStandardPaths::Get().GetExecutablePath().BeforeLast(PATH_SEPARATOR[0]).ToUTF8().data());

	//Datei zum Testen, ob ein Icon geladen werden kann.
	ifstream testfile;
	testfile.open(data_directory + PATH_SEPARATOR + "icons" + PATH_SEPARATOR + "analyze_point.png");

	//Liegt die Datei nicht im Verzeichnis der ausführbaren Datei?
	if (!testfile.is_open()) {
		//Die zusätzlichen Pfade versuchen
		for (int i = 0; i < NUMBEROFPATHS; i++) {
			data_directory = configpaths[i];
			testfile.open(data_directory + string("icons")
					+ string(PATH_SEPARATOR)
							+ string("analyze_point.png"));

			//Unter diesem Pfad gefunden?
			if (testfile.is_open()) {
				break;
			}

			//Unter keinem Pfad gefunden?
			if (i == NUMBEROFPATHS - 1) {
				cerr << endl;
				cerr
						<< "could not find application data! make sure the data is located in one of this paths:"
						<< endl;
				for (int i = 0; i < NUMBEROFPATHS; i++) {
					cerr << configpaths[i] << endl;
				}

				//Das Programm sofort beenden
				Close(true);
			}
		}
	}

	//Die Toolbar bestücken
	toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxTB_HORZ_TEXT, _T("ID_TOOLBAR1"));
	//Icon zum Laden eines Objekts laden
	wxImage obj_icon(
			wxString::FromUTF8(
					(data_directory + PATH_SEPARATOR + "icons" + PATH_SEPARATOR
							+ "obj_icon.png").c_str()), wxBITMAP_TYPE_PNG);
	obj_icon = obj_icon.Scale(toolbar->GetToolBitmapSize().x,
			toolbar->GetToolBitmapSize().y, wxIMAGE_QUALITY_HIGH);
	toolbar->AddTool(ID_IMPORT_OBJ, obj_icon,
			wxT("Objekt und Sensordaten importieren"));
	//Icon zum Laden einfacher Sensordaten laden
	wxImage sd_icon(
			wxString::FromUTF8(
					(data_directory + PATH_SEPARATOR + "icons" + PATH_SEPARATOR
							+ "sd_icon.png").c_str()), wxBITMAP_TYPE_PNG);
	sd_icon = sd_icon.Scale(toolbar->GetToolBitmapSize().x,
			toolbar->GetToolBitmapSize().y, wxIMAGE_QUALITY_HIGH);
	toolbar->AddTool(ID_IMPORT_SD, sd_icon, wxT("Sensordaten importieren"));
	//Icon zum Laden zeitbezogener Sensordaten laden
	wxImage tsd_icon(
			wxString::FromUTF8(
					(data_directory + PATH_SEPARATOR + "icons" + PATH_SEPARATOR
							+ "tsd_icon.png").c_str()), wxBITMAP_TYPE_PNG);
	tsd_icon = tsd_icon.Scale(toolbar->GetToolBitmapSize().x,
			toolbar->GetToolBitmapSize().y, wxIMAGE_QUALITY_HIGH);
	toolbar->AddTool(ID_IMPORT_TSD, tsd_icon,
			wxT("Sensordatenpaket importieren"));
	toolbar->AddSeparator();

	toolbar->AddTool(ID_CHANGE_ACTIVE_OBJ, wxT("aktives Objekt"),
			wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_TOOLBAR),
			wxT("Aktives Objekt wählen"));
	toolbar->AddTool(ID_DELETE_ACTIVE_OBJ,
			wxArtProvider::GetBitmap(wxART_DELETE, wxART_TOOLBAR),
			wxT("aktives Objekt löschen"));
	toolbar->AddSeparator();
	//Icon für die Analysedatenübersicht laden
	wxImage overview_icon(
			wxString::FromUTF8(
					(data_directory + PATH_SEPARATOR + "icons" + PATH_SEPARATOR
							+ "overview_icon.png").c_str()), wxBITMAP_TYPE_PNG);
	overview_icon = overview_icon.Scale(toolbar->GetToolBitmapSize().x,
			toolbar->GetToolBitmapSize().y, wxIMAGE_QUALITY_HIGH);
	toolbar->AddTool(ID_ANALYZE, overview_icon, wxT("Analysedatenübersicht"));
	//Icon für Analyse an einem Punkt laden
	wxImage analyze_point_icon(
			wxString::FromUTF8(
					(data_directory + PATH_SEPARATOR + "icons" + PATH_SEPARATOR
							+ "analyze_point.png").c_str()),
			wxBITMAP_TYPE_PNG);
	analyze_point_icon = analyze_point_icon.Scale(
			toolbar->GetToolBitmapSize().x, toolbar->GetToolBitmapSize().y,
			wxIMAGE_QUALITY_HIGH);
	toolbar->AddTool(ID_ANALYZE_POINT, analyze_point_icon,
			wxT("Punkt analysieren"));
	//Icon für die Berechnung einer 2D-Temperaturverteilung laden
	wxImage render_cut_icon(
			wxString::FromUTF8(
					(data_directory + PATH_SEPARATOR + "icons" + PATH_SEPARATOR
							+ "cut_icon.png").c_str()), wxBITMAP_TYPE_PNG);
	render_cut_icon = render_cut_icon.Scale(toolbar->GetToolBitmapSize().x,
			toolbar->GetToolBitmapSize().y, wxIMAGE_QUALITY_HIGH);
	toolbar->AddTool(ID_RENDER_CUT, render_cut_icon,
			wxT("2D-Temperaturverteilung berechnen"));

	toolbar->Realize();

	//Toolbar mit dem Programmfenster verknüpfen
	SetToolBar(toolbar);

	//Programmicon laden
#if defined (__WIN32__)
	SetIcon(wxICON(program_icon));
#else
	SetIcon(wxIcon(wxString::FromUTF8(data_directory.c_str())
		+ wxT("icons/prgm-icon.png")));
#endif

	//Fenster auf dem Bildschirm zentrieren
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

	wxGetApp().addObject(obj);
	//als aktives Objekt setzen
	setActiveObject(wxGetApp().getDataObjects()->size() - 1);
}

void GUIMainWindow::setActiveObject(int index) {

	wxGetApp().setCurrentDataObjectIndex(index);

	//Oberfläche aktualisieren
	gl_context->setRenderObject(wxGetApp().getActiveObject());
	propbox->setCurrentMaterial(0);
	updateObjectPropGUI();
	updateViewPropGUI();
}

void GUIMainWindow::OnActiveObjectDelete(wxCommandEvent &event) {

	//Liegen mehrere Objekte vor?
	if (wxGetApp().getDataObjects()->size() > 1) {
		//Objekt löschen
		wxGetApp().removeCurrentObject();
		//Neues auswählen
		setActiveObject(wxGetApp().getCurrentDataObjectIndex());
	} else {
		wxMessageBox(
				wxT("Das aktuelle Objekt ist das Einzige, kann also nicht gelöscht werden!"),
				wxT("Fehler"), wxICON_ERROR);
	}
}

void GUIMainWindow::OnResize(wxSizeEvent &event) {
	//Das 3D-Fenster aktualisieren
	gl_context->SetSize(VIEWBOXWIDTH, 0,
			GetSize().x - PROPBOXWIDTH - VIEWBOXWIDTH, GetSize().y, 0);

	//Objekteigenschaften repositionieren
	propbox->SetSize(propbox->GetPosition().x, propbox->GetPosition().y,
			PROPBOXWIDTH - 10, 0, 0);
	propbox->resize();
	//Größe des Scrollfensters für die Objekteigenschaften anpassen
	prop_scroll_win->SetSize(GetSize().x - PROPBOXWIDTH + 5, 0,
			PROPBOXWIDTH - 10, GetSize().y - 25, 0);
	prop_scroll_win->SetVirtualSize(propbox->GetSize().x,
			propbox->GetSize().y + 30);

	//Visualisierungsoptionen repositionieren
	viewbox->SetSize(5, 0, VIEWBOXWIDTH - 10, GetSize().y - 25, 0);
	viewbox->resize();
	//Größe des Scrollfensters für die Visualisierungsoptionen anpassen
	view_scroll_win->SetSize(5, 0, VIEWBOXWIDTH - 10, GetSize().y - 25, 0);
	view_scroll_win->SetVirtualSize(viewbox->GetSize().x,
			viewbox->GetSize().y + 30);

}

void GUIMainWindow::OnSDTimelineChange(wxCommandEvent &event) {
	//gleiches Update wie für alle Attribute
	OnGeneralPropChange(event);

	//Checkbox für das Markieren des Zeitpunkts aktualisieren
	updating = true;
	propbox->getAnalyzeMarkerCheckBox()->SetValue(
			propbox->getSdTimeline()->isMarked(
					propbox->getSdTimeline()->getValue()));
	updating = false;
}

void GUIMainWindow::OnSensorDataChange(wxCommandEvent &event) {
	//gleiches Update wie für alle Attribute
	OnGeneralPropChange(event);

	//aktualisieren der Objekteigenschaftenoberfläche (evtl. durch einblenden der Timeline nötig)
	propbox->resize();
	prop_scroll_win->SetVirtualSize(propbox->GetSize().x,
			propbox->GetSize().y + 50);
}

void GUIMainWindow::OnSDTLMarkerClear(wxCommandEvent &event) {
	//löschen aller markierten Stellen
	propbox->getSdTimeline()->clearMarkers();

	//Checkbox für das Markieren des Zeitpunkts aktualisieren
	updating = true;
	propbox->getAnalyzeMarkerCheckBox()->SetValue(false);
	updating = false;
}

void GUIMainWindow::OnAnalyzeMarkerChange(wxCommandEvent &event) {
	//dürfen Events verarbeitet werden?
	if (!updating) {
		//markieren / demarkieren des Zeitpunkts
		int val = propbox->getSdTimeline()->getValue();
		propbox->getSdTimeline()->setMarked(val,
				propbox->getAnalyzeMarkerCheckBox()->GetValue());
	}
}

void GUIMainWindow::OnFindMaxTSD(wxCommandEvent &event) {

	//ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		//aktuelles Objekt
		ObjectData* obj = wxGetApp().getActiveObject();

		//Ist die Temperaturverteilung aktuell?
		if (obj->getCurrentSensorIndex()
				!= propbox->getSensorDataList()->GetSelection()) {
			wxMessageBox(
					wxT("Das Objekt wurde noch nicht neu berechnet!\nBitte berechnen sie das Objekt neu, um die Maximumssuche fortzusetzen!"),
					wxT("Fehler"));
			return;
		}

		//zu verwendende Methode erfragen
		wxMessageDialog dlg(this,
				wxT("Nur Messwertdurchschnitt verwenden? (schneller)"),
				wxT("Schnelle Methode verwenden?"),
				wxYES_NO | wxYES_DEFAULT | wxCANCEL);
		int res = dlg.ShowModal();

		//Soll fortgefahren werden?
		if (res != wxID_CANCEL) {
			propbox->getSdTimeline()->findMaxValue(obj, (res == wxID_YES));
		}
	}
}

void GUIMainWindow::OnSDTLNextMarker(wxCommandEvent &event) {

	//indices aller markierten Zeitpunkte, ist aufsteigend sortiert
	vector<int>* markers = propbox->getSdTimeline()->getMarkers();

	//Sind marker gesetzt?
	if (markers->size() == 0) {
		wxMessageBox(wxT("Es sind keine Analyze-Marker gesetzt!"),
				wxT("Hinweis"), wxICON_INFORMATION);
	} else {
		//nächsten marker suchen
		for (size_t i = 0; i < markers->size(); i++) {
			if (markers->at(i) > propbox->getSdTimeline()->getValue()) {
				propbox->getSdTimeline()->setValue(markers->at(i));
				break;
			}
		}
	}
}

void GUIMainWindow::OnSDTLPrevMarker(wxCommandEvent &event) {

	//indices aller markierten Zeitpunkte, ist aufsteigend sortiert
	vector<int>* markers = propbox->getSdTimeline()->getMarkers();

	//Sind marker gesetzt?
	if (markers->size() == 0) {
		wxMessageBox(wxT("Es sind keine Analyze-Marker gesetzt!"),
				wxT("Hinweis"), wxICON_INFORMATION);
	} else {
		//vorherigen marker suchen
		for (size_t i = 0; i < markers->size(); i++) {

			//ist die markierte Stelle hinter/an dem aktuell ausgewählte Zeitpunkt?
			if (markers->at(i) >= propbox->getSdTimeline()->getValue()) {
				//gibt es eine merkierte Stelle davor?
				if (i > 0) {
					//vorherige Stelle verwenden
					propbox->getSdTimeline()->setValue(markers->at(i - 1));
				} else {
					//erste Stelle verwenden
					propbox->getSdTimeline()->setValue(markers->at(0));
				}

				break;
			}

			//Sind alle markiereten Stellen vor dem aktuellen Zeitpunkt?
			if (i == markers->size() - 1) {
				//die naheste verwenden
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

void GUIMainWindow::assignCurrentObjectProps() {

	//Ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		//aktives Objekt
		ObjectData* obj = wxGetApp().getActiveObject();
		//Punkt als Dezimaltrennzeichen verwenden
		setlocale(LC_NUMERIC, "C");

		//Übertragen der Objekt- und Materialeigenschaften aus der Oberfläche in das Objekt
		obj->setName(string(propbox->getObjNameEdit()->GetValue().ToAscii()));
		obj->setMaxvolume(
				atof(propbox->getMaxVolumeEdit()->GetValue().ToAscii()));
		obj->setQuality(atof(propbox->getQualityEdit()->GetValue().ToAscii()));
		obj->setCurrentSensorIndex(
				propbox->getSensorDataList()->GetSelection());
		SensorData* sd = &obj->getSensorDataList()->at(
				propbox->getSensorDataList()->GetSelection());
		if (sd->timed) {
			sd->current_time_index = propbox->getSdTimeline()->getValue();
		}
		ObjectData::MaterialData* mat = &obj->getMaterials()->at(
				propbox->getCurrentMaterial());
		mat->name = propbox->getMatNameEdit()->GetValue().ToAscii();
		mat->interpolation_mode =
				(Interpolator::InterpolationMode) propbox->getInterpolationModeList()->GetSelection();
		mat->density = atof(propbox->getDensityEdit()->GetValue().ToAscii());
		mat->specificheatcapacity = atof(
				propbox->getSpecificHeatCapEdit()->GetValue().ToAscii());
	}
}

void GUIMainWindow::updateObjectPropGUI() {

	//Ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		//Es werden keine Events bezüglich der Oberfläche verarbeitet werden
		updating = true;
		//aktives Objekt
		ObjectData* obj = wxGetApp().getActiveObject();
		//Punkt als Dezimaltrennzeichen verwenden
		setlocale(LC_NUMERIC, "C");

		//Übertragen der Objekt- und Materialeigenschaften in die GUI.
		propbox->SetLabel(
				wxString::FromAscii(
						(obj->getName() + " - Objekteigenschaften:").c_str()));
		propbox->getObjNameEdit()->SetValue(
				wxString::FromAscii(obj->getName().c_str()));
		propbox->getMaxVolumeEdit()->SetValue(
				wxString::FromAscii(floattostr(obj->getMaxvolume()).c_str()));
		propbox->getQualityEdit()->SetValue(
				wxString::FromAscii(floattostr(obj->getQuality()).c_str()));
		propbox->getSensorDataList()->Clear();
		for (unsigned int i = 0; i < obj->getSensorDataList()->size(); i++) {
			propbox->getSensorDataList()->Insert(
					wxString::FromAscii(
							obj->getSensorDataList()->at(i).name.c_str()), i);
		}
		propbox->getSensorDataList()->SetSelection(
				obj->getCurrentSensorIndex());
		propbox->getMatListBox()->Clear();
		for (unsigned int i = 0; i < obj->getMaterials()->size(); i++) {
			propbox->getMatListBox()->Insert(
					wxString::FromAscii(
							obj->getMaterials()->at(i).name.c_str()), i);
		}
		propbox->getMatListBox()->SetSelection(propbox->getCurrentMaterial());
		ObjectData::MaterialData* mat = &obj->getMaterials()->at(
				propbox->getCurrentMaterial());
		propbox->getMatPropBox()->SetLabel(
				wxString::FromAscii(
						(mat->name + " - Materialeigenschaften").c_str()));

		propbox->getInterpolationModeList()->SetSelection(
				mat->interpolation_mode);
		propbox->getDensityEdit()->SetValue(
				wxString::FromAscii(floattostr(mat->density).c_str()));
		propbox->getSpecificHeatCapEdit()->SetValue(
				wxString::FromAscii(
						floattostr(mat->specificheatcapacity).c_str()));
		propbox->getMatNameEdit()->SetValue(
				wxString::FromAscii(mat->name.c_str()));
		propbox->resize();
		updating = false;

		//Oberfläche ist aktuell
		propbox->getUpToDateLbl()->Hide();

		//Anzeige des aktiven Objekts aktualisieren
		int nPos = toolbar->GetToolPos(ID_CHANGE_ACTIVE_OBJ);
		wxToolBarToolBase* pTool = toolbar->RemoveTool(ID_CHANGE_ACTIVE_OBJ);
		pTool->SetLabel(
				wxString::FromAscii(
						("aktives Objekt: " + obj->getName()).c_str()));
		toolbar->InsertTool(nPos, pTool);
		toolbar->Realize();
	}
}

void GUIMainWindow::OnMaterialSelect(wxCommandEvent &event) {

	//dürfen Events verarbeitet werden?
	if (!updating) {
		propbox->setCurrentMaterial(propbox->getMatListBox()->GetSelection());
		//Objekteigenschaften-Oberfläche aktualisieren
		updateObjectPropGUI();
	}
}

void GUIMainWindow::OnRecalcBtClick(wxCommandEvent& event) {

	//Ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		//das aktive Objekt
		ObjectData* obj = wxGetApp().getActiveObject();
		//Speichern der Objekteigenschaften
		assignCurrentObjectProps();
		//neuberechnen der Temperaturverteilung
		obj->calculateIO();
		//aktualisieren des 3D-Fensters
		gl_context->refresh();

		//Objekteigenschaften aktuell
		propbox->getUpToDateLbl()->Hide();

		//evtl. Analysedatenübersicht aktualisieren
		if (analyze_window_valid) {
			analyzerframe->Update();
		}
	}
}

void GUIMainWindow::OnAnalyze(wxCommandEvent &event) {

	//ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		//Ist das Analysedatenübersichtsfenster noch nicht offen?
		if (!analyze_window_valid) {
			//erstellen des Analysedatenübersichtsfensters
			analyzerframe = new GUIAnalyzeOutputWindow(this,
					wxT("Analysedaten"), 100, 100, 800, 300);
			analyzerframe->Show(true);
			//Status vermerken
			analyze_window_valid = true;
		}
	}
}

void GUIMainWindow::OnAnalyzePoint(wxCommandEvent &event) {

	//ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		//Punktanalysefenster öffnen
		GUIAnalyzePointWindow* analyzewin = new GUIAnalyzePointWindow(this,
				wxT("Analyse an Punkt"), 100, 100, 350, 120);
		analyzewin->Show();
	} else {
		wxMessageBox(wxT("Es ist kein Objekt geladen!"), wxT("Fehler"));
	}
}

void GUIMainWindow::OnRenderCut(wxCommandEvent &event) {

	//ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		//Ist das Fenster für die 2D-Temperaturverteilung offen?
		if (!render_cut_window_valid) {
			//öffnen des Fensters für die 2D-Temperaturverteilung
			rendercutwindow = new GUICutRenderWindow(this,
					wxT("Schnitt brechnen"), 100, 100, 800, 600);
			rendercutwindow->Show(true);
			//Status vermerken
			render_cut_window_valid = true;
			//3D-Fenster aktualisieren
			gl_context->Refresh(false, NULL);
		}
	}
}

void GUIMainWindow::OnImmediateUpdatePropChange(wxCommandEvent &event) {

	//dürfen Events verarbeitet werden?
	if (!updating) {
		//Sofortiges Speichern der Objekteigenschaften und Aktualisieren der Oberfläche
		propbox->getUpToDateLbl()->Show();
		assignCurrentObjectProps();
		updateObjectPropGUI();
		updateViewPropGUI();
	}
}

void GUIMainWindow::OnGeneralPropChange(wxCommandEvent &event) {

	//dürfen Events verarbeitet werden?
	if (!updating) {
		//vermerk zu Eingenschaftenänderung
		propbox->getUpToDateLbl()->Show();

		//automatische Neuberechnung?
		if (propbox->getAutoUpdateCeckBox()->IsChecked()) {
			OnRecalcBtClick(event);
		}
	}
}

void GUIMainWindow::assignViewProps() {

	//ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		//Die Rendereinstellungen des 3D-Fensters
		Renderer::Viewport_info* view =
				gl_context->getRenderer()->getViewport();

		//Übertragen der Visualisierungsoptionen und Rendereinstellungen aus der GUI in die Datenhaltung
		view->showPoints =
				(Renderer::RenderMode) viewbox->getPointsCheckBox()->GetSelection();
		view->showEdges =
				(Renderer::RenderMode) viewbox->getEdgesCheckBox()->GetSelection();
		view->showFaces =
				(Renderer::RenderMode) viewbox->getFacesCheckBox()->GetSelection();
		view->show_extrapolated =
				viewbox->getShowExtrapolatedCheckBox()->IsChecked();
		view->show_sensordata = viewbox->getShowShowSensorData()->IsChecked();

		//Die Visualisierungsoptionen
		Visualization_info* vis_info = wxGetApp().getVisualizationInfo();
		vis_info->min_visualisation_temp =
				viewbox->getColorRangeMinEdit()->GetValue();
		vis_info->max_visualisation_temp =
				viewbox->getColorRangeMaxEdit()->GetValue();
		view->scale = atof(viewbox->getViewScaleEdit()->GetValue().ToAscii());

		//Die Materialsichtbarkeit ist auf das Objekt bezogen
		for (unsigned int i = 0;
				i < wxGetApp().getActiveObject()->getMaterials()->size(); i++) {
			ObjectData::MaterialData* mat =
					&wxGetApp().getActiveObject()->getMaterials()->at(i);
			mat->visible = viewbox->getMatVisibilityListBox()->IsChecked(i);
		}
	}
}

void GUIMainWindow::updateViewPropGUI() {

	//ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex() > -1) {
		//Es werden keine Events bezüglich der Oberfläche verarbeitet werden
		updating = true;
		//Die Rendereinstellungen des 3D-Fensters
		Renderer::Viewport_info* view =
				gl_context->getRenderer()->getViewport();

		//Übertragen der Visualisierungsoptionen und Rendereinstellungen aus der Datenhaltung in die GUI
		viewbox->getShowExtrapolatedCheckBox()->SetValue(
				view->show_extrapolated);
		viewbox->getShowShowSensorData()->SetValue(view->show_sensordata);
		viewbox->getPointsCheckBox()->SetSelection(view->showPoints);
		viewbox->getEdgesCheckBox()->SetSelection(view->showEdges);
		viewbox->getFacesCheckBox()->SetSelection(view->showFaces);

		//Die Visualisierungsoptionen
		Visualization_info* vis_info = wxGetApp().getVisualizationInfo();
		viewbox->getColorRangeMinEdit()->SetValue(
				vis_info->min_visualisation_temp);
		viewbox->getColorRangeMaxEdit()->SetValue(
				vis_info->max_visualisation_temp);
		viewbox->getViewScaleEdit()->SetValue(floattowxstr(view->scale));

		//Die Materialsichtbarkeit ist auf das Objekt bezogen
		viewbox->getMatVisibilityListBox()->Clear();

		for (unsigned int i = 0;
				i < wxGetApp().getActiveObject()->getMaterials()->size(); i++) {
			ObjectData::MaterialData* mat =
					&wxGetApp().getActiveObject()->getMaterials()->at(i);
			viewbox->getMatVisibilityListBox()->Insert(
					wxString::FromAscii(mat->name.c_str()), i);
			viewbox->getMatVisibilityListBox()->Check(i, mat->visible);
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

void GUIMainWindow::OnMenuImportObj(wxCommandEvent &event) {

	//Dialog zur Dateiauswahl
	wxFileDialog *OpenDialog = new wxFileDialog(this, wxT("Datei öffnen..."),
			_(""), _(""), _("Wavefront-Objektdateien (*.obj)|*.obj"),
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	//Öffnen bestätigt?
	if (OpenDialog->ShowModal() == wxID_OK) {

		//Versuch, ein neues Objekt zu Laden
		ObjectData* newobj = new ObjectData();
		wxString path = OpenDialog->GetPath();
		int status = newobj->loadFromFile(path);

		//Fehler auswerten
		switch (status) {
		case ObjectData::OD_LOAD_ALREADY_LOADED:
			wxMessageBox(path + wxT(" ist bereits geöffnet!"));
			delete newobj;
			break;
		case ObjectData::OD_LOAD_INVALID_FILE:
			wxMessageBox(path + wxT(" ist keine Wavefront(.obj)-Datei."));
			delete newobj;
			break;
		case ObjectData::OD_LOAD_INVALID_SENSOR_FILE:
			wxMessageBox(
					wxT("Sensordaten konnten nicht geladen werden: ")
							+ (path.BeforeLast('.') + wxT(".(t)sd")));
			delete newobj;
			break;
		case ObjectData::OD_SUCCESS:
			//Objekt im Propgramm aufnehmen
			addObject(newobj);
			break;
		default:
			cout << "undefined status value!" << endl;
			delete newobj;
			break;
		}

		//aktualisieren der Oberfläche
		propbox->setCurrentMaterial(0);
		updateObjectPropGUI();
		updateViewPropGUI();
		propbox->getUpToDateLbl()->Hide();
	}

	OpenDialog->Close();
	OpenDialog->Destroy();
}

void GUIMainWindow::OnMenuImportSD(wxCommandEvent &event) {

	//Dialog zur Dateiauswahl
	wxFileDialog *OpenDialog = new wxFileDialog(this, wxT("Datei öffnen..."),
			_(""), _(""), _("Sensordaten (*.sd)|*.sd"), wxFD_OPEN);

	//Öffnen bestätigt?
	if (OpenDialog->ShowModal() == wxID_OK) {
		//Laden der Sensordaten und verknüpfen mit dem Objekt
		ObjectData* obj = wxGetApp().getActiveObject();
		wxString path = OpenDialog->GetPath();
		obj->addSensorData(path);

		//aktualisieren der Oberfläche
		updateObjectPropGUI();
	}

	OpenDialog->Close();
	OpenDialog->Destroy();
}

void GUIMainWindow::OnMenuImportTSD(wxCommandEvent &event) {

	//Dialog zur Dateiauswahl
	wxFileDialog *OpenDialog = new wxFileDialog(this, wxT("Datei öffnen..."),
			_(""), _(""), _("Sensordaten-Pakete (*.tsd)|*.tsd"),
			wxFD_OPEN);

	//Öffnen bestätigt?
	if (OpenDialog->ShowModal() == wxID_OK) {
		//Laden der Sensordaten und verknüpfen mit dem Objekt
		ObjectData* obj = wxGetApp().getActiveObject();
		wxString path = OpenDialog->GetPath();
		obj->addTimedData(path);

		//aktualisieren der Oberfläche
		updateObjectPropGUI();
	}
	OpenDialog->Close();
	OpenDialog->Destroy();
}

void GUIMainWindow::OnExportViewportImage(wxCommandEvent &event) {

	//Ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex()<0) {
		cerr << "no object loaded!" << endl;
		return;
	}

	//Dialog zur Dateiauswahl
	wxFileDialog *SaveDialog = new wxFileDialog(this, wxT("Export nach..."),
			_(""), _(""), _("Portable Network Graphics (*.png)|*.png"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	//Speichern bestätigt?
	if (SaveDialog->ShowModal() == wxID_OK) {
		//auslesen der 3D-Fensteranzeige
		gl_context->SetCurrent();
		wxImage* img = gl_context->getRenderer()->getViewportImage();

		//Grafik in die Datei speichern
		img->SaveFile(SaveDialog->GetPath());

		img->Destroy();
	}

	SaveDialog->Close();
	SaveDialog->Destroy();
}

void GUIMainWindow::OnExportVTK(wxCommandEvent &event) {

	//Ist ein Objekt geladen?
	if (wxGetApp().getCurrentDataObjectIndex()<0) {
		cerr << "no object loaded!" << endl;
		return;
	}

	//Dialog zur Dateiauswahl
	wxFileDialog *SaveDialog = new wxFileDialog(this, wxT("Export nach..."),
			_(""), _(""), _("Visualization Toolkit file(*.vtk)|*.vtk"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	//Speichern bestätigt?
	if (SaveDialog->ShowModal() == wxID_OK) {
		//Export des Objekts
		Exporter exporter;
		exporter.ExportLegacyVTK(string(SaveDialog->GetPath().ToUTF8().data()),
				wxGetApp().getActiveObject());
	}

	SaveDialog->Close();
	SaveDialog->Destroy();
}

void GUIMainWindow::OnActiveObjectChangePopup(wxCommandEvent &event) {
	if (event.GetId() != 0) {
		//Die event-ID enthält den ausgewählten Menüpunkt
		setActiveObject(event.GetId() - wxID_HIGHEST - 1);
	}
}

void GUIMainWindow::OnActiveObjectChange(wxCommandEvent &event) {

	//Popupmenü aller geladenen Objekte
	wxMenu mnu;
	mnu.Append(0, wxT("verfügbare Objekte:"));
	mnu.AppendSeparator();

	//Für alle geladenen Objekte...
	for (unsigned int i = 0; i < wxGetApp().getDataObjects()->size(); i++) {
		//Hinzufügen des Objektnamens, ausgewähltes Objekt später an ID des Menüpunkts erkennbar
		mnu.Append(wxID_HIGHEST + i + 1,
				wxString::FromAscii(
						wxGetApp().getDataObjects()->at(i)->getName().c_str()));
	}

	//Mit dem Event für die Auswahl eines Objekts verknüpfen
	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED,
			(wxObjectEventFunction) &GUIMainWindow::OnActiveObjectChangePopup,
			NULL, this);

	//Anzeigen des Auswahlmenüs
	PopupMenu(&mnu);
}

void GUIMainWindow::OnMenuFileQuit(wxCommandEvent &event) {
	//Programm schließen
	Close();
}

void GUIMainWindow::OnMenuOpenManual(wxCommandEvent &event) {

#if defined (__WIN32__)
	ShellExecute(NULL, NULL, wxString::FromUTF8(
					(data_directory + PATH_SEPARATOR
							+ "simpleanalyzer-man.pdf").c_str()).wc_str(),
								NULL, NULL, SW_SHOWNORMAL);
#else
	system(
			string(
					"xdg-open " + data_directory + PATH_SEPARATOR
							+ "simpleanalyzer-man.pdf\n").c_str());
#endif
}

void GUIMainWindow::OnMenuHelpAbout(wxCommandEvent &event) {
	wxMessageBox(wxT("Simple Analyzer\n©2013-2014 by Valentin Roland"));
}

GUIMainWindow::~GUIMainWindow() {
}


/*
 * GUIAnalyzeOutputWindow.cpp
 *
 *  Created on: 12.09.2013
 *      Author: Valentin Roland
 */

#include "GUIAnalyzeOutputWindow.h"
#include "GUIMainWindow.h"
#include <iostream>
#include <sstream>
#include "../SimpleAnalyzerApp.h"
#include "../processing/ObjectData.h"
#include "../processing/Analyzer.h"
#include <wx/clipbrd.h>
using namespace std;

//Eventtabelle zum Verknüpfen der Events
BEGIN_EVENT_TABLE(GUIAnalyzeOutputWindow, wxFrame)
EVT_KEY_DOWN(GUIAnalyzeOutputWindow::OnKeyPress)
END_EVENT_TABLE()

GUIAnalyzeOutputWindow::GUIAnalyzeOutputWindow(wxWindow * parent,
		const wxChar *title, int xpos, int ypos, int width, int height) :
		wxFrame(parent, -1, title, wxPoint(xpos, ypos), wxSize(width, height),
		wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT) {

	//Format für die Zahlendarstellung auf Punkt als Dezimaltrennzeichen setzen
	setlocale(LC_NUMERIC, "C");
	//erstellen und positionieren der Tabelle, wird automatisch in der Größe an das Fenster angepasst
	table = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(600, 300));
	table->SetSize(0, 0, width, height, 0);
	//aktualisieren der Tabelleninhalte
	Update();
}

void GUIAnalyzeOutputWindow::Update() {

	//zurücksetzen der Tabelle
	table->ClearGrid();
	table->DeleteRows(0, table->GetRows(), true);
	table->DeleteCols(0, table->GetCols(), true);
	table->CreateGrid(0, 0);

	//Erstellen der Zeilenbezeichnungen
	table->SetRowLabelValue(0, wxT("Objekt:"));
	table->SetRowLabelValue(1, wxT("Sensordatensatz:"));
	table->SetRowLabelValue(2, wxT("Volumen in m³:"));
	table->SetRowLabelValue(3, wxT("Energie in kJ:"));
	table->SetRowLabelValue(4, wxT("Material:"));
	table->SetRowLabelValue(5, wxT("Volumen in m³:"));
	table->SetRowLabelValue(6, wxT("Energie in kJ:"));

	//Setzen der Tabelleneinstellungen
	table->SetRowLabelSize(150);
	table->SetColLabelSize(0);
	table->SetDefaultColSize(100, true);

	//Anzahl der dargestellten Objekt- und Materialeigenschaften
	const int OBJECT_PROPERTY_COUNT = 3;
	const int MATERIAL_PROPERTY_COUNT = 3;

	//erstellen der Zeilen der Tabelle
	table->AppendRows(OBJECT_PROPERTY_COUNT + MATERIAL_PROPERTY_COUNT + 1,
			true);

	//Breite der Tabelle in Zellen (für Zellenindices benötigt)
	int all_mat_cell_count = 0;

	//Für alle Objekte...
	for (unsigned int o = 0; o < wxGetApp().getDataObjects()->size(); o++) {
		//aktuelles Objekt
		ObjectData* obj = wxGetApp().getDataObjects()->at(o);
		//Objekt zum speichern der Analysedaten
		Analyzer::AnalyzerData_object data;

		//Analysieren des Objekts
		Analyzer analyzer;
		analyzer.analyzeObject(wxGetApp().getDataObjects()->at(o), &data);

		//Anzahl der Materialien des Objekts
		int matcount = obj->getMaterials()->size();
		//Anzahl der Sensordatensätze des Objekts
		int sdcount = data.data_sets.size();

		//Die benötigten Spalten hinzufügen
		table->AppendCols(sdcount * matcount, true);

		//Die Spalte für den Objektnamen auf volle Breite vergrößern
		table->SetCellSize(0, all_mat_cell_count, 1, matcount * sdcount);
		//Objektnamen eintragen
		table->SetCellValue(0, all_mat_cell_count,
				wxString::FromAscii(obj->getName().c_str()));
		// Objektnamen zentrieren
		table->SetCellAlignment(wxALIGN_CENTRE, 0, all_mat_cell_count);

		//Anzahl der bereits mit Materialdaten gefüllten Zellen
		int materialcells = 0;

		//Für alle Sensordatensätze
		for (int s = 0; s < sdcount; s++) {
			//Analysedaten für einen Datensatz
			Analyzer::AnalyzerData_dataset* data_set = &data.data_sets.at(s);

			//Zellen für den Datensatznamen über alle Materialzellen vergrößern
			for (int i = 0; i < OBJECT_PROPERTY_COUNT; i++) {
				table->SetCellSize(i + 1, all_mat_cell_count + s * matcount, 1,
						matcount);
			};

			//Den Namen des Datensatzes eintragen
			table->SetCellValue(1, all_mat_cell_count + s * matcount,
					wxString::FromAscii(data_set->name.c_str()));
			//Den Namen des Datensatzes zentrieren
			table->SetCellAlignment(wxALIGN_CENTRE, 1,
					all_mat_cell_count + s * matcount);

			//Die Analyseergebnisse für den Sensordatensatz eintragen
			table->SetCellValue(2, all_mat_cell_count + s * matcount,
					wxString::FromAscii(floattostr(data.volume).c_str()));
			table->SetCellValue(3, all_mat_cell_count + s * matcount,
					wxString::FromAscii(
							floattostr(data_set->heat_energy).c_str()));

			//Für alle Materialien...
			for (int i = 0; i < matcount; i++) {
				//Analysedaten für einen Material
				Analyzer::AnalyzerData_material* mat = &data_set->mat_data.at(
						i);
				//Zelle Mitzählen
				materialcells++;
				//Materialnamen eintragen
				table->SetCellValue(OBJECT_PROPERTY_COUNT + 1,
						all_mat_cell_count + s * matcount + i,
						wxString::FromAscii(mat->name.c_str()));
				//Materialnamen zentrieren
				table->SetCellAlignment(wxALIGN_CENTRE,
						OBJECT_PROPERTY_COUNT + 1,
						all_mat_cell_count + s * matcount + i);

				//Die Analyseergebnisse für das Material eintragen
				table->SetCellValue(OBJECT_PROPERTY_COUNT + 2,
						all_mat_cell_count + s * matcount + i,
						wxString::FromAscii(floattostr(mat->volume).c_str()));
				table->SetCellValue(OBJECT_PROPERTY_COUNT + 3,
						all_mat_cell_count + s * matcount + i,
						wxString::FromAscii(
								floattostr(mat->heat_energy).c_str()));
			}
		}
		//Materialzellen mitzählen
		all_mat_cell_count += materialcells;
	}
}

void GUIAnalyzeOutputWindow::ToClipboard() {

	wxString copy_data;
	bool something_in_this_line;

	copy_data.Clear();

	//Kopieren der Tabelleninhalte
	for (int i = 0; i < table->GetRows(); i++) {

		something_in_this_line = false;
		for (int j = 0; j < table->GetCols(); j++) {

			if (table->IsInSelection(i, j)) {

				if (something_in_this_line == false) {

					if (copy_data.IsEmpty() == false) {
						copy_data.Append(wxT("\r\n")); // in windows if copy to notepad need \r\n to newline
					}

					something_in_this_line = true;
				} else {
					copy_data.Append(wxT("\t"));
				}

				copy_data = copy_data + table->GetCellValue(i, j);
			}
		}
	}

	//Schreiben in die Zwischenablage
	if (wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxTextDataObject(copy_data));
		wxTheClipboard->Close();
	}
}

void GUIAnalyzeOutputWindow::SelectAll() {

	//Alle Zeilen zur Selektion hinzufügen
	for (int i = 0; i < table->GetRows(); i++) {
		table->SelectRow(i, true);
	}
}

void GUIAnalyzeOutputWindow::OnKeyPress(wxKeyEvent& event) {

	//Kopieren der Tabelle in die Zwischenablage
	if ((event.GetUnicodeKey() == 'C') && (event.ControlDown() == true)) {
		ToClipboard();
	}

	//Selektieren aller Zellen
	if ((event.GetUnicodeKey() == 'A') && (event.ControlDown() == true)) {
		SelectAll();
	}
}

GUIAnalyzeOutputWindow::~GUIAnalyzeOutputWindow() {

	//Vermerken des Fensterschließens im Hauptfenster
	GUIMainWindow* parent = (GUIMainWindow*) GetParent();
	parent->setAnalyzeWindowStatus(false);
}

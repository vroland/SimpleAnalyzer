/*
 * GUIAnalyzeOutputWindow.cpp
 *
 *  Created on: 12.09.2013
 *      Author: valentin
 */

#include "GUIAnalyzeOutputWindow.h"
#include "GUIMainWindow.h"
#include <iostream>
#include <sstream>
#include "../processing/ObjectData.h"
#include "../processing/Analyzer.h"
using namespace std;

extern std::vector<ObjectData*> data_objects;

string ftostr(double val) {
	ostringstream ss;
	ss << val;
	return ss.str();
}
GUIAnalyzeOutputWindow::GUIAnalyzeOutputWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height):
wxFrame(parent, -1, title, wxPoint(xpos, ypos), wxSize(width, height), wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT) {
	setlocale(LC_NUMERIC, "C");
	table = new wxGrid( this, wxID_ANY, wxPoint( 0, 0 ), wxSize( 600, 300 ) );
	table->SetSize(0,0,width,height,0);
	Update();
}
void GUIAnalyzeOutputWindow::Update() {
	table->ClearGrid();
	table->DeleteRows(0,table->GetRows(), true);
	table->DeleteCols(0,table->GetCols(), true);
	table->CreateGrid(0,0);
	table->SetRowLabelValue(0,wxT("Objekt:"));
	table->SetRowLabelValue(1,wxT("Sensordatensatz:"));
	table->SetRowLabelValue(2,wxT("Volumen in m³:"));
	table->SetRowLabelValue(3,wxT("Energie in kJ:"));
	table->SetRowLabelValue(4,wxT("Material:"));
	table->SetRowLabelValue(5,wxT("Volumen in m³:"));
	table->SetRowLabelValue(6,wxT("Energie in kJ:"));
	table->SetRowLabelSize(150);
	table->SetColLabelSize(0);
	table->SetDefaultColSize(100,true);
	int obj_prop_count = 3;
	int material_prop_cout = 3;
	int all_sd_count = 0;
	int all_mat_cell_count = 0;
	table->AppendRows(obj_prop_count+material_prop_cout+1,true);
	for (unsigned int o=0;o<data_objects.size();o++) {
		ObjectData* obj = data_objects.at(o);
		Analyzer::AnalyzerData_object data;
		Analyzer analyzer;
		analyzer.analyzeObject(data_objects.at(o),&data);
		int matcount = obj->materials.size();
		int sdcount  = data.data_sets.size();
		table->AppendCols(sdcount*matcount,true);
		table->SetCellSize(0,all_mat_cell_count , 1, matcount*sdcount);
		table->SetCellValue(0,all_mat_cell_count,wxString::FromAscii(obj->name.c_str()));
		table->SetCellAlignment(wxALIGN_CENTRE,0,all_mat_cell_count);
		int materialcells = 0;
		for (int s=0;s<sdcount;s++) {
			Analyzer::AnalyzerData_dataset* data_set = &data.data_sets.at(s);
			for (int i=0;i<obj_prop_count;i++) {
				table->SetCellSize(i+1,all_mat_cell_count+s*matcount , 1, matcount);
			};
			table->SetCellValue(1,all_mat_cell_count+s*matcount,wxString::FromAscii(data_set->name.c_str()));
			table->SetCellAlignment(wxALIGN_CENTRE,1,all_mat_cell_count+s*matcount);
			table->SetCellValue(2,all_mat_cell_count+s*matcount,wxString::FromAscii(ftostr(data.volume).c_str()));
			table->SetCellValue(3,all_mat_cell_count+s*matcount,wxString::FromAscii(ftostr(data_set->heat_energy).c_str()));

			for (int i=0;i<matcount;i++) {
				Analyzer::AnalyzerData_material* mat = &data_set->mat_data.at(i);
				materialcells++;
				table->SetCellValue(obj_prop_count+1,all_mat_cell_count+s*matcount+i,wxString::FromAscii(mat->name.c_str()));
				table->SetCellAlignment(wxALIGN_CENTRE,obj_prop_count+1,all_mat_cell_count+s*matcount+i);
				table->SetCellValue(obj_prop_count+2,all_mat_cell_count+s*matcount+i,wxString::FromAscii(ftostr(mat->volume).c_str()));
				table->SetCellValue(obj_prop_count+3,all_mat_cell_count+s*matcount+i,wxString::FromAscii(ftostr(mat->heat_energy).c_str()));
			}
		}
		all_mat_cell_count+=materialcells;
		all_sd_count += sdcount;
	}
}
GUIAnalyzeOutputWindow::~GUIAnalyzeOutputWindow() {
	GUIMainWindow* parent = (GUIMainWindow*) GetParent();
	parent->setAnalyzeWindowStatus(false);
}

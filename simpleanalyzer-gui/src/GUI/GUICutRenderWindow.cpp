/*
 * GUICutRenderWindow.cpp
 *
 *  Created on: 01.10.2013
 *      Author: valentin
 */
#include "GUICutRenderWindow.h"
#include "constants.h"
#include <vector>
#include "../SimpleAnalyzerApp.h"
#include "../processing/Analyzer.h"
#include "../processing/ObjectData.h"
#include "../processing/utils.h"
#include "../libraries/interpolate/Interpolator.h"
#include "../libraries/tetgen/tetgen.h"
#include "../fileIO/Exporter.h"
#include "GUIMainWindow.h"
#include <thread>
#include <sys/time.h>
#include <wx/graphics.h>

//Eventtabelle zum Verknüpfen der Events
BEGIN_EVENT_TABLE(GUICutRenderWindow, wxFrame)
	EVT_BUTTON(ID_RENDER_CUT_BT, GUICutRenderWindow::renderCutBtClick)
	EVT_BUTTON(ID_EXPORT_CUT_IMG_BT, GUICutRenderWindow::OnExportImage)
	EVT_BUTTON(ID_EXPORT_CUT_CSV_BT, GUICutRenderWindow::OnExportCSV)
	EVT_TEXT(ID_CUT_TRI_EDIT, GUICutRenderWindow::OnCutPropsChanged)
	EVT_SPINCTRL(ID_CUT_TRI_EDIT, GUICutRenderWindow::OnSCutPropsChanged_spin)
	EVT_SIZE(GUICutRenderWindow::OnResize)
	EVT_COMBOBOX(ID_COLORSCALE_PROP,GUICutRenderWindow::OnColorScaleChanged)
	EVT_SPINCTRL(ID_COLORSCALE_PROP,GUICutRenderWindow::OnColorScaleChanged_spin)
	EVT_BUTTON(ID_COLORSCALE_COLORBT,GUICutRenderWindow::OnCSColorBtClick)
END_EVENT_TABLE()

GUICutRenderWindow::GUICutRenderWindow(wxWindow * parent, const wxChar *title,
		int xpos, int ypos, int width, int height) :
		wxFrame(parent, -1, title, wxPoint(xpos, ypos), wxSize(width, height),
				wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT) {

	//erstellen der Komponenten und setzen der Standardwerte
	canvas = new GUIRenderCutCanvas(this);
	scroll_pane = new wxScrolledWindow(this, wxID_ANY);
	scroll_pane->SetScrollRate(10, 10);
	trilabel = new wxStaticText(scroll_pane, wxID_ANY,
			wxT("Dreiecksebene (Punkt1 ist Mittelpunkt):"));
	p1label = new wxStaticText(scroll_pane, wxID_ANY, wxT("Punkt 1:"));
	p1label = new wxStaticText(scroll_pane, wxID_ANY, wxT("Punkt 1:"));
	p1xedit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);
	p1yedit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);
	p1zedit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);

	p2label = new wxStaticText(scroll_pane, wxID_ANY, wxT("Punkt 2:"));
	p2xedit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);
	p2yedit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);
	p2zedit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);

	p3label = new wxStaticText(scroll_pane, wxID_ANY, wxT("Punkt 3:"));
	p3xedit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);
	p3yedit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);
	p3zedit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);

	optionslbl = new wxStaticText(scroll_pane, wxID_ANY, wxT("Optionen:"));
	widthHeightlbl = new wxStaticText(scroll_pane, wxID_ANY,
			wxT("Breite/Höhe:"));
	imgWidthEdit = new wxSpinCtrl(scroll_pane, ID_CUT_TRI_EDIT);
	imgHeightEdit = new wxSpinCtrl(scroll_pane, ID_CUT_TRI_EDIT);
	mmperpixellabel = new wxStaticText(scroll_pane, wxID_ANY,
			wxT("Maßstab (mm/px):"));
	mmperpixeledit = new wxTextCtrl(scroll_pane, ID_CUT_TRI_EDIT);
	threadcountlbl = new wxStaticText(scroll_pane, wxID_ANY,
			wxT("CPU-Threads:"));
	core_count = thread::hardware_concurrency();
	threadcountedit = new wxSpinCtrl(scroll_pane, wxID_ANY);

	calcbt = new wxButton(scroll_pane, ID_RENDER_CUT_BT, wxT("Analysieren"));
	export_img_bt = new wxButton(scroll_pane, ID_EXPORT_CUT_IMG_BT,
			wxT("Export (.png)..."));
	export_csv_bt = new wxButton(scroll_pane, ID_EXPORT_CUT_CSV_BT,
			wxT("Export (.csv)..."));

	scalelbl = new wxStaticText(scroll_pane, wxID_ANY, wxT("Temperaturskala:"));
	scalemodelbl = new wxStaticText(scroll_pane, wxID_ANY, wxT("Modus/ΔT:"));
	scalemodecb = new wxComboBox(scroll_pane, ID_COLORSCALE_PROP, wxT(""),
			wxDefaultPosition, wxDefaultSize, 0, NULL,
			wxCB_READONLY | wxCB_DROPDOWN);
	scalemodecb->Insert(wxT("Vertikal"), 0);
	scalemodecb->Insert(wxT("Horizontal"), 0);
	scalemodecb->Insert(wxT("Kein"), 0);

	scalefontpropslbl = new wxStaticText(scroll_pane, wxID_ANY,
			wxT("Schriftgröße/Farbe:"));
	scalefontsizeedit = new wxSpinCtrl(scroll_pane, ID_COLORSCALE_PROP);
	scalefontcolorbt = new wxButton(scroll_pane, ID_COLORSCALE_COLORBT,
			wxT("Text"));
	scalestepedit = new wxSpinCtrl(scroll_pane, ID_COLORSCALE_PROP);

	//Initialisieren der Temperaturverteilung
	value_img = new float[1];
	//Initialisieren Grafik zur Temperaturverteilung
	image = new wxImage(100, 100, true);

	canvas->setImage(image);
	canvas->setValueImg(value_img);

	//getrennte Initialisierung für wxMSW erforderlich
	p1xedit->SetValue(wxT("0.0"));
	p1yedit->SetValue(wxT("0.0"));
	p1zedit->SetValue(wxT("0.0"));

	p2xedit->SetValue(wxT("1.0"));
	p2yedit->SetValue(wxT("0.0"));
	p2zedit->SetValue(wxT("0.0"));

	p3xedit->SetValue(wxT("0.0"));
	p3yedit->SetValue(wxT("1.0"));
	p3zedit->SetValue(wxT("0.0"));

	imgWidthEdit->SetRange(1, 100000000);
	imgWidthEdit->SetValue(800);

	imgHeightEdit->SetRange(1, 100000000);
	imgHeightEdit->SetValue(600);

	threadcountedit->SetRange(1, 1000);
	threadcountedit->SetValue(core_count);

	mmperpixeledit->SetValue(wxT("15.0"));

	scalemodecb->Select(GUIColorScalePanel::SCM_HORIZONTAL);

	scalestepedit->SetRange(1, 1000);
	scalestepedit->SetValue(12);

	scalefontsizeedit->SetRange(1, 1000);
	scalefontsizeedit->SetValue(12);

	scalefontcolorbt->SetForegroundColour(
			canvas->getScalePanel()->getTextColor());

	//update der Oberfläche auslösen
	p1xedit->SetValue(p1xedit->GetValue());

	//Neupositionierung der Oberflächenkomponenten für wxMSW
	wxSizeEvent evt;
	OnResize(evt);
}

/**
 * Funktion zum verteilten berechnen der 2D-Temperaturverteilung.
 * @param status_flag Zeiger auf Variable, die enthält ob der Thread beendet ist. (0 = Beendet)
 * @param value_img Liste für die Daten der Temperaturverteilung.
 * @param image Grafik für die Temperaturverteilung.
 * @param width Breite der Temperaturverteilungsgrafik.
 * @param height Höhe der Temperaturverteilungsgrafik.
 * @param startheight Starthöhe für diesen Thread in der Grafik.
 * @param delta_h Höhe des von diesem Thread zu berechnenden Streifens.
 * @param info Informationen über die Eigenschaften der zu berechnenden Ebene.
 * @param xvec X-Achse der Ebene.
 * @param yvec Y-Achse der Ebene.
 * @param v0 Mittelpunkt der Ebene.
 * @param bases Möglichst einfache Geometrien Geometrien der Materialien.
 * @param obj Das aktuelle Objekt.
 * @param sensor_data Die zu verwendenden Sensordaten.
 * @param use_last_tet Versuchen, die Interpolation durch vorgezogenes Testen des zuletzt verwendeten Tetraeders zu beschleunigen.
 * Diese Option ist verursacht Ungenauigkeiten und bietet zumeist wenig Performancegewinn.
 */
void render_thread(bool* status_flag, float* value_img, wxImage* image,
		int width, int height, int startheight, int delta_h,
		CutRender_info* info, Vector3D* xvec, Vector3D* yvec, Vector3D* v0,
		vector<tetgenio*>* bases, ObjectData* obj,
		vector<SensorPoint>* sensor_data, bool use_last_tet) {

	Interpolator interpolator;
	//Referenz auf den letzten für die Interpolation gewählten Tetraeder
	vector<SensorPoint*>* last_tet = new vector<SensorPoint*>;
	//Referenz auf den aktuell für die Interpolation gewählten Tetraeder
	vector<SensorPoint*>* new_tet = new vector<SensorPoint*>;
	//Die Visualisierungsinformationen
	Visualization_info* vis_info = wxGetApp().getVisualizationInfo();

	//Wurde der zuletzt verwendete Tetraeder initialisiert?
	bool last_tet_init = false;
	//Für alle Pixel im zu berechnenden Streifen...
	for (int x = 0; x < width; x++) {
		for (int y = startheight; y < startheight + delta_h; y++) {

			//Position des Pixels im 3D-Raum
			Vector3D* p = v0->copy();

			//Verschiebung auf der X-Achse der Ebene im vergleich zum Ebenenmittelpunkt
			Vector3D* part_x = xvec->copy();
			part_x->mult(
					x * info->mmperpixel / 1000.
							- width * info->mmperpixel / 2000);

			//Verschiebung auf der Y-Achse der Ebene im vergleich zum Ebenenmittelpunkt
			Vector3D* part_y = yvec->copy();
			part_y->mult(
					y * info->mmperpixel / 1000.
							- height * info->mmperpixel / 2000);

			//Berechnen der Position des Pixels im 3D-Raum
			p->add(part_x);
			p->add(part_y);
			delete part_x;
			delete part_y;

			//Initialisieren des Pixels
			image->SetAlpha(x, y, 0);
			image->SetRGB(x, y, 0, 0, 0);
			value_img[y * width + x] = -300;

			//Für alle Materialien des objekts...
			for (unsigned int m = 0; m < obj->getMaterials()->size(); m++) {
				//Das aktuelle Material
				ObjectData::MaterialData* mat = &obj->getMaterials()->at(m);

				//Befindet sich der Punkt im aktuellen Material?
				if (pointInsideMesh(p, bases->at(m), info->in_volume_algorithm)) {

					//Ermitteln des Wertes für den Punkt
					int status = 0;
					interpolator.setMode(mat->interpolation_mode);
					float value = (float) getPointValue(status, sensor_data,
							p->getXYZ(), &interpolator,
							(last_tet_init && use_last_tet) ? last_tet : NULL,
							use_last_tet ? new_tet : 0);

					//Tausch der Speicherorte des Tetraeders, in dem sich der Punkt in diesem und im vorherigen Durchlauf befand
					vector<SensorPoint*>* temp;
					temp = last_tet;
					last_tet = new_tet;
					new_tet = temp;
					last_tet_init = true;

					//Speichern des Wertes in der Temperaturverteilung
					value_img[y * width + x] = value;

					//Berechnen der Farbe zum Temperaturwert
					float inverse_hue = (value - vis_info->min_visualisation_temp)
							/ (vis_info->max_visualisation_temp
									+ vis_info->min_visualisation_temp);
					float* color = hsvToRgb((1.0 - clampHue(inverse_hue)) * .666, 1, 1);

					//Speichern des Wertes in der Grafik
					image->SetRGB(x, y, (unsigned char) (color[0] * 255),
							(unsigned char) (color[1] * 255),
							(unsigned char) (color[2] * 255));
					delete color;
					image->SetAlpha(x, y, 255);
					break;
				}
			}
			delete p;
		}
	}
	delete new_tet;
	delete last_tet;

	//markieren des Threads als beendet
	*status_flag = 0;
}

void GUICutRenderWindow::renderImage(wxImage* image) {

	//Speichern der Zeit für die Laufzeitmessung
	timeval tm1;
	gettimeofday(&tm1, NULL);

	int width = imgWidthEdit->GetValue();
	int height = imgHeightEdit->GetValue();

	//aktualisieren der Skala
	canvas->getScalePanel()->refresh(width, height);

	//die Anzahl der zur Berechnung zu verwendenden Kerne
	core_count = threadcountedit->GetValue();

	/*
	 * Versuchen, die Interpolation durch vorgezogenes Testen des zuletzt verwendeten Tetraeders zu beschleunigen.
	 * Diese Option ist verursacht Ungenauigkeiten und bietet zumeist wenig Performancegewinn.
	 * Sie ist deshalb standardmäßig deaktiviert und nicht über die Programmoberfläche aktivierbar.
	 */
	bool use_last_tet = false;

	//zurücksetzen der Grafik
	delete image;
	image = new wxImage(width, height, true);
	image->InitAlpha();

	//Punkt als Dezimaltrennzeichen
	setlocale(LC_NUMERIC, "C");

	//Eigenschaften der Temperaturverteilung
	CutRender_info* info = getCutRenderProperties();
	Triangle* tri = info->tri;
	//X-Achse der Ebene im 3D-Raum
	Vector3D* xvec = tri->getV2()->copy();
	xvec->sub(tri->getV1());
	//Normale der Ebene
	Vector3D* tri_nor = tri->getNormal();
	//Y-Achse der Ebene im 3D-Raum
	Vector3D* yvec = xvec->crossProduct(tri_nor);
	delete tri_nor;
	xvec->normalize();
	yvec->normalize();

	//Das aktive Objekt
	ObjectData* obj = wxGetApp().getActiveObject();

	//Erstellen möglichst einfacher Geometrien für die Materialien
	vector<tetgenio*> bases(obj->getMaterials()->size());
	for (unsigned int i = 0; i < obj->getMaterials()->size(); i++) {
		tetgenio* tri_io = new tetgenio();
		string args = "Q";
		tetrahedralize(const_cast<char*>(args.c_str()),
				obj->getMaterials()->at(i).tetgeninput, tri_io, NULL, NULL);
		bases.at(i) = tri_io;
	}

	//Zurücksetzen des Datenarrays für die Temperaturverteilung
	if (value_img != NULL) {
		delete[] value_img;
	}
	value_img = new float[width * height];

	//Verknüpfen der Ausgabe mit den Temperaturverteilungsdaten und der Grafik
	canvas->setImage(image);
	canvas->setValueImg(value_img);

	//Erstellen des Statusregisters für die Berechnungsthreads
	bool thread_running[core_count];
	for (int i = 0; i < core_count; i++) {
		thread_running[i] = 1;
	}

	//Array für die Thread-Objekts
	vector<thread*> threads = vector<thread*>(0);
	//Array für die Kopierten Sensordaten. Das Kopieren ist erforderlich, da die Threads die Daten verändern (sortieren).
	vector<vector<SensorPoint>> copied_sensor_data =
			vector<vector<SensorPoint>>(core_count);

	//Höhe für die Streifen, die die einzelnen Threads berechnen
	int delta_h = height / core_count;

	//Für alle Threads...
	for (int i = 0; i < core_count; i++) {
		//Die Starthöhe des Threads in der Temperaturverteilung
		int startheight = delta_h * i;

		//eventuelle Korrektur der Streifenhöhe für den letzten Thread
		if (i == core_count - 1) {
			if (startheight + delta_h < height) {
				delta_h = height - startheight;
			}
		}

		//Aktueller Sensordatensatz
		SensorData* dataset = &obj->getSensorDataList()->at(
				obj->getCurrentSensorIndex());
		vector<SensorPoint>* original_sd = &dataset->data.at(
				dataset->current_time_index);
		copied_sensor_data.at(i).resize(original_sd->size());

		//Kopieren der Sensordaten den Thread
		for (int p = 0; p < int(original_sd->size()); p++) {
			copySensorPoint(&original_sd->at(p),
					&copied_sensor_data.at(i).at(p));
		}

		//Starten des Threads
		threads.resize(threads.size() + 1,
				new thread(render_thread, &thread_running[i], value_img, image,
						width, height, startheight, delta_h, info, xvec, yvec,
						tri->getV1(), &bases, obj, &copied_sensor_data.at(i),
						use_last_tet));
	}

	//Pfüfzahl, ob noch Threads laufen
	unsigned int running = 0;

	//Solange threads laufen...
	do {
		//Ausgabe aktualisieren
		canvas->Update();
		canvas->Refresh();

		//ermitteln der Pfüfzahl, ob noch Threads laufen. Wenn diese 0 bleibt, sind alle Threads fertig.
		running = 0;
		for (int i = 0; i < core_count; i++) {
			running = running << 1;
			running += thread_running[i];
		};
	} while (running);

	//Threads zusammenführen
	for (int i = 0; i < core_count; i++) {
		threads.at(i)->join();
		delete threads.at(i);
	}

	//Freigeben des Ebenendreiecks
	for (int i = 0; i < 3; i++) {
		delete tri->getVert(i);
	}
	delete tri;

	//Zeitmessung beenden
	timeval tm2;
	gettimeofday(&tm2, NULL);

	unsigned long long t = 1000 * (tm2.tv_sec - tm1.tv_sec)
			+ (tm2.tv_usec - tm1.tv_usec) / 1000;

	//Ausgeben der Berechnungsdauer auf
	SetTitle(wxT("Berechnung abgeschlossen. ( ") + floattowxstr(t / 1000.)+wxT( "s )"));

	//Freigeben der Ressourcen
	delete xvec;
	delete yvec;
	for (unsigned int i = 0; i < obj->getMaterials()->size(); i++) {
		delete bases.at(i);
	}
	delete info;
}

void GUICutRenderWindow::OnCutPropsChanged(wxCommandEvent &event) {
	refreshVisualisation();
}

void GUICutRenderWindow::OnSCutPropsChanged_spin(wxSpinEvent &event) {
	refreshVisualisation();
}

void GUICutRenderWindow::refreshVisualisation() {
	GUIMainWindow* parent = (GUIMainWindow*) GetParent();
	parent->getGLCanvas()->getRenderer()->setCutRenderInfo(
			this->getCutRenderProperties());
	parent->getGLCanvas()->Refresh(false, NULL);
}

void GUICutRenderWindow::OnResize(wxSizeEvent &event) {

	//Positionieren der Fensterkomponenten
	int width = 0;
	int height = 0;
	GetSize(&width, &height);
	trilabel->SetSize(10, 10, 300, 20);
	p1label->SetSize(20, 30, 80, 20);
	p1xedit->SetSize(90, 30, 70, 20);
	p1yedit->SetSize(160, 30, 70, 20);
	p1zedit->SetSize(230, 30, 70, 20);
	p2label->SetSize(20, 60, 80, 20);
	p2xedit->SetSize(90, 60, 70, 20);
	p2yedit->SetSize(160, 60, 70, 20);
	p2zedit->SetSize(230, 60, 70, 20);
	p3label->SetSize(20, 90, 80, 20);
	p3xedit->SetSize(90, 90, 70, 20);
	p3yedit->SetSize(160, 90, 70, 20);
	p3zedit->SetSize(230, 90, 70, 20);
	optionslbl->SetSize(310, 10, 80, 20);
	widthHeightlbl->SetSize(320, 30, 200, 20);
	imgWidthEdit->SetSize(320, 50, 70, 20);
	imgHeightEdit->SetSize(390, 50, 70, 20);
	mmperpixellabel->SetSize(320, 70, 100, 20);
	mmperpixeledit->SetSize(320, 90, 140, 20);
	scalelbl->SetSize(470, 10, 140, 20);
	scalemodelbl->SetSize(480, 30, 140, 20);
	scalemodecb->SetSize(480, 50, 100, 20);
	scalestepedit->SetSize(580, 50, 40, 20);
	scalefontpropslbl->SetSize(480, 70, 140, 20);
	scalefontsizeedit->SetSize(480, 90, 75, 20);
	scalefontcolorbt->SetSize(555, 90, 75, 20);
	threadcountlbl->SetSize(640, 10, 100, 20);
	threadcountedit->SetSize(740, 10, 50, 20);
	calcbt->SetSize(640, 30, 150, 30);
	export_img_bt->SetSize(640, 60, 150, 30);
	export_csv_bt->SetSize(640, 90, 150, 30);
	canvas->SetSize(0, 140, width, height - canvas->GetPosition().y);
	refreshVisualisation();
	scroll_pane->SetSize(0, 0, width, 140);
	scroll_pane->SetVirtualSize(790, 120);
	canvas->Refresh();
}

void GUICutRenderWindow::OnExportCSV(wxCommandEvent &event) {
	wxFileDialog *SaveDialog = new wxFileDialog(this, wxT("Speichern unter..."),
			_(""), _(""), _("Character-separated values (*.csv)|*.csv"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	//Speichern bestätigt?
	if (SaveDialog->ShowModal() == wxID_OK) {
		Exporter exporter;
		CutRender_info* info = getCutRenderProperties();
		exporter.ExportCutCSV(string(SaveDialog->GetPath().ToUTF8().data()),
				value_img, info);
		delete info;
	}

	SaveDialog->Close();
	SaveDialog->Destroy();
}

void GUICutRenderWindow::OnExportImage(wxCommandEvent &event) {
	wxFileDialog *SaveDialog = new wxFileDialog(this, wxT("Speichern unter..."),
			_(""), _(""), _("Portable Network Graphics (*.png)|*.png"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	//Speichern bestätigt?
	if (SaveDialog->ShowModal() == wxID_OK) {
		//Skalenbild kopieren
		wxImage* scale_img = canvas->getScalePanel()->getImage();
		//Temperaturverteilungsgrafik kopieren
		wxImage cut_img = image->Copy();

		//Für alle Pixel der Temperaturverteilung
		for (int x = 0; x < cut_img.GetWidth(); x++) {
			for (int y = 0; y < cut_img.GetWidth(); y++) {
				//Transparenz des Skalenbildes
				float alpha = scale_img->GetAlpha(x, y) / 255.;

				//Kombinieren der Farbwerte
				cut_img.SetRGB(x, y,
						cut_img.GetRed(x, y) * (1 - alpha)
								+ scale_img->GetRed(x, y) * alpha,
						cut_img.GetGreen(x, y) * (1 - alpha)
								+ scale_img->GetGreen(x, y) * alpha,
						cut_img.GetBlue(x, y) * (1 - alpha)
								+ scale_img->GetBlue(x, y) * alpha);

				//Kombinieren der Transparenzinformation
				int new_alpha = cut_img.GetAlpha(x, y) + alpha * 255;
				new_alpha = new_alpha > 255 ? 255 : new_alpha;
				cut_img.SetAlpha(x, y, new_alpha);
			}
		}

		//exportieren des Bildes
		cut_img.SaveFile(SaveDialog->GetPath(), wxBITMAP_TYPE_PNG);
	}

	SaveDialog->Close();
	SaveDialog->Destroy();
}

CutRender_info* GUICutRenderWindow::getCutRenderProperties() {

	CutRender_info* info = new CutRender_info();
	info->tri = new Triangle(
			new Vector3D(atof(p1xedit->GetValue().ToAscii()),
					atof(p1yedit->GetValue().ToAscii()),
					atof(p1zedit->GetValue().ToAscii())),
			new Vector3D(atof(p2xedit->GetValue().ToAscii()),
					atof(p2yedit->GetValue().ToAscii()),
					atof(p2zedit->GetValue().ToAscii())),
			new Vector3D(atof(p3xedit->GetValue().ToAscii()),
					atof(p3yedit->GetValue().ToAscii()),
					atof(p3zedit->GetValue().ToAscii())));

	info->mmperpixel = atof(mmperpixeledit->GetValue().ToAscii());
	info->img_width = imgWidthEdit->GetValue();
	info->img_height = imgHeightEdit->GetValue();
	info->in_volume_algorithm = ALGORITHM_TETRAHEDRONS;
	return info;
}

void GUICutRenderWindow::OnColorScaleChanged_spin(wxSpinEvent &event) {
	wxCommandEvent temp_evt;
	OnColorScaleChanged(temp_evt);
}

void GUICutRenderWindow::OnCSColorBtClick(wxCommandEvent &event) {
	GUIColorScalePanel* scale = canvas->getScalePanel();
	//Farbauswahldialog erstellen
	wxColourDialog dialog(this);
	dialog.ShowModal();
	//Textfarbe auf die Skala und den Button übertragen
	scale->setTextColor(dialog.GetColourData().GetColour());
	scalefontcolorbt->SetForegroundColour(scale->getTextColor());
	//aktualisieren der Skala
	scale->refresh(image->GetWidth(), image->GetHeight());
	canvas->Refresh(false, NULL);
}

void GUICutRenderWindow::OnColorScaleChanged(wxCommandEvent &event) {
	GUIColorScalePanel* scale = canvas->getScalePanel();
	scale->setMode((GUIColorScalePanel::ScaleMode) scalemodecb->GetSelection());
	scale->setFontSize(scalefontsizeedit->GetValue());
	scale->setStepWidth(scalestepedit->GetValue());
	//aktualisieren der Skala
	scale->refresh(image->GetWidth(), image->GetHeight());
	canvas->Refresh(false, NULL);
}

void GUICutRenderWindow::renderCutBtClick(wxCommandEvent &event) {
	canvas->setValueImg(NULL);
	renderImage(image);
	canvas->Refresh(false, NULL);
}

GUICutRenderWindow::~GUICutRenderWindow() {
	delete image;
	delete[] value_img;
	GUIMainWindow* parent = (GUIMainWindow*) GetParent();

	if (parent != NULL) {
		//Vermerk im Hauptfenster, dass dieses Fenster geschlossen ist
		parent->setCutRenderWindowStatus(false);
		//löschen der Visualisierungsinformationen aus dem 3D-Fenster
		parent->getGLCanvas()->getRenderer()->setCutRenderInfo(NULL);
		parent->getGLCanvas()->Refresh(false, NULL);
	} else {
		cerr << "GUICutRenderWindow parent is null!" << endl;
	}
}


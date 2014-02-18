/*
 * GUICutRenderWindow.h
 *
 *  Created on: 01.10.2013
 *      Author: valentin
 */

#ifndef GUICUTRENDERWINDOW_H_
#define GUICUTRENDERWINDOW_H_
#include <wx/wx.h>
#include "GUIRenderCutCanvas.h"
#include "../libraries/interpolate/GeometryClasses.h"
#include "../processing/utils.h"
#include <wx/spinctrl.h>
#include "Renderer.h"
#include <wx/colordlg.h>
using namespace Utils;
/**
 * @brief Fenster zum erstellen zweidimensionaler Temperaturverteilungen.
 *
 * Das Fenster ermöglicht es, eine zweidimensionale Temperaturverteilung auf einer Schnittebene durch
 * das dreidimensionale Modell zu berechnen. Diese Schnittebene wird im 3D-Fenster des Hauptfensters visualisiert.
 */
class GUICutRenderWindow: public wxFrame {
public:
	/**
	 * Der Konstuktor.
	 * @param parent Das Übergeordnete Fenster. Muss vom Typ GUIMainWindow sein.
	 * @param title Titel des Fensters.
	 * @param xpos horizontale Position des Fensters.
	 * @param ypos vertikale Position des Fensters.
	 * @param width Breite des Fensters.
	 * @param height Höhe des Fenster
	 */
	GUICutRenderWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height);

	/**
	 * Der Destruktor.
	 */
	virtual ~GUICutRenderWindow();
protected:
	/**
	 * Event-Tabellendeklaration für wxWidgets.
	 */
	DECLARE_EVENT_TABLE();
private:
	/**
	 * Gibt die aktuell eingestellten Eigenschaften für die zweidimensionale Temperaturverteilung zurück, damit Sie
	 * später an den Renderer des 3D-Fensters zur Visualisierung übergeben werden können.
	 */
	CutRender_info* getCutRenderProperties();

	/**
	 * Behandelt das Drücken des Buttons zur Berechnung der zweidimensionalen Temperaturverteilung.
	 */
	void renderCutBtClick(wxCommandEvent &event);

	/**
	 * Behandelt Änderungen der Größe des Fensters.
	 */
	void OnResize(wxSizeEvent &event);

	/**
	 * Behandelt das Ändern von Parametern zur Berechnung der 2D-Temperaturverteilung.
	 */
	void OnCutPropsChanged(wxCommandEvent &event);

	/**
	 * Aktualisiert die Visualisierung der Schnittebene im Hauptfenster.
	 */
	void refreshVisualisation();

	/**
	 * Fragt den Benutzer nach dem Pfad und Exporiert eine Grafik aus 2D-Temperaturverteilung und Temperaturskala.
	 */
	void OnExportImage(wxCommandEvent &event);

	/**
	 * Fragt den Benutzer nach dem Pfad und Exporiert die 2D-Temperaturverteilung als .csv-Datei.
	 */
	void OnExportCSV(wxCommandEvent &event);

	/**
	 * Behandelt das Ändern von Parametern zur Berechnung der 2D-Temperaturverteilung.
	 */
	void OnSCutPropsChanged_spin(wxSpinEvent &event);

	/**
	 * Behandelt das Ändern von Parametern zur darstellung der Temperaturskala.
	 */
	void OnColorScaleChanged(wxCommandEvent &event);

	/**
	 * Behandelt das Ändern von Parametern zur darstellung der Temperaturskala.
	 */
	void OnColorScaleChanged_spin(wxSpinEvent &event);

	/**
	 * Behandelt das Klicken auf den Button zur Wahl der Schriftfarbe auf der Skala.
	 */
	void OnCSColorBtClick(wxCommandEvent &event);

	/**
	 * Berechnet die 2D-Temperaturverteilung als Grafik.
	 */
	void renderImage(wxImage* image);

	/**
	 * Scrollender Bereich, in den die anderen Komponenten außer der Zeichenfläche (canvas) eingebettet sind.
	 */
	wxScrolledWindow* scroll_pane;

	/**
	 * Textfeld zur Eingabe der Position des Dreiecks, dass die Schnittebene definiert.
	 */
	wxTextCtrl* p1xedit;

	/**
	 * Textfeld zur Eingabe der Position des Dreiecks, dass die Schnittebene definiert.
	 */
	wxTextCtrl* p1yedit;

	/**
	 * Textfeld zur Eingabe der Position des Dreiecks, dass die Schnittebene definiert.
	 */
	wxTextCtrl* p1zedit;

	/**
	 * Textfeld zur Eingabe der Position des Dreiecks, dass die Schnittebene definiert.
	 */
	wxTextCtrl* p2xedit;

	/**
	 * Textfeld zur Eingabe der Position des Dreiecks, dass die Schnittebene definiert.
	 */
	wxTextCtrl* p2yedit;

	/**
	 * Textfeld zur Eingabe der Position des Dreiecks, dass die Schnittebene definiert.
	 */
	wxTextCtrl* p2zedit;

	/**
	 * Textfeld zur Eingabe der Position des Dreiecks, dass die Schnittebene definiert.
	 */
	wxTextCtrl* p3xedit;

	/**
	 * Textfeld zur Eingabe der Position des Dreiecks, dass die Schnittebene definiert.
	 */
	wxTextCtrl* p3yedit;

	/**
	 * Textfeld zur Eingabe der Position des Dreiecks, dass die Schnittebene definiert.
	 */
	wxTextCtrl* p3zedit;

	/**
	 * Textfeld zur Eingabe der Breite des Bereichs, für den die 2D-Temperaturverteilung als Grafik berechnet wird.
	 */
	wxSpinCtrl* imgWidthEdit;

	/**
	 * Feld zur Eingabe der Höhe des Bereichs, für den die 2D-Temperaturverteilung als Grafik berechnet wird.
	 */
	wxSpinCtrl* imgHeightEdit;

	/**
	 * Feld zur Eingabe der zum Berechnen zu verwendenden Prozessorkerne.
	 */
	wxSpinCtrl* threadcountedit;

	/**
	 * Feld zur Eingabe des Maßstabs in \f$\frac{mm}{px}\f$.
	 */
	wxTextCtrl* mmperpixeledit;

	/**
	 * Beschriftung für den 1. die Schnittebene definierenden Punkt.
	 */
	wxStaticText* p1label;

	/**
	 * Beschriftung für den 2. die Schnittebene definierenden Punkt.
	 */
	wxStaticText* p2label;

	/**
	 * Beschriftung für den 3. die Schnittebene definierenden Punkt.
	 */
	wxStaticText* p3label;

	/**
	 * Beschriftung für den Maßstab in \f$\frac{mm}{px}\f$.
	 */
	wxStaticText* mmperpixellabel;

	/**
	 * Beschriftung für das die Schnittebene definierende Dreieck.
	 */
	wxStaticText* trilabel;

	/**
	 * Beschriftung für die die 2D-Temperaturverteilung betreffenden Parameter.
	 */
	wxStaticText* optionslbl;

	/**
	 * Beschriftung für Breite und Höhe der Grafik.
	 */
	wxStaticText* widthHeightlbl;

	/**
	 * Beschriftung für die Anzahl bei der Berechnung zu verwendender Prozessorkerne.
	 */
	wxStaticText* threadcountlbl;

	/**
	 * Beschriftung für die die Skala betreffenden Optionen.
	 */
	wxStaticText* scalelbl;

	/**
	 * Beschriftung für den Darstellungsmodus der Skala.
	 */
	wxStaticText* scalemodelbl;

	/**
	 * Menübox zur Auswahl des Darstellungsmodus der Skala.
	 */
	wxComboBox* scalemodecb;

	/**
	 * Beschriftung für die Schrifteigenschaften der Skala.
	 */
	wxStaticText* scalefontpropslbl;

	/**
	 * Feld zur Eingabe der Schriftgröße der Skala.
	 */
	wxSpinCtrl* scalefontsizeedit;

	/**
	 * Button zur Auswahl der Schriftfarbe.
	 */
	wxButton* scalefontcolorbt;

	/**
	 * Feld zur Eingabe der Schrittweite der Skala.
	 */
	wxSpinCtrl* scalestepedit;

	/**
	 * Button zum Starten der Berechnung der 2D-Temperaturverteilung.
	 */
	wxButton* calcbt;

	/**
	 * Button zum Export der Grafik.
	 */
	wxButton* export_img_bt;

	/**
	 * Button zum Export der Temperaturverteilung als .csv-Datei.
	 */
	wxButton* export_csv_bt;

	/**
	 * Die Zeichenfläche zur Darstellung der berechneten Grafik und der Skala.
	 */
	GUIRenderCutCanvas* canvas;

	/**
	 * Die berechnete Temperaturverteilung als Grafik.
	 */
	wxImage* image;

	/**
	 * Die berechnete Temperaturverteilung als Temperaturwerte.
	 */
	float *value_img;

	/**
	 * Die Anzahl der zu bei der Berechnung zu verwendender Prozessorkerne.
	 */
	int core_count;
};
#endif /* GUICUTRENDERWINDOW_H_ */

/*
 * GUIMainWindow.h
 *
 *  Created on: 04.09.2013
 *      Author: valentin
 */

#ifndef GUIMAINWINDOW_H_
#define GUIMAINWINDOW_H_

#include "GUIGLCanvas.h"
#include "PropertiesBox.h"
#include "ViewpropBox.h"
#include "GUIAnalyzeOutputWindow.h"
#include "GUIAnalyzePointWindow.h"
#include "GUICutRenderWindow.h"
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/stdpaths.h>

/**
 * @brief Hauptfenster mit Hauptmenü und Zugriff auf die einzelnen Programmfunktionen.
 *
 * Das Hauptfenster bietet über das Hauptmenü und die Oberfläche Zugriff auf die Funktionen des Programms.
 * Dazu kann das aktuelle Objekt gewählt werden, welches dann im eingebetteten 3D-Fenster angezeigt wird. Eigenschaften der Visualisierung
 * und des Objekts können ebenfalls über die Oberfläche des Hauptfensters festgelegt werden.
 */
class GUIMainWindow: public wxFrame {
public:
	/**
	 * Der Konstruktor.
	 * @param title Der Titel des Programmfensters.
	 * @param xpos horizontale Position des Fensters.
	 * @param ypos vertikale Position des Fensters.
	 * @param width Breite des Fensters.
	 * @param height Höhe des Fensters.
	 */
	GUIMainWindow(const wxChar *title, int xpos, int ypos, int width, int height);

	/**
	 * Setzt den Status des Übersichstfensters über die Analysedaten.
	 * @param isValid Ob das Fenster ein gültiges Objekt oder ob der Speicher bereits freigegeben ist.
	 */
	void setAnalyzeWindowStatus(bool isValid);

	/**
	 * Setzt den Status des Übersichstfensters über die Analysedaten.
	 * @param isValid Ob das Fenster ein gültiges Objekt oder ob der Speicher bereits freigegeben ist.
	 */
	void setCutRenderWindowStatus(bool isValid);

	/**
	 * Gibt die Zeichenfläche des 3D-Fensters zurück.
	 */
	GUIGLCanvas* getGLCanvas();

	/**
	 * Der Destruktor.
	 */
	virtual ~GUIMainWindow();
protected:
	/**
	 * Anzahl der Suchpfade für die Anwendungsdaten (z.B. Icons).
	 */
	static const int NUMBEROFPATHS = 2;


	/**
	 * Suchpfade für die Anwendungsdaten.
	 * Das Verzeichnis der ausführbaren Datei wird immer und zuerst geprüft.
	 */
	string configpaths[NUMBEROFPATHS] {
			"/usr/local/share/simpleanalyzer/",
			"/usr/share/simpleanalyzer/",
	};

	/**
	 * Event-Tabellendeklaration für wxWidgets.
	 */
	DECLARE_EVENT_TABLE()
private:
	/**
	 * Öffnet den Dialog zum Importieren eines Objekts.
	 */
	void OnMenuImportObj(wxCommandEvent &event);

	/**
	 * Öffnet den Dialog zum Importieren einfacher Sensordaten.
	 */
	void OnMenuImportSD(wxCommandEvent &event);

	/**
	 * Öffnet den Dialog zum Importieren zeitbezogener Sensordaten.
	 */
	void OnMenuImportTSD(wxCommandEvent &event);

	/**
	 * Beendet das Programm.
	 */
	void OnMenuFileQuit(wxCommandEvent &event);

	/**
	 * Öffnet ein Fenster mit Informationen über das Programm.
	 */
	void OnMenuHelpAbout(wxCommandEvent &event);

	/**
	 * Öffnet das Handbuch mit dem PDF-Viewer des Systems.
	 */
	void OnMenuOpenManual(wxCommandEvent &event);

	/**
	 * Berechnet die 3D-Temperaturverteilung neu.
	 */
	void OnRecalcBtClick(wxCommandEvent &event);

	/**
	 * Behandelt Größenänderungen des Fensters.
	 */
	void OnResize(wxSizeEvent &event);

	/**
	 * Aktualisiert die Oberfläche nach dem Auswählen eines anderen Materials im Objekteigenschaften-Fenster.
	 */
	void OnMaterialSelect(wxCommandEvent &event);

	/**
	 * Öffnet das Analysedaten-Übersichtsfenster.
	 */
	void OnAnalyze(wxCommandEvent &event);

	/**
	 * Behandelt das Aktualisieren der Oberfläche nach einer Änderung an Objekteigenschaften,bei denen ein sofortiges Update der Oberfläche möglich ist, durch den Nutzer.
	 */
	void OnImmediateUpdatePropChange(wxCommandEvent &event);

	/**
	 * Behandelt das Aktualisieren der Oberfläche nach einer Änderung an Objekteigenschaften durch den Nutzer.
	 */
	void OnGeneralPropChange(wxCommandEvent &event);

	/**
	 * Behandelt das Aktualisieren der Oberfläche nach einer Änderung an Visualisierungsoptionen durch den Nutzer.
	 */
	void OnViewPropChange(wxCommandEvent &event);

	/**
	 * Behandelt das Aktualisieren der Oberfläche nach einer Änderung an Visualisierungsoptionen durch den Nutzer.
	 */
	void OnViewPropSpinChange(wxSpinEvent &event);

	/**
	 * Behandelt das Auswählen eines anderen Sensordatensatzes.
	 */
	void OnSensorDataChange(wxCommandEvent &event);

	/**
	 * Behandelt Änderungen an der Sensordaten-Zeitleiste (bei zeitbezogenen Datensätzen).
	 */
	void OnSDTimelineChange(wxCommandEvent &event);

	/**
	 * Löscht alle Markierungen auf der Sensordaten-Zeitleiste (bei zeitbezogenen Datensätzen).
	 */
	void OnSDTLMarkerClear(wxCommandEvent &event);

	/**
	 * Setzen des auf der Sensordaten-Zeitleiste ausgewählten Zeitpunktes auf den nächsten markierten Zeitpunkt.
	 */
	void OnSDTLNextMarker(wxCommandEvent &event);

	/**
	 * Setzen des auf der Sensordaten-Zeitleiste ausgewählten Zeitpunktes auf den vorherigen markierten Zeitpunkt.
	 */
	void OnSDTLPrevMarker(wxCommandEvent &event);

	/**
	 * Behandelt das Markieren eines Zeitpunktes auf der Sensordaten-Zeitleiste.
	 */
	void OnAnalyzeMarkerChange(wxCommandEvent &event);

	/**
	 * Setzt das aktive Objetk nach dem Auswählen im Popup-Menü.
	 */
	void OnActiveObjectChangePopup(wxCommandEvent &event);

	/**
	 * Öffnet das Popup-Menü zum auswählen des aktiven Objekts.
	 */
	void OnActiveObjectChange(wxCommandEvent &event);

	/**
	 * Löscht das aktive Objekt, sofern es nicht das einzige geladene Objekt ist.
	 */
	void OnActiveObjectDelete(wxCommandEvent&event);

	/**
	 * Öffnet das Fenster zur Analyse eines Punktes (GUIAnalyzePointWindow).
	 */
	void OnAnalyzePoint(wxCommandEvent &event);

	/**
	 * Öffnet das Fenster zur Berechnung einer zweidimensionalen Temperaturverteilung.
	 */
	void OnRenderCut(wxCommandEvent &event);

	/**
	 * Registriert ein neues (Versuchs-) Objekt im Programm.
	 * @param obj Das zu registrierende Objekt.
	 */
	void addObject(ObjectData* obj);

	/**
	 * Setzt das aktive Objekt.
	 * @param index Index des als aktives Objekt zu verwendeten Objekts.
	 */
	void setActiveObject(int index);

	/**
	 * Öffnet ein Fenster zum Exportieren der Ansicht des 3D-Fensters.
	 */
	void OnExportViewportImage(wxCommandEvent &event);

	/**
	 * Öffnet ein Fenster zum Exportieren der Temperaturverteilung und des Objekts im VTK-Format.
	 */
	void OnExportVTK(wxCommandEvent &event);

	/**
	 * Sucht den Zeitpunkt zwischen zwei markierten Stellen auf der Sensordaten-Zeitleiste, für den der
	 * Wäremeenergiegehalt maximal wird. Dabei wird der Bereich zwischen den beiden markierten Stellen ausgewählt, zwischen denen
	 * sich der aktuell ausgewählte Zeitpunkt befindet.
	 */
	void OnFindMaxTSD(wxCommandEvent &event);

	/**
	 * Behandelt das aktivieren/deaktivieren der Option zum automatischen neuberechnen der Temperaturverteilung eines
	 * Objekts, sobald Änderungen an dessen Eigenschaften vorgenommen werden.
	 */
	void OnAutoUpdateChange(wxCommandEvent &event);

	/**
	 * Überträgt die in der GUI eingetragenen Objekteigenschaften in das aktive Objekt.
	 */
	void assignCurrentObjectProps();

	/**
	 * Überträgt die Eigenschaften des aktiven Objekts in die GUI.
	 */
	void updateObjectPropGUI();

	/**
	 * Speichert die Visualisierungsoptionen aus der GUI.
	 */
	void assignViewProps();

	/**
	 * Lädt die Visualisierungsoptionen in die GUI.
	 */
	void updateViewPropGUI();


	/**
	 * Die Zeichenfläche für das 3D-Fenster.
	 */
	GUIGLCanvas* gl_context;

	/**
	 * Die Tollbarkomponente.
	 */
	wxToolBar* toolbar;

	/**
	 * Die Hauptmenükomponente.
	 */
	wxMenuBar* mwMenuBar;

	/**
	 * Das "Datei"-Untermenü.
	 */
	wxMenu* mwFileMenu;

	/**
	 * Das "Hilfe"-Untermenü.
	 */
	wxMenu* mwHelpMenu;

	/**
	 * Das "Import"-Untermenü.
	 */
	wxMenu* mwImportMenu;

	/**
	 * Das "Export"-Untermenü.
	 */
	wxMenu* mwExportMenu;

	/**
	 * Das "Analysieren"-Untermenü.
	 */
	wxMenu* mwAnalyzeMenu;

	/**
	 * Das "Bearbeiten"-Untermenü
	 */
	wxMenu* mwEditMenu;

	/**
	 * Die Unterkomponente, die die Objekteigenschaften-Oberfläche enthält.
	 */
	PropertiesBox *propbox;

	/**
	 * Die Unterkomponente, die die Visualisierungsoptionen-Oberfläche enthält.
	 */
	ViewpropBox* viewbox;

	/**
	 * Das Analysedaten-Übersichtsfenster. Der Zeiger ist ungültig, wenn das Analysedaten-Übersichtsfenster nicht
	 * geöffnet ist. (siehe analyze_window_valid)
	 */
	GUIAnalyzeOutputWindow* analyzerframe;

	/**
	 * Das Fenster zur Berechnung einer zweidimensionalen Temperaturverteilung. Der Zeiger ist ungültig, wenn das
	 * 2D-Fenster nicht geöffnet ist. (siehe render_cut_window_valid)
	 */
	GUICutRenderWindow* rendercutwindow;

	/**
	 * Scrollender Bereich, in den die Objekteigenschaften-Oberfläche eingebettet ist.
	 */
	wxScrolledWindow* prop_scroll_win;

	/**
	 * Scrollender Bereich, in den die Visualisierungsoptionen-Oberfläche eingebettet ist.
	 */
	wxScrolledWindow* view_scroll_win;

	/**
	 * Die Oberfläche wird gerade vom Programm verändert. Signalisiert, dass die Eingabe nicht durch den Nutzer erfolgt ist.
	 */
	bool updating;

	/**
	 * Das Analysedaten-Übersichtsfenster ist gerade geöffnet.
	 */
	bool analyze_window_valid;

	/**
	 * Das Fenster zur Berechnung einer zweidimensionalen Temperaturverteilung ist gerade geöffnet.
	 */
	bool render_cut_window_valid;

	/**
	 * Der Pfad zum Verzeichnis, das die von der Anwendung verwendeten Daten (z.B. Icons) enthält. Wird im Konstruktor bestimmt.
	 */
	string data_directory;
};

#endif /* GUIMAINWINDOW_H_ */

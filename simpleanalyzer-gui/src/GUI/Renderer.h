/*
 * Renderer.h
 *
 *  Created on: 27.07.2013
 *      Author: valentin
 */

#ifndef RENDERER_H_
#define RENDERER_H_
#include "../libraries/tetgen/tetgen.h"
#include <vector>
#include "../libraries/interpolate/GeometryClasses.h"
#include "../processing/ObjectData.h"
#include "../processing/utils.h"
#include <wx/wx.h>
using namespace std;
using namespace Utils;
/**
 * @brief Zeichnet den Inhalt der 3D-Fensters.
 *
 * Zeichnet das 3D-Objekt, Sensordaten und Koordinatensystem je nach
 * Visualisierungsoptionen mithilfe der OpenGL-Bibliothek.
 */
class Renderer {
public:
	/**
	 * Darstellungsmodus für Elemente /Punkte, Kanten, Flächen) des 3D-Objekts.
	 */
	enum RenderMode {
		RM_NONE = 0,
		RM_MATERIALCOLOR,
		RM_VALUECOLOR,
	};

	/**
	 * @brief Informationen über die Ansicht des Modells (Virtuelle Kamera) und welche Elemente dargestellt werden.
	 */
	struct Viewport_info {
		float zoom;	/**< Aktueller Zoomfaktor */
		float rotationY; 			/**< Rotation der Ansicht um die Y-Achse */
		float rotationX; 			/**< Rotation der Ansicht um die (Kameralokale) X-Achse */
		Vector3D* cameraPosition; 	/**< position der Virtuellen Kamera */
		Triangle* cut;				/**< Dreieck der Schnittebene, wenn nicht NULL, werden nur Elemente oberhalb der Dreiecksebene dargestellt (Momentan nicht verwendet). */
		bool invertcut;				/**< Nur Elemente unterhalb der durch cut definierte Ebene darstellen (Momentan nicht verwendet). */
		RenderMode showPoints;		/**< Modus der Darstellung von Punkten des 3D-Objekts. */
		RenderMode showEdges;		/**< Modus der Darstellung von Kanten des 3D-Objekts. */
		RenderMode showFaces;		/**< Modus der Darstellung von Flächen des 3D-Objekts. */
		bool show_extrapolated;		/**< Extrapolierte Elemente anzeigen */
		bool show_sensordata;		/**< Sensordaten als Punkte anzeigen */
		int width;					/**< Breite des dargestellten Bereichs */
		int height;					/**< Höhe des dargestellten Bereichs */
		float scale;				/**< Skalierungsfaktor für das 3D-Objekt. */
	};
public:
	/**
	 * Der Konstruktor.
	 */
	Renderer();

	/**
	 * Initialisiert die OpenGL-Bibliothek.
	 * @param width Breite des Anzeigebereichs.
	 * @param height Höhe des Anzeigebereichs.
	 */
	void initGL(int width, int height);

	/**
	 * Verändert die Größe des Anzeigebereichs.
	 * @param width Neue Breite des Anzeigebereichs.
	 * @param height Neue Höhe des Anzeigebereichs.
	 */
	void resize(int width, int height);

	/**
	 * Zeichnet das Objekt (Attribut object).
	 */
	void render();

	/**
	 * Setzt das zu zeichnende Objekt.
	 * @param obj Das zu zeichnende Objekt.
	 */
	void setObject(ObjectData* obj);

	/**
	 * Setzt die Eigenschaften einer 2D-Temperaturverteilung, welche teilweise zur Visualisierung
	 * der Ebene der 2D-Temperaturverteilung benötigt werden.
	 * NULL bedeutet keine Visualisierung.
	 * @param info Die Eigenschaften der 2D-Temperaturverteilung.
	 */
	void setCutRenderInfo(CutRender_info* info);

	/**
	 * Gibt den Inhalt der Zeichenfläche als Bild zurück.
	 */
	wxImage* getViewportImage();

	/**
	 * Gibt eine Referenz auf die verwendeten Anzeigeeigenschaften zurück.
	 */
	Viewport_info* getViewport();

	/**
	 * Der Destruktor.
	 */
	virtual ~Renderer();
private:
	/**
	 * Zeichnet die Elemente eines Materials des Objekts.
	 * @param mat Das zu zeichnende Material.
	 */
	void renderMaterial(ObjectData::MaterialData* mat);

	/**
	 * Zeichnet die Tetraeder eines Materials des Objekts.
	 * @param mat Das zu zeichnende Material.
	 * @param rendermode Der zu verwendende Zeichenmodus.
	 */
	void renderTetrahedra(ObjectData::MaterialData* mat,RenderMode rendermode);

	/**
	 * Zeichnet Sensordaten als Punkte.
	 * @param data Sensordaten als Liste von Punkten.
	 */
	void renderSensorData(vector<SensorPoint>* data);

	/**
	 * Informationen über die Darstellung des zu zeichnenden Inhalts.
	 */
	Viewport_info viewport;

	/**
	 * Das darzustellende Objekt.
	 */
	ObjectData* object;

	/**
	 * Eigenschaften einer 2D-Temperaturverteilung, welche teilweise zur Visualisierung
	 * der Ebene der 2D-Temperaturverteilung benötigt werden.
	 * NULL bedeutet keine Visualisierung.
	 */
	CutRender_info* cut_visualisation_info;

	/**
	 * Adresse der OpenGL-Displaylist, die die Geometriedaten auf der Grafikkarte vorhält.
	 */
	int displayList;
};


#endif /* RENDERER_H_ */

/*
 * Renderer.cpp
 *
 *  Created on: 27.07.2013
 *      Author: valentin
 */

#include "Renderer.h"

#include <iostream>
#include <cmath>
#include "../SimpleAnalyzerApp.h"
//#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>

using namespace std;

Renderer::Renderer() {
	object = NULL;
	displayList = -1;
	//Anzeigeoptionen initialisieren
	viewport.zoom = 3;
	viewport.cameraPosition = new Vector3D(0, 0, 0);
	viewport.rotationY = 0;
	viewport.rotationX = 0;
	viewport.invertcut = true;
	viewport.cut = NULL;
	viewport.showPoints = RM_NONE;
	viewport.showEdges = RM_VALUECOLOR;
	viewport.showFaces = RM_NONE;
	viewport.show_extrapolated = true;
	viewport.show_sensordata = true;
	viewport.scale = 1;
	/*Vector3D *p1 = new Vector3D(0,1,-5);
	 Vector3D *p2 = new Vector3D(0,1,5);
	 Vector3D *p3 = new Vector3D(3,3,0);
	 viewport.cut = new Triangle(p1,p2,p3);*/
	cut_visualisation_info = NULL;
}

Renderer::Viewport_info* Renderer::getViewport() {
	return &viewport;
}

bool pointBehindCut(Vector3D* point, Triangle* cut) {
	/*for (unsigned int i=0;i<cut->size();i++) {
	 Triangle* tri = cut->at(i);
	 Vector3D* normal = tri->getNormal();
	 Vector3D* tpoint1 = point->copy();
	 tpoint1->sub(tri->getV1());
	 Vector3D* tpoint2 = point->copy();
	 tpoint2->sub(tri->getV2());
	 Vector3D* tpoint3 = point->copy();
	 tpoint3->sub(tri->getV3());
	 Vector3D* e1 = tri->getV2()->copy();
	 e1->sub(tri->getV1());
	 Vector3D* e2 = tri->getV3()->copy();
	 e2->sub(tri->getV2());
	 Vector3D* e3 = tri->getV1()->copy();
	 e3->sub(tri->getV3());	//
	 Vector3D* ov1 = e1->crossProduct(e2);
	 Vector3D* enorm1 = ov1->crossProduct(e1);
	 Vector3D* ov2 = e2->crossProduct(e3);
	 Vector3D* enorm2 = ov2->crossProduct(e2);
	 Vector3D* ov3 = e3->crossProduct(e1);
	 Vector3D* enorm3 = ov3->crossProduct(e3);
	 if (normal->dotProduct(tpoint1)>0 && enorm1->dotProduct(tpoint1)>0 && enorm2->dotProduct(tpoint2)>0 && enorm3->dotProduct(tpoint3)>0) {
	 behind = true;
	 }
	 delete normal;
	 delete tpoint1;
	 delete tpoint2;
	 delete tpoint3;
	 delete e1;
	 delete e2;
	 delete e3;
	 delete ov1;
	 delete ov2;
	 delete ov3;
	 delete enorm1;
	 delete enorm2;
	 delete enorm3;
	 }*/
	Vector3D* normal = cut->getNormal();
	Vector3D* tpoint = point->copy();
	tpoint->sub(cut->getV1());
	bool behind = (normal->dotProduct(tpoint) < 0);
	delete normal;
	delete tpoint;
	return behind;
}

void Renderer::renderTetrahedra(ObjectData::MaterialData* mat,
		RenderMode rendermode) {

	//die darzustellende Geometrie
	tetgenio* io = mat->tetgenoutput;
	glEnable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);

	//die Visualisierungsoptionen
	Visualization_info* vis_info = wxGetApp().getVisualizationInfo();

	//für alle Tetraeder...
	for (int j = 0; j < io->numberoftetrahedra; j++) {
		//Enthält der Tetraeder extrapolierte Punkte?
		bool contains_extrapolated_points = false;
		for (int i = 0; i < 4; i++) {
			if (mat->extrapolated[io->tetrahedronlist[4 * j + i]]) {
				contains_extrapolated_points = true;
			}
		}

		//Darf der Tetraeder nicht gezeichnet werden?
		if (contains_extrapolated_points && !viewport.show_extrapolated) {
			continue;
		}

		//Die Eckpunkte des Tetraeders
		Vector3D v1 = Vector3D(&io->pointlist[3 * io->tetrahedronlist[4 * j]]);
		Vector3D v2 = Vector3D(&io->pointlist[3 * io->tetrahedronlist[4 * j + 1]]);
		Vector3D v3 = Vector3D(&io->pointlist[3 * io->tetrahedronlist[4 * j + 2]]);
		Vector3D v4 = Vector3D(&io->pointlist[3 * io->tetrahedronlist[4 * j + 3]]);

		//Wird die Anzeige der Tetraeder durch eine Schnittebene eingeschränkt?
		if (viewport.cut != NULL) {
			//Zentrum des Tetraeders ermitteln
			Vector3D center = Vector3D(0, 0, 0);
			center.add(&v1);
			center.add(&v2);
			center.add(&v3);
			center.add(&v4);
			center.mult(.25);

			//Liegt der Tetraeder vor der Schnittebene?
			if (pointBehindCut(&center, viewport.cut) != viewport.invertcut) {
				continue;
			}
		}

		//Koordinaten lokal zu v1
		v2.sub(&v1);
		v3.sub(&v1);
		v4.sub(&v1);
		v1.sub(&v1);

		//Normalen der Tetraederflächen berechnen
		Vector3D* n1 = v2.crossProduct(&v3);
		if (v4.dotProduct(n1) > 0) {
			n1->mult(-1);
		}
		Vector3D* n2 = v4.crossProduct(&v2);
		if (v3.dotProduct(n2) > 0) {
			n2->mult(-1);
		}
		Vector3D* n3 = v3.crossProduct(&v4);
		if (v2.dotProduct(n3) > 0) {
			n3->mult(-1);
		}
		Vector3D* backedge1 = v2.copy();
		Vector3D* backedge2 = v3.copy();
		backedge1->sub(&v4);
		backedge2->sub(&v4);
		Vector3D* n4 = backedge1->crossProduct(backedge2);
		if (v4.dotProduct(n4) < 0) {
			n4->mult(-1);
		}
		n1->normalize();
		n2->normalize();
		n3->normalize();
		n4->normalize();

		//Farben der Eckpunkte
		float* colors[4];
		//In Materialfarbe anzeigen?
		if (rendermode == RM_MATERIALCOLOR) {
			glColor4f(mat->color[0], mat->color[1], mat->color[2], .3);
		} else {
			//Ermitteln der Farben (basierend auf der Temperatur) für die Eckpunkte
			for (int k = 0; k < 4; k++) {
				float value = io->pointattributelist[io->numberofpointattributes
						* (io->tetrahedronlist[4 * j + k])];
				float inverse_hue = (value - vis_info->min_visualisation_temp)
						/ (vis_info->max_visualisation_temp
								- vis_info->min_visualisation_temp);
				colors[k] = hsvToRgb((1.0 - clampHue(inverse_hue)) * .666, 1, 1);
			}
		}

		//Incides der Punkte für die Flächen des Tetraeders
		int vertlist[4][3] = { { 0, 1, 2 }, { 0, 1, 3 }, { 0, 2, 3 },
				{ 1, 2, 3 } };
		Vector3D* normallist[4] = { n1, n2, n3, n4 };

		//Zeichnen der Tetraeder
		for (int k = 0; k < 4; k++) {
			glNormal3dv(normallist[k]->getXYZ());
			if (rendermode == RM_VALUECOLOR) {
				glColor3fv(colors[vertlist[k][0]]);
			}
			glVertex3dv(
					&io->pointlist[3
							* io->tetrahedronlist[4 * j + vertlist[k][0]]]);
			if (rendermode == RM_VALUECOLOR) {
				glColor3fv(colors[vertlist[k][1]]);
			}
			glVertex3dv(
					&io->pointlist[3
							* io->tetrahedronlist[4 * j + vertlist[k][1]]]);
			if (rendermode == RM_VALUECOLOR) {
				glColor3fv(colors[vertlist[k][2]]);
			}
			glVertex3dv(
					&io->pointlist[3
							* io->tetrahedronlist[4 * j + vertlist[k][2]]]);

		}

		//freigeben der Ressourcen
		delete backedge1;
		delete backedge2;
		delete n1;
		delete n2;
		delete n3;
		delete n4;
		if (rendermode == RM_VALUECOLOR) {
			for (int k = 0; k < 4; k++) {
				delete[] colors[k];
			}
		}
	}
	glEnd();
}

void Renderer::renderSensorData(vector<SensorPoint>* data) {

	//Sensordaten über alles Zeichnen
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_LIGHTING);

	//die Visualisierungsoptionen
	Visualization_info* vis_info = wxGetApp().getVisualizationInfo();

	//für alle Sensorpunkte
	for (unsigned int i = 0; i < data->size(); i++) {
		SensorPoint* point = &data->at(i);

		//Ermitteln der Farbe aus der Temperatur
		float value = point->temperature;
		float inverse_hue = (value - vis_info->min_visualisation_temp)
				/ (vis_info->max_visualisation_temp
						- vis_info->min_visualisation_temp);
		float* color = hsvToRgb((1.0 - clampHue(inverse_hue)) * .666, 1, 1);

		//Zeichnen des Punktes
		glPointSize(6.0);
		glColor3f(0, 0, 0);
		glBegin(GL_POINTS);
			glVertex3dv(point->coords);
		glEnd();
		glPointSize(4.0);
		glColor3fv(color);
		glBegin(GL_POINTS);
			glVertex3dv(point->coords);
		glEnd();
		delete[] color;
	}

	glEnable(GL_LIGHTING);
	glDepthFunc(GL_LESS);
}

void Renderer::renderMaterial(ObjectData::MaterialData* mat) {

	//die darzustellende Geometrie
	tetgenio* io = mat->tetgenoutput;

	//die Visualisierungsoptionen
	Visualization_info* vis_info = wxGetApp().getVisualizationInfo();

	//sollen die Punkte angezeigt werden?
	if (viewport.showPoints) {
		glPointSize(3.0);
		glDisable(GL_LIGHTING);
		glBegin(GL_POINTS);

		//sollen die Punkte in Materialfarbe dargestellt werden?
		if (viewport.showPoints == RM_MATERIALCOLOR) {
			glColor3fv(mat->color);
		}

		//für alle Punkte...
		for (int i = 0; i < io->numberofpoints; i++) {

			//soll der Punkt von der Darstellung ausgeschlossen werden?
			if (mat->extrapolated[i] && !viewport.show_extrapolated) {
				continue;
			}

			//farblich der Temperatur entsprechend anzeigen?
			if (viewport.showPoints == RM_VALUECOLOR) {
				//Ermitteln der Farbe zum Temperaturwert
				float value = io->pointattributelist[io->numberofpointattributes
						* (i)];
				float inverse_hue = (value - vis_info->min_visualisation_temp)
						/ (vis_info->max_visualisation_temp
								- vis_info->min_visualisation_temp);
				float* color = hsvToRgb((1.0 - clampHue(inverse_hue)) * .666, 1, 1);

				//Zeichnen des Punktes
				glColor3fv(color);
				glVertex3dv(&io->pointlist[3 * (i)]);
				delete[] color;
			}

			//sollen die Punkte in Materialfarbe dargestellt werden?
			if (viewport.showPoints == RM_MATERIALCOLOR) {
				//Zeichnen des Punktes
				glVertex3dv(&io->pointlist[3 * (i)]);
			}
		}
		glEnd();
	}

	//sollen die Kanten angezeigt werden
	if (viewport.showEdges) {
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		renderTetrahedra(mat, viewport.showEdges);
	}

	//sollen die Flächen angezeigt werden
	if (viewport.showFaces) {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
		renderTetrahedra(mat, viewport.showFaces);

	}

	//zeichenmodus zurücksetzen
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
}

void Renderer::initGL(int width, int height) {
	//Hintergrundfarbe initialisieren
	glClearColor(.5, .5, .5, 1);
	//Viewport und Projektion setzen
	resize(width,height);
}

void Renderer::resize(int width, int height) {
	//Viewport aktualisieren
	glViewport(0, 0, width, height);
	viewport.width = width;
	viewport.height = height;

	//Projektion aktualisieren
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double h = (double) height / (double) width;
	glFrustum(-1.0, 1.0, -h, h, 1.5, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/**
 * Zeichnet einen Vektor als Pfeil.
 * @param pos Startpunkt des Pfeils.
 * @param dir Richtung und Länge des Pfeils.
 */
void drawVector(Vector3D* pos, Vector3D* dir) {

	//spitze des Vektors
	Vector3D tip = Vector3D(pos);
	//normalisierte Richtung des Vektors
	Vector3D ndir = Vector3D(dir);
	ndir.normalize();

	//Referenzvektor, darf dem Vektor nicht entsprechen
	Vector3D refdir = Vector3D(1, 0, 0);
	if (refdir.equals(&ndir)) {
		refdir.getXYZ()[0] = 0;
		refdir.getXYZ()[1] = 1;
	}

	//Radius der Vektorspitze
	float radius = .012*dir->getLength();
	//Länge der Vektorspitze
	float length = .024*dir->getLength();

	//Zwei Referenzvektoren, die zum Ausgansvektor und zueinander einen Winkel von 90° einschließen
	Vector3D* refvec = ndir.crossProduct(&refdir);
	Vector3D* refvec2 = ndir.crossProduct(refvec);
	refvec->normalize();
	refvec2->normalize();

	//zeichnen der Linie von Ausganspunkt bis Spitze des Pfeils
	tip.add(dir);
	Vector3D behind = Vector3D(pos);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex3dv(behind.getXYZ());
	glVertex3dv(tip.getXYZ());
	glEnd();

	//Hilfsvektoren zum zusammentragen der Spitze des Vektors
	Vector3D v1 = Vector3D(refvec);
	Vector3D v2 = Vector3D(refvec2);
	Vector3D v3 = Vector3D(v1);
	v3.mult(-1);
	Vector3D v4 = Vector3D(refvec2);
	v4.mult(-1);

	//zeichen der Pfeilspitze
	v1.mult(radius);
	v2.mult(radius);
	v3.mult(radius);
	v4.mult(radius);
	ndir.mult(dir->getLength() - length);
	v1.add(&ndir);
	v2.add(&ndir);
	v3.add(&ndir);
	v4.add(&ndir);
	v1.add(pos);
	v2.add(pos);
	v3.add(pos);
	v4.add(pos);
	glBegin(GL_TRIANGLES);
	glVertex3dv(v1.getXYZ());
	glVertex3dv(v2.getXYZ());
	glVertex3dv(tip.getXYZ());

	glVertex3dv(v2.getXYZ());
	glVertex3dv(v3.getXYZ());
	glVertex3dv(tip.getXYZ());

	glVertex3dv(v3.getXYZ());
	glVertex3dv(v4.getXYZ());
	glVertex3dv(tip.getXYZ());

	glVertex3dv(v4.getXYZ());
	glVertex3dv(v1.getXYZ());
	glVertex3dv(tip.getXYZ());
	glEnd();
	glLineWidth(1.0);
	delete refvec;
	delete refvec2;
}

/**
 * Zeichnet markante Linien zum leichteren Erfassen des Koordinatensystems.
 */
void renderGrid() {

	//Achsen einzeichnen
	Vector3D x_axis(10, 0, 0);
	Vector3D y_axis(0, 10, 0);
	Vector3D z_axis(0, 0, 10);
	Vector3D orgin(0, 0, 0);
	glColor3f(1, 0, 0);
	drawVector(&orgin, &x_axis);
	glColor3f(0, 1, 0);
	drawVector(&orgin, &y_axis);
	glColor3f(0, 0, 1);
	drawVector(&orgin, &z_axis);

	glColor3f(.2, .2, .2);
	glLineWidth(1.0);
	glBegin(GL_LINES);
	//Linien in z-Richtung zeichnen
	for (int i = 0; i < 10; i++) {
		glVertex3i(i, 0, 10);
		glVertex3i(i, 0, -10);
		glVertex3i(-i, 0, 10);
		glVertex3i(-i, 0, -10);
	}
	//Linien in x-Richtung zeichnen
	for (int i = 0; i < 10; i++) {
		glVertex3i(10, 0, i);
		glVertex3i(-10, 0, i);
		glVertex3i(10, 0, -i);
		glVertex3i(-10, 0, -i);
	}
	glEnd();
}

void Renderer::setCutRenderInfo(CutRender_info* info) {
	//sind bereits 2D-Schnittebeneninformationen verknüpft?
	if (cut_visualisation_info != NULL) {
		//löschen dieser
		for (int i = 0; i < 3; i++) {
			delete cut_visualisation_info->tri->getVert(i);
		}

		delete cut_visualisation_info->tri;
		delete cut_visualisation_info;
	}

	cut_visualisation_info = info;
}
void Renderer::setObject(ObjectData* obj) {

	object = obj;

	//Löschen der Geometriedaten auf der Grafikkarte
	if (displayList > -1) {
		glDeleteLists(displayList, 1);
	}

	//Generieren einer neuen Displayliste
	displayList = glGenLists(1);
	if (!glIsList(displayList)) {
		cout << "could not create display list!" << endl;
	}

	//Speichern der neuen Geometriedaten auf der Grafikkarte
	glNewList(displayList, GL_COMPILE);
	for (unsigned int i = 0; i < obj->getMaterials()->size(); i++) {
		if (object->getMaterials()->at(i).visible) {
			renderMaterial(&object->getMaterials()->at(i));
		}
		if (viewport.show_sensordata) {
			SensorData* sd = &object->getSensorDataList()->at(
					object->getCurrentSensorIndex());
			renderSensorData(&sd->data.at(sd->current_time_index));
		}
	}
	glEndList();
}
wxImage* Renderer::getViewportImage() {

	//Auslesen des OpenGL-Viewports
	GLint view[4];
	glGetIntegerv(GL_VIEWPORT, view);

	//Auslesen des Bildes aus Opengl
	void* pixels = malloc(3 * view[2] * view[3]); //OpenGL setzt malloc voraus
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, pixels);

	//Die Ausgelesenen Farbinformationen in ein wxImage schreiben
	wxImage* img = new wxImage((int) view[2], (int) view[3]);
	for (int i = 0; i < view[2] * view[3]; i++) {
		img->SetRGB(i % view[2], view[3] - i / view[2] - 1,
				((unsigned char*) pixels)[3 * i],
				((unsigned char*) pixels)[3 * i + 1],
				((unsigned char*) pixels)[3 * i + 2]);
	}
	free(pixels);

	//Auslesen der Tiefeninformationen aus Opengl
	void* depth_buff = malloc(view[2] * view[3]); //OpenGL setzt malloc voraus
	glReadPixels(0, 0, view[2], view[3], GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
			depth_buff);

	//Maskieren aller umbeschriebenen Bereiche im Bild
	img->InitAlpha();
	for (int i = 0; i < view[2] * view[3]; i++) {
		if (((unsigned char*) depth_buff)[i] == 255) {
			img->SetAlpha(i % view[2], view[3] - i / view[2] - 1, 0);
		}
	}

	free(depth_buff);

	return img;
}

/**
 * Visualisiert Informationen über eine 2D-Temperaturverteilung.
 * @param info Die zu visualisierenden Informationen.
 */
void drawCutRenderInfo(CutRender_info* info) {

	//Die Visualisierung soll nicht verdeckt weden
	glDepthFunc(GL_ALWAYS);
	//Blending für das die Ebene definierende Dreieck
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_LIGHTING);

	//zeichnen des die Ebene definierenden Dreiecks
	Triangle* tri = info->tri;
	glColor4f(.5, 0, 0, .5);
	glBegin(GL_TRIANGLES);
		Vector3D* nor = tri->getNormal();
		glNormal3dv(nor->getXYZ());
		glVertex3dv(tri->getV1()->getXYZ());
		glVertex3dv(tri->getV2()->getXYZ());
		glVertex3dv(tri->getV3()->getXYZ());
	glEnd();
	glDisable(GL_BLEND);

	//Vektoren zur Visualisierung der X- und Y-Achse der Ebene
	Vector3D* xvec = tri->getV2()->copy();
	xvec->sub(tri->getV1());
	Vector3D* yvec = nor->crossProduct(xvec);
	xvec->normalize();
	yvec->normalize();
	xvec->mult(
			info->img_width / 2
					* info->mmperpixel / 1000.);
	yvec->mult(
			info->img_height / 2
					* info->mmperpixel / 1000.);

	//Zeichnen der Achsen als Pfeile
	glColor3f(1, 0, 0);
	drawVector(tri->getV1(), xvec);
	glColor3f(0, 1, 0);
	drawVector(tri->getV1(), yvec);

	//Zeichnen des Rahmens für den dargestellten Bereich
	glColor4f(0, 0, 0, 1);
	Vector3D* tl = tri->getV1()->copy();
	tl->sub(xvec);
	tl->add(yvec);
	Vector3D* tr = tri->getV1()->copy();
	tr->add(xvec);
	tr->add(yvec);
	Vector3D* bl = tri->getV1()->copy();
	bl->sub(xvec);
	bl->sub(yvec);
	Vector3D* br = tri->getV1()->copy();
	br->add(xvec);
	br->sub(yvec);

	glBegin(GL_LINES);
		glVertex3dv(tl->getXYZ());
		glVertex3dv(tr->getXYZ());

		glVertex3dv(tl->getXYZ());
		glVertex3dv(bl->getXYZ());

		glVertex3dv(br->getXYZ());
		glVertex3dv(tr->getXYZ());

		glVertex3dv(br->getXYZ());
		glVertex3dv(bl->getXYZ());
	glEnd();

	delete tl;
	delete tr;
	delete bl;
	delete br;
	delete nor;
	delete xvec;
	delete yvec;

	//normaler Tiefentest
	glDepthFunc(GL_LESS);
}
void Renderer::render() {

	//Löschen der Farb- und Tiefeninformationen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Transformieren der Kamera
	glLoadIdentity();
	glTranslatef(0, 0, -viewport.zoom * viewport.zoom);
	glRotatef(viewport.rotationY, 1.0, 0.0, 0.0);
	glRotatef(viewport.rotationX, 0.0, 1.0, 0.0);
	glTranslatef(viewport.cameraPosition->getX(),
			viewport.cameraPosition->getY(), viewport.cameraPosition->getZ());

	//zeichnen der Orientierungslinien
	glDisable(GL_LIGHTING);
	renderGrid();

	//Objektskalierung, Licht
	glScalef(viewport.scale, viewport.scale, viewport.scale);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	GLfloat DiffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat AmbientLight[] = { .1, .1, .1 };
	GLfloat LightPosition[] = { -1, 1, 1, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);

	//Ist die Geometrie auf der Grafikkarte gespeichert?
	if (displayList > -1) {
		//zeichnen der Objektgeometrie
		glCallList(displayList);
	}

	//Sind Visualisierungsinformationen zur 2D-Temperaturverteilung verknüpft?
	if (cut_visualisation_info != NULL) {
		//zeichnen der Visualisierungsinformationen zur 2D-Temperaturverteilung
		drawCutRenderInfo(cut_visualisation_info);
	}

}

Renderer::~Renderer() {
	//löschen der Geometrie von der Grafikkarte
	glDeleteLists(displayList, 1);
}


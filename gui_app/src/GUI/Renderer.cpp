/*
 * Renderer.cpp
 *
 *  Created on: 27.07.2013
 *      Author: valentin
 */

#include "Renderer.h"

#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/gl.h>


using namespace std;

Renderer::Renderer() {
	object = NULL;
	displayList = -1;
	viewport.zoom = 3;
	//delete viewport.cameraPosition;
	viewport.cameraPosition = new Vector3D(0,0,0);
	viewport.rotationY = 0;
	viewport.rotationX = 0;
	viewport.invertcut = true;
	viewport.cut = NULL;
	viewport.showpoints = RM_NONE;
	viewport.showedges = RM_VALUECOLOR;
	viewport.showfaces = RM_NONE;
	viewport.show_extrapolated = true;
	viewport.show_sensordata = true;
	/*Vector3D *p1 = new Vector3D(0,1,-5);
	Vector3D *p2 = new Vector3D(0,1,5);
	Vector3D *p3 = new Vector3D(3,3,0);
	viewport.cut = new Triangle(p1,p2,p3);*/
	cut_visualisation_info = NULL;
	cout << "init renderer!" << endl;
}
bool pointBehindCut(Vector3D* point,Triangle* cut) {
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
	bool behind = (normal->dotProduct(tpoint)<0);
	delete normal;
	delete tpoint;
	return behind;
}
void Renderer::renderTetrahedrons(MaterialData* mat,int rendermode) {
	tetgenio* io = mat->tetgenoutput;
	glEnable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
	//cout << "nof: "<<obj->numberoftrifaces<<endl;
	for (int j=0;j<io->numberoftetrahedra;j++) {
		bool contains_extrapolated_points = false;
		for (int i=0;i<4;i++) {
			if (mat->extrapolated[io->tetrahedronlist[4*j+i]]) {
				contains_extrapolated_points = true;
			}
		}
		if (contains_extrapolated_points  && !viewport.show_extrapolated) {
			continue;
		}
		Vector3D v1 = Vector3D(&io->pointlist[3*io->tetrahedronlist[4*j]]);
		Vector3D v2 = Vector3D(&io->pointlist[3*io->tetrahedronlist[4*j+1]]);
		Vector3D v3 = Vector3D(&io->pointlist[3*io->tetrahedronlist[4*j+2]]);
		Vector3D v4 = Vector3D(&io->pointlist[3*io->tetrahedronlist[4*j+3]]);
		Vector3D center = Vector3D(0,0,0);
		center.add(&v1);
		center.add(&v2);
		center.add(&v3);
		center.add(&v4);
		center.mult(.25);
		if (viewport.cut!=NULL) {
			if (pointBehindCut(&center,viewport.cut)!=viewport.invertcut) {
				continue;
			}
		}
		v2.sub(&v1);
		v3.sub(&v1);
		v4.sub(&v1);
		v1.sub(&v1);

		Vector3D* n1 = v2.crossProduct(&v3);
		if (v4.dotProduct(n1)>0) {
			n1->mult(-1);
		}
		Vector3D* n2 = v4.crossProduct(&v2);
		if (v3.dotProduct(n2)>0) {
			n2->mult(-1);
		}
		Vector3D* n3 = v3.crossProduct(&v4);
		if (v2.dotProduct(n3)>0) {
			n3->mult(-1);
		}
		Vector3D* backedge1 = v2.copy();
		Vector3D* backedge2 = v3.copy();
		backedge1->sub(&v4);
		backedge2->sub(&v4);
		Vector3D* n4 = backedge1->crossProduct(backedge2);
		if (v4.dotProduct(n4)<0) {
			n4->mult(-1);
		}
		n1->normalize();
		n2->normalize();
		n3->normalize();
		n4->normalize();
		float* colors[4];
		if (rendermode==RM_MATERIALCOLOR) {
			glColor4f(mat->color[0],mat->color[1],mat->color[2],.3);
		} else {
			for (int k=0;k<4;k++) {
				float value = io->pointattributelist[io->numberofpointattributes*(io->tetrahedronlist[4*j+k])];
				if (value>100) {
					value = 100;
				}
				if (value<0) {
					value = 0;
				}
				colors[k] = hsvToRgb((1.0-(float)value/100.)*.65,1,1);
			}
		}
		int vertlist[4][3] = {{0,1,2},{0,1,3},{0,2,3},{1,2,3}};
		Vector3D* normallist[4] = {n1,n2,n3,n4};
		for (int k=0;k<4;k++) {
			glNormal3dv(normallist[k]->getXYZ());
			if(rendermode==RM_VALUECOLOR) {glColor3fv(colors[vertlist[k][0]]);}
			glVertex3dv(&io->pointlist[3*io->tetrahedronlist[4*j+vertlist[k][0]]]);
			if(rendermode==RM_VALUECOLOR) {glColor3fv(colors[vertlist[k][1]]);}
			glVertex3dv(&io->pointlist[3*io->tetrahedronlist[4*j+vertlist[k][1]]]);
			if(rendermode==RM_VALUECOLOR) {glColor3fv(colors[vertlist[k][2]]);}
			glVertex3dv(&io->pointlist[3*io->tetrahedronlist[4*j+vertlist[k][2]]]);

		}
		delete backedge1;
		delete backedge2;
		delete n1;
		delete n2;
		delete n3;
		delete n4;
		if (rendermode==RM_VALUECOLOR) {
			for (int k=0;k<4;k++) {
				delete[] colors[k];
			}
		}
	}
	glEnd();
}
void Renderer::renderSensorData(vector<SensorPoint>* data) {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glBegin(GL_POINTS);
	for (unsigned int i=0;i<data->size();i++) {
		SensorPoint* point = &data->at(i);
		float value = point->temperature;
		if (value>100) {
			value = 100;
		}
		if (value==-1) {
			continue;
		}
		float* color = hsvToRgb((1.0-value/100.)*.65,1,1);
		glPointSize(6.0);
		glColor3f(0,0,0);
		glVertex3dv(point->coords);
		glPointSize(4.0);
		glColor3fv(color);
		glVertex3dv(point->coords);
		delete[] color;
	}
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}
void Renderer::renderMaterial(MaterialData* mat) {
	tetgenio* io = mat->tetgenoutput;
	if (viewport.showpoints) {
		glPointSize(3.0);
		glDisable(GL_LIGHTING);
		glBegin(GL_POINTS);
		if (viewport.showpoints==RM_MATERIALCOLOR) {
			glColor3fv(mat->color);
		}
		for (int i=0;i<io->numberofpoints;i++) {

			if (mat->extrapolated[i] && !viewport.show_extrapolated) {
				continue;
			}
			if (viewport.showpoints==RM_VALUECOLOR) {
				float value = io->pointattributelist[io->numberofpointattributes*(i)];
				if (value>100) {
					value = 100;
				}
				if (value==-1) {
					continue;
				}
				float* color = hsvToRgb((1.0-(float)value/100.)*.65,1,1);

				glColor3fv(color);
				glVertex3dv(&io->pointlist[3*(i)]);
				delete[] color;
			}
			if (viewport.showpoints==RM_MATERIALCOLOR) {
				glVertex3dv(&io->pointlist[3*(i)]);
			}
		}
		glEnd();
	}
	if (viewport.showedges) {
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		renderTetrahedrons(mat,viewport.showedges);
	}

	if (viewport.showfaces) {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
		renderTetrahedrons(mat,viewport.showfaces);

	}
	//glEnable(GL_DEPTH_TEST);
}
void Renderer::initGL(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double h = (double)height/(double)width;
	glFrustum(-1.0,1.0,-h,h,1.5,500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(.5,.5,.5,1);
}
void Renderer::resize(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double h = (double)height/(double)width;
	glFrustum(-1.0,1.0,-h,h,1.5,500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void renderGrid() {
	glColor3f(.2,.2,.2);
	glLineWidth(1.0);
	glBegin(GL_LINES);

		for (int i=0;i<10;i++) {
			glVertex3i(i,0,10);
			glVertex3i(i,0,-10);
			glVertex3i(-i,0,10);
			glVertex3i(-i,0,-10);
		}
		for (int i=0;i<10;i++) {
			glVertex3i(10,0,i);
			glVertex3i(-10,0,i);
			glVertex3i(10,0,-i);
			glVertex3i(-10,0,-i);
		}
	glEnd();
	glLineWidth(2.0);
	glBegin(GL_LINES);
		glColor3f(0.8,0,0);
		glVertex3i(-10,0,0);
		glVertex3i(10,0,0);

		glColor3f(0,.8,0);
		glVertex3i(0,-10,0);
		glVertex3i(0,10,0);

		glColor3f(0,0,.8);
		glVertex3i(0,0,-10);
		glVertex3i(0,0,10);
	glEnd();
	glLineWidth(1.0);
}
void drawVector(Vector3D* pos,Vector3D* dir) {
	Vector3D tip  = Vector3D(pos);
	Vector3D ndir = Vector3D(dir);
	ndir.normalize();
	Vector3D refdir = Vector3D(1,0,0);
	if (refdir.equals(&ndir)) {
		refdir.getXYZ()[0] = 0;
		refdir.getXYZ()[1] = 1;
	}
	float radius = .08;
	float length = .16;
	Vector3D* refvec = ndir.crossProduct(&refdir);
	Vector3D* refvec2= ndir.crossProduct(refvec);
	refvec->normalize();
	refvec2->normalize();
	Vector3D v1 = Vector3D(refvec);
	Vector3D v2 = Vector3D(refvec2);
	Vector3D v3 = Vector3D(v1);
	v3.mult(-1);
	Vector3D v4 = Vector3D(refvec2);
	v4.mult(-1);
	// Line:
	tip.add(dir);
	Vector3D behind = Vector3D(pos);
	behind.sub(dir);
	glLineWidth(2.0);
	glBegin(GL_LINES);
		glVertex3dv(behind.getXYZ());
		glVertex3dv(tip.getXYZ());
	glEnd();
	// Tip:
	v1.mult(radius);
	v2.mult(radius);
	v3.mult(radius);
	v4.mult(radius);
	ndir.mult(dir->getLength()-length);
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
	delete refvec;
	delete refvec2;
}
void Renderer::setCutRenderInfo(CutRender_info* info) {
	if (cut_visualisation_info!=NULL) {
		for (int i=0;i<3;i++) {
			delete cut_visualisation_info->tri->getVert(i);
		}
		delete cut_visualisation_info->tri;
		delete cut_visualisation_info;
	}
	cut_visualisation_info = info;
}
void Renderer::setObject(ObjectData* obj) {
	object = obj;
	if (displayList>-1) {
		glDeleteLists(displayList,1);
	}
	displayList = glGenLists(1);
	if (!glIsList(displayList)) {
		cout << "could not create display list!" << endl;
	}
	glNewList(displayList, GL_COMPILE);

	for (unsigned int i=0;i<obj->materials.size();i++) {
		if (object->materials.at(i).visible) {
			renderMaterial(&object->materials.at(i));
		}
		if (viewport.show_sensordata) {
			SensorData* sd = &object->sensordatalist.at(object->current_sensor_index);
			renderSensorData(&sd->data.at(sd->current_time_index));
		}
	}
	glEndList();
}
void Renderer::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0,0,-viewport.zoom*viewport.zoom);
	glRotatef(viewport.rotationY,1.0,0.0,0.0);
	glRotatef(viewport.rotationX,0.0,1.0,0.0);
	glTranslatef(viewport.cameraPosition->getX(),viewport.cameraPosition->getY(),viewport.cameraPosition->getZ());
	glDisable(GL_LIGHTING);
	renderGrid();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	GLfloat DiffuseLight[] = {1.0,1.0,1.0,1.0};
	GLfloat AmbientLight[] = {.1, .1, .1};
	GLfloat LightPosition[] = {10, 40, 10, 0};
	glLightfv (GL_LIGHT0, GL_POSITION, LightPosition);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv (GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);

	if (displayList>-1) {
		glCallList(displayList);
	}

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	if (cut_visualisation_info!=NULL) {
		glClear(GL_DEPTH_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable( GL_BLEND );
		glDisable(GL_LIGHTING);
		Triangle* tri = cut_visualisation_info->tri;
		glBegin(GL_TRIANGLES);
			glColor4f(.5,0,0,.5);
			Vector3D* nor = tri->getNormal();
			glNormal3dv(nor->getXYZ());
			glVertex3dv(tri->getV1()->getXYZ());
			glVertex3dv(tri->getV2()->getXYZ());
			glVertex3dv(tri->getV3()->getXYZ());
		glEnd();
		glDisable( GL_BLEND );
		Vector3D* xvec = tri->getV2()->copy();
		xvec->sub(tri->getV1());
		Vector3D* yvec = nor->crossProduct(xvec);
		xvec->normalize();
		yvec->normalize();
		xvec->mult(cut_visualisation_info->img_width/2*cut_visualisation_info->mmperpixel/1000.);
		yvec->mult(cut_visualisation_info->img_height/2*cut_visualisation_info->mmperpixel/1000.);
		glColor3f(1,0,0);
		drawVector(tri->getV1(),xvec);
		glColor3f(0,1,0);
		drawVector(tri->getV1(),yvec);
		delete nor;
		delete xvec;
		delete yvec;
	}

}

Renderer::~Renderer() {
	glDeleteLists(displayList,1);
}


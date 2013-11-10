/*
 * GUIRenderCutCanvas.cpp
 *
 *  Created on: 03.10.2013
 *      Author: valentin
 */

#include "GUIRenderCutCanvas.h"
#include "GUICutRenderWindow.h"
#include <vector>
#include "../processing/utils.h"
#include "../processing/ObjectData.h"
#include "GUIMainWindow.h"
#include "Renderer.h"
#include <iostream>
BEGIN_EVENT_TABLE(GUIRenderCutCanvas, wxPanel)
	EVT_PAINT    (GUIRenderCutCanvas::onCanvasPaint)
END_EVENT_TABLE()
using namespace std;
using namespace Utils;

extern std::vector<ObjectData*> data_objects;
extern int current_data_object_index;

GUIRenderCutCanvas::GUIRenderCutCanvas(wxWindow* parent):wxPanel(parent, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas")) {
	recalculate_img = 0;
	draw_grid = 0;
	image = new wxImage(100,100,true);
}
void GUIRenderCutCanvas::renderImage(wxImage& image) {
	int width = image.GetWidth();
	int height = image.GetHeight();
	setlocale(LC_NUMERIC, "C");
	GUICutRenderWindow* pw = dynamic_cast<GUICutRenderWindow*>(GetParent());
	CutRender_info* info = pw->getCutRenderProperties();
	Viewport_info* vis_info = &dynamic_cast<GUIMainWindow*>(pw->GetParent())->getGLCanvas()->renderer.viewport;
	Triangle* tri = info->tri;
	Vector3D* xvec = tri->getV2()->copy();
	xvec->sub(tri->getV1());
	Vector3D* tri_nor = tri->getNormal();
	Vector3D* yvec = xvec->crossProduct(tri_nor);
	delete tri_nor;
	ObjectData* obj = data_objects.at(current_data_object_index);
	xvec->normalize();
	yvec->normalize();
	Interpolator interpolator;
	vector<tetgenio*> bases(obj->materials.size());
	for (unsigned int i=0;i<obj->materials.size();i++) {
		tetgenio* tri_io = new tetgenio();
		char arg = 'Q';
		tetrahedralize(&arg, obj->materials.at(i).tetgeninput, tri_io,NULL,NULL);
		bases.at(i) = tri_io;
	}
	yvec->print();
	wxString original_window_title = pw->GetTitle();
	for (int x=0;x<width;x++) {
		for (int y=0;y<=height;y++) {
			Vector3D* p = tri->getV1()->copy();
			Vector3D* part_x = xvec->copy();
			part_x->mult(x*info->mmperpixel/1000.-width*info->mmperpixel/2000);
			Vector3D* part_y = yvec->copy();
			part_y->mult(y*info->mmperpixel/1000.-height*info->mmperpixel/2000);
			p->add(part_x);
			p->add(part_y);
			delete part_x;
			delete part_y;


			image.SetRGB(x,y,0,0,0);
			image.SetAlpha(x,y,0);
			for (unsigned int m=0;m<obj->materials.size();m++) {
				MaterialData* mat = &obj->materials.at(m);
				bool found = pointInsideMesh(p,bases.at(m));
				if (found) {
					int status = 0;
					interpolator.setMode(mat->interpolation_mode);
					SensorData* sd = &obj->sensordatalist.at(obj->current_sensor_index);
					double value = getPointValue(status,&sd->data.at(sd->current_time_index),p->getXYZ(),&interpolator);
					float* color = hsvToRgb((1.0-clampHue((value-vis_info->min_visualisation_temp)/(vis_info->max_visualisation_temp+vis_info->min_visualisation_temp)))*.666,1,1);
					image.SetAlpha(x,y,255);
					image.SetRGB(x,y,(unsigned char)(color[0]*255),(unsigned char)(color[1]*255),(unsigned char)(color[2]*255));

					delete color;
					break;
				}
			}
			delete p;
		}
		pw->SetTitle(original_window_title+wxT(" (")+floattowxstr(int((float)x/(float)width*100))+wxT("%)"));
		pw->Update();
	}
	for (int i=0;i<3;i++) {
		delete tri->getVert(i);
	}
	delete tri;
	pw->SetTitle(original_window_title);
	delete xvec;
	delete yvec;
	for (unsigned int i=0;i<obj->materials.size();i++) {
		delete bases.at(i);
	}
}
void GUIRenderCutCanvas::onCanvasPaint(wxPaintEvent &event) {

	 // Create paint DC
	wxPaintDC dc(this);
	// Create graphics context from it
	//wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	int width = 0;
	int height = 0;
	GetSize(&width,&height);
	// Clear BG
	dc.SetPen( *wxBLACK_PEN );
	dc.SetBrush( *wxBLACK_BRUSH);
	dc.DrawRectangle(0, 0, width, height);
	if (recalculate_img) {
		image->Rescale(width,height);
		image->InitAlpha();
		cout << image->HasAlpha() << endl;
		renderImage(*image);
		wxBitmap drawbmp(*image);
		dc.DrawBitmap(drawbmp,0,0);
	}
	if (draw_grid) {
		dc.SetPen( *wxGREY_PEN );
		dc.SetPen( *wxRED_PEN );
		dc.DrawLine(0,height/2,width,height/2);
		dc.SetPen( *wxGREEN_PEN );
		dc.DrawLine(width/2,0,width/2,height);
	}
	recalculate_img = 0;
}
GUIRenderCutCanvas::~GUIRenderCutCanvas() {
}


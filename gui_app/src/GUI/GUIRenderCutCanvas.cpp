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
}
void GUIRenderCutCanvas::renderImage(wxImage& image) {
	int width = image.GetWidth();
	int height = image.GetHeight();
	setlocale(LC_NUMERIC, "C");
	GUICutRenderWindow* pw = dynamic_cast<GUICutRenderWindow*>(GetParent());
	CutRender_info* info = pw->getCutRenderProperties();
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
			for (unsigned int m=0;m<obj->materials.size();m++) {
				MaterialData* mat = &obj->materials.at(m);
				bool found = pointInsideMesh(p,bases.at(m));
				if (found) {
					int status = 0;
					interpolator.setMode(mat->interpolation_mode);
					double value = getPointValue(status,&obj->sensordatalist.at(obj->current_sensor_index).points,p->getXYZ(),&interpolator);
					if (value>100) {
						value = 100;
					}
					if (value<0) {
						value = 0;
					}
					float* color = hsvToRgb((1.0-(float)value/100.)*.65,1,1);
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
	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	int width = 0;
	int height = 0;
	GetSize(&width,&height);
	if (gc)
	{
		// Clear BG
		gc->SetPen( *wxWHITE_PEN );
		gc->SetBrush( *wxWHITE_BRUSH);
		wxGraphicsPath path1 = gc->CreatePath();
		path1.AddRectangle(0, 0, width, height);
		gc->FillPath(path1);
		if (recalculate_img) {
			wxImage renderimg(width,height,false);
			renderImage(renderimg);
			wxBitmap drawbmp(renderimg);
			gc->DrawBitmap(drawbmp,0,0,width,height);
		}
		if (draw_grid) {
			gc->SetPen( *wxGREY_PEN );
			wxGraphicsPath gridpath2 = gc->CreatePath();
			gc->SetPen( *wxRED_PEN );
			gc->StrokeLine(0,height/2,width,height/2);
			gc->SetPen( *wxGREEN_PEN );
			gc->StrokeLine(width/2,0,width/2,height);
		}
		delete gc;
	}
	recalculate_img = 0;
}
GUIRenderCutCanvas::~GUIRenderCutCanvas() {
}


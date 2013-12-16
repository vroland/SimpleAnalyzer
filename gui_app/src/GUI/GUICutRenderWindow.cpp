/*
 * GUICutRenderWindow.cpp
 *
 *  Created on: 01.10.2013
 *      Author: valentin
 */

#include "GUICutRenderWindow.h"
#include "constants.h"
#include <vector>
#include "../processing/Analyzer.h"
#include "../processing/utils.h"
#include "../libraries/interpolate/Interpolator.h"
#include "GUIMainWindow.h"
BEGIN_EVENT_TABLE(GUICutRenderWindow, wxFrame)
	EVT_BUTTON(ID_RENDER_CUT_BT, GUICutRenderWindow::renderCutBtClick)
	EVT_BUTTON(ID_EXPORT_CUT_BT, GUICutRenderWindow::exportImage)
	EVT_TEXT(ID_CUT_TRI_EDIT, GUICutRenderWindow::OnCutPropsChanged)
	EVT_SPINCTRL(ID_CUT_TRI_EDIT, GUICutRenderWindow::OnSCutPropsChanged_spin)
	EVT_SIZE(GUICutRenderWindow::OnResize)
END_EVENT_TABLE()

extern std::vector<ObjectData*> data_objects;
extern int current_data_object_index;

GUICutRenderWindow::GUICutRenderWindow(wxWindow * parent,const wxChar *title, int xpos, int ypos, int width, int height):
	wxFrame(parent, -1, title, wxPoint(xpos, ypos), wxSize(width, height), wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT) {
	trilabel= new wxStaticText(this,wxID_ANY,wxT("Dreiecksebene (Punkt1 ist Mittelpunkt):"));
	p1label = new wxStaticText(this,wxID_ANY,wxT("Punkt 1:"));
	p1xedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));
	p1yedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));
	p1zedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));

	p2label = new wxStaticText(this,wxID_ANY,wxT("Punkt 2:"));
	p2xedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("1.0"));
	p2yedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));
	p2zedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));

	p3label = new wxStaticText(this,wxID_ANY,wxT("Punkt 3:"));
	p3xedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));
	p3yedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("1.0"));
	p3zedit = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("0.0"));

	optionslbl = new wxStaticText(this,wxID_ANY,wxT("Optionen:"));
	whlbl	   = new wxStaticText(this,wxID_ANY,wxT("Breite/Höhe:"));
	imgWidthEdit = new wxSpinCtrl(this,ID_CUT_TRI_EDIT);
	imgWidthEdit->SetRange(1,100000000);
	imgWidthEdit->SetValue(800);
	imgHeightEdit = new wxSpinCtrl(this,ID_CUT_TRI_EDIT);
	imgHeightEdit->SetRange(1,100000000);
	imgHeightEdit->SetValue(600);
	mmperpixellabel = new wxStaticText(this,wxID_ANY,wxT("Maßstab (mm/px):"));
	mmperpixeledit  = new wxTextCtrl(this,ID_CUT_TRI_EDIT,wxT("15.0"));

	calcbt= new wxButton(this,ID_RENDER_CUT_BT,wxT("Analysieren"));
	exportbt= new wxButton(this,ID_EXPORT_CUT_BT,wxT("Export (.png)..."));
	canvas = new GUIRenderCutCanvas(this);
	p1xedit->SetValue(p1xedit->GetValue()); //TEXT-Event auslösen
	value_img = new float[3*3];
	image = new wxImage(100,100,true);
	canvas->setImage(image);
	canvas->setValueImg(value_img);
	Update();
}
void GUICutRenderWindow::renderImage(wxImage* image) {
	int width = imgWidthEdit->GetValue();
	int height = imgHeightEdit->GetValue();
	image->Rescale(width,height,wxIMAGE_QUALITY_NORMAL);
	image->InitAlpha();
	setlocale(LC_NUMERIC, "C");
	CutRender_info* info = getCutRenderProperties();
	Viewport_info* vis_info = &dynamic_cast<GUIMainWindow*>(GetParent())->getGLCanvas()->renderer.viewport;
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
	if (value_img!=NULL) {
		delete[] value_img;
	}
	value_img = new float[width*height];
	wxString original_window_title = GetTitle();
	for (int x=0;x<width;x++) {
		for (int y=0;y<height;y++) {
			Vector3D* p = tri->getV1()->copy();
			Vector3D* part_x = xvec->copy();
			part_x->mult(x*info->mmperpixel/1000.-width*info->mmperpixel/2000);
			Vector3D* part_y = yvec->copy();
			part_y->mult(y*info->mmperpixel/1000.-height*info->mmperpixel/2000);
			p->add(part_x);
			p->add(part_y);
			delete part_x;
			delete part_y;
			image->SetRGB(x,y,0,0,0);
			image->SetAlpha(x,y,0);
			value_img[y*width+x] = 0;
			for (unsigned int m=0;m<obj->materials.size();m++) {
				MaterialData* mat = &obj->materials.at(m);
				bool found = pointInsideMesh(p,bases.at(m));
				if (found) {
					int status = 0;
					interpolator.setMode(mat->interpolation_mode);
					SensorData* sd = &obj->sensordatalist.at(obj->current_sensor_index);
					float value = (float)getPointValue(status,&sd->data.at(sd->current_time_index),p->getXYZ(),&interpolator);
					value_img[y*width+x] = value;
					float* color = hsvToRgb((1.0-clampHue((value-vis_info->min_visualisation_temp)/(vis_info->max_visualisation_temp+vis_info->min_visualisation_temp)))*.666,1,1);
					image->SetAlpha(x,y,255);
					image->SetRGB(x,y,(unsigned char)(color[0]*255),(unsigned char)(color[1]*255),(unsigned char)(color[2]*255));

					delete color;
					break;
				}
			}
			delete p;
		}
		SetTitle(original_window_title+wxT(" (")+floattowxstr(int((float)x/(float)width*100))+wxT("%)"));
		Update();
	}
	for (int i=0;i<3;i++) {
		delete tri->getVert(i);
	}
	delete tri;
	SetTitle(original_window_title);
	delete xvec;
	delete yvec;
	for (unsigned int i=0;i<obj->materials.size();i++) {
		delete bases.at(i);
	}
}
void GUICutRenderWindow::OnCutPropsChanged(wxCommandEvent &event) {
	refreshVisualisation();
}
void GUICutRenderWindow::OnSCutPropsChanged_spin(wxSpinEvent &event) {
	refreshVisualisation();
}
void GUICutRenderWindow::refreshVisualisation() {
	GUIMainWindow* parent = (GUIMainWindow*) GetParent();
	parent->getGLCanvas()->renderer.setCutRenderInfo(this->getCutRenderProperties());
	parent->getGLCanvas()->Refresh(false,NULL);
}
void GUICutRenderWindow::OnResize(wxSizeEvent &event) {
	int width = 0;
	int height = 0;
	GetSize(&width,&height);
	trilabel->SetSize(10,10,300,20);
	p1label->SetSize(20,30,300,20);
	p1xedit->SetSize(90,30,70,20);
	p1yedit->SetSize(160,30,70,20);
	p1zedit->SetSize(230,30,70,20);
	p2label->SetSize(20,60,300,20);
	p2xedit->SetSize(90,60,70,20);
	p2yedit->SetSize(160,60,70,20);
	p2zedit->SetSize(230,60,70,20);
	p3label->SetSize(20,90,300,20);
	p3xedit->SetSize(90,90,70,20);
	p3yedit->SetSize(160,90,70,20);
	p3zedit->SetSize(230,90,70,20);
	optionslbl->SetSize(310,10,300,20);
	whlbl->SetSize(320,30,200,20);
	imgWidthEdit->SetSize(320,50,70,20);
	imgHeightEdit->SetSize(390,50,70,20);
	mmperpixellabel->SetSize(320,70,200,40);
	mmperpixeledit->SetSize(320,90,140,20);
	calcbt->SetSize(480,20,150,30);
	exportbt->SetSize(480,50,150,30);
	canvas->SetSize(10,120,width-20,height-canvas->GetPosition().y-10);
	refreshVisualisation();
	canvas->Refresh(false,NULL);

}
void GUICutRenderWindow::exportImage(wxCommandEvent &event) {
	wxFileDialog *SaveDialog= new wxFileDialog(this, wxT("Speichern unter..."), _(""), _(""), _("Portable Network Graphics (*.png)|*.png"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if ( SaveDialog->ShowModal() == wxID_OK )
	{
		cout << SaveDialog->GetFilename().ToAscii() << endl;
		image->SaveFile(SaveDialog->GetPath(),wxBITMAP_TYPE_PNG);
	}
	SaveDialog->Close();
	SaveDialog->Destroy();
}
CutRender_info* GUICutRenderWindow::getCutRenderProperties() {
	CutRender_info* info = new CutRender_info();
	info->tri = new Triangle(new Vector3D(atof(p1xedit->GetValue().ToAscii()),atof(p1yedit->GetValue().ToAscii()),atof(p1zedit->GetValue().ToAscii())),
				new Vector3D(atof(p2xedit->GetValue().ToAscii()),atof(p2yedit->GetValue().ToAscii()),atof(p2zedit->GetValue().ToAscii())),
				new Vector3D(atof(p3xedit->GetValue().ToAscii()),atof(p3yedit->GetValue().ToAscii()),atof(p3zedit->GetValue().ToAscii())));
	info->mmperpixel = atof(mmperpixeledit->GetValue().ToAscii());
	info->img_width = imgWidthEdit->GetValue();
	info->img_height= imgHeightEdit->GetValue();
	return info;
}
void GUICutRenderWindow::renderCutBtClick(wxCommandEvent &event) {
	canvas->setValueImg(NULL);
	renderImage(image);
	canvas->setImage(image);
	canvas->setValueImg(value_img);
	canvas->Refresh(false,NULL);
}
GUICutRenderWindow::~GUICutRenderWindow() {
	delete image;
	delete[] value_img;
	GUIMainWindow* parent = (GUIMainWindow*) GetParent();
	if (parent!=NULL) {
		parent->render_cut_window_valid = false;
		parent->getGLCanvas()->renderer.setCutRenderInfo(NULL);
		parent->getGLCanvas()->Refresh(false,NULL);
	} else {
		cout << "parent is null!" << endl;
	}
}


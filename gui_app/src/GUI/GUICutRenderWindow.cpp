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
#include <thread>
#include <sys/time.h>
BEGIN_EVENT_TABLE(GUICutRenderWindow, wxFrame)
	EVT_BUTTON(ID_RENDER_CUT_BT, GUICutRenderWindow::renderCutBtClick)
	EVT_BUTTON(ID_EXPORT_CUT_BT, GUICutRenderWindow::exportImage)
	EVT_TEXT(ID_CUT_TRI_EDIT, GUICutRenderWindow::OnCutPropsChanged)
	EVT_SPINCTRL(ID_CUT_TRI_EDIT, GUICutRenderWindow::OnSCutPropsChanged_spin)
	EVT_SIZE(GUICutRenderWindow::OnResize)
END_EVENT_TABLE()

extern std::vector<ObjectData*> data_objects;
extern int current_data_object_index;

static struct timeval tm1;

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
	threadcountlbl = new wxStaticText(this,wxID_ANY,wxT("CPU-Threads:"));
	core_count = thread::hardware_concurrency();
	threadcountedit = new wxSpinCtrl(this,wxID_ANY);
	threadcountedit->SetRange(1,1000);
	threadcountedit->SetValue(core_count);

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
void render_thread(bool* status_flag,float* value_img,wxImage* image,int width,int height,int startheight,int delta_h,CutRender_info* info,Viewport_info* vis_info,Vector3D* xvec,Vector3D* yvec,Vector3D* v0,vector<tetgenio*>* bases,ObjectData* obj,vector<SensorPoint>* sensor_data) {
	Interpolator interpolator;
	for (int x=0;x<width;x++) {
		for (int y=startheight;y<startheight+delta_h;y++) {
			Vector3D* p = v0->copy();
			Vector3D* part_x = xvec->copy();
			part_x->mult(x*info->mmperpixel/1000.-width*info->mmperpixel/2000);
			Vector3D* part_y = yvec->copy();
			part_y->mult(y*info->mmperpixel/1000.-height*info->mmperpixel/2000);
			p->add(part_x);
			p->add(part_y);
			delete part_x;
			delete part_y;
			image->SetAlpha(x,y,0);
			image->SetRGB(x,y,0,0,0);
			value_img[y*width+x] = 0;
			for (unsigned int m=0;m<obj->materials.size();m++) {
				MaterialData* mat = &obj->materials.at(m);
				bool found = pointInsideMesh(p,bases->at(m));
				if (found) {
					int status = 0;
					interpolator.setMode(mat->interpolation_mode);
					float value = (float)getPointValue(status,sensor_data,p->getXYZ(),&interpolator);
					value_img[y*width+x] = value;
					float* color = hsvToRgb((1.0-clampHue((value_img[y*width+x]-vis_info->min_visualisation_temp)/(vis_info->max_visualisation_temp+vis_info->min_visualisation_temp)))*.666,1,1);
					image->SetRGB(x,y,(unsigned char)(color[0]*255),(unsigned char)(color[1]*255),(unsigned char)(color[2]*255));
					delete color;
					image->SetAlpha(x,y,255);
					break;
				}
			}
			delete p;
		}
	}
	*status_flag = 0;
}
void GUICutRenderWindow::renderImage(wxImage* image) {
	timeval tm1;
	gettimeofday(&tm1, NULL);
	int width = imgWidthEdit->GetValue();
	int height = imgHeightEdit->GetValue();
	core_count = threadcountedit->GetValue();
	delete image;
	image = new wxImage(width,height,true);
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
	vector<tetgenio*> bases(obj->materials.size());
	for (unsigned int i=0;i<obj->materials.size();i++) {
		tetgenio* tri_io = new tetgenio();
		char arg = 'Q';
		tetrahedralize(&arg, obj->materials.at(i).tetgeninput, tri_io,NULL,NULL);
		cout << tri_io->numberoftrifaces << endl;
		bases.at(i) = tri_io;
	}
	yvec->print();
	if (value_img!=NULL) {
		delete[] value_img;
	}
	value_img = new float[width*height];
	bool thread_running[core_count];
	for (int i=0;i<core_count;i++) thread_running[i] = 1;
	wxString original_window_title = GetTitle();

	vector<thread*> threads= vector<thread*>(0);
	vector<vector<SensorPoint>> copied_sensor_data = vector<vector<SensorPoint>>(core_count);
	int delta_h = height/core_count;
	for (int i=0;i<core_count;i++) {
		int startheight = delta_h*i;
		if (i==core_count-1) {
			if (startheight+delta_h<height) {
				delta_h = height-startheight;
				cout << "corrected delta_h";
			}
		}
		//copy sensor data for each thread
		SensorData* dataset = &obj->sensordatalist.at(obj->current_sensor_index);
		vector<SensorPoint>* original_sd = &dataset->data.at(dataset->current_time_index);
		copied_sensor_data.at(i).resize(original_sd->size());
		for (int p=0;p<int(original_sd->size());p++) {
			copySensorPoint(&original_sd->at(p),&copied_sensor_data.at(i).at(p));
		}
		threads.resize(threads.size()+1,new thread(render_thread,&thread_running[i],value_img,image,width,height,startheight,delta_h,info,vis_info,xvec,yvec,tri->getV1(),&bases,obj,&copied_sensor_data.at(i)));
	}
	unsigned int running = 0;
	do {
		Update();
		canvas->Refresh();
		running = 0;
		for (int i=0;i<core_count;i++) {
			running = running<<1;
			running+=thread_running[i];
		};
	} while (running);
	for (int i=0;i<core_count;i++) {
		threads.at(i)->join();
		delete threads.at(i);
	}
	for (int i=0;i<3;i++) {
		delete tri->getVert(i);
	}
	delete tri;
	timeval tm2;
	gettimeofday(&tm2, NULL);

	unsigned long long t = 1000 * (tm2.tv_sec - tm1.tv_sec) + (tm2.tv_usec - tm1.tv_usec) / 1000;
	SetTitle(wxT("Berechnung abgeschlossen. ( ")+floattowxstr(t/1000.)+wxT( "s )"));
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
	threadcountlbl->SetSize(480,30,100,20);
	threadcountedit->SetSize(580,30,50,20);
	calcbt->SetSize(480,50,150,30);
	exportbt->SetSize(480,80,150,30);
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


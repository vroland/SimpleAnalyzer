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
#include <wx-2.8/wx/wx.h>
using namespace std;

enum rendermode {
	RM_NONE = 0,
	RM_MATERIALCOLOR,
	RM_VALUECOLOR,
};
struct Viewport_info {
	float zoom;
	float rotationY;
	float rotationX;
	Vector3D* cameraPosition;
	Triangle* cut;
	bool invertcut;
	int showpoints;
	int showedges;
	int showfaces;
	bool show_extrapolated;
	bool show_sensordata;
	int width;
	int height;
	int max_visualisation_temp;
	int min_visualisation_temp;
};
struct CutRender_info {
	Triangle* tri;
	float mmperpixel;
	int img_width;
	int img_height;
};
class Renderer {
public:
	Renderer();
	void initGL(int width, int height);
	void resize(int width, int height);
	void render();
	void setObject(ObjectData* obj);
	void setCutRenderInfo(CutRender_info* info);
	wxImage* getViewportImage();
	Viewport_info viewport;
	virtual ~Renderer();
private:
	void renderMaterial(MaterialData* mat);
	void renderTetrahedrons(MaterialData* mat,int rendermode);
	void renderSensorData(vector<SensorPoint>* data);
	ObjectData* object;
	CutRender_info* cut_visualisation_info;
	int displayList;
};


#endif /* RENDERER_H_ */

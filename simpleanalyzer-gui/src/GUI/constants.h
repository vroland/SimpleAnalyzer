/*
 * event_ids.h
 *
 *  Created on: 07.09.2013
 *      Author: valentin
 */

#ifndef EVENT_IDS_H_
#define EVENT_IDS_H_

/** @file */

#include <wx/wx.h>
/**
 * Anzahl der verfügbaren Interpolationsmodi.
 */
const int NUMBER_OF_INTERPOLATION_MODES = 2;
/**
 * Bezeichnungen für die von "Interpolator" verwendeten Interpolationsmodi.
 */
const wxString INTERPOLATION_MODE_STRINGS[NUMBER_OF_INTERPOLATION_MODES] = {wxT("Linear"),wxT("Logarithmisch")};


/**
 * \enum EventID
 * IDs für die Events der Programmoberfläche. Müssen kleiner als wxID_LOWEST (wxWidgets 2.8: 4999) sein!
 */
enum EventID {
	ID_ABOUT = 1,
	ID_TEST,
	ID_IMPORT_OBJ,
	ID_IMPORT_SD,
	ID_RECALCBT,
	ID_MATERIALBOX,
	ID_ANALYZE,
	ID_GENERAL_PROP,
	ID_IMMEDIATE_UPDATE_PROP,
	ID_GENERAL_VIEW_PROP,
	ID_CHECKLISTBOX_VIEW_PROP,
	ID_CHANGE_ACTIVE_OBJ,
	ID_ANALYZE_POINT,
	ID_ANALYZE_POINT_BT,
	ID_CUT_CANVAS,
	ID_RENDER_CUT,
	ID_RENDER_CUT_BT,
	ID_CUT_TRI_EDIT,
	ID_DELETE_ACTIVE_OBJ,
	ID_IMPORT_TSD,
	ID_SD_BOX,
	ID_SD_TIMELINE,
	ID_ANALYZE_MARKER_CB,
	ID_CLEAR_MARKER_BT,
	ID_MARKER_NEXT_BT,
	ID_MARKER_PREV_BT,
	ID_EXPORT_CUT_IMG_BT,
	ID_EXPORT_VIEWPORT,
	ID_FIND_MAX_BT,
	ID_AUTO_UPDATE_CB,
	ID_EXPORT_VTK,
	ID_EXPORT_CUT_CSV_BT,
	ID_COLORSCALE_PROP,
	ID_COLORSCALE_COLORBT,
	ID_OPEN_MANUAL,
};

#endif /* EVENT_IDS_H_ */

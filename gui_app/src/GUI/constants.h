/*
 * event_ids.h
 *
 *  Created on: 07.09.2013
 *      Author: valentin
 */

#ifndef EVENT_IDS_H_
#define EVENT_IDS_H_

#include <wx-2.8/wx/wx.h>
#define NUMBEROFINTERPOLATIONMODES 2
const wxString imenumstrings[NUMBEROFINTERPOLATIONMODES] = {wxT("Linear"),wxT("Logarithmisch")};

enum {
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
	ID_CHANGE_ACTIVE_OBJ,
	ID_ANALYZE_POINT,
	ID_ANALYZE_POINT_BT,
	ID_CUT_CANVAS,
	ID_RENDER_CUT,
	ID_RENDER_CUT_BT,
	ID_CUT_TRI_EDIT,
};
// IDs > wxID_HIGHEST for temporary uses

#endif /* EVENT_IDS_H_ */

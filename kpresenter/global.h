/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: global definitions (header)                            */
/******************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <qsize.h>

// factors
#define MM_TO_POINT 2.83465
#define POINT_TO_MM 0.3527772388    

#define MAX_UNDO_REDO 100

// types
enum BackType {BT_COLOR,BT_PICTURE,BT_CLIPART};
enum BackView {BV_ZOOM,BV_CENTER,BV_TILED};
enum BCType {BCT_PLAIN = 0,BCT_GHORZ = 1,BCT_GVERT = 2};
enum ObjType {OT_PICTURE,OT_LINE,OT_RECT,OT_ELLIPSE,OT_TEXT,OT_AUTOFORM,OT_CLIPART,OT_UNDEFINED};
enum LineType {LT_HORZ,LT_VERT,LT_LU_RD,LT_LD_RU};
enum RectType {RT_NORM,RT_ROUND};
enum ModifyType {MT_NONE,MT_MOVE,MT_RESIZE_UP,MT_RESIZE_DN,MT_RESIZE_LF,MT_RESIZE_RT,MT_RESIZE_LU,MT_RESIZE_LD,MT_RESIZE_RU,MT_RESIZE_RD};
enum Effect {EF_NONE = 0,EF_COME_RIGHT = 1,EF_COME_LEFT = 2,EF_COME_TOP = 3,EF_COME_BOTTOM = 4,EF_COME_RIGHT_TOP = 5,\
	     EF_COME_RIGHT_BOTTOM = 6,EF_COME_LEFT_TOP = 7,EF_COME_LEFT_BOTTOM = 8,EF_WIPE_LEFT = 9,EF_WIPE_RIGHT = 10,\
	     EF_WIPE_TOP = 11,EF_WIPE_BOTTOM = 12};
enum Effect2 {EF2_NONE = 0,EF2T_PARA = 1};
enum PageEffect {PEF_NONE = 0,PEF_CLOSE_HORZ = 1,PEF_CLOSE_VERT = 2,PEF_CLOSE_ALL = 3,PEF_OPEN_HORZ = 4,PEF_OPEN_VERT = 5,\
		 PEF_OPEN_ALL = 6,PEF_INTERLOCKING_HORZ_1 = 7,PEF_INTERLOCKING_HORZ_2 = 8,PEF_INTERLOCKING_VERT_1 = 9,\
		 PEF_INTERLOCKING_VERT_2 = 10};
enum LineEnd {L_NORMAL,L_ARROW,L_SQUARE,L_CIRCLE};
enum ShadowDirection {SD_LEFT_UP = 1,SD_UP = 2,SD_RIGHT_UP = 3,SD_RIGHT = 4,SD_RIGHT_BOTTOM = 5,SD_BOTTOM = 6,\
		      SD_LEFT_BOTTOM = 7,SD_LEFT = 8};

// offsets of the effects in the Effect2 enum accoording to a objType
const int TxtObjOffset = 0;

const QSize orig_size(-1,-1);

#undef SHOW_INFO

#endif //GLOBAL_H

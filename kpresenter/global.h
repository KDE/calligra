/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
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
#define NUM_OBJ_TYPES 8

// types
enum BackType {BT_COLOR = 0,BT_PICTURE = 1,BT_CLIPART = 2};
enum BackView {BV_ZOOM = 0,BV_CENTER = 1,BV_TILED = 2};
enum BCType {BCT_PLAIN = 0,BCT_GHORZ = 1,BCT_GVERT = 2,BCT_GDIAGONAL1 = 3,BCT_GDIAGONAL2 = 4,BCT_GCIRCLE = 5,BCT_GRECT = 6};
enum ObjType {OT_PICTURE = 0,OT_LINE = 1,OT_RECT = 2,OT_ELLIPSE = 3,OT_TEXT = 4,OT_AUTOFORM = 5,OT_CLIPART = 6,OT_UNDEFINED = 7};
enum LineType {LT_HORZ,LT_VERT,LT_LU_RD,LT_LD_RU};
enum RectType {RT_NORM,RT_ROUND};
enum ModifyType {MT_NONE,MT_MOVE,MT_RESIZE_UP,MT_RESIZE_DN,MT_RESIZE_LF,MT_RESIZE_RT,MT_RESIZE_LU,MT_RESIZE_LD,MT_RESIZE_RU,MT_RESIZE_RD};
enum Effect {EF_NONE = 0,EF_COME_RIGHT = 1,EF_COME_LEFT = 2,EF_COME_TOP = 3,EF_COME_BOTTOM = 4,EF_COME_RIGHT_TOP = 5,\
	     EF_COME_RIGHT_BOTTOM = 6,EF_COME_LEFT_TOP = 7,EF_COME_LEFT_BOTTOM = 8,EF_WIPE_LEFT = 9,EF_WIPE_RIGHT = 10,\
	     EF_WIPE_TOP = 11,EF_WIPE_BOTTOM = 12};
enum Effect2 {EF2_NONE = 0,EF2T_PARA = 1};
enum PageEffect {PEF_NONE = 0,PEF_CLOSE_HORZ = 1,PEF_CLOSE_VERT = 2,PEF_CLOSE_ALL = 3,PEF_OPEN_HORZ = 4,PEF_OPEN_VERT = 5,\
		 PEF_OPEN_ALL = 6,PEF_INTERLOCKING_HORZ_1 = 7,PEF_INTERLOCKING_HORZ_2 = 8,PEF_INTERLOCKING_VERT_1 = 9,\
		 PEF_INTERLOCKING_VERT_2 = 10,PEF_SURROUND1};
enum LineEnd {L_NORMAL,L_ARROW,L_SQUARE,L_CIRCLE};
enum ShadowDirection {SD_LEFT_UP = 1,SD_UP = 2,SD_RIGHT_UP = 3,SD_RIGHT = 4,SD_RIGHT_BOTTOM = 5,SD_BOTTOM = 6,\
		      SD_LEFT_BOTTOM = 7,SD_LEFT = 8};
enum FillType {FT_BRUSH = 0,FT_GRADIENT = 1};
enum PresSpeed {PS_SLOW = 0,PS_NORMAL = 1,PS_FAST = 2};

static const float ObjSpeed[] = {70.0,50.0,30.0};
static const float PageSpeed[] = {8.0,16.0,32.0};

// offsets of the effects in the Effect2 enum accoording to a objType
const int TxtObjOffset = 0;

const QSize orig_size(-1,-1);

#undef SHOW_INFO

#endif //GLOBAL_H

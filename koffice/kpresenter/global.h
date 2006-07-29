/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <kiconloader.h>
#include "KPrFactory.h"

class KPrPage;

#define KPBarIcon( x ) BarIcon( x, KPrFactory::global() )

#define MAX_UNDO_REDO 100

// types
enum AlignType {
    AT_LEFT = 0,
    AT_TOP,
    AT_RIGHT,
    AT_BOTTOM,
    AT_HCENTER,
    AT_VCENTER
};

enum BackType {
    BT_COLOR = 0,
    BT_PICTURE = 1,
    BT_CLIPART = 2,
    BT_BRUSH = 3
};

enum BackView {
    BV_ZOOM = 0,
    BV_CENTER = 1,
    BV_TILED = 2
};

enum ObjType {
    OT_PICTURE = 0,
    OT_LINE = 1,
    OT_RECT = 2,
    OT_ELLIPSE = 3,
    OT_TEXT = 4,
    OT_AUTOFORM = 5,
    OT_CLIPART = 6,
    OT_UNDEFINED = 7,
    OT_PIE = 8,
    OT_PART = 9,
    OT_GROUP = 10,
    OT_FREEHAND = 11,
    OT_POLYLINE = 12,
    OT_QUADRICBEZIERCURVE = 13,
    OT_CUBICBEZIERCURVE = 14,
    OT_POLYGON = 15,
    OT_CLOSED_LINE = 16
};
enum SelectionMode {
    SM_NONE = 0,
    SM_MOVERESIZE = 1,
    SM_ROTATE = 2,
    SM_PROTECT = 3
};
enum LineType {
    LT_HORZ = 0,
    LT_VERT,
    LT_LU_RD,
    LT_LD_RU
};
enum RectType {
    RT_NORM = 0,
    RT_ROUND
};
enum ModifyType {
    MT_NONE = 0,
    MT_MOVE,
    MT_RESIZE_UP,
    MT_RESIZE_DN,
    MT_RESIZE_LF,
    MT_RESIZE_RT,
    MT_RESIZE_LU,
    MT_RESIZE_LD,
    MT_RESIZE_RU,
    MT_RESIZE_RD
};
enum Effect {
    EF_NONE = 0,
    EF_COME_RIGHT = 1,
    EF_COME_LEFT = 2,
    EF_COME_TOP = 3,
    EF_COME_BOTTOM = 4,
    EF_COME_RIGHT_TOP = 5,
    EF_COME_RIGHT_BOTTOM = 6,
    EF_COME_LEFT_TOP = 7,
    EF_COME_LEFT_BOTTOM = 8,
    EF_WIPE_LEFT = 9,
    EF_WIPE_RIGHT = 10,
    EF_WIPE_TOP = 11,
    EF_WIPE_BOTTOM = 12
};
enum Effect2 {
    EF2_NONE = 0,
    EF2T_PARA = 1
};
enum Effect3 {
    EF3_NONE = 0,
    EF3_GO_RIGHT = 1,
    EF3_GO_LEFT = 2,
    EF3_GO_TOP = 3,
    EF3_GO_BOTTOM = 4,
    EF3_GO_RIGHT_TOP = 5,
    EF3_GO_RIGHT_BOTTOM = 6,
    EF3_GO_LEFT_TOP = 7,
    EF3_GO_LEFT_BOTTOM = 8,
    EF3_WIPE_LEFT = 9,
    EF3_WIPE_RIGHT = 10,
    EF3_WIPE_TOP = 11,
    EF3_WIPE_BOTTOM = 12
};
enum PageEffect {
    PEF_NONE = 0,
    PEF_CLOSE_HORZ = 1,
    PEF_CLOSE_VERT = 2,
    PEF_CLOSE_ALL = 3,
    PEF_OPEN_HORZ = 4,
    PEF_OPEN_VERT = 5,
    PEF_OPEN_ALL = 6,
    PEF_INTERLOCKING_HORZ_1 = 7,
    PEF_INTERLOCKING_HORZ_2 = 8,
    PEF_INTERLOCKING_VERT_1 = 9,
    PEF_INTERLOCKING_VERT_2 = 10,
    PEF_SURROUND1 = 11,
    PEF_FLY1 = 12,
    PEF_BLINDS_HOR = 13,
    PEF_BLINDS_VER = 14,
    PEF_BOX_IN = 15,
    PEF_BOX_OUT = 16,
    PEF_CHECKBOARD_ACROSS = 17,
    PEF_CHECKBOARD_DOWN = 18,
    PEF_COVER_DOWN = 19,
    PEF_UNCOVER_DOWN = 20,
    PEF_COVER_UP = 21,
    PEF_UNCOVER_UP = 22,
    PEF_COVER_LEFT = 23,
    PEF_UNCOVER_LEFT = 24,
    PEF_COVER_RIGHT = 25,
    PEF_UNCOVER_RIGHT = 26,
    PEF_COVER_LEFT_UP = 27,
    PEF_UNCOVER_LEFT_UP = 28,
    PEF_COVER_LEFT_DOWN = 29,
    PEF_UNCOVER_LEFT_DOWN = 30,
    PEF_COVER_RIGHT_UP = 31,
    PEF_UNCOVER_RIGHT_UP = 32,
    PEF_COVER_RIGHT_DOWN = 33,
    PEF_UNCOVER_RIGHT_DOWN = 34,
    PEF_DISSOLVE = 35,
    PEF_STRIPS_LEFT_UP = 36,
    PEF_STRIPS_LEFT_DOWN = 37,
    PEF_STRIPS_RIGHT_UP = 38,
    PEF_STRIPS_RIGHT_DOWN = 39,
    PEF_MELTING = 40,
    PEF_LAST_MARKER = 41, // only marker, don't use this !
    PEF_RANDOM = -1
};

enum ImageEffect {
    IE_NONE = -1,
    IE_CHANNEL_INTENSITY = 0,
    IE_FADE = 1,
    IE_FLATTEN = 2,
    IE_INTENSITY = 3,
    IE_DESATURATE = 4,
    IE_CONTRAST = 5,
    IE_NORMALIZE = 6,
    IE_EQUALIZE = 7,
    IE_THRESHOLD = 8,
    IE_SOLARIZE = 9,
    IE_EMBOSS = 10,
    IE_DESPECKLE = 11,
    IE_CHARCOAL = 12,
    IE_NOISE = 13,
    IE_BLUR = 14,
    IE_EDGE = 15,
    IE_IMPLODE = 16,
    IE_OIL_PAINT = 17,
    IE_SHARPEN = 18,
    IE_SPREAD = 19,
    IE_SHADE = 20,
    IE_SWIRL = 21,
    IE_WAVE = 22
};

enum LineEnd {
    L_NORMAL = 0,
    L_ARROW,
    L_SQUARE,
    L_CIRCLE,
    L_LINE_ARROW,
    L_DIMENSION_LINE,
    L_DOUBLE_ARROW,
    L_DOUBLE_LINE_ARROW
};
enum ShadowDirection {
    SD_LEFT_UP = 1,
    SD_UP = 2,
    SD_RIGHT_UP = 3,
    SD_RIGHT = 4,
    SD_RIGHT_BOTTOM = 5,
    SD_BOTTOM = 6,
    SD_LEFT_BOTTOM = 7,
    SD_LEFT = 8
};

typedef int PresSpeed;

enum EffectSpeed {
  ES_SLOW = 0,
  ES_MEDIUM = 1,
  ES_FAST = 2
};

enum InsertPos {
    IP_BEFORE = 0,
    IP_AFTER = 1
};
enum PieType {
    PT_PIE = 0,
    PT_ARC = 1,
    PT_CHORD = 2
};
enum ToolEditMode {
    TEM_MOUSE = 0,
    INS_RECT = 1,
    INS_ELLIPSE = 2,
    INS_TEXT = 3,
    INS_PIE = 4,
    INS_OBJECT = 5,
    INS_LINE = 6,
    INS_DIAGRAMM = 7,
    INS_TABLE = 8,
    INS_FORMULA = 9,
    INS_AUTOFORM = 10,
    INS_FREEHAND = 11,
    INS_POLYLINE = 12,
    INS_QUADRICBEZIERCURVE = 13,
    INS_CUBICBEZIERCURVE = 14,
    INS_POLYGON = 15,
    INS_PICTURE = 16,
    INS_CLIPART = 17,
    TEM_ROTATE = 18,
    TEM_ZOOM = 19,
    INS_CLOSED_FREEHAND = 20,
    INS_CLOSED_POLYLINE = 21,
    INS_CLOSED_QUADRICBEZIERCURVE = 22,
    INS_CLOSED_CUBICBEZIERCURVE = 23
};

enum PictureMirrorType {
    PM_NORMAL = 0,
    PM_HORIZONTAL = 1,
    PM_VERTICAL = 2,
    PM_HORIZONTALANDVERTICAL = 3
};

enum PropValue {
    STATE_ON = 0,
    STATE_OFF = 1,
    STATE_UNDEF = 2
};

enum VerticalAlignmentType {
    KP_CENTER=0,
    KP_TOP=1,
    KP_BOTTOM=2
};


// offsets of the effects in the Effect2 enum accoording to a objType
const int TxtObjOffset = 0;

typedef QMap<QString, QValueList<KPrPage *> > CustomSlideShowMap;

#endif //GLOBAL_H

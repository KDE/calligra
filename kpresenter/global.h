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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <qsize.h>
#include <kiconloader.h>
#include <kpresenter_factory.h>

#define KPBarIcon( x ) BarIcon( x, KPresenterFactory::global() )

#define MAX_UNDO_REDO 100

// types
enum BackType {
    BT_COLOR = 0,
    BT_PICTURE = 1,
    BT_CLIPART = 2
};
enum BackView {
    BV_ZOOM = 0,
    BV_CENTER = 1,
    BV_TILED = 2
};
enum BCType {
    BCT_PLAIN = 0,
    BCT_GHORZ = 1,
    BCT_GVERT = 2,
    BCT_GDIAGONAL1 = 3,
    BCT_GDIAGONAL2 = 4,
    BCT_GCIRCLE = 5,
    BCT_GRECT = 6,
    BCT_GPIPECROSS = 7,
    BCT_GPYRAMID = 8
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
    OT_GROUP = 10
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
    PEF_FLY1 = 12
};
enum LineEnd {
    L_NORMAL = 0,
    L_ARROW,
    L_SQUARE,
    L_CIRCLE
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
enum FillType
{
    FT_BRUSH = 0,
    FT_GRADIENT = 1
};

typedef int PresSpeed;

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
    INS_AUTOFORM = 10
};

// offsets of the effects in the Effect2 enum accoording to a objType
const int TxtObjOffset = 0;

const QSize orig_size( -1, -1 );

#undef SHOW_INFO

#endif //GLOBAL_H

/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0alpha                                            */
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

#include <qpixmap.h>
#include <qpicture.h>

class BackPic;
class GraphObj;
class KTextObject;

#ifndef GLOBAL_H
#define GLOBAL_H

// factors
#define MM_TO_POINT 2.83465
#define POINT_TO_MM 0.3527772388    

// types
enum BackType {BT_COLOR,BT_PIC,BT_CLIP};
enum BackView {BV_ZOOM,BV_CENTER,BV_TILED};
enum ObjType {OT_PICTURE,OT_LINE,OT_RECT,OT_CIRCLE,OT_TEXT,OT_AUTOFORM,OT_CLIPART};
enum LineType {LT_HORZ,LT_VERT,LT_LU_RD,LT_LD_RU};
enum RectType {RT_NORM,RT_ROUND};
enum ModifyType {MT_NONE,MT_MOVE,MT_RESIZE_UP,MT_RESIZE_DN,MT_RESIZE_LF,MT_RESIZE_RT,MT_RESIZE_LU,MT_RESIZE_LD,MT_RESIZE_RU,MT_RESIZE_RD};

// page configuration for screenpresentations
struct SpPageConfiguration                 
{
  int time;
};

// page background
enum BCType {BCT_PLAIN = 0,BCT_GHORZ = 1,BCT_GVERT = 2};

struct Background
{
  unsigned int pageNum;  
  BackType backType; 
  BackView backPicView; 
  QColor backColor1;
  QColor backColor2;
  BCType bcType;
  const char *backPic;
  const char *backClip; 
  QPixmap backPix;
  BackPic *pic;
};

// page object
struct PageObjects
{
  ObjType objType;
  bool isSelected;
  unsigned int objNum;
  int ox,oy,ow,oh;
  KTextObject *textObj;
  GraphObj* graphObj;
  QPicture *objPic;
};

#endif //GLOBAL_H

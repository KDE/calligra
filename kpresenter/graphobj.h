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
/* Module: Graphic Object (header)                                */
/******************************************************************/

#ifndef GRAPHOBJ_H
#define GRAPHOBJ_H

#include <stdlib.h>

#include <qwidget.h>
#include <qpainter.h>
#include <qpicture.h>
#include <qpen.h>
#include <qbrush.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <qrect.h>
#include <qlist.h>
#include <qpntarry.h>
#include <qfileinf.h>
#include <qregexp.h>
#include <qsize.h>

#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>
#include <kapp.h>

#include <iostream.h>
#include <fstream.h>

#include "autoformEdit/atfinterpreter.h"
#include "qwmf.h"
#include "global.h"
#include "kpresenter_utils.h"

/******************************************************************/
/* class GraphObj                                                 */
/******************************************************************/

class GraphObj : public QWidget
{
  Q_OBJECT

public:

  // constructor - destructor
  GraphObj(QWidget* parent=0,const char* name=0,ObjType _objType=OT_LINE, 
	   QString fName=0);
  ~GraphObj();                                              
  
  // load - get pictures
  QPicture* getPic(int,int,int,int);                        
  void loadPixmap();
  void loadClipart();
  QPixmap getPix();
  void drawInPainter(QPainter*);

  // set values
  void setLineType(LineType t) {lineType = t;}
  int getLineType() {return lineType;}
  void setRectType(RectType t) {rectType = t;}
  int getRectType() {return rectType;}
  void setObjPen(QPen p) {oPen.operator=(p);}
  QPen getObjPen() {return oPen;}
  void setObjBrush(QBrush b) {oBrush.operator=(b);}
  QBrush getObjBrush() {return oBrush;}
  void setFileName(QString fn);
  QString getFileName() {return fileName;}
  void setRnds(int rx,int ry) {xRnd = rx; yRnd = ry;}
  int getRndX() {return xRnd;}
  int getRndY() {return yRnd;}
  void setLineBegin(LineEnd lb) {lineBegin = lb;}
  LineEnd getLineBegin() {return lineBegin;}
  void setLineEnd(LineEnd le) {lineEnd = le;}
  LineEnd getLineEnd() {return lineEnd;}
  ObjType getObjType() {return objType;}
  QString getPixData() {return QString(pix_data);}
  QString getPixDataNative() {return QString(pix_data_native);}
  QPixmap get_pix() {return QPixmap(pix);}
  QPixmap getOrigPix() {return QPixmap(origPix);}

  // save - load
  void save(ostream&);
  void load(KOMLParser&,vector<KOMLAttrib>&);

  GraphObj& operator=(GraphObj&);

protected:

  // events
  void paintEvent(QPaintEvent*);                     
  void paintObj(QPainter*);                          
  void mousePressEvent(QMouseEvent*);                
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  QString toPixString(QString _filename);
  float getAngle(QPoint,QPoint);

private:

  // store values
  QPicture pic;                                      
  QPicture clip;
  ObjType objType;
  LineType lineType;
  RectType rectType;
  QPen oPen;
  QBrush oBrush;
  int xRnd,yRnd;
  QString fileName;
  QPixmap pix;
  QPixmap origPix;
  ATFInterpreter *atfInterp;
  QWinMetaFile wmf;
  QString pix_data;
  QString pix_data_native;
  LineEnd lineBegin,lineEnd;

};
#endif //GRAPHOBJ_H


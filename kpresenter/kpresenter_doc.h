/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Document (header)                           */
/******************************************************************/

#ifndef __kpresenter_doc_h__
#define __kpresenter_doc_h__

class KPresenterDocument_impl;
class KPresenterView_impl;

#include <stdlib.h>
#include <stdio.h>

#include <document_impl.h>
#include <view_impl.h>

#include <qapp.h>
#include <qlist.h>
#include <qobject.h>
#include <qrect.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <qbrush.h>
#include <qpen.h>
#include <qpopmenu.h>
#include <qcursor.h>
#include <qmsgbox.h>
#include <qstring.h>
#include <qclipbrd.h>
#include <qregexp.h>
#include <qfileinf.h>

#include <koPageLayoutDia.h>
#include <koIMR.h>
#include <koDocument.h>
#include <koPrintExt.h>

#include <kurl.h>

#include "kpobject.h"
#include "kplineobject.h"
#include "kprectobject.h"
#include "kpellipseobject.h"
#include "kpautoformobject.h"
#include "kpclipartobject.h"
#include "kptextobject.h"
#include "kppixmapobject.h"
#include "kpresenter_view.h"
#include "global.h"
#include "kpbackground.h"

#include "kppixmapcollection.h"
#include "kpgradientcollection.h"

#include "movecmd.h"
#include "insertcmd.h"
#include "commandhistory.h"

#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>

#include <iostream.h>
#include <fstream.h>
#include <torben.h>
#include <strstream.h>

#include <math.h>

#define MIME_TYPE "application/x-kpresenter"
#define EDITOR "IDL:KPresenter/KPresenterDocument:1.0"

/******************************************************************/
/* class KPresenterChild                                          */
/******************************************************************/
class KPresenterChild : public KoDocumentChild
{
public:

  // constructor - destructor
  KPresenterChild( KPresenterDocument_impl *_kpr,const QRect& _rect,OPParts::Document_ptr _doc,int,int);
  KPresenterChild( KPresenterDocument_impl *_kpr );
  ~KPresenterChild();

  QRect _geometry() {return __geometry;}
  void _setGeometry(QRect g) {__geometry = g;}
  
  // get parent
  KPresenterDocument_impl* parent() {return m_pKPresenterDoc;}
  
protected:

  // parent, document and geometry
  KPresenterDocument_impl *m_pKPresenterDoc;
  QRect __geometry;
};

/*****************************************************************/
/* class KPresenterDocument_impl                                 */
/*****************************************************************/
class KPresenterDocument_impl : public QObject,
				virtual public KoDocument,
				virtual public KoPrintExt,
				virtual public KPresenter::KPresenterDocument_skel
{
  Q_OBJECT

public:

  // ------ C++ ------ 
  // constructor - destructor
  KPresenterDocument_impl(const CORBA::BOA::ReferenceData &refdata);
  KPresenterDocument_impl();
  ~KPresenterDocument_impl();
  
  // clean
  virtual void cleanUp();
  
  // save
  virtual bool save(ostream&);
  virtual bool exportHTML(QString _filename);

  // load
  virtual bool load_template(const char *_url);
  virtual bool load(KOMLParser&);
  virtual bool loadChildren(OPParts::MimeMultipartDict_ptr _dict);

  // ------ IDL ------
  virtual CORBA::Boolean init() {insertNewTemplate(0,0); return true;}

  // create a view
  virtual OPParts::View_ptr createView();

  // get list of views
  virtual void viewList(OPParts::Document::ViewList*& _list);

  // get mime type
  virtual char* mimeType() {return CORBA::string_dup(MIME_TYPE);}
  
  // ask, if document is modified
  virtual CORBA::Boolean isModified() {return m_bModified;}
  virtual void setModified(bool _c) {m_bModified = _c;}
  
  // url of part
  const char* url() {return m_strFileURL.data();}

  // ------ C++ ------
  // get output- and inputformats
  virtual QStrList outputFormats();
  virtual QStrList inputFormats();

  // add - remove a view
  virtual void addView(KPresenterView_impl *_view);
  virtual void removeView(KPresenterView_impl *_view);
  
  // insert an object
  virtual void insertObject(const QRect&,const char*,int,int);
  virtual void insertChild( KPresenterChild *_child );
  
  // change geomentry of a child
  virtual void changeChildGeometry(KPresenterChild*,const QRect&,int,int);
  
  // get iterator if a child
  virtual QListIterator<KPresenterChild> childIterator();
  
  // page layout
  void setPageLayout(KoPageLayout,int,int);
  KoPageLayout pageLayout() {return _pageLayout;}

  // insert a page
  unsigned int insertNewPage(int,int,bool _restore=true); 
  unsigned int insertNewTemplate(int,int,bool clean=false);

  // get number of pages nad objects
  unsigned int getPageNums() {return _backgroundList.count();}
  unsigned int objNums() {return _objectList.count();}

  // background
  void setBackColor(unsigned int,QColor,QColor,BCType);
  void setBackPixFilename(unsigned int,QString);
  void setBackClipFilename(unsigned int,QString);
  void setBackView(unsigned int,BackView);
  void setBackType(unsigned int,BackType);
  bool setPenBrush(QPen,QBrush,LineEnd,LineEnd,int,int);
  void setPageEffect(unsigned int,PageEffect);
  BackType getBackType(unsigned int);
  BackView getBackView(unsigned int);
  QString getBackPixFilename(unsigned int);
  QString getBackClipFilename(unsigned int);
  QColor getBackColor1(unsigned int);
  QColor getBackColor2(unsigned int);
  BCType getBackColorType(unsigned int);
  PageEffect getPageEffect(unsigned int);
  QPen getPen(QPen);
  QBrush getBrush(QBrush);
  LineEnd getLineBegin(LineEnd);
  LineEnd getLineEnd(LineEnd);

  // raise and lower objs
  void raiseObjs(int,int);
  void lowerObjs(int,int);

  // insert/change objects
  void insertPicture(QString,int,int);
  void insertClipart(QString,int,int);
  void changePicture(QString,int,int);
  void changeClipart(QString,int,int);
  void insertLine(QPen,LineEnd,LineEnd,LineType,int,int);
  void insertRectangle(QPen,QBrush,RectType,int,int);
  void insertCircleOrEllipse(QPen,QBrush,int,int);
  void insertText(int,int);
  void insertAutoform(QPen,QBrush,LineEnd,LineEnd,QString,int,int);
  
  // get list of pages and objects
  QList<KPBackGround> *backgroundList() {return &_backgroundList;}
  QList<KPObject> *objectList() {return &_objectList;}

  // get - set raster
  unsigned int rastX() {return _rastX;}
  unsigned int rastY() {return _rastY;}
  unsigned int getRastX() {return _rastX;}
  unsigned int getRastY() {return _rastY;}
  void setRasters(unsigned int rx,unsigned int ry)
    {_rastX = rx; _rastY = ry; replaceObjs();}

  // get - set options for editmodi
  QColor txtBackCol() {return _txtBackCol;}
  QColor txtSelCol() {return _txtSelCol;}
  QColor getTxtBackCol() {return _txtBackCol;}
  QColor getTxtSelCol() {return _txtSelCol;}
  void setTxtBackCol(QColor c) {_txtBackCol = c;}
  void setTxtSelCol(QColor c) {_txtSelCol = c;}

  // get - set roundedness
  unsigned int getRndX() {return _xRnd;}
  unsigned int getRndY() {return _yRnd;}
  void setRnds(unsigned int rx,unsigned int ry)
    {_xRnd = rx; _yRnd = ry; replaceObjs();}

  // get values for screenpresentations
  bool spInfinitLoop() {return _spInfinitLoop;}
  bool spManualSwitch() {return _spManualSwitch;}
  void setInfinitLoop(bool il) {_spInfinitLoop = il;}
  void setManualSwitch(bool ms) {_spManualSwitch = ms;}

  // size of page
  QRect getPageSize(unsigned int,int,int,float fakt=1.0);

  // delete/reorder obejcts
  void deleteObjs();
  void copyObjs(int,int);
  void pasteObjs(int,int);

  // repaint all views
  void repaint(bool);

  void repaint(QRect);
  void repaint(KPObject*);

  // stuff for screen-presentations
  QList<int> reorderPage(unsigned int,int,int,float fakt = 1.0);
  int getPageOfObj(int,int,int,float fakt = 1.0);

  QPen presPen() {return _presPen;}
  void setPresPen(QPen p) {_presPen = p;}

  int numSelected();
  KPObject* getSelectedObj();

  void restoreBackground(int);
  void loadStream(istream&);

  void deSelectAllObj();

  KPPixmapCollection *getPixmapCollection()
    { return &_pixmapCollection; }
  KPGradientCollection *getGradientCollection()
    { return &_gradientCollection; }

  CommandHistory *commands()
    { return &_commands; }

  void alignObjsLeft();
  void alignObjsCenterH();
  void alignObjsRight();
  void alignObjsTop();
  void alignObjsCenterV();
  void alignObjsBottom();

signals:

  // document modified
  void sig_KPresenterModified();

  // object inserted - removed
  void sig_insertObject(KPresenterChild *_child);
  void sig_removeObject(KPresenterChild *_child);

  // update child geometry
  void sig_updateChildGeometry(KPresenterChild *_child);

protected slots:
  void slotUndoRedoChanged(QString,QString);

protected:

  // ************ functions ************
  /**
   * Overloaded function from @ref Document_impl. This function is needed for
   * saving. We must know about every direct and indirect child so that we
   * can save them all.
   */
  virtual void makeChildListIntern( OPParts::Document_ptr _root, const char *_path );  
  /*
   * Overloaded function from @ref KoDocument.
   *
   * @return true if one of the direct children wants to
   *              be saved embedded. If there are no children or if
   *              every direct child saves itself into its own file
   *              then false is returned.
   */
  virtual bool hasToWriteMultipart();

  virtual void draw(QPaintDevice*,CORBA::Long,CORBA::Long);

  void saveBackground(ostream&);
  void saveObjects(ostream&);
  void loadBackground(KOMLParser&,vector<KOMLAttrib>&);
  void loadObjects(KOMLParser&,vector<KOMLAttrib>&);
  void replaceObjs();

  // ************ variables ************

  struct PixCache
  {
    QString filename;
    QString pix_string;
  };

  // list of views and children
  QList<KPresenterView_impl> m_lstViews;
  QList<KPresenterChild> m_lstChildren;
  KPresenterView_impl *viewPtr;

  // modified?
  bool m_bModified;

  // page layout and background
  KoPageLayout _pageLayout;
  QList<KPBackGround> _backgroundList;

  // list of objects
  QList<KPObject> _objectList;

  // screenpresentations
  bool _spInfinitLoop,_spManualSwitch;

  // options
  int _rastX,_rastY;   
  int _xRnd,_yRnd;

  // options for editmode
  QColor _txtBackCol;
  QColor _txtSelCol;

  // url
  QString m_strFileURL;

  bool _clean;
  int objStartY,objStartNum;

  QPen _presPen;

  KPPixmapCollection _pixmapCollection;
  KPGradientCollection _gradientCollection;

  CommandHistory _commands;

};

#endif





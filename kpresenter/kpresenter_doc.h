/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
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

class KPresenterDoc;
class KPresenterView;

#include <stdlib.h>
#include <stdio.h>

#include <koDocument.h>
#include <koView.h>
#include <koQueryTypes.h>

#include <qapp.h>
#include <qlist.h>
#include <qobject.h>
#include <krect.h>
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
#include <koTemplateChooseDia.h>

#include <kurl.h>

#include "kpobject.h"
#include "kplineobject.h"
#include "kprectobject.h"
#include "kpellipseobject.h"
#include "kpautoformobject.h"
#include "kpclipartobject.h"
#include "kptextobject.h"
#include "kppixmapobject.h"
#include "kppieobject.h"
#include "kppartobject.h"
#include "kpresenter_view.h"
#include "global.h"
#include "kpbackground.h"

#include "kppixmapcollection.h"
#include "kpgradientcollection.h"

#include "movecmd.h"
#include "insertcmd.h"
#include "deletecmd.h"
#include "setoptionscmd.h"
#include "chgclipcmd.h"
#include "chgpixcmd.h"
#include "lowraicmd.h"
#include "pievaluecmd.h"
#include "rectvaluecmd.h"
#include "penbrushcmd.h"
#include "commandhistory.h"

#include "styledia.h"

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

class KPFooterHeaderEditor;

/******************************************************************/
/* class KPresenterChild                                          */
/******************************************************************/
class KPresenterChild : public KoDocumentChild
{
public:

  // constructor - destructor
  KPresenterChild(KPresenterDoc *_kpr,const KRect& _rect,KOffice::Document_ptr _doc,int,int);
  KPresenterChild(KPresenterDoc *_kpr);
  ~KPresenterChild();

  // get parent
  KPresenterDoc* parent() {return m_pKPresenterDoc;}

protected:

  // parent, document and geometry
  KPresenterDoc *m_pKPresenterDoc;

};

/*****************************************************************/
/* class KPresenterDoc                                           */
/*****************************************************************/
class KPresenterDoc : public QObject,
		      virtual public KoDocument,
		      virtual public KoPrintExt,
		      virtual public KPresenter::KPresenterDocument_skel
{
  Q_OBJECT

public:

  // ------ C++ ------
  // constructor - destructor
  KPresenterDoc();
  ~KPresenterDoc();

  // clean
  virtual void cleanUp();

  // save
  virtual bool save(ostream&,const char *_format);
  virtual bool exportHTML(QString _filename);

  // load
  virtual bool load_template(const QString &_url);
  virtual bool loadXML(KOMLParser&,KOStore::Store_ptr);
  virtual bool loadChildren(KOStore::Store_ptr _store);

  virtual KPresenterView* createPresenterView( QWidget* _parent = 0 );

  // ------ IDL ------
  virtual CORBA::Boolean init() { return insertNewTemplate(0,0); }

  KOffice::MainWindow_ptr createMainWindow();

  // create a view
  virtual OpenParts::View_ptr createView();

  // get list of views
  virtual void viewList(KOffice::Document::ViewList*& _list);

  // get mime type
  virtual char* mimeType() {return CORBA::string_dup(MIME_TYPE);}

  // ask, if document is modified
  virtual CORBA::Boolean isModified() { return m_bModified; }
  virtual void setModified(bool _c) { m_bModified = _c; if (_c) m_bEmpty = false; }
  virtual bool isEmpty() { return m_bEmpty; }

  // ------ C++ ------
  virtual int viewCount();

  // ------ C++ ------
  // get output- and inputformats
  virtual QStrList outputFormats();
  virtual QStrList inputFormats();

  // add - remove a view
  virtual void addView(KPresenterView *_view);
  virtual void removeView(KPresenterView *_view);

  // insert an object
  virtual void insertObject(const KRect&,KoDocumentEntry&,int,int);
  virtual void insertChild(KPresenterChild *_child);

  // change geomentry of a child
  virtual void changeChildGeometry(KPresenterChild*,const KRect&,int,int);

  // get iterator if a child
  virtual QListIterator<KPresenterChild> childIterator();

  // page layout
  void setPageLayout(KoPageLayout,int,int);
  KoPageLayout pageLayout() {return _pageLayout;}

  // insert a page
  unsigned int insertNewPage(int,int,bool _restore=true);
  bool insertNewTemplate(int,int,bool clean=false);

  // get number of pages nad objects
  unsigned int getPageNums() {return _backgroundList.count();}
  unsigned int objNums() {return _objectList->count();}

  // background
  void setBackColor(unsigned int,QColor,QColor,BCType);
  void setBackPixFilename(unsigned int,QString);
  void setBackClipFilename(unsigned int,QString);
  void setBackView(unsigned int,BackView);
  void setBackType(unsigned int,BackType);
  bool setPenBrush(QPen,QBrush,LineEnd,LineEnd,FillType,QColor,QColor,BCType);
  bool setLineBegin(LineEnd);
  bool setLineEnd(LineEnd);
  bool setPieSettings(PieType,int,int);
  bool setRectSettings(int,int);
  void setPageEffect(unsigned int,PageEffect);
  bool setPenColor(QColor,bool);
  bool setBrushColor(QColor,bool);
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
  FillType getFillType(FillType);
  QColor getGColor1(QColor);
  QColor getGColor2(QColor);
  BCType getGType(BCType);
  PieType getPieType(PieType);
  int getRndX(int);
  int getRndY(int);
  int getPieLength(int);
  int getPieAngle(int);

  // raise and lower objs
  void raiseObjs(int,int);
  void lowerObjs(int,int);

  // insert/change objects
  void insertPicture(QString,int,int,int _x = 10,int _y = 10);
  void insertClipart(QString,int,int);
  void changePicture(QString,int,int);
  void changeClipart(QString,int,int);
  void insertLine(KRect,QPen,LineEnd,LineEnd,LineType,int,int);
  void insertRectangle(KRect,QPen,QBrush,FillType,QColor,QColor,BCType,int,int,int,int);
  void insertCircleOrEllipse(KRect,QPen,QBrush,FillType,QColor,QColor,BCType,int,int);
  void insertPie(KRect,QPen pen,QBrush brush,FillType ft,QColor g1,QColor g2,
		 BCType gt,PieType pt,int _angle,int _len,LineEnd lb,LineEnd le,int diffx,int diffy);
  void insertText(KRect,int,int,QString text = QString::null,KPresenterView *_view = 0L);
  void insertAutoform(KRect,QPen,QBrush,LineEnd,LineEnd,FillType,QColor,QColor,BCType,QString,int,int);

  // get list of pages and objects
  QList<KPBackGround> *backgroundList() {return &_backgroundList;}
  QList<KPObject> *objectList() {return _objectList;}

  // get - set raster
  unsigned int rastX() {return _rastX;}
  unsigned int rastY() {return _rastY;}
  unsigned int getRastX() {return _rastX;}
  unsigned int getRastY() {return _rastY;}
  void setRasters(unsigned int rx,unsigned int ry,bool _replace = true);

  // get - set options for editmodi
  QColor txtBackCol() {return _txtBackCol;}
  QColor getTxtBackCol() {return _txtBackCol;}
  void setTxtBackCol(QColor c) {_otxtBackCol = _txtBackCol; _txtBackCol = c;}

  // get - set roundedness
  unsigned int getRndX() {return _xRnd;}
  unsigned int getRndY() {return _yRnd;}

  // get values for screenpresentations
  bool spInfinitLoop() {return _spInfinitLoop;}
  bool spManualSwitch() {return _spManualSwitch;}
  void setInfinitLoop(bool il) {_spInfinitLoop = il;}
  void setManualSwitch(bool ms) {_spManualSwitch = ms;}

  // size of page
  KRect getPageSize(unsigned int,int,int,float fakt=1.0,bool decBorders = true);

  // delete/reorder obejcts
  void deleteObjs(bool _add = true);
  void copyObjs(int,int);
  void pasteObjs(int,int);

  // repaint all views
  void repaint(bool);

  void repaint(KRect);
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
  void replaceObjs();

  PresSpeed getPresSpeed()
    { return presSpeed; }
  void setPresSpeed(PresSpeed _presSpeed)
    { presSpeed = _presSpeed; }

  int getLeftBorder();
  int getTopBorder();
  int getBottomBorder();

  void deletePage(int _page,DelPageMode _delPageMode);
  void insertPage(int _page,InsPageMode _insPageMode,InsertPos _insPos);

  void setObjectList(QList<KPObject> *_list)
    { _objectList->setAutoDelete(false); _objectList = _list; _objectList->setAutoDelete(false); }

  void hideAllFrames();

  int getPenBrushFlags();

  void enableEmbeddedParts(bool f);

  void setUnit(KoUnit _unit,QString __unit);

  QString getPageTitle(unsigned int pgNum,const QString &_title);

  KPTextObject *header() { return _header; }
  KPTextObject *footer() { return _footer; }
  bool hasHeader() { return _hasHeader; }
  bool hasFooter() { return _hasFooter; }
  void setHeader(bool b);
  void setFooter(bool b);
  KPFooterHeaderEditor *getHeaderFooterEdit() { return headerFooterEdit; }
  
signals:

  // document modified
  void sig_KPresenterModified();

  // object inserted - removed
  void sig_insertObject(KPresenterChild *_child,KPPartObject *_kppo);
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
  virtual void makeChildListIntern(KOffice::Document_ptr _root,const char *_path);
  /*
   * Overloaded function from @ref KoDocument.
   *
   * @return true if one of the direct children wants to
   *              be saved embedded. If there are no children or if
   *              every direct child saves itself into its own file
   *              then false is returned.
   */
  virtual bool hasToWriteMultipart();

  virtual void draw(QPaintDevice*,CORBA::Long,CORBA::Long,CORBA::Float _scale);

  void saveBackground(ostream&);
  void saveObjects(ostream&);
  void loadBackground(KOMLParser&,vector<KOMLAttrib>&);
  void loadObjects(KOMLParser&,vector<KOMLAttrib>&,bool _paste = false);

  // ************ variables ************

  struct PixCache
  {
    QString filename;
    QString pix_string;
  };

  // list of views and children
  QList<KPresenterView> m_lstViews;
  QList<KPresenterChild> m_lstChildren;
  KPresenterView *viewPtr;

  // modified?
  bool m_bModified;
  bool m_bEmpty;

  // page layout and background
  KoPageLayout _pageLayout;
  QList<KPBackGround> _backgroundList;

  // list of objects
  QList<KPObject> *_objectList;

  // screenpresentations
  bool _spInfinitLoop,_spManualSwitch;

  // options
  int _rastX,_rastY;
  int _xRnd,_yRnd;

  // options for editmode
  QColor _txtBackCol;
  QColor _otxtBackCol;

  bool _clean;
  int objStartY,objStartNum;

  int _orastX,_orastY;
  PresSpeed presSpeed;

  QPen _presPen;

  KPPixmapCollection _pixmapCollection;
  KPGradientCollection _gradientCollection;

  CommandHistory _commands;
  bool pasting;
  int pasteXOffset,pasteYOffset;

  KPTextObject *_header,*_footer;
  bool _hasHeader,_hasFooter;
  KPFooterHeaderEditor *headerFooterEdit;
    
};

#endif

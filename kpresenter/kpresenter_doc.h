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

#include <koPageLayoutDia.h>
#include <koIMR.h>
#include <koDocument.h>

#include <kurl.h>

#include "kpresenter_view.h"
#include "global.h"
#include "qwmf.h"
#include "graphobj.h"
#include "ktextobject.h"

#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>

#include <iostream.h>
#include <fstream.h>

#define MIME_TYPE "application/x-kpresenter"
#define EDITOR "IDL:KPresenter/KPresenterDocument:1.0"

/******************************************************************/
/* class BackPic                                                  */
/******************************************************************/

class BackPic : public QWidget
{
  Q_OBJECT

public:

  // constructor - destructor
  BackPic(QWidget* parent=0,const char* name=0);
  ~BackPic();

  // get pictures
  void setClipart(const char*);
  const char* getClipart() {return (const char*)fileName;}
  QPicture* getPic();

private:

  // datas
  QPicture *pic;
  QString fileName;
  QWinMetaFile wmf;

};

/******************************************************************/
/* class KPresenterChild                                          */
/******************************************************************/
class KPresenterChild : public KoDocumentChild
{
public:

  // constructor - destructor
  KPresenterChild( KPresenterDocument_impl *_kpr,const QRect& _rect,OPParts::Document_ptr _doc );
  KPresenterChild( KPresenterDocument_impl *_kpr );
  ~KPresenterChild();
  
  // get parent
  KPresenterDocument_impl* parent() {return m_pKPresenterDoc;}
  
protected:

  // parent, document and geometry
  KPresenterDocument_impl *m_pKPresenterDoc;
};

/*****************************************************************/
/* class KPresenterDocument_impl                                 */
/*****************************************************************/
class KPresenterDocument_impl : public QObject,
				virtual public KoDocument,
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

  // load
  virtual bool load(KOMLParser&);
  virtual bool loadChildren( OPParts::MimeMultipartDict_ptr _dict );

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
  virtual void insertObject(const QRect&,const char*);
  virtual void insertChild( KPresenterChild *_child );
  
  // change geomentry of a child
  virtual void changeChildGeometry(KPresenterChild*,const QRect&);
  
  // get iterator if a child
  virtual QListIterator<KPresenterChild> childIterator();
  
  // page layout
  void setPageLayout(KoPageLayout,int,int);
  KoPageLayout pageLayout() {return _pageLayout;}

  // insert a page
  unsigned int insertNewPage(int,int); 
  unsigned int insertNewTemplate(int,int,bool clean=false);

  // get number of pages nad objects
  unsigned int getPageNums() {return _pageNums;}
  unsigned int objNums() {return _objNums;}

  // background
  void setBackColor(unsigned int,QColor,QColor,BCType);
  void setBackPic(unsigned int,const char*);
  void setBackClip(unsigned int,const char*);
  void setBPicView(unsigned int,BackView);
  void setBackType(unsigned int,BackType);
  bool setPenBrush(QPen,QBrush,int,int);
  void setPageEffect(unsigned int,PageEffect);
  BackType getBackType(unsigned int);
  BackView getBPicView(unsigned int);
  const char* getBackPic(unsigned int);
  const char* getBackClip(unsigned int);
  QColor getBackColor1(unsigned int);
  QColor getBackColor2(unsigned int);
  BCType getBackColorType(unsigned int);
  PageEffect getPageEffect(unsigned int);
  QPen getPen(QPen);
  QBrush getBrush(QBrush);

  // raise and lower objs
  void raiseObjs(int,int);
  void lowerObjs(int,int);

  // insert/change objects
  void insertPicture(const char*,int,int);
  void insertClipart(const char*,int,int);
  void changePicture(const char*,int,int);
  void changeClipart(const char*,int,int);
  void insertLine(QPen,LineType,int,int);
  void insertRectangle(QPen,QBrush,RectType,int,int);
  void insertCircleOrEllipse(QPen,QBrush,int,int);
  void insertText(int,int);
  void insertAutoform(QPen,QBrush,const char*,int,int);
  
  // get list of pages and objects
  QList<Background> *pageList() {return &_pageList;}
  QList<PageObjects> *objList() {return &_objList;}

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

  // delete/rotate/rearrange/reorder obejcts
  void deleteObjs();
  void copyObjs(int,int);
  void pasteObjs(int,int);
  void rotateObjs();
  void reArrangeObjs();

  // repaint all views
  void repaint(bool);
  void repaint(unsigned int,unsigned int,unsigned int,unsigned int,bool);

  // stuff for screen-presentations
  QList<int> reorderPage(unsigned int,int,int,float fakt = 1.0);
  int getPageOfObj(int,int,int,float fakt = 1.0);

  QList<KPresenterChild> lstChildren() {return m_lstChildren;}

signals:

  // document modified
  void sig_KPresenterModified();

  // object inserted - removed
  void sig_insertObject(KPresenterChild *_child);
  void sig_removeObject(KPresenterChild *_child);

  // update child geometry
  void sig_updateChildGeometry(KPresenterChild *_child);

  // restore back color
  void restoreBackColor(unsigned int);

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

  void saveBackground(ostream&);
  void saveObjects(ostream&);
  void saveTxtObj(ostream&,KTextObject*);
  void loadBackground(KOMLParser&,vector<KOMLAttrib>&);
  void loadObjects(KOMLParser&,vector<KOMLAttrib>&);
  void loadTxtObj(KOMLParser&,vector<KOMLAttrib>&,KTextObject*);
  void replaceObjs();
  
  // ************ variables ************

  // list of views and children
  QList<KPresenterView_impl> m_lstViews;
  QList<KPresenterChild> m_lstChildren;
  KPresenterView_impl *viewPtr;

  // modified?
  bool m_bModified;

  // page layout and background
  KoPageLayout _pageLayout;
  QList<Background> _pageList; 

  // list of objects
  QList<PageObjects> _objList;     

  // number of pages and objects
  unsigned int _pageNums;
  unsigned int _objNums;

  // screenpresentations
  bool _spInfinitLoop,_spManualSwitch;

  // options
  int _rastX,_rastY;   
  int _xRnd,_yRnd;

  // options for editmode
  QColor _txtBackCol;
  QColor _txtSelCol;

  // pointers
  PageObjects *objPtr;
  Background *pagePtr;
  
  // url
  QString m_strFileURL;

  bool _clean;
  int objStartY,objStartNum;

};

#endif





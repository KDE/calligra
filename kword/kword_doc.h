/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer 1997-1998                   */
/* based on Torben Weis' KWord                                    */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Document (header)                                      */
/******************************************************************/

#ifndef kword_doc_h
#define kword_doc_h

class KWordDocument_impl;

#include <koDocument.h>
#include <koPrintExt.h>
#include <document_impl.h>
#include <view_impl.h>

#include <iostream>

#include "kword_view.h"
#include "defs.h"
#include "font.h"
#include "fc.h"
#include "parag.h"
#include "paraglayout.h"
#include "formatcollection.h"

#include <qlist.h>
#include <qobject.h>
#include <qrect.h>
#include <qevent.h>
#include <qpainter.h>

#include <koPageLayoutDia.h>

#define MIME_TYPE "application/x-kword"
#define KWordRepoID "IDL:KWord/KWordDocument:1.0"

/******************************************************************/
/* Class: KWordChild                                              */
/******************************************************************/

class KWordChild : public KoDocumentChild
{
public:
  KWordChild(KWordDocument_impl *_wdoc,const QRect& _rect,OPParts::Document_ptr _doc);
  KWordChild(KWordDocument_impl *_wdoc);
  ~KWordChild();
  
  KWordDocument_impl* parent() 
    { return m_pKWordDoc; }
  
protected:
  KWordDocument_impl *m_pKWordDoc;

};

/******************************************************************/
/* Class: KWordDocument_impl                                      */
/******************************************************************/

class KWordDocument_impl : public QObject,
			   virtual public KoDocument,
			   virtual public KoPrintExt,
			   virtual public KWord::KWordDocument_skel
{
  Q_OBJECT

public:
  KWordDocument_impl();
  ~KWordDocument_impl();
  
protected:
  virtual void cleanUp();

  virtual bool hasToWriteMultipart();
  
public:
  // IDL
  virtual CORBA::Boolean init();

  // C++
  virtual bool load(istream& in,bool _randomaccess);
  virtual bool load(KOMLParser& parser);
  virtual bool loadChildren(OPParts::MimeMultipartDict_ptr _dict);
  virtual bool save(ostream& out);
  
  // IDL
  virtual OPParts::View_ptr createView();

  virtual void viewList(OPParts::Document::ViewList*& _list);

  virtual char* mimeType() 
    { return CORBA::string_dup(MIME_TYPE); }
  
  virtual CORBA::Boolean isModified() 
    { return m_bModified; }
  
  // C++

  unsigned int getNumViews() { return m_lstViews.count(); }

  virtual QStrList outputFormats();
  virtual QStrList inputFormats();

  virtual void addView(KWordView_impl *_view);
  virtual void removeView(KWordView_impl *_view);

  virtual void insertObject(const QRect& _rect,const char *_part_name);
  virtual void changeChildGeometry(KWordChild *_child,const QRect&);
  
  virtual QListIterator<KWordChild> childIterator();

  /**
   * If another parag becomes the first one it uses this function
   * to tell the document about it.
   */
  void setFirstParag(KWParag *_parag) 
    { parags = _parag; }

  /**
   * Sets the paper size and recalculates the papers width and height.
   */
  void setPageLayout(KoPageLayout _layout,KoKWord _kw)
    { pageLayout = _layout; otherLayout = _kw; calcColumnWidth(); updateAllViews(0L); }

  void getPageLayout(KoPageLayout& _layout,KoKWord& _kw)
    { _layout = pageLayout; _kw = otherLayout; }
    
  /**
   * @return the default user font.
   *
   * @see KWUserFont
   */
  KWUserFont* getDefaultUserFont() 
    { return defaultUserFont; }
    
  /**
   * List of all available user fonts
   */    
  QList<KWUserFont> userFontList;

  /**
   * List of all available display fonts
   */
  QList<KWDisplayFont> displayFontList;

  /**
   * List of all available parag layouts.
   */
  QList<KWParagLayout> paragLayoutList;

  /**
   * @return a pointer to the user font with the given name or 0L if not found.
   *
   * @see KWUserFont
   */
  KWUserFont* findUserFont(char* _fontname);

  /**
   * @return a display font matching the criteriums or 0L if none is found.
   */
  KWDisplayFont* findDisplayFont(KWUserFont* _font,unsigned int _size,int _weight,bool _italic,bool _underline);

  /**
   * @return a pointer to the parag with the given name or 0L if not found.
   *
   * @see KWParagLayout
   */
  KWParagLayout* findParagLayout(const char *_name);

  KWParag* findFirstParagOfPage(unsigned int _page);
    
  KWParag* getFirstParag() { return parags; }

  float getMMTopBorder() { return pageLayout.top; }
  float getMMBottomBorder() { return pageLayout.bottom; }
  float getMMLeftBorder() { return pageLayout.left; }
  float getMMRightBorder() { return pageLayout.right; }
  
  unsigned int getPTTopBorder() { return MM_TO_POINT(pageLayout.top); }
  unsigned int getPTBottomBorder() { return MM_TO_POINT(pageLayout.bottom); }
  unsigned int getPTLeftBorder() { return MM_TO_POINT(pageLayout.left); }
  unsigned int getPTRightBorder() { return MM_TO_POINT(pageLayout.right); }
  unsigned int getPTPaperHeight() { return MM_TO_POINT(pageLayout.height); }
  unsigned int getPTPaperWidth() { return MM_TO_POINT(pageLayout.width); }
  unsigned int getPTColumnWidth() { return ptColumnWidth; }
  unsigned int getPTColumnSpacing() { return MM_TO_POINT(otherLayout.columnSpacing); }
    
  unsigned int getColumns() { return otherLayout.columns; }
  
  void print() {}
  
  /**
   * @param _ypos is relative to the upper left corner of the FIRST
   *              page of the document.
   *
   * @return TRUE if '_ypos' is within the printable area of the
   *         page with number '_page'. The borders of the paper
   *         are not considered to belong to the printable area.
   */
  bool isPTYIn(unsigned int _page,unsigned int _ypos);
  
  void printLine(KWFormatContext &_fc,QPainter &_painter,int xOffset,int yOffset);
  
  void drawMarker(KWFormatContext &_fc,QPainter *_painter,int xOffset,int yOffset);

  void updateAllViews(KWordView_impl *_view);
  void updateAllRanges();

  int getPages() { return pages; }

  void setPages(int _pages)
    { pages = _pages; updateAllRanges(); }

  void deleteParag(KWParag *_parag);
  void joinParag(KWParag *_parag1,KWParag *_parag2);
  void insertParag(KWParag *_parag,InsertPos _pos);
  void splitParag(KWParag *_parag,unsigned int _pos);

  KWFormatCollection *getFormatCollection()
    { return &formatCollection; }

signals:
  void sig_imageModified();
  void sig_insertObject(KWordChild *_child);
  void sig_updateChildGeometry(KWordChild *_child);
  void sig_removeObject(KWordChild *_child);

protected:
  virtual void insertChild(KWordChild*);
  virtual void makeChildListIntern(OPParts::Document_ptr _doc,const char *_path);
  
  virtual void draw(QPaintDevice*,CORBA::Long _width,CORBA::Long _height);

  QList<KWordView_impl> m_lstViews;
  QList<KWordChild> m_lstChildren;

  bool m_bModified;

  /**
   * Calculates the width of a column.
   *
   * @see #columnWidth
   * @see #ptColumnWidth
   */
  void calcColumnWidth();
    
  KoPageLayout pageLayout;
  KoKWord otherLayout;
    
  /**
   * Width of a column in unzoomed points. This value is automatically calculated,
   * so dont write to it if you dont know exactly what you are doing.
   *
   * @see #calcColumnWidth
   */
  unsigned int ptColumnWidth;
  
  /**
   * List of all parags that make up the text.
   *
   * @see KWParag
   */
  KWParag *parags;
  
  /**
   * The default user font.
   *
   * @see KWUserFont
   */
  KWUserFont *defaultUserFont;
  /**
   * The default parag layout
   *
   * @see KWParagLayout
   */
  KWParagLayout *defaultParagLayout;
  
  int pages;

  KWFormatCollection formatCollection;

};

#endif

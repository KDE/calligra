/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Document structure (header)                            */
/******************************************************************/

#ifndef docstruct_h
#define docstruct_h

#include <qlistview.h>
#include <qframe.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qintdict.h>

#include "parag.h"
#include "frame.h"

class KWordDocument;
class KWordGUI;

/******************************************************************/
/* Class: KWDocStructParagItem                                    */
/******************************************************************/

class KWDocStructParagItem : public QObject,
			     virtual public QListViewItem
{
  Q_OBJECT

public:
  KWDocStructParagItem(QListViewItem *_parent,QString _text,KWParag *_parag,KWordGUI*__parent);
  KWDocStructParagItem(QListViewItem *_parent,QListViewItem *_after,QString _text,KWParag *_parag,KWordGUI*__parent);

public slots:
  void slotDoubleClicked(QListViewItem *_item);

protected:
  KWParag *parag;
  KWordGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructFrameItem                                    */
/******************************************************************/

class KWDocStructFrameItem : public QObject,
			     virtual public QListViewItem
{
  Q_OBJECT

public:
  KWDocStructFrameItem(QListViewItem *_parent,QString _text,KWFrameSet *_frameset,KWFrame *_frame,KWordGUI*__parent);

public slots:
  void slotDoubleClicked(QListViewItem *_item);

protected:
  KWFrame *frame;
  KWFrameSet *frameset;
  KWordGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructTableItem                                    */
/******************************************************************/

class KWDocStructTableItem : public QObject,
			     virtual public QListViewItem
{
  Q_OBJECT

public:
  KWDocStructTableItem(QListViewItem *_parent,QString _text,KWGroupManager *_table,KWordGUI*__parent);

public slots:
  void slotDoubleClicked(QListViewItem *_item);

protected:
  KWGroupManager *table;
  KWordGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructPictureItem                                  */
/******************************************************************/

class KWDocStructPictureItem : public QObject,
			       virtual public QListViewItem
{
  Q_OBJECT

public:
  KWDocStructPictureItem(QListViewItem *_parent,QString _text,KWPictureFrameSet *_pic,KWordGUI*__parent);

public slots:
  void slotDoubleClicked(QListViewItem *_item);

protected:
  KWPictureFrameSet *pic;
  KWordGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructPartItem                                     */
/******************************************************************/

class KWDocStructPartItem : public QObject,
			    virtual public QListViewItem
{
  Q_OBJECT

public:
  KWDocStructPartItem(QListViewItem *_parent,QString _text,KWPartFrameSet *_part,KWordGUI*__parent);

public slots:
  void slotDoubleClicked(QListViewItem *_item);

protected:
  KWPartFrameSet *part;
  KWordGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructRootItem                                     */
/******************************************************************/

class KWDocStructRootItem : public QListViewItem
{
public:
  enum Type {Arrangement,Tables,Pictures,Cliparts,TextFrames,Embedded};

  KWDocStructRootItem(QListView *_parent,KWordDocument *_doc,Type _type,KWordGUI*__parent);

  void setupArrangement();
  void setupTextFrames();
  void setupTables();
  void setupPictures();
  void setupCliparts();
  void setupEmbedded();

  virtual void setOpen(bool o);

protected:
  KWordDocument *doc;
  Type type;
  KWordGUI *gui;

};

/******************************************************************/
/* Class: KWDocStructTree                                         */
/******************************************************************/

class KWDocStructTree : public QListView
{
  Q_OBJECT

public:
  KWDocStructTree(QWidget *_parent,KWordDocument *_doc,KWordGUI*__parent);

  void setup();

protected:
  KWordDocument *doc;
  KWordGUI *gui;

  KWDocStructRootItem *arrangement,*tables,*pictures,*cliparts,*textfrms,*embedded;

};

/******************************************************************/
/* Class: KWDocStruct                                             */
/******************************************************************/

class KWDocStruct : public QWidget
{
  Q_OBJECT

public:
  KWDocStruct(QWidget *_parent,KWordDocument *_doc,KWordGUI*__parent);

protected:
  KWDocStructTree *tree;
  QGridLayout *layout;
  
  KWordDocument *doc;
  KWordGUI *parent;

};

#endif

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

class KWordDocument;
class KWordGUI;

/******************************************************************/
/* Class: KWDocStructRootItem                                     */
/******************************************************************/

class KWDocStructRootItem : public QListViewItem
{
public:
  enum Type {Arrangement,Tables,Pictures,Cliparts,TextFrames,Embedded};

  KWDocStructRootItem(QListView *_parent,KWordDocument *_doc,Type _type);

protected:
  KWordDocument *doc;
  Type type;

};

/******************************************************************/
/* Class: KWDocStructTree                                         */
/******************************************************************/

class KWDocStructTree : public QListView
{
  Q_OBJECT

public:
  KWDocStructTree(QWidget *_parent,KWordDocument *_doc);

  void setup();

protected:
  KWordDocument *doc;

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

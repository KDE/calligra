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
/* Module: Stylist Dialog                                         */
/******************************************************************/

#ifndef stylist_h
#define stylist_h

#include <qtabdialog.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qfont.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpainter.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kfontdialog.h>
#include <kcolordlg.h>

#include "paragdia.h"

class KWordDocument;
class KWStyleEditor;

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

class KWStyleManager : public QTabDialog
{
  Q_OBJECT

public:
  KWStyleManager(QWidget *_parent,KWordDocument *_doc);

protected:
  void setupTab1();

  QWidget *tab1;
  QGridLayout *grid1;
  QListBox *lStyleList;
  QPushButton *bEdit,*bDelete,*bAdd,*bUp,*bDown,*bCopy;
  KButtonBox *bButtonBox;

  KWordDocument *doc;
  KWStyleEditor *editor;

protected slots:
  void editStyle();
  void editStyle(int) { editStyle(); }
  void updateStyles();

};

/******************************************************************/
/* Class: KWStylePreview                                         */
/******************************************************************/

class KWStylePreview : public QGroupBox
{
  Q_OBJECT

public:
  KWStylePreview(const char *title,QWidget *parent,KWParagLayout *_style) : QGroupBox(title,parent,"")
    { style = _style; }

protected:
  void drawContents(QPainter *painter);

  KWParagLayout *style;

};

/******************************************************************/
/* Class: KWStyleEditor                                           */
/******************************************************************/

class KWStyleEditor : public QTabDialog
{
  Q_OBJECT

public:
  KWStyleEditor(QWidget *_parent,KWParagLayout *_style,KWordDocument *_doc);

protected:
  void setupTab1();

  QWidget *tab1,*nwid;
  QGridLayout *grid1,*grid2;
  QPushButton *bFont,*bColor,*bSpacing,*bAlign,*bBorders,*bNumbering;
  KButtonBox *bButtonBox;
  KWStylePreview *preview;
  QLabel *lName;
  QLineEdit *eName;

  KWParagLayout *style;
  KWordDocument *doc;
  KWParagDia *paragDia;

signals:
  void updateStyles();

protected slots:
  void changeFont();
  void changeColor();
  void changeSpacing();
  void changeAlign();
  void changeBorders();
  void changeNumbering();
  void paragDiaOk();

};

#endif





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
#include <qpen.h>
#include <qbrush.h>
#include <qstrlist.h>
#include <qcombobox.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kcolordlg.h>
#include <kfontdialog.h>

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
  KWStyleManager(QWidget *_parent,KWordDocument *_doc,QStrList _fontList);

protected:
  void setupTab1();
  void setupTab2();

  QWidget *tab1,*tab2;
  QGridLayout *grid1,*grid2;
  QListBox *lStyleList;
  QPushButton *bEdit,*bDelete,*bAdd,*bUp,*bDown,*bCopy;
  KButtonBox *bButtonBox;
  QComboBox *cFont,*cColor,*cBorder,*cIndent,*cAlign,*cNumbering;

  KWordDocument *doc;
  KWStyleEditor *editor;
  QStrList fontList;

protected slots:
  void apply();
  void editStyle();
  void editStyle(int) { editStyle(); }
  void addStyle();

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
  KWStyleEditor(QWidget *_parent,KWParagLayout *_style,KWordDocument *_doc,QStrList _fontList);
  ~KWStyleEditor() { delete style; }

protected:
  void setupTab1();

  QWidget *tab1,*nwid;
  QGridLayout *grid1,*grid2;
  QPushButton *bFont,*bColor,*bSpacing,*bAlign,*bBorders,*bNumbering;
  KButtonBox *bButtonBox;
  KWStylePreview *preview;
  QLabel *lName,*lFollowing;
  QLineEdit *eName;
  QComboBox *cFollowing;

  KWParagLayout *style,*ostyle;
  KWordDocument *doc;
  KWParagDia *paragDia;
  QStrList fontList;

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
  void apply();
  void fplChanged(const char* n) { style->setFollowingParagLayout(QString(n)); }

};

#endif





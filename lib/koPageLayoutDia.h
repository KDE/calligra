/******************************************************************/
/* KOffice Library - (c) by Reginald Stadlbauer 1998              */
/* Version: 1.0                                                   */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page Layout Dialog (header)                            */
/******************************************************************/

#ifndef KOPGLAYOUTDIA_H
#define KOPGLAYOUTDIA_H

#include <stdio.h>
#include <stdlib.h>

#include <qtabdlg.h>
#include <qcombo.h>
#include <qlabel.h>
#include <qgrpbox.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qpixmap.h>

#include <kapp.h>
#include <krestrictedline.h>

// paper formats (mm)
#define PG_A3_WIDTH          297.0
#define PG_A3_HEIGHT         420.0
#define PG_A4_WIDTH          210.0
#define PG_A4_HEIGHT         297.0
#define PG_A5_WIDTH          148.0
#define PG_A5_HEIGHT         210.0
#define PG_US_LETTER_WIDTH   216.0
#define PG_US_LETTER_HEIGHT  279.0
#define PG_US_LEGAL_WIDTH    216.0
#define PG_US_LEGAL_HEIGHT   356.0
#define PG_SCREEN_WIDTH      240.0
#define PG_SCREEN_HEIGHT     180.0

// enums
enum KoTabs {FORMAT_AND_BORDERS = 1,HEADER_AND_FOOTER = 2};
enum KoFormat {PG_DIN_A3 = 0,PG_DIN_A4 = 1,PG_DIN_A5 = 2,PG_US_LETTER = 3,PG_US_LEGAL = 4,PG_SCREEN = 5,PG_CUSTOM = 6};
enum KoOrientation {PG_PORTRAIT = 0,PG_LANDSCAPE = 1};
enum KoUnit {PG_MM = 0,PG_CM = 1,PG_INCH = 2};

// structure for page layout
struct KoPageLayout
{
  KoFormat format;
  KoOrientation orientation;
  double width,height;
  double left,right,top,bottom;
  KoUnit unit;
};

/******************************************************************/
/* class KoPagePreview                                            */
/******************************************************************/

class KoPagePreview : public QGroupBox
{
  Q_OBJECT

public:

  // constructor - destructor
  KoPagePreview(QWidget*,const char*,KoPageLayout);
  ~KoPagePreview();
  
  // set page layout
  void setPageLayout(KoPageLayout);

protected:
  
  // paint page
  void drawContents(QPainter*);

  // vars
  int pgWidth,pgHeight,pgX,pgY,pgW,pgH;

};

/******************************************************************/
/* class KoPageLayoutDia                                          */
/******************************************************************/

class KoPageLayoutDia : public QTabDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  KoPageLayoutDia(QWidget*,const char*,KoPageLayout,KoTabs);
  ~KoPageLayoutDia();              

  // show page layout dialog
  static bool pageLayout(KoPageLayout&,KoTabs);

  // get a standard page layout
  static KoPageLayout standardLayout();

  // should NOT be used by an app - just for internal use!
  KoPageLayout getLayout() {return layout;};

protected:

  // setup tabs
  void setupTab1();
  void setValuesTab1();
  void setupTab2();

  // update preview
  void updatePreview(KoPageLayout);

  // dialog objects
  QGroupBox *formatFrame,*borderFrame;
  QWidget *tab1,*tab2;
  QGridLayout *grid1,*formatGrid,*borderGrid,*grid2;
  QLabel *lpgFormat,*lpgOrientation,*lpgUnit,*lpgWidth,*lpgHeight,*lbrLeft,*lbrRight,*lbrTop,*lbrBottom;
  QComboBox *cpgFormat,*cpgOrientation,*cpgUnit;
  KRestrictedLine *epgWidth,*epgHeight,*ebrLeft,*ebrRight,*ebrTop,*ebrBottom;
  KoPagePreview *pgPreview;

  // layout
  KoPageLayout layout;

  bool retPressed;

private slots:     

  // take changes
  void Ok() {}                                  
  void Apply() {} 

  // combos
  void unitChanged(int);
  void formatChanged(int);
  void orientationChanged(int);

  // linedits
  void widthChanged();
  void heightChanged();
  void leftChanged();
  void rightChanged();
  void topChanged();
  void bottomChanged();
  void rPressed() {retPressed = true;}

};
#endif //KOPGLAYOUTDIA_H




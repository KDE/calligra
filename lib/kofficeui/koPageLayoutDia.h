/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// Description: Page Layout Dialog (header)

#ifndef __KOPGLAYOUTDIA_H__
#define __KOPGLAYOUTDIA_H__

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
#include <qlined.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

#include <kapp.h>
#include <qspinbox.h>

// paper formats ( mm )
#define PG_A3_WIDTH		297.0
#define PG_A3_HEIGHT		420.0
#define PG_A4_WIDTH		210.0
#define PG_A4_HEIGHT		297.0
#define PG_A5_WIDTH		148.0
#define PG_A5_HEIGHT		210.0
#define PG_B5_WIDTH		182.0
#define PG_B5_HEIGHT		257.0
#define PG_US_LETTER_WIDTH	216.0
#define PG_US_LETTER_HEIGHT	279.0
#define PG_US_LEGAL_WIDTH	216.0
#define PG_US_LEGAL_HEIGHT	356.0
#define PG_US_EXECUTIVE_WIDTH	191.0
#define PG_US_EXECUTIVE_HEIGHT	254.0
#define PG_SCREEN_WIDTH		240.0
#define PG_SCREEN_HEIGHT	180.0

// paper formats ( inch )
#define PG_A3_WIDTH_I		  11.69
#define PG_A3_HEIGHT_I		  16.54
#define PG_A4_WIDTH_I		  8.26
#define PG_A4_HEIGHT_I		  11.7
#define PG_A5_WIDTH_I		  5.83
#define PG_A5_HEIGHT_I		  8.27
#define PG_B5_WIDTH_I		  7.17
#define PG_B5_HEIGHT_I		  10.13
#define PG_US_LETTER_WIDTH_I	  8.5
#define PG_US_LETTER_HEIGHT_I	  11.0
#define PG_US_LEGAL_WIDTH_I	  8.5
#define PG_US_LEGAL_HEIGHT_I	  14.0
#define PG_US_EXECUTIVE_WIDTH_I	  7.5
#define PG_US_EXECUTIVE_HEIGHT_I  10.0
#define PG_SCREEN_WIDTH_I	  9.45
#define PG_SCREEN_HEIGHT_I	  7.09

// enums
const int FORMAT_AND_BORDERS = 1;
const int HEADER_AND_FOOTER = 2;
const int COLUMNS = 4;
const int DISABLE_BORDERS = 8;
const int KW_HEADER_AND_FOOTER = 16;
const int DISABLE_UNIT = 32;
const int USE_NEW_STUFF = 64;

enum KoFormat {
    PG_DIN_A3 = 0, 
    PG_DIN_A4 = 1, 
    PG_DIN_A5 = 2, 
    PG_US_LETTER = 3, 
    PG_US_LEGAL = 4, 
    PG_SCREEN = 5, 
    PG_CUSTOM = 6, 
    PG_DIN_B5 = 7, 
    PG_US_EXECUTIVE = 8
};
enum KoOrientation {
    PG_PORTRAIT = 0, 
    PG_LANDSCAPE = 1
};
enum KoUnit {
    PG_MM = 0, 
    PG_PT = 1, 
    PG_INCH = 2
};
enum KoHFType {
    HF_SAME = 0, 
    HF_FIRST_DIFF = 2, 
    HF_EO_DIFF = 3
};

// structure for page layout
struct KoPageLayout
{
    KoFormat format;
    KoOrientation orientation;

    // WARNING: using these values is obsolote!!!!!!!!!!!
    double width;
    double height;
    double left;
    double right;
    double top;
    double bottom;

    KoUnit unit;

    unsigned int ptWidth;
    unsigned int ptHeight;
    unsigned int ptLeft;
    unsigned int ptRight;
    unsigned int ptTop;
    unsigned int ptBottom;
    float mmWidth;
    float mmHeight;
    float mmLeft;
    float mmTop;
    float mmRight;
    float mmBottom;
    float inchWidth;
    float inchHeight;
    float inchLeft;
    float inchTop;
    float inchRight;
    float inchBottom;

//   bool operator==( const KoPageLayout _l ) {
//     return ( _l.format == format &&
//	    _l.orientation == orientation &&
//	    _l.width == width &&
//	    _l.height == height &&
//	    _l.left == left && _l.right == right &&
//	    _l.top == top && _l.bottom == bottom &&
//	    _l.unit == unit );
//   }
//   bool operator!=( const KoPageLayout _l ) {
//     return ( _l.format != format &&
//	    _l.orientation != orientation ||
//	    _l.width != width ||
//	    _l.height != height ||
//	    _l.left != left || _l.right != right ||
//	    _l.top != top || _l.bottom != bottom ||
//	    _l.unit != unit );
//   }
};

// structure for header-footer
struct KoHeadFoot
{
    QString headLeft;
    QString headMid;
    QString headRight;
    QString footLeft;
    QString footMid;
    QString footRight;
};

// structure for columns
struct KoColumns
{
    int columns;
    unsigned int ptColumnSpacing;
    float mmColumnSpacing;
    float inchColumnSpacing;
};

// structure for KWord header-Footer
struct KoKWHeaderFooter
{
    KoHFType header;
    KoHFType footer;
    unsigned int ptHeaderBodySpacing;
    unsigned int ptFooterBodySpacing;
    float mmHeaderBodySpacing;
    float mmFooterBodySpacing;
    float inchHeaderBodySpacing;
    float inchFooterBodySpacing;
};

/**
 *  KoPagePreview
 */

class KoPagePreview : public QGroupBox
{
    Q_OBJECT

public:

    /**
     *  constructor
     */
    KoPagePreview( QWidget*, const char*, KoPageLayout );
    /**
     *  destructor
     */
    ~KoPagePreview();

    /**
     *  set page layout
     */
    void setPageLayout( KoPageLayout );
    void setPageColumns( KoColumns );

protected:

    // paint page
    void drawContents( QPainter* );

    // vars
    int pgWidth;
    int pgHeight;
    int pgX;
    int pgY;
    int pgW;
    int pgH;
    int columns;

};

/**
 *  KoPageLayoutDia
 */
class KoPageLayoutDia : public QTabDialog
{
    Q_OBJECT

public:
    // constructor - destructor
    KoPageLayoutDia( QWidget*, const char*, KoPageLayout, KoHeadFoot, int );
    KoPageLayoutDia( QWidget* parent, const char* name, KoPageLayout _layout, KoHeadFoot _hf, 
		    KoColumns _kw, KoKWHeaderFooter _kwhf, int tabs );
    ~KoPageLayoutDia();

    // show page layout dialog
    static bool pageLayout( KoPageLayout&, KoHeadFoot&, int );
    static bool pageLayout( KoPageLayout&, KoHeadFoot&, KoColumns&, KoKWHeaderFooter&, int );

    // get a standard page layout
    static KoPageLayout standardLayout();

    // should NOT be used by an app - just for internal use!
    KoPageLayout getLayout() {return layout;};
    KoHeadFoot getHeadFoot();
    KoColumns getColumns();
    KoKWHeaderFooter getKWHeaderFooter();

protected:
    // setup tabs
    void setupTab1();
    void setValuesTab1();
    void setupTab2();
    void setupTab3();
    void setupTab4();

    // update preview
    void updatePreview( KoPageLayout );

    // dialog objects
    QGroupBox *formatFrame;
    QGroupBox *borderFrame;
    QButtonGroup *gHeader;
    QButtonGroup *gFooter;
    QWidget *tab1;
    QWidget *tab2;
    QWidget *tab3;
    QWidget *tab4;
    QGridLayout *grid1;
    QGridLayout *grid3;
    QGridLayout *formatGrid;
    QGridLayout *borderGrid;
    QGridLayout *grid2;
    QGridLayout *grid4;
    QGridLayout *footerGrid;
    QGridLayout *headerGrid;
    QLabel *lpgFormat;
    QLabel *lpgOrientation;
    QLabel *lpgUnit;
    QLabel *lpgWidth;
    QLabel *lpgHeight;
    QLabel *lbrLeft;
    QLabel *lbrRight;
    QLabel *lbrTop;
    QLabel *lbrBottom;
    QComboBox *cpgFormat;
    QComboBox *cpgOrientation;
    QComboBox *cpgUnit;
    QLineEdit *epgWidth;
    QLineEdit *epgHeight;
    QLineEdit *ebrLeft;
    QLineEdit *ebrRight;
    QLineEdit *ebrTop;
    QLineEdit *ebrBottom;
    KoPagePreview *pgPreview;
    KoPagePreview *pgPreview2;
    QLabel *lHeadLeft;
    QLabel *lHeadMid;
    QLabel *lHeadRight;
    QLabel *lHead;
    QLineEdit *eHeadLeft;
    QLineEdit *eHeadMid;
    QLineEdit *eHeadRight;
    QLabel *lFootLeft;
    QLabel *lFootMid;
    QLabel *lFootRight;
    QLabel *lFoot;
    QLabel *lMacros1;
    QLabel *lMacros2;
    QLineEdit *eFootLeft;
    QLineEdit *eFootMid;
    QLineEdit *eFootRight;
    QSpinBox *nColumns;
    QLineEdit *nCSpacing;
    QLineEdit *nHSpacing;
    QLineEdit *nFSpacing;
    QLabel *lColumns;
    QLabel *lCSpacing;
    QLabel *lHSpacing;
    QLabel *lFSpacing;
    QRadioButton *rhSame;
    QRadioButton *rhFirst;
    QRadioButton *rhEvenOdd;
    QRadioButton *rfSame;
    QRadioButton *rfFirst;
    QRadioButton *rfEvenOdd;

    // layout
    KoPageLayout layout;
    KoHeadFoot hf;
    KoColumns cl;
    KoKWHeaderFooter kwhf;

    bool retPressed;
    bool enableBorders;
    int flags;

private slots:
    // take changes
    void Ok() {}

    // combos
    void unitChanged( int );
    void formatChanged( int );
    void orientationChanged( int );

    // linedits
    void widthChanged();
    void heightChanged();
    void leftChanged();
    void rightChanged();
    void topChanged();
    void bottomChanged();
    void rPressed() {retPressed = true;}

    // spinboxes
    void nColChanged( int );
    void nSpaceChanged( const QString & );

};

#endif

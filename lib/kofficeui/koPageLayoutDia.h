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

#include <qtabdialog.h>
#include <qgroupbox.h>

#include <koGlobal.h>

class QButtonGroup;
class QWidget;
class QGridLayout;
class QLabel;
class QComboBox;
class QLineEdit;
class QSpinBox;
class QRadioButton;

// enums
const int FORMAT_AND_BORDERS = 1;
const int HEADER_AND_FOOTER = 2;
const int COLUMNS = 4;
const int DISABLE_BORDERS = 8;
const int KW_HEADER_AND_FOOTER = 16;
const int DISABLE_UNIT = 32;

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
    void setPageLayout( const KoPageLayout& );
    void setPageColumns( const KoColumns& );

protected:

    // paint page
    void drawContents( QPainter* );

    double pgWidth;
    double pgHeight;
    double pgX;
    double pgY;
    double pgW;
    double pgH;
    int columns;
};

/**
 *  With this dialog the user can specify the layout of this papaer during printing.
 */
class KoPageLayoutDia : public QTabDialog
{
    Q_OBJECT

public:

    /**
     *  Constructor.
     *
     *  @param parent   The parent of the dialog.
     *  @param name     The name of the dialog.
     *  @param layout   The layout.
     *  @param headfoot The header and the footer.
     *  @param tabs     The number of tabs.
     */
    KoPageLayoutDia( QWidget* _parent, const char* _name, KoPageLayout _layout, KoHeadFoot _headfoot, int _tabs );

    /**
     *  Constructor.
     *
     *  @param parent     The parent of the dialog.
     *  @param name       The name of the dialog.
     *  @param layout     The layout.
     *  @param headfoot   The header and the footer.
     *  @param columns    The number of columns on the page.
     *  @param kwheadfoot The KWord header and footer.
     *  @param tabs       The number of tabs.
     */
    KoPageLayoutDia( QWidget* parent, const char* name, KoPageLayout layout, KoHeadFoot headfoot,
		    KoColumns columns, KoKWHeaderFooter kwheadfoot, int tabs );

    /**
     *  Destructor.
     */
    ~KoPageLayoutDia();

    /**
     *  Show page layout dialog.
     */
    static bool pageLayout( KoPageLayout&, KoHeadFoot&, int );

    /**
     *  Show page layout dialog.
     */
    static bool pageLayout( KoPageLayout&, KoHeadFoot&, KoColumns&, KoKWHeaderFooter&, int );

    /**
     *  Retrieves a standard page layout.
     */
    static KoPageLayout standardLayout();

    /**
     *  Do not use, for internal only.
     */
    KoPageLayout getLayout() {return layout;};

    /**
     *  Do not use, for internal only.
     */
    KoHeadFoot getHeadFoot();

    /**
     *  Do not use, for internal only.
     */
    KoColumns getColumns();

    /**
     *  Do not use, for internal only.
     */
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

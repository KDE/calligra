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

#include <qgroupbox.h>

#include <koGlobal.h>
#include <kdialogbase.h>

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
 *  KoPagePreview.
 *  Internal to KoPageLayoutDia.
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


class KoPageLayoutDiaPrivate;

/**
 *  With this dialog the user can specify the layout of this paper during printing.
 */
class KoPageLayoutDia : public KDialogBase
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

protected:
    KoPageLayout getLayout() {return layout;};
    KoHeadFoot getHeadFoot();
    KoColumns getColumns();
    KoKWHeaderFooter getKWHeaderFooter();

    // setup tabs
    void setupTab1();
    void setValuesTab1();
    void setValuesTab1Helper();
    void setupTab2();
    void setupTab3();
    void setupTab4();

    // update preview
    void updatePreview( KoPageLayout );

    // dialog objects
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
    QLineEdit *eHeadLeft;
    QLineEdit *eHeadMid;
    QLineEdit *eHeadRight;
    QLineEdit *eFootLeft;
    QLineEdit *eFootMid;
    QLineEdit *eFootRight;
    QSpinBox *nColumns;
    QLineEdit *nCSpacing;
    QLineEdit *nHSpacing;
    QLineEdit *nFSpacing;
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

private:
    void changed(QLineEdit *line, double &mm, double &pt, double &inch);

    KoPageLayoutDiaPrivate *d;
};

#endif

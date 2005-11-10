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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// Description: Page Layout Dialog (header)

#ifndef __KOPGLAYOUTDIA_H__
#define __KOPGLAYOUTDIA_H__

#include <qgroupbox.h>
#include <koGlobal.h>
#include <koUnit.h>
#include <kdialogbase.h>
#include <koPageLayout.h>

class QButtonGroup;
class QWidget;
class QGridLayout;
class QLabel;
class QComboBox;
class QLineEdit;
class QSpinBox;
class QRadioButton;
class QCheckBox;
class KDoubleNumInput;
class KoUnitDoubleSpinBox;

enum { FORMAT_AND_BORDERS = 1, HEADER_AND_FOOTER = 2, COLUMNS = 4, DISABLE_BORDERS = 8,
       KW_HEADER_AND_FOOTER = 16, DISABLE_UNIT = 32 };

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
    KoPagePreview( QWidget*, const char*, const KoPageLayout & );
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
 *  With this dialog the user can specify the layout of the paper during printing.
 */
class KOFFICEUI_EXPORT KoPageLayoutDia : public KDialogBase
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
     *  @param unit     The unit to use for displaying the values to the user.
     *  @param modal    Whether the dialog is modal or not.
     */
    KoPageLayoutDia( QWidget* parent, const char* name,
		     const KoPageLayout& layout,
		     const KoHeadFoot& headfoot,
		     int tabs, KoUnit::Unit unit, bool modal=true );

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
     *  @param unit       The unit to use for displaying the values to the user
     */
    KoPageLayoutDia( QWidget* parent, const char* name,
		     const KoPageLayout& layout,
		     const KoHeadFoot& headfoot,
		     const KoColumns& columns,
		     const KoKWHeaderFooter& kwheadfoot,
		     int tabs, KoUnit::Unit unit );

    /**
     *  Destructor.
     */
    ~KoPageLayoutDia();

    /**
     *  Show page layout dialog.
     *  See constructor for documentation on the parameters
     */
    static bool pageLayout( KoPageLayout&, KoHeadFoot&, int tabs, KoUnit::Unit& unit, QWidget* parent = 0 );

    /**
     *  Show page layout dialog.
     *  See constructor for documentation on the parameters
     */
    static bool pageLayout( KoPageLayout&, KoHeadFoot&, KoColumns&, KoKWHeaderFooter&, int tabs, KoUnit::Unit& unit, QWidget* parent = 0 );
    /**
     *  Retrieves a standard page layout.
     *  Deprecated: better use KoPageLayout::standardLayout()
     */
    static KDE_DEPRECATED KoPageLayout standardLayout();

    /**
     *  Returns the layout
     */
    const KoPageLayout& layout() const { return m_layout; }

    /**
     *  Returns the header and footer information
     */
    KoHeadFoot headFoot() const;

    /**
     *  Returns the unit
     */
    KoUnit::Unit unit() const { return m_unit; }

private:
    const KoColumns& columns();
    const KoKWHeaderFooter& getKWHeaderFooter();

    // setup tabs
    void setupTab1();
    void setValuesTab1();
    void setValuesTab1Helper();
    void setupTab2( const KoHeadFoot& hf );
    void setupTab3();
    void setupTab4();

    // update preview
    void updatePreview( const KoPageLayout& );

    // dialog objects
    QComboBox *cpgFormat;
    QComboBox *cpgUnit;
    QRadioButton *rbPortrait;
    QRadioButton *rbLandscape;
    KoUnitDoubleSpinBox *epgWidth;
    KoUnitDoubleSpinBox *epgHeight;
    KoUnitDoubleSpinBox *ebrLeft;
    KoUnitDoubleSpinBox *ebrRight;
    KoUnitDoubleSpinBox *ebrTop;
    KoUnitDoubleSpinBox *ebrBottom;
    KoPagePreview *pgPreview;
    KoPagePreview *pgPreview2;
    QLineEdit *eHeadLeft;
    QLineEdit *eHeadMid;
    QLineEdit *eHeadRight;
    QLineEdit *eFootLeft;
    QLineEdit *eFootMid;
    QLineEdit *eFootRight;
    QSpinBox *nColumns;
    KoUnitDoubleSpinBox *nCSpacing;

    KoUnitDoubleSpinBox *nHSpacing;
    KoUnitDoubleSpinBox *nFSpacing;
    KoUnitDoubleSpinBox *nFNSpacing;
    QCheckBox *rhFirst;
    QCheckBox *rhEvenOdd;
    QCheckBox *rfFirst;
    QCheckBox *rfEvenOdd;

    // layout
    KoPageLayout m_layout;
    KoColumns m_cl;
    KoKWHeaderFooter kwhf;

    KoUnit::Unit m_unit;

    bool retPressed;
    bool enableBorders;
    int flags;

public slots:

    // linedits
    void widthChanged();
    void heightChanged();
    void leftChanged();
    void rightChanged();
    void topChanged();
    void bottomChanged();

protected slots:
    virtual void slotOk();

private slots:
    // combos and radios
    void unitChanged( int );
    void formatChanged( int );
    void orientationChanged();

    void rPressed() {retPressed = true;}

    // spinboxes
    void nColChanged( int );
    void nSpaceChanged( double );

private:
    void changed(KoUnitDoubleSpinBox *line, double &pt);

    KoPageLayoutDiaPrivate *d;
};

#endif

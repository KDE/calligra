/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef stylist_h
#define stylist_h

#include <kdialogbase.h>
#include <qstringlist.h>

#include "paragdia.h"

class KWDocument;
class KWStyleEditor;
class QWidget;
class QGridLayout;
class QListBox;
class QPushButton;
class KButtonBox;
class QComboBox;
class QLineEdit;
class QLabel;
class QCheckBox;
class KWStyle;

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

class KWStyleManager : public KDialogBase
{
    Q_OBJECT

public:
    KWStyleManager( QWidget *_parent, KWDocument *_doc, QStringList _fontList );

protected:
    void setupTab1();
    void setupTab2();
    bool apply();

    QWidget *tab1, *tab2;
    QGridLayout *grid1, *grid2;
    QListBox *lStyleList;
    QPushButton *bEdit, *bDelete, *bAdd, *bUp, *bDown, *bCopy;
    KButtonBox *bButtonBox;
    QComboBox *cFont, *cColor, *cBorder, *cIndent, *cAlign, *cNumbering, *cTabs;
    QCheckBox *cSmart;

    KWDocument *doc;
    KWStyleEditor *editor;
    QStringList fontList;

protected slots:
    virtual void slotOk();
    void editStyle();
    void editStyle( int ) { editStyle(); }
    void addStyle();
    void copyStyle();
    void upStyle();
    void downStyle();
    void deleteStyle();
    void updateStyleList();
    void updateButtons( const QString & );
    void smartClicked();
};

/******************************************************************/
/* Class: KWStylePreview                                         */
/******************************************************************/

class KWStylePreview : public QGroupBox
{
    Q_OBJECT

public:
    KWStylePreview( const QString &title, QWidget *parent, KWStyle *_style ) : QGroupBox( title, parent, "" )
    { style = _style; }

protected:
    void drawContents( QPainter *painter );

    KWStyle *style;

};

/******************************************************************/
/* Class: KWStyleEditor                                           */
/******************************************************************/

class KWStyleEditor : public KDialogBase
{
    Q_OBJECT

public:
    KWStyleEditor( QWidget *_parent, KWStyle *_style, KWDocument *_doc, QStringList _fontList );
    ~KWStyleEditor();

protected:
    void setupTab1();
    bool apply();

    QWidget *tab1, *nwid;
    QGridLayout *grid1, *grid2;
    QPushButton *bFont, *bColor, *bSpacing, *bAlign, *bBorders, *bNumbering, *bTabulators;
    KButtonBox *bButtonBox;
    KWStylePreview *preview;
    QLineEdit *eName;
    QComboBox *cFollowing;

    KWStyle *style, *ostyle;
    KWDocument *doc;
    KWParagDia *paragDia;
    QStringList fontList;

signals:
    void updateStyleList();

protected slots:
    virtual void slotOk();
    void changeFont();
    void changeColor();
    void changeSpacing();
    void changeAlign();
    void changeBorders();
    void changeNumbering();
    void changeTabulators();
    void paragDiaOk();
};

#endif


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

#ifndef KCHARSELECTDIA_H
#define KCHARSELECTDIA_H

#include <qdialog.h>
#include <qfont.h>
#include <qcolor.h>
#include <qlist.h>

class QWidget;
class QGridLayout;
class QComboBox;
class KButtonBox;
class QPushButton;
class KCharSelect;
class QLabel;
class KColorButton;
class QComboBox;
class QCheckBox;

/******************************************************************/
/* class KCharSelectDia                                           */
/******************************************************************/

class KCharSelectDia : public QDialog
{
    Q_OBJECT

public:
    // constructor - destructor
    KCharSelectDia( QWidget *parent, const char *name, QList<QFont> *_font, QList<QColor>* _color, QList<QChar>* _chr );

    // select char dialog
    static bool selectChar( QList<QFont> *_font, QList<QColor> *_color, QList<QChar> *_chr );

    // internal
    QList<QFont> *font() { return vFont; }
    QList<QColor> *color() { return vColor; }
    QList<QChar> *chr() { return vChr; }

protected:
    // dialog objects
    QGridLayout *grid, *grid2;
    QComboBox *sizeCombo, *depthCombo;
    KButtonBox *bbox;
    QPushButton *bOk, *bCancel;
    KCharSelect *charSelect;
    QWidget *wid;
    QLabel *lSize, *lColor, *lPreview, *lAttrib, *lDepth;
    KColorButton *colorButton;
    QCheckBox *bold, *italic, *underl;

    // values
    QList<QFont> *vFont;
    QList<QColor> *vColor;
    QList<QChar> *vChr;
    int _depth;

protected slots:
    void fontSelected( const QString & );
    void sizeSelected( int );
    void depthSelected( int );
    void colorChanged( const QColor& );
    void boldChanged();
    void italicChanged();
    void underlChanged();
    void charChanged( const QChar & );

};
#endif //KCHARSELECTDIA_H

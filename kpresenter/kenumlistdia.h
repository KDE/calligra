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

#ifndef KENUMLISTDIA_H
#define KENUMLISTDIA_H

#include <qdialog.h>
#include <qfont.h>
#include <qcolor.h>
#include <qstring.h>
#include <qstrlist.h>

class QWidget;
class QGridLayout;
class QLabel;
class QComboBox;
class KColorButton;
class QCheckBox;
class QRadioButton;
class KButtonBox;
class QPushButton;
class QLineEdit;

/******************************************************************/
/* class KEnumListDia                                             */
/******************************************************************/

class KEnumListDia : public QDialog
{
    Q_OBJECT

public:

    static const int NUMBER = 1;
    static const int ALPHABETH = 2;

    // constructor - destructor
    KEnumListDia( QWidget*, const char*, int, QFont, QColor, QString, QString, int, QStringList );
    ~KEnumListDia();

    // show enum list dialog
    static bool enumListDia( int&, QFont&, QColor&, QString&, QString&, int&, QStringList );

    // return values
    int type() {return _type; }
    QFont font() {return _font; }
    QColor color() {return _color; }
    QString before() {return _before; }
    QString after() {return _after; }
    int start() {return _start; }

protected:

    // dialog objects
    QGridLayout *grid;
    QLabel *lFont, *lSize, *lColor, *lAttrib, *lBefore, *lAfter, *lStart;
    QLineEdit *eBefore, *eAfter, *eStart;
    QComboBox *fontCombo, *sizeCombo;
    KColorButton *colorButton;
    QCheckBox *bold, *italic, *underl;
    QRadioButton *number, *alphabeth;
    KButtonBox *bbox;
    QPushButton *bOk, *bCancel;

    // values
    int _type;
    QFont _font;
    QColor _color;
    QString _before;
    QString _after;
    int _start;

    QStringList fontList;

protected slots:
    void fontSelected( const QString & );
    void sizeSelected( int );
    void colorChanged( const QColor& );
    void boldChanged();
    void italicChanged();
    void underlChanged();
    void beforeChanged( const QString & );
    void afterChanged( const QString & );
    void startChanged( const QString & );
    void numChanged();
    void alphaChanged();

};

#endif // KENUMLISTDIA

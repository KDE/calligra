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

#ifndef autoformatdia_h
#define autoformatdia_h

#include <kdialogbase.h>

class KWDocument;
class KWPage;
class QPushButton;
class QCheckBox;
class QVBox;
class QHBox;
class KCharSelect;
class QListView;

/******************************************************************/
/* Class: KWAutoFormatDia                                         */
/******************************************************************/

class KWAutoFormatDia : public KDialogBase
{
    Q_OBJECT

public:
    KWAutoFormatDia( QWidget *parent, const char *name, KWDocument *_doc, KWCanvas *_canvas );

protected:
    bool applyConfig();
    void setupTab1();
    void setupTab2();

    QWidget *tab1;
    QWidget *tab2;
    QCheckBox *cbTypographicQuotes, *cbUpperCase, *cbUpperUpper;
    QPushButton *pbQuote1, *pbQuote2, *pbEdit, *pbRemove, *pbAdd;
    KCharSelect *charselect;
    QListView *entries;

    KWDocument *doc;
    QChar oBegin, oEnd;
    bool quotesChanged;

    KWCanvas *canvas;
    KWAutoFormat m_autoFormat;
protected slots:
    virtual void slotOk();

    void chooseQuote1();
    void chooseQuote2();

};

#endif



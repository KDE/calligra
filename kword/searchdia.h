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

#ifndef search_h
#define search_h

#include <kdialogbase.h>
#include <qstringlist.h>
#include <qcolor.h>
#include <qstring.h>

#include "format.h"

class KWordDocument;
class KWPage;
class KWordView;
class QWidget;
class QGridLayout;
class QGroupBox;
class QComboBox;
class KColorButton;
class QLabel;
class KButtonBox;
class QPushButton;
class QLineEdit;
class QCheckBox;

/******************************************************************/
/* Class: KWSearchDia                                             */
/******************************************************************/

class KWSearchDia : public KDialogBase
{
    Q_OBJECT

public:
    struct KWSearchEntry
    {
        KWSearchEntry() {
            checkFamily = checkColor = checkSize = checkBold = checkItalic = checkUnderline = checkVertAlign = false;
            expr = "";
            family = "times";
            color = black;
            size = 12;
            bold = italic = underline = false;
            vertAlign = KWFormat::VA_NORMAL;
            caseSensitive = false;
            regexp = false;
            reverse = false;
            wholeWords = false;
            wildcard = false;
            ask = true;
        }

        QString expr;
        bool checkFamily, checkColor, checkSize, checkBold, checkItalic, checkUnderline, checkVertAlign;
        QString family;
        QColor color;
        int size;
        bool bold, italic, underline;
        KWFormat::VertAlign vertAlign;
        bool caseSensitive, regexp, reverse, wholeWords, wildcard;
        bool ask;
    };

    KWSearchDia( QWidget *parent, const char *name, KWordDocument *_doc, KWPage *_page, KWordView *_view,
                 KWSearchEntry *_searchEntry, KWSearchEntry *_replaceEntry, QStringList _fontlist );

protected:
    void setupTab1();
    void setupTab2();

    QWidget *tab1;
    QGridLayout *grid1, *sGrid;
    QGroupBox *gSearch;
    QCheckBox *cRegExp, *cFamily, *cSize, *cColor, *cBold, *cItalic, *cUnderline, *cVertAlign, *cmBold, *cmItalic, *cmUnderline, *cCase, *cWholeWords, *cRev;
    QComboBox *cmFamily, *cmSize, *cmVertAlign;
    KColorButton *bColor;
    QLabel *lSearch;
    KButtonBox *bbSearch;
    QPushButton *bSearchFirst, *bSearchNext, *bSearchAll;
    QLineEdit *eSearch;

    QWidget *tab2;
    QGridLayout *grid2, *rGrid;
    QGroupBox *gReplace;
    QCheckBox *rcFamily, *rcSize, *rcColor, *rcBold, *rcItalic, *rcUnderline, *rcVertAlign, *rcmBold, *rcmItalic, *rcmUnderline, *cAsk, *cWildcard;
    QComboBox *rcmFamily, *rcmSize, *rcmVertAlign;
    KColorButton *rbColor;
    QLabel *lReplace;
    KButtonBox *bbReplace;
    QPushButton *bReplaceFirst, *bReplaceNext, *bReplaceAll;
    QLineEdit *eReplace;

    KWordDocument *doc;
    KWPage *page;
    KWordView *view;
    KWSearchEntry *searchEntry, *replaceEntry;
    QStringList fontlist;

protected slots:
    void searchFirst();
    void searchNext();
    void slotCheckFamily();
    void slotCheckColor();
    void slotCheckSize();
    void slotCheckBold();
    void slotCheckItalic();
    void slotCheckUnderline();
    void slotCheckVertAlign();
    void slotFamily( const QString & );
    void slotSize( const QString & );
    void slotColor( const QColor& );
    void slotBold();
    void slotItalic();
    void slotUnderline();
    void slotVertAlign( int );
    void replaceFirst();
    void replaceNext();
    void replaceAll();
    void rslotCheckFamily();
    void rslotCheckColor();
    void rslotCheckSize();
    void rslotCheckBold();
    void rslotCheckItalic();
    void rslotCheckUnderline();
    void rslotCheckVertAlign();
    void rslotFamily( const QString & );
    void rslotSize( const QString & );
    void rslotColor( const QColor& );
    void rslotBold();
    void rslotItalic();
    void rslotUnderline();
    void rslotVertAlign( int );
    void saveSettings();
    void slotRegExp();
};

#endif



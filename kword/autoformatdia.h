/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2001       Sven Leiber         <s.leiber@web.de>

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
#include "autoformat.h"

class QPushButton;
class QCheckBox;
class QVBox;
class QHBox;
class KCharSelect;
class KListView;
class KLineEdit;
class QLineEdit;
class QListBox;


/******************************************************************/
/* Class: KWAutoFormatExceptionWidget                             */
/******************************************************************/

class KWAutoFormatExceptionWidget : public QWidget
{
    Q_OBJECT

public:
    KWAutoFormatExceptionWidget(QWidget *_parent, const QString &name,const QStringList &_list,bool _abbreviation=false);


    QStringList getListException(){return m_listException;}
protected slots:
    void slotAddException();
    void slotRemoveException();
    void textChanged ( const QString & );

 private:
    QListBox *exceptionList;
    QLineEdit *exceptionLine;
    QPushButton *pbAddException,*pbRemoveException;

    QStringList m_listException;
    bool m_bAbbreviation;
};

/******************************************************************/
/* Class: KWAutoFormatDia                                         */
/******************************************************************/

class KWAutoFormatDia : public KDialogBase
{
    Q_OBJECT

public:
    KWAutoFormatDia( QWidget *parent, const char *name, KWAutoFormat * autoFormat );
    void addEntryList(const QString &key, KWAutoFormatEntry &_autoEntry);
    void editEntryList(const QString &key,const QString &newFindString, KWAutoFormatEntry &_autoEntry);
protected:
    bool applyConfig();
    void setupTab1();
    void setupTab2();
    void setupTab3();
    void refreshEntryList();

    QWidget *tab1;
    QWidget *tab2;
    QWidget *tab3;
    QCheckBox *cbTypographicQuotes, *cbUpperCase, *cbUpperUpper;
    QPushButton *pbQuote1, *pbQuote2, /**pbEdit,*/ *pbRemove, *pbAdd, *pbDefault,
                *pbSpecialChar1, *pbSpecialChar2;
    KCharSelect *charselect;
    KLineEdit *m_find, *m_replace;
    KListView *m_pListView;

    QChar oBegin, oEnd;

    KWAutoFormat m_autoFormat; // The copy we're working on
    KWAutoFormat * m_docAutoFormat; // Pointer to the real one (in KWDocument)

    KWAutoFormatExceptionWidget *abbreviation;
    KWAutoFormatExceptionWidget *twoUpperLetter;

protected slots:
    virtual void slotOk();
    void slotItemRenamed(QListViewItem * item, const QString & newText, int column);
    void slotRemoveEntry();
    void slotEditEntry();
    void slotfind( const QString & );
    void slotfind2( const QString & );
    void chooseSpecialChar1();
    void chooseSpecialChar2();
    void chooseQuote1();
    void chooseQuote2();
    void defaultQuote();
    void slotChangeState(bool);
    void slotAddEntry();

};	
#endif



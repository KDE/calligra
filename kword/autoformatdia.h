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
#include "autoformat.h"

class QPushButton;
class QCheckBox;
class QVBox;
class QHBox;
class KCharSelect;
class KListView;
class QLineEdit;

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
    void refreshEntryList();

    QWidget *tab1;
    QWidget *tab2;
    QCheckBox *cbTypographicQuotes, *cbUpperCase, *cbUpperUpper;
    QPushButton *pbQuote1, *pbQuote2, *pbEdit, *pbRemove, *pbAdd;
    KCharSelect *charselect;
    KListView * m_pListView;

    QChar oBegin, oEnd;
    bool quotesChanged;

    KWAutoFormat m_autoFormat; // The copy we're working on
    KWAutoFormat * m_docAutoFormat; // Pointer to the real one (in KWDocument)
protected slots:
    virtual void slotOk();
    void slotItemRenamed(QListViewItem * item, const QString & newText, int column);
    void slotRemoveEntry();
    void slotEditEntry();
    void chooseQuote1();
    void chooseQuote2();
    void slotAddEntry();
};

/******************************************************************/
/* Class: KWAutoFormatEditDia                                     */
/******************************************************************/
class KWAutoFormatEditDia : public QDialog
{
    Q_OBJECT
public:
    KWAutoFormatEditDia( KWAutoFormatDia *parent, const char *name, const QString &title,const QString &findStr,const QString &replaceStr, bool _replaceEntry=false,const QString &_str="" );

protected slots:
    void slotOk();
    void slotCancel();
    void chooseSpecialChar1();
    void chooseSpecialChar2();
private:
    QLineEdit *lineEditFind;
    QLineEdit *lineEditReplace;

    QPushButton* m_pOk;
    QPushButton* m_pClose;
    QPushButton *pbSpecialChar1, *pbSpecialChar2;
    KWAutoFormatDia *parentWidget;
    bool replaceEntry;
    QString replaceEntryString;
};
#endif



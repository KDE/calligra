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

#ifndef KOVARIABLEDLGS_H
#define KOVARIABLEDLGS_H

#include "koVariable.h"

#include <qdialog.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qlistview.h>

class QComboBox;
class QVBox;
class QResizeEvent;
class QLineEdit;
class QCloseEvent;

/******************************************************************
 *
 * Class: KoVariableNameDia
 *
 ******************************************************************/

class KoVariableNameDia : public QDialog
{
    Q_OBJECT

public:
    // For KWSerialLetterVariableInsertDia
    KoVariableNameDia( QWidget *parent );
    // For kwview
    KoVariableNameDia( QWidget *parent, const QPtrList<KoVariable> &vars );
    QString getName() const;

protected slots:
    void textChanged ( const QString &_text );
protected:
    void init();
    void resizeEvent( QResizeEvent *e );

    QComboBox *names;
    QVBox *back;
    QPushButton *ok;

};

/******************************************************************
 * Class: KoCustomVariablesListItem
 * Used by KoCustomVariablesDia
 ******************************************************************/

class KoCustomVariablesListItem : public QListViewItem
{
public:
    KoCustomVariablesListItem( QListView *parent );

    void setVariable( KoCustomVariable *v );
    KoCustomVariable *getVariable() const;

    void setup();
    void update();

    void applyValue();

protected:
    KoCustomVariable *var;
    QLineEdit *editWidget;

};

/******************************************************************
 * Class: KoCustomVariablesList
 * Used by KoCustomVariablesDia
 ******************************************************************/

class KoCustomVariablesList : public QListView
{
    Q_OBJECT

public:
    KoCustomVariablesList( QWidget *parent );

    void setValues();
    void updateItems();

protected slots:
    void columnSizeChange( int c, int os, int ns );
    void sectionClicked( int c );

};

/******************************************************************
 * Class: KoCustomVariablesDia
 * This dialog allows to set the value of the custom variables.
 ******************************************************************/

class KoCustomVariablesDia : public QDialog
{
    Q_OBJECT

public:
    KoCustomVariablesDia( QWidget *parent, const QPtrList<KoVariable> &variables );
protected slots:
    void slotOk();

protected:
    void resizeEvent( QResizeEvent *e );
    //void closeEvent( QCloseEvent *e );

    QVBox *back;
    KoCustomVariablesList *list;

};

#endif

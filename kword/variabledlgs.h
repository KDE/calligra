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

#ifndef KWVARIABLEDLGS_H
#define KWVARIABLEDLGS_H

#include "variable.h"

#include <qdialog.h>
#include <qlist.h>
#include <qstring.h>
#include <qlistview.h>

class QComboBox;
class QVBox;
class QResizeEvent;
class QLineEdit;
class QCloseEvent;

/******************************************************************
 *
 * Class: KWVariableNameDia
 *
 ******************************************************************/

class KWVariableNameDia : public QDialog
{
    Q_OBJECT

public:
    // For KWSerialLetterVariableInsertDia
    KWVariableNameDia( QWidget *parent );
    // For kwview
    KWVariableNameDia( QWidget *parent, const QList<KWVariable> &vars );
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
 * Class: KWCustomVariablesListItem
 * Used by KWCustomVariablesDia
 ******************************************************************/

class KWCustomVariablesListItem : public QListViewItem
{
public:
    KWCustomVariablesListItem( QListView *parent );

    void setVariable( KWCustomVariable *v );
    KWCustomVariable *getVariable() const;

    void setup();
    void update();

    void applyValue();

protected:
    KWCustomVariable *var;
    QLineEdit *editWidget;

};

/******************************************************************
 * Class: KWCustomVariablesList
 * Used by KWCustomVariablesDia
 ******************************************************************/

class KWCustomVariablesList : public QListView
{
    Q_OBJECT

public:
    KWCustomVariablesList( QWidget *parent );

    void setValues();
    void updateItems();

protected slots:
    void columnSizeChange( int c, int os, int ns );
    void sectionClicked( int c );

};

/******************************************************************
 * Class: KWCustomVariablesDia
 * This dialog allows to set the value of the custom variables.
 ******************************************************************/

class KWCustomVariablesDia : public QDialog
{
    Q_OBJECT

public:
    KWCustomVariablesDia( QWidget *parent, const QList<KWVariable> &variables );
protected slots:
    void slotOk();

protected:
    void resizeEvent( QResizeEvent *e );
    //void closeEvent( QCloseEvent *e );

    QVBox *back;
    KWCustomVariablesList *list;

};

#endif

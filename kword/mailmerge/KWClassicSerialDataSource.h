/* This file is part of the KDE project
   Original file (KWMailMergeDataBase.h): Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _SERIALLETTER_CLASSIC_PLUGIN_H_
#define _SERIALLETTER_CLASSIC_PLUGIN_H_

#include <qdom.h>
#include <q3listview.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3ValueList>
#include <QResizeEvent>
#include <kdialogbase.h>
#include <KoCustomVariablesDia.h>
#include "KWMailMergeDataSource.h"
#include <QSpinBox>
#include <kvbox.h>

class KHBox;
class KVBox;
class QPushButton;
class Q3ListBox;
class QLabel;
class QLineEdit;
class QToolButton;

/******************************************************************
 *
 * Class: KWClassicSerialDataSource
 *
 ******************************************************************/
typedef Q3ValueList< DbRecord > Db;

class KWClassicSerialDataSource: public KWMailMergeDataSource
{
    Q_OBJECT
    public:
    KWClassicSerialDataSource(KInstance *inst,QObject *parent);
    ~KWClassicSerialDataSource();

    virtual void save( QDomDocument &doc,QDomElement&);
    virtual void load( QDomElement& elem );
    virtual class QString getValue( const class QString &name, int record = -1 ) const;
    virtual int getNumRecords() const {
        return (int)db.count();
    }
    virtual  bool showConfigDialog(QWidget *,int);
    virtual void refresh(bool){};

    protected:
    friend class KWClassicMailMergeEditor;
    friend class KWClassicMailMergeEditorList;

    void setValue( const QString &name, const QString &value, int record = -1 );
    void appendRecord();
    void addEntry( const QString &name );
    void removeEntry( const QString &name );
    void removeRecord( int i );
    Db db;
};

/******************************************************************
 *
 * Class: KWClassicMailMergeEditorListItem
 *
 ******************************************************************/

class KWClassicMailMergeEditorListItem : public Q3ListViewItem
{
public:
    KWClassicMailMergeEditorListItem( Q3ListView *parent );
    KWClassicMailMergeEditorListItem( Q3ListView *parent, Q3ListViewItem *after );
    virtual ~KWClassicMailMergeEditorListItem();

    virtual void setText( int i, const QString &text );
    virtual QString text( int i ) const;
    void setup();
    void update();

protected:
    QLineEdit *editWidget;

};

/******************************************************************
 *
 * Class: KWClassicMailMergeEditorList
 *
 ******************************************************************/

class KWClassicMailMergeEditorList : public Q3ListView
{
    Q_OBJECT

public:
    KWClassicMailMergeEditorList( QWidget *parent, KWClassicSerialDataSource *db_ );
    virtual ~KWClassicMailMergeEditorList();

    void invalidateCurrentRecord();
    void updateItems();
    void displayRecord( int i );

    void setSorting( int, bool increasing = true ) {
        Q3ListView::setSorting( -1, increasing );
    }

protected slots:
    void columnSizeChange( int c, int os, int ns );
    void sectionClicked( int c );

protected:
    KWClassicSerialDataSource *db;
    int currentRecord;

};

/******************************************************************
 *
 * Class: KWClassicMailMergeEditor
 *
 ******************************************************************/

class KWClassicMailMergeEditor : public KDialogBase
{
    Q_OBJECT

public:
    KWClassicMailMergeEditor( QWidget *parent, KWClassicSerialDataSource *db_ );

protected:
    void resizeEvent( QResizeEvent *e );
    void updateButton();

    QSpinBox *records;
    KWClassicMailMergeEditorList *dbList;
    QWidget *back;
    KWClassicSerialDataSource *db;

    QToolButton *first;
    QToolButton *back_;
    QToolButton *forward;
    QToolButton *finish;
    QToolButton *newRecord;
    QToolButton *newEntry;
    QToolButton *deleteRecord;
    QToolButton *deleteEntry;

protected slots:
    void changeRecord( int i );
    void addEntry();
    void addRecord();
    void removeEntry();
    void removeRecord();
    void firstRecord();
    void prevRecord();
    void nextRecord();
    void lastRecord();
};

#endif

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

#ifndef serialletter_h
#define serialletter_h

#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdialog.h>
#include <qvbox.h>
#include <qlistview.h>
#include <qspinbox.h>

#include <qtextstream.h>

#include <koStream.h>

class QListBox;
class QPushButton;
class QLineEdit;
class QSpinBox;
class KWDocument;

/******************************************************************
 *
 * Class: KWSerialLetterDataBase
 *
 ******************************************************************/

class KWSerialLetterDataBase
{
public:
    KWSerialLetterDataBase( KWDocument *doc_ );

    QString getValue( const QString &name, int record = -1 ) const;
    void setValue( const QString &name, const QString &value, int record = -1 );
    void appendRecord();
    void addEntry( const QString &name );
    void removeEntry( const QString &name );
    void removeRecord( int i );

    const QMap< QString, QString > &getRecordEntries() const {
        return sampleRecord;
    }
    int getNumRecords() const {
        return (int)db.count();
    }

    void save( QTextStream&out );
    void load( KOMLParser&, QValueList<KOMLAttrib>& );

protected:
    typedef QMap< QString, QString > DbRecord;
    typedef QValueList< DbRecord > Db;

    KWDocument *doc;
    Db db;
    DbRecord sampleRecord;

};

/******************************************************************
 *
 * Class: KWSerialLetterVariableInsertDia
 *
 ******************************************************************/

class KWSerialLetterVariableInsertDia : public QDialog
{
    Q_OBJECT

public:
    KWSerialLetterVariableInsertDia( QWidget *parent, KWSerialLetterDataBase *db );
    QString getName() const;

protected:
    void resizeEvent( QResizeEvent *e );

    QListBox *names;
    QVBox *back;

};

/******************************************************************
 *
 * Class: KWSerialLetterEditorListItem
 *
 ******************************************************************/

class KWSerialLetterEditorListItem : public QListViewItem
{
public:
    KWSerialLetterEditorListItem( QListView *parent );
    KWSerialLetterEditorListItem( QListView *parent, QListViewItem *after );
    virtual ~KWSerialLetterEditorListItem();

    virtual void setText( int i, const QString &text );
    virtual QString text( int i ) const;

    void setup();
    void update();

protected:
    QLineEdit *editWidget;

};

/******************************************************************
 *
 * Class: KWSerialLetterEditorList
 *
 ******************************************************************/

class KWSerialLetterEditorList : public QListView
{
    Q_OBJECT

public:
    KWSerialLetterEditorList( QWidget *parent, KWSerialLetterDataBase *db_ );
    virtual ~KWSerialLetterEditorList();

    void updateItems();
    void displayRecord( int i );

    void setSorting( int, bool increasing = TRUE ) {
        QListView::setSorting( -1, increasing );
    }

protected slots:
    void columnSizeChange( int c, int os, int ns );
    void sectionClicked( int c );

protected:
    KWSerialLetterDataBase *db;
    int currentRecord;

};

/******************************************************************
 *
 * Class: KWSerialLetterEditor
 *
 ******************************************************************/

class KWSerialLetterEditor : public QDialog
{
    Q_OBJECT

public:
    KWSerialLetterEditor( QWidget *parent, KWSerialLetterDataBase *db_ );

protected:
    void resizeEvent( QResizeEvent *e );

    QSpinBox *records;
    KWSerialLetterEditorList *dbList;
    QVBox *back;
    KWSerialLetterDataBase *db;

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
    void firstRecord() { records->setValue(1); }
    void prevRecord() { records->setValue(records->value()-1); }
    void nextRecord() { records->setValue(records->value()+1); }
    void lastRecord() { records->setValue(records->maxValue()); }
};

#endif

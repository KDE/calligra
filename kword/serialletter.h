/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Serial Letters                                         */
/******************************************************************/

#ifndef serialletter_h
#define serialletter_h

#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdialog.h>
#include <qvbox.h>
#include <qlistview.h>

class QListBox;
class QPushButton;
class QLineEdit;
class KWordDocument;

/******************************************************************
 *
 * Class: KWSerialLetterDataBase
 *
 ******************************************************************/

class KWSerialLetterDataBase
{
public:
    KWSerialLetterDataBase( KWordDocument *doc_ );

    QString getValue( const QString &name, int record = -1 ) const;
    void setValue( const QString &name, const QString &value, int record = -1 );
    void appendRecord();
    void addEntry( const QString &name );
    void removeEntry( const QString &name );

    const QMap< QString, QString > &getRecordEntries() const {
	return sampleRecord;
    }
    int getNumRecords() const {
	return (int)db.count();
    }
    
protected:
    typedef QMap< QString, QString > DbRecord;
    typedef QValueList< DbRecord > Db;
    
    KWordDocument *doc;
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
    KWSerialLetterEditorList( QWidget *parent );

    void updateItems();

protected slots:
    void columnSizeChange( int c, int os, int ns );
    void sectionClicked( int c );

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
    KWSerialLetterEditor( QWidget *parent, KWSerialLetterDataBase *db );

protected:
    void resizeEvent( QResizeEvent *e );

    QPushButton *first, *last, *prev, *next, *newRecord,
	*removeRecord, *removeEntry, *addEntry, *addRecord;
    QLineEdit *record;
    KWSerialLetterEditorList *list;
    QVBox *back;

};

#endif

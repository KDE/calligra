/* This file is part of the KDE project
   Copyright (C) 2002 Norbert Andres, nandres@web.de

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

#ifndef __kspread_changes__
#define __kspread_changes__

#include <qcstring.h>
#include <qdatetime.h>
#include <qmap.h>
#include <qobject.h>
#include <qpoint.h>
#include <qptrlist.h>
#include <qstring.h>

class FilterMain;

class KSpreadAcceptDlg;
class KSpreadCell;
class KSpreadFilterDlg;
class KSpreadMap;
class KSpreadSheet;

class QDomDocument;
class QDomElement;

class FilterSettings
{
 public:
  FilterSettings();

  bool loadXml( QDomElement const & settings );
  void saveXml( QDomDocument & doc, QDomElement & parent ) const;

  void setShowChanges( bool b );
  void setShowAccepted( bool b );
  void setShowRejected( bool b );
 
  bool dateSet() const         { return m_dateSet; }
  bool authorSet() const       { return m_authorSet; }
  bool commentSet() const      { return m_commentSet; }
  bool rangeSet() const        { return m_rangeSet; }
                               
  bool showChanges() const     { return m_showChanges; }
  bool showRejected() const    { return m_showRejected; }
  bool showAccepted() const    { return m_showAccepted; }
                               
  int  dateUsage() const       { return m_dateUsage; }
                               
  QString author() const       { return m_author; }
  QString comment() const      { return m_comment; }
  QString range() const        { return m_range; }

  QDateTime firstTime() const  { return m_firstTime; }
  QDateTime secondTime() const { return m_secondTime; }
  
 private:
  friend class FilterMain;

  bool      m_dateSet;
  int       m_dateUsage;
  QDateTime m_firstTime;
  QDateTime m_secondTime;

  bool      m_authorSet;
  QString   m_author;
  
  bool      m_commentSet;
  QString   m_comment;

  bool      m_rangeSet;
  QString   m_range;

  bool      m_showChanges;
  bool      m_showAccepted;
  bool      m_showRejected;
};

class KSpreadChanges : public QObject
{
 public:
  KSpreadChanges( KSpreadMap * map );
  ~KSpreadChanges();

  void setProtected( QCString const & hash );
  bool isProtected() const { return !m_strPassword.isNull(); }
  void password( QCString & passwd ) const { passwd = m_strPassword; }
  bool checkPassword( QCString const & passwd ) const { return ( passwd == m_strPassword ); }

  void addChange( KSpreadSheet * table, KSpreadCell * cell, QPoint const & point,
                  QString const & oldFormat, QString const & oldValue, bool hasDepandancy = true );
  void fillDependancyList();

  void saveXml( QDomDocument & doc, QDomElement & map );
  bool loadXml( QDomElement const & changes );

 private:
  class ChangeRecord;
  class RecordMap : public QMap<int, ChangeRecord *> {};

  class AuthorInfo
  {
   public:
    AuthorInfo( int id, QString const & name )
      : m_id( id ), m_name( name ) {}

    int  id() const { return m_id; };
    QString name() const { return m_name; }

   private:
    int       m_id;
    QString   m_name;
  };

  class Change
  {
   public:
    Change() : timestamp( QDateTime::currentDateTime() ), comment( 0 ) {}
    Change( int _authorID, QDateTime const & _timestamp, QString const & _comment ) 
      : authorID( _authorID ), timestamp( _timestamp ), comment( new QString( _comment ) ) {}

    virtual ~Change();

    virtual bool loadXml( QDomElement const & change, KSpreadSheet const * const table,
                          QPoint const & cellRef ) = 0;
    virtual void saveXml( QDomDocument & doc, QDomElement & change ) const = 0;


// protected:
    int        authorID;
    QDateTime  timestamp;
    QString *  comment;
  };

  class CellChange : public Change
  {
   public:    
    CellChange() : Change(), cell( 0 ) {}
    ~CellChange();


    bool loadXml( QDomElement const & change, KSpreadSheet const * const table,
                  QPoint const & cellRef );

    void saveXml( QDomDocument & doc, QDomElement & change ) const;

    QString       formatString;
    QString       oldValue;
    KSpreadCell * cell;
  };

  class ChangeRecord
  {
   public:
    typedef enum E1 { ACCEPTED, FILTERED, PENDING, REJECTED } State;
    typedef enum E2 { CELL, INSERTCOLUMN, INSERTROW, INSERTTABLE, 
                      DELETECOLUMN, DELETEROW, DELETETABLE, MOVE } ChangeType;


    ChangeRecord();
    ChangeRecord( int id, State state, ChangeType type, KSpreadSheet * table, 
                  QPoint const & cellRef, Change * change );
    ~ChangeRecord();

    bool loadXml( QDomElement & changes, KSpreadMap * map, RecordMap & records );
    void saveXml( QDomDocument & doc, QDomElement & parent ) const;

    bool isDependant( KSpreadSheet const * const table, QPoint const & cell ) const;
    void addDependant( ChangeRecord * record, QPoint const & cellRef );

    void  setState( State state ) { m_state = state; }
    State state() const { return m_state; }

    int id() const { return m_id; }
    KSpreadSheet const * const table() const { return m_table; }

    int dependancies() const { return m_dependancies; }
    void increaseDependancyCounter() { ++m_dependancies; }

   private:
    friend class KSpreadAcceptDlg;
    friend class KSpreadCommentDlg;
    friend class KSpreadFilterDlg;
    
    int            m_id;
    State          m_state;
    ChangeType     m_type;
    KSpreadSheet * m_table;
    QPoint         m_cell; // Rows: (0, row), Columns: (col, 0), Cells (>0, >0)
    Change *       m_change;
    int            m_dependancies;

    QPtrList<ChangeRecord> m_dependants;
  };

  friend class KSpreadAcceptDlg;
  friend class KSpreadCommentDlg;
  friend class KSpreadFilterDlg;  

  QPtrList<ChangeRecord> m_dependancyList;
  QPtrList<AuthorInfo>   m_authors;
  RecordMap              m_changeRecords;
  uint                   m_counter;
  QString                m_name;
  QCString               m_strPassword;
  KSpreadMap *           m_map;
  FilterSettings         m_filterSettings;
  bool                   m_locked;

  int  addAuthor();
  bool loadAuthors( QDomElement const & authors );
  bool loadChanges( QDomElement const & changes );
  void saveAuthors( QDomDocument & doc, QDomElement & changes );
  void saveChanges( QDomDocument & doc, QDomElement & changes );
  QString getAuthor( int id );
};

#endif


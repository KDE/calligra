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
#include <qobject.h>
#include <qpoint.h>
#include <qptrlist.h>
#include <qstring.h>

class KSpreadCell;
class KSpreadMap;
class KSpreadSheet;

class QDomDocument;
class QDomElement;

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
                  QString const & oldFormat, QString const & oldValue );

  void saveXml( QDomDocument & doc, QDomElement & map );
  bool loadXml( QDomElement const & changes );

 private:
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
    typedef enum E1 { ACCEPTED, PENDING, REJECTED } State;
    typedef enum E2 { CELL, INSERTCOLUMN, INSERTROW, INSERTTABLE, 
                      DELETECOLUMN, DELETEROW, DELETETABLE } ChangeType;

    ChangeRecord();
    ChangeRecord( int id, State state, ChangeType type, KSpreadSheet const * table, 
                  QPoint const & cellRef, Change * change );
    ~ChangeRecord();

    bool loadXml( QDomElement & changes );
    void saveXml( QDomDocument & doc, QDomElement & parent ) const;

    bool isDependant( KSpreadSheet const * const table, QPoint const & cell ) const;
    void addDependant( ChangeRecord * record, QPoint const & cellRef );

    int id() const { return m_id; }
    KSpreadSheet const * const table() const { return m_table; }

   private:
    
    int            m_id;
    State          m_state;
    ChangeType     m_type;
    KSpreadSheet * m_table;
    QPoint         m_cell; // Rows: (0, row), Columns: (col, 0), Cells (>0, >0)
    Change *       m_change;

    QPtrList<ChangeRecord> m_dependants;
  };

  QPtrList<ChangeRecord> m_dependancyList;
  QPtrList<ChangeRecord> m_changeRecords;
  QPtrList<AuthorInfo>   m_authors;
  uint                   m_counter;
  QString                m_name;
  QCString               m_strPassword;
  KSpreadMap *           m_map;

  int  addAuthor();
  bool loadAuthors( QDomElement const & authors );
  bool loadChanges( QDomElement const & changes );
  void saveAuthors( QDomDocument & doc, QDomElement & changes );
  void saveChanges( QDomDocument & doc, QDomElement & changes );
};

#endif


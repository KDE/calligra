/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#ifndef __ko_query_types_h__
#define __ko_query_types_h__

#include <qstring.h>
#include <qstringlist.h>
#include <qpixmap.h>
#include <qvaluelist.h>

// Some forward declarations
namespace KOffice
{
  class Document;
  typedef Document *Document_ptr;
};

namespace CORBA
{
  class Object;
  typedef Object* Object_ptr;
};

/**
 *  Represents an available component.
 */
class KoComponentEntry
{

public:

  KoComponentEntry() { reference = 0; }
  KoComponentEntry( const KoComponentEntry& _entry );

  /**
   * Releases the @ref #reference.
   */
  virtual ~KoComponentEntry();

  const KoComponentEntry& operator=( const KoComponentEntry& _entry );

  QString comment;
  QString name;
  QString exec;
  QString activationMode;
  QStringList repoID;
  QPixmap miniIcon;
  QPixmap icon;

  /**
   * Reference to the components factory. It is a virtual reference.
   * That means that the component is started when the
   * first function call is invoked on this reference.
   */
  CORBA::Object_ptr reference;

  bool isEmpty() const { return name.isEmpty(); }
};

/**
 *  Represents an available koffice component
 *  that supports the document interface.
 */
class KoDocumentEntry : public KoComponentEntry
{

public:

  KoDocumentEntry() { }
  KoDocumentEntry( const KoComponentEntry& _entry );
  KoDocumentEntry( const KoDocumentEntry& _entry );
  ~KoDocumentEntry() { }

  const KoDocumentEntry& operator=( const KoDocumentEntry& _entry );

  /**
   *  Mimetypes which this document can handle.
   */
  QStringList mimeTypes;

  /**
   *  @return TRUE if the document can handle the requested mimetype.
   */
  bool supportsMimeType( const char* _mimetype ) const
  { return ( mimeTypes.find( _mimetype ) != mimeTypes.end() ); }

  /**
   *  Uses the factory of the component (@ref #reference) to create
   *  a document. If that is not possible, the user gets an error dialog
   *  and 0 is returned.
   */
  KOffice::Document_ptr createDoc();

  /**
   *  This function will query KDED to find all available components.
   *
   *  @param _constr is a constraint expression as used by KDEDs trader interface.
   *                 You can use it to set additional restrictions on the available
   *                 components.
   *  @param _count  is the amount of query results we are interested in.
   */
  static QValueList<KoDocumentEntry> query( const char* _constr = "", int _count = 1 );

  /**
   *  This is only a convenience function.
   *
   *  @return a document entry for the KOffice component that supports
   *          the requested mimetype and fits the user best.
   */
  static KoDocumentEntry queryByMimeType( const char *mimetype );
};

/**
 *  Represents an available filter.
 */
class KoFilterEntry : public KoComponentEntry
{

public:

  KoFilterEntry() { }
  KoFilterEntry( const KoComponentEntry& _entry );
  KoFilterEntry( const KoFilterEntry& _entry );
  ~KoFilterEntry() { }

  /**
   *  The imported mimetype.
   */
  QString import;

  /**
   *  Comment regarding the imported data format.
   */
  QString importDescription;

  /**
   *    The exported mimetype.
   */
  QString export;

  /**
   *  Comment regarding the exported data format.
   */
  QString exportDescription;

  /**
   *  @return TRUE if the filter can imports the requested mimetype.
   */
  bool imports( const char* _mimetype ) const
  { return ( import.find( _mimetype ) != -1 ); }
  
  /**
   *  @return TRUE if the filter can exports the requested mimetype.
   */
  bool exports( const char *_m ) const
  { return ( export.find( _m ) != -1 ); }

  /**
   *  This function will query KDED to find all available filters.
   *
   *  @param _constr is a constraint expression as used by KDEDs trader interface.
   *                 You can use it to set additional restrictions on the available
   *                 components.
   */
  static QValueList<KoFilterEntry> query( const char* _constr = "", int _count = 100 );
};

#endif

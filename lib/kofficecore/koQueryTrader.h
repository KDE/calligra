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

#ifndef __ko_query_trader_h__
#define __ko_query_trader_h__

#include <qobject.h>
#include <qstring.h>
#include <kservice.h>
#include <qvaluelist.h>

class QStringList;
class KoDocument;
class KoFilter;
class KoFilterDialog;

/**
 *  Represents an available koffice component
 *  that supports the document interface.
 */
class KoDocumentEntry
{

public:

  KoDocumentEntry() { m_service = 0L; } // for QValueList
  KoDocumentEntry( KService::Ptr _service );
  ~KoDocumentEntry() { }

  KService::Ptr service() const { return m_service; }

  bool isEmpty() const { return m_service == 0L; }

  QString name() const { return m_service->name(); }

  /**
   *  Mimetypes (and other service types) which this document can handle.
   */
  QStringList mimeTypes() const { return m_service->serviceTypes(); }

  /**
   *  @return TRUE if the document can handle the requested mimetype.
   */
  bool supportsMimeType( const QString & _mimetype ) const
  { return mimeTypes().contains( _mimetype ); }

  /**
   *  Uses the factory of the component  to create
   *  a document. If that is not possible 0 is returned.
   */
  KoDocument* createDoc( KoDocument* parent = 0, const char* name = 0 );

  /**
   *  This function will query KDED to find all available components.
   *
   *  @param _constr is a constraint expression as used by KDEDs trader interface.
   *                 You can use it to set additional restrictions on the available
   *                 components.
   */
  static QValueList<KoDocumentEntry> query( const QString &  _constr = QString::null );

  /**
   *  This is only a convenience function.
   *
   *  @return a document entry for the KOffice component that supports
   *          the requested mimetype and fits the user best.
   */
  static KoDocumentEntry queryByMimeType( const QString & mimetype );

private:
  KService::Ptr m_service;
};

/**
 *  Represents an available filter.
 */
class KoFilterEntry
{

public:

  KoFilterEntry() { m_service = 0L; } // for QValueList
  KoFilterEntry( KService::Ptr service );
  ~KoFilterEntry() { }

  KoFilter* createFilter( QObject* parent = 0, const char* name = 0);

  /**
   *  The imported mimetype.
   */
  QStringList import;

  /**
   *    The exported mimetype.
   */
  QStringList export_;

  /**
   *  Which one of the filter methods is implemented?
   */
  QString implemented;

  /**
   *  @return TRUE if the filter can imports the requested mimetype.
   */
  bool imports( const QString & _mimetype ) const
  { return ( import.contains( _mimetype ) ); }

  /**
   *  @return TRUE if the filter can exports the requested mimetype.
   */
  bool exports( const QString & _m ) const
  { return ( export_.contains( _m ) ); }

  /**
   *  This function will query KDED to find all available filters.
   *
   *  @param _constr is a constraint expression as used by KDEDs trader interface.
   *                 You can use it to set additional restrictions on the available
   *                 components.
   */
  static QValueList<KoFilterEntry> query( const QString & _constr = QString::null );

  /**
   * @internal for debugging purposes
   */
  KService::Ptr service() const { return m_service; }

private:
  KService::Ptr m_service;
};

/**
 *  Represents an available filter dialog.
 */
class KoFilterDialogEntry
{

public:

  KoFilterDialogEntry() { m_service = 0L; }
  KoFilterDialogEntry( KService::Ptr service );
  ~KoFilterDialogEntry() { }

  KoFilterDialog* createFilterDialog( QObject* parent = 0, const char* name = 0);

  /**
   *  The imported mimetype of the matching filter.
   */
  QStringList import;

  /**
   *    The exported mimetype of the matching filter.
   */
  QStringList export_;

  /**
   *  This function will query KDED to find all available filter dialogs.
   *
   *  @param _constr is a constraint expression as used by KDEDs trader interface.
   *                 You can use it to set additional restrictions on the available
   *                 components.
   */
  static QValueList<KoFilterDialogEntry> query( const QString & _constr = QString::null );
 
private:
  KService::Ptr m_service;
};

/**
 * Torben says: DONT USE. Use KoDataToolInfo instead!
class KoToolEntry
{
public:
  KoToolEntry() { };
  KoToolEntry( const KoComponentEntry& _entry );
  KoToolEntry( const KoToolEntry& entry );

  QStringList mimeTypes;
  QStringList commands;
  QStringList commandsI18N;

  bool supports( const QString &_mime_type ) const { return ( mimeTypes.find( _mime_type ) != mimeTypes.end() ); }

  static QValueList<KoToolEntry> query( const QString &_mime_type );

private:
  KService::Ptr m_service;
};
*/

#endif

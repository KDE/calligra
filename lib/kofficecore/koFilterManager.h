/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
                 2000, 2001 Werner Trobin <trobin@kde.org>

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

#ifndef __koffice_filter_manager_h__
#define __koffice_filter_manager_h__

#include <qobject.h>
#include <qmap.h>
#include <koFilterChain.h>

class KoDocument;

/**
 *  This class manages all filters for a KOffice application. Normally
 *  you won't have to use it, since KoMainWindow takes care of loading
 *  and saving documents.
 *
 *  @short The class managing all the filters.
 *  @ref KoFilter
 *
 *  @author Kalle Dalheimer <kalle@kde.org>
 *  @author Torben Weis <weis@kde.org>
 *  @author Werner Trobin <trobin@kde.org>
 *  @version $Id$
 */
class KoFilterManager : public QObject
{
    Q_OBJECT
public:
    enum Direction { Import = 1,  Export = 2 };

    // Filter manager for a document
    KoFilterManager( KoDocument* document );
    // Filter manager for a filter which wants to embed something. The url
    // it passes is the file to convert, obviously. You can't use the
    // import method, use exp0rt to convert the file to the destination
    // mimetype you prefer.
    KoFilterManager( const QString& url );

    virtual ~KoFilterManager();

    // Imports the passed URL and returns the resultant filename
    // (most likely some file in /tmp).
    // The status vaiable signals the success/error of the conversion
    // If the QString which is returned isEmpty() and the status is OK,
    // then we imported the file directly into the document.
    QString import( const QString& url, KoFilter::ConversionStatus& status );

    // Exports the given file/document *to* the specified URL/mimetype.
    // Oh, well, export is a C++ keyword ;)
    KoFilter::ConversionStatus exp0rt( const QString& url, QCString& mimeType );


    // ### Static API ###
    // Suitable for passing to KFileDialog::setMimeFilter. The default mime
    // gets set by the "users" of this method, as we don't have enough
    // information here.
    static QStringList mimeFilter( const QCString& mimetype, Direction direction );

    // The same method as above for KoShell.
    // We don't need the mimetype, as we will simply use all available
    // KOffice mimetypes, the Direction enum is omitted, as we only
    // call this for importing. When saving we already know the KOffice
    // part we're using.
    static QStringList mimeFilter();

    // Is that filter available at all?
    // Note: Slow, but cached (static)
    static bool filterAvailable( KoFilterEntry::Ptr entry );

signals:
    void sigProgress( int );

private:
    // ### API for KoFilterChains ###
    // The friend methods are private in KoFilterChain and
    // just forward calls to the shorter methods here.
    friend QString KoFilterChain::filterManagerImportFile() const;
    QString importFile() const { return m_importUrl; }
    friend QString KoFilterChain::filterManagerExportFile() const;
    QString exportFile() const { return m_exportUrl; }
    friend KoDocument* KoFilterChain::filterManagerKoDocument() const;
    KoDocument* document() const { return m_document; }
    friend int KoFilterChain::filterManagerDirection() const;
    int direction() const { return static_cast<int>( m_direction ); }

    // Private API
    KoFilterManager( const KoFilterManager& rhs );
    KoFilterManager &operator=( const KoFilterManager& rhs );

    void importErrorHelper( const QString& mimeType );

    static const int s_area = 30500;

    KoDocument* m_document;
    QString m_importUrl, m_exportUrl;
    KOffice::Graph m_graph;
    Direction m_direction;

    // A static cache for the availability checks of filters
    static QMap<QString, bool> m_filterAvailable;

    // unused right now
    class Private;
    Private *d;
};

#endif  // __koffice_filter_manager_h__

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


#include <koFilterManager.h>

#include <qfile.h>
#include <qptrlist.h>
#include <qapplication.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <koDocument.h>
#include <klibloader.h>
#include <kmimetype.h>
#include <kdebug.h>

#include <queue>

#include <unistd.h>


// static cache for filter availability
QMap<QString, bool> KoFilterManager::m_filterAvailable;

const int KoFilterManager::s_area = 30500;

KoFilterManager::KoFilterManager( KoDocument* document ) :
    m_document( document ), m_parentChain( 0 ), m_graph( "" ), d( 0 )
{
    if ( document )
        QObject::connect( this, SIGNAL( sigProgress( int ) ),
                          document, SIGNAL( sigProgress( int ) ) );
}


KoFilterManager::KoFilterManager( const QString& url, const QCString& mimetypeHint,
                                  KoFilterChain* const parentChain ) :
    m_document( 0 ), m_parentChain( parentChain ), m_importUrl( url ), m_importUrlMimetypeHint( mimetypeHint ),
    m_graph( "" ), d( 0 )
{
}

KoFilterManager::~KoFilterManager()
{
}

QString KoFilterManager::import( const QString& url, KoFilter::ConversionStatus& status )
{
    // Find the mime type for the file to be imported.
    KURL u;
    u.setPath( url );
    KMimeType::Ptr t = KMimeType::findByURL( u, 0, true );
    if ( t->name() == KMimeType::defaultMimeType() ) {
        kdError(s_area) << "No mimetype found for " << url << endl;
        status = KoFilter::BadMimeType;
        return QString::null;
    }

    m_graph.setSourceMimeType( t->name().latin1() );  // .latin1() is okay here (Werner)
    if ( !m_graph.isValid() ) {
        kdError(s_area) << "Couldn't create a valid graph for this source mimetype: "
                        << t->name() << endl;
        importErrorHelper( t->name() );
        status = KoFilter::BadConversionGraph;
        return QString::null;
    }

    KoFilterChain::Ptr chain( 0 );
    // Are we owned by a KoDocument?
    if ( m_document ) {
        QCString mimeType( m_document->nativeFormatMimeType() );
        chain = m_graph.chain( this, mimeType );
    }
    else {
        kdError(s_area) << "You aren't supposed to use import() from a filter!" << endl;
        status = KoFilter::UsageError;
        return QString::null;
    }

    if ( !chain ) {
        kdError(s_area) << "Couldn't create a valid filter chain!" << endl;
        importErrorHelper( t->name() );
        status = KoFilter::BadConversionGraph;
        return QString::null;
    }

    // Okay, let's invoke the filters one after the other
    m_direction = Import; // vital information!
    m_importUrl = url;  // We want to load that file
    m_exportUrl = QString::null;  // This is null for sure, as embedded stuff isn't
                                  // allowed to use that method
    status = chain->invokeChain();

    m_importUrl = QString::null;  // Reset the import URL

    if ( status == KoFilter::OK )
        return chain->chainOutput();
    return QString::null;
}

KoFilter::ConversionStatus KoFilterManager::exp0rt( const QString& url, QCString& mimeType )
{
    // The import url should already be set correctly (null if we have a KoDocument
    // file manager and to the correct URL if we have an embedded manager)
    m_direction = Export; // vital information!
    m_exportUrl = url;

    if ( m_document )
        m_graph.setSourceMimeType( m_document->nativeFormatMimeType() );
    else if ( !m_importUrlMimetypeHint.isEmpty() ) {
        kdDebug(s_area) << "Using the mimetype hint: '" << m_importUrlMimetypeHint << "'" << endl;
        m_graph.setSourceMimeType( m_importUrlMimetypeHint );
    }
    else {
        KURL u;
        u.setPath( m_importUrl );
        KMimeType::Ptr t = KMimeType::findByURL( u, 0, true );
        if ( t->name() == KMimeType::defaultMimeType() ) {
            kdError(s_area) << "No mimetype found for " << m_importUrl << endl;
            return KoFilter::BadMimeType;
        }
        m_graph.setSourceMimeType( t->name().latin1() );
    }

    if ( !m_graph.isValid() ) {
        kdError(s_area) << "Couldn't create a valid graph for this source mimetype." << endl;
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0L, i18n("Could not export file"), i18n("Missing Export Filter") );
        return KoFilter::BadConversionGraph;
    }

    KoFilterChain::Ptr chain = m_graph.chain( this, mimeType );

    if ( !chain ) {
        kdError(s_area) << "Couldn't create a valid filter chain!" << endl;
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0L, i18n("Could not export file"), i18n("Missing Export Filter") );
        return KoFilter::BadConversionGraph;
    }

    return chain->invokeChain();
}

namespace  // in order not to mess with the global namespace ;)
{
    // This class is needed only for the static mimeFilter method
    class Vertex
    {
    public:
        Vertex( const QCString& mimeType ) : m_color( White ), m_mimeType( mimeType ) {}

        enum Color { White, Gray, Black };
        Color color() const { return m_color; }
        void setColor( Color color ) { m_color = color; }

        QCString mimeType() const { return m_mimeType; }

        void addEdge( Vertex* vertex ) { if ( vertex ) m_edges.append( vertex ); }
        QPtrList<Vertex> edges() const { return m_edges; }

    private:
        Color m_color;
        QCString m_mimeType;
        QPtrList<Vertex> m_edges;
    };

    // Some helper methods for the static stuff
    // This method builds up the graph in the passed ascii dict
    void buildGraph( QAsciiDict<Vertex>& vertices, KoFilterManager::Direction direction )
    {
        vertices.setAutoDelete( true );

        // partly copied from build graph, but I don't see any other
        // way without crude hacks, as we have to obey the direction here
        QValueList<KoDocumentEntry> parts( KoDocumentEntry::query() );
        QValueList<KoDocumentEntry>::ConstIterator partIt( parts.begin() );
        QValueList<KoDocumentEntry>::ConstIterator partEnd( parts.end() );

        while ( partIt != partEnd ) {
            QCString key( ( *partIt ).service()->property( "X-KDE-NativeMimeType" ).toString().latin1() );
            if ( !key.isEmpty() )
                vertices.insert( key, new Vertex( key ) );
            ++partIt;
        }

        QValueList<KoFilterEntry::Ptr> filters = KoFilterEntry::query(); // no constraint here - we want *all* :)
        QValueList<KoFilterEntry::Ptr>::ConstIterator it = filters.begin();
        QValueList<KoFilterEntry::Ptr>::ConstIterator end = filters.end();

        for ( ; it != end; ++it ) {
            // First add the "starting points" to the dict
            QStringList::ConstIterator importIt = ( *it )->import.begin();
            QStringList::ConstIterator importEnd = ( *it )->import.end();
            for ( ; importIt != importEnd; ++importIt ) {
                QCString key = ( *importIt ).latin1();  // latin1 is okay here (werner)
                // already there?
                if ( !vertices[ key ] )
                    vertices.insert( key, new Vertex( key ) );
            }

            // Are we allowed to use this filter at all?
            if ( KoFilterManager::filterAvailable( *it ) ) {
                QStringList::ConstIterator exportIt = ( *it )->export_.begin();
                QStringList::ConstIterator exportEnd = ( *it )->export_.end();

                for ( ; exportIt != exportEnd; ++exportIt ) {
                    // First make sure the export vertex is in place
                    QCString key = ( *exportIt ).latin1();  // latin1 is okay here
                    Vertex* exp = vertices[ key ];
                    if ( !exp ) {
                        exp = new Vertex( key );
                        vertices.insert( key, exp );
                    }
                    // Then create the appropriate edges depending on the
                    // direction (import/export)
                    // This is the chunk of code which actually differs from the
                    // graph stuff (apart from the different vertex class)
                    importIt = ( *it )->import.begin();
                    if ( direction == KoFilterManager::Import ) {
                        for ( ; importIt != importEnd; ++importIt )
                            exp->addEdge( vertices[ ( *importIt ).latin1() ] );
                    } else {
                        for ( ; importIt != importEnd; ++importIt )
                            vertices[ ( *importIt ).latin1() ]->addEdge( exp );
                    }
                }
            }
            else
                kdDebug( 30500 ) << "Filter: " << ( *it )->service()->name() << " not available." << endl;
        }
    }

    // This method runs a BFS on the graph to determine the connected
    // nodes. Make sure that the graph is "cleared" (the colors of the
    // nodes are all white)
    QStringList connected( const QAsciiDict<Vertex>& vertices, const QCString& mimetype )
    {
        if ( mimetype.isEmpty() )
            return QStringList();
        Vertex *v = vertices[ mimetype ];
        if ( !v )
            return QStringList();

        v->setColor( Vertex::Gray );
        std::queue<Vertex*> queue;
        queue.push( v );
        QStringList connected;

        while ( !queue.empty() ) {
            v = queue.front();
            queue.pop();
            QPtrList<Vertex> edges = v->edges();
            QPtrListIterator<Vertex> it( edges );
            for ( ; it.current(); ++it ) {
                if ( it.current()->color() == Vertex::White ) {
                    it.current()->setColor( Vertex::Gray );
                    queue.push( it.current() );
                }
            }
            v->setColor( Vertex::Black );
            connected.append( v->mimeType() );
        }
        return connected;
    }
}

// The static method to figure out to which parts of the
// graph this mimetype has a connection to.
QStringList KoFilterManager::mimeFilter( const QCString& mimetype, Direction direction )
{
    QAsciiDict<Vertex> vertices;
    buildGraph( vertices, direction );
    return connected( vertices, mimetype );
}

QStringList KoFilterManager::mimeFilter()
{
    QAsciiDict<Vertex> vertices;
    buildGraph( vertices, KoFilterManager::Import );

    QValueList<KoDocumentEntry> parts( KoDocumentEntry::query() );
    QValueList<KoDocumentEntry>::ConstIterator partIt( parts.begin() );
    QValueList<KoDocumentEntry>::ConstIterator partEnd( parts.end() );

    if ( partIt == partEnd )
        return QStringList();

    // To find *all* reachable mimetypes, we have to resort to
    // a small hat trick, in order to avoid multiple searches:
    // We introduce a fake vertex, which is connected to every
    // single KOffice mimetype. Due to that one BFS is enough :)
    // Now we just need an... ehrm.. unique name for our fake mimetype
    Vertex *v = new Vertex( "supercalifragilistic/x-pialadocious" );
    vertices.insert( "supercalifragilistic/x-pialadocious", v );
    while ( partIt != partEnd ) {
        QCString key( ( *partIt ).service()->property( "X-KDE-NativeMimeType" ).toString().latin1() );
        if ( !key.isEmpty() )
            v->addEdge( vertices[ key ] );
        ++partIt;
    }
    QStringList result = connected( vertices, "supercalifragilistic/x-pialadocious" );

    // Finally we have to get rid of our fake mimetype again
    result.remove( "supercalifragilistic/x-pialadocious" );
    return result;
}

// Here we check whether the filter is available. This stuff is quite slow,
// but I don't see any other convenient (for the user) way out :}
bool KoFilterManager::filterAvailable( KoFilterEntry::Ptr entry )
{
    if ( !entry )
        return false;
    if ( entry->available != "check" )
        return true;

    kdDebug( 30500 ) << "Checking whether " << entry->service()->name() << " is available." << endl;
    // generate some "unique" key
    QString key( entry->service()->name() );
    key += " - ";
    key += entry->service()->library();

    if ( !m_filterAvailable.contains( key ) ) {
        kdDebug( 30500 ) << "Not cached, checking..." << endl;

        KLibrary* library = KLibLoader::self()->library( QFile::encodeName( entry->service()->library() ) );
        if ( !library ) {
            kdWarning( 30500 ) << "Huh?? Couldn't load the lib: "
                               << KLibLoader::self()->lastErrorMessage() << endl;
            m_filterAvailable[ key ] = false;
            return false;
        }

        // This code is "borrowed" from klibloader ;)
        QCString symname;
        symname.sprintf("check_%s", library->name().latin1() );
        void* sym = library->symbol( symname );
        if ( !sym )
        {
            kdWarning( 30500 ) << "The library " << library->name()
                               << " does not offer a check_" << library->name()
                               << " function." << endl;
            m_filterAvailable[ key ] = false;
        }
        else {
            typedef int (*t_func)();
            t_func check = (t_func)sym;
            m_filterAvailable[ key ] = check() == 1;
        }
    }
    return m_filterAvailable[ key ];
}

void KoFilterManager::importErrorHelper( const QString& mimeType )
{
    QString tmp = i18n("Could not import file of type\n%1").arg( mimeType );
    QApplication::restoreOverrideCursor();
    // ###### FIXME: use KLibLoader::lastErrorMessage() here
    KMessageBox::error( 0L, tmp, i18n("Missing Import Filter") );
}

#include <koFilterManager.moc>

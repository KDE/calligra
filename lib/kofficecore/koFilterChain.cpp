/* This file is part of the KOffice libraries
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <koFilterChain.h>
#include <koQueryTrader.h>
#include <kdebug.h>

#include <limits.h> // UINT_MAX

namespace KOffice {

    Edge::Edge( Vertex* vertex, const QString& libName, unsigned int weight ) :
        m_vertex( vertex ), m_libName( libName ), m_weight( weight )
    {
    }

    void Edge::relax( unsigned int vertexWeight, PriorityQueue<Vertex>& queue )
    {
        if ( !m_vertex )
            return;
        m_vertex->setKey( vertexWeight + m_weight );
        queue.keyDecreased( m_vertex ); // maintain the heap property
    }

    void Edge::dump( const QCString& indent ) const
    {
        if ( m_vertex )
            kdDebug() << indent << "Edge -> '" << m_vertex->mimeType() << "' (" << m_weight << ")" << endl;
        else
            kdDebug() << indent << "Edge -> '(null)' (" << m_weight << ")" << endl;
    }


    Vertex::Vertex( const QCString& mimeType ) : m_predecessor( 0 ), m_mimeType( mimeType ),
        m_weight( UINT_MAX ), m_index( -1 )
    {
        m_edges.setAutoDelete( true );  // we take ownership of added edges
    }

    void Vertex::addEdge( const Edge* edge )
    {
        if ( !edge || edge->weight()==0)
            return;
        m_edges.append(edge);
    }

    const Edge* Vertex::findEdge( const Vertex* vertex ) const
    {
        if ( !vertex )
            return 0;
        const Edge* edge = 0;
        QPtrListIterator<Edge> it( m_edges );
        for ( ; it.current(); ++it ) {
            if ( it.current()->vertex() == vertex ) {
                edge = it.current(); // we found an edge...
                ++it;
                break;
            }
        }

        // Okay, we didn't find any matching edge
        if ( !edge )
            return 0;

        // Apparently we found one - is there a lighter one left?
        for ( ; it.current(); ++it ) {
            if ( it.current()->vertex() == vertex &&
                 it.current()->weight() < edge->weight() )
                edge = it.current(); // we found a lighter one.. keep on trying
        }
        return edge;
    }

    void Vertex::relaxVertices( PriorityQueue<Vertex>& queue )
    {
        for ( Edge *e = m_edges.first(); e; e = m_edges.next() )
            e->relax( m_weight, queue );
    }

    void Vertex::dump( const QCString& indent ) const
    {
        kdDebug() << indent << "Vertex: " << m_mimeType << " (" << m_weight << "):" << endl;
        QCString i( indent + "   " );
        QPtrListIterator<Edge> it( m_edges );
        for ( ; it.current(); ++it )
            it.current()->dump( i );
    }


    Graph::Graph( const QCString& from ) : m_vertices( 42 ), m_from( from ), m_graphValid( false )
    {
        m_vertices.setAutoDelete( true );
        buildGraph();
    }

    void Graph::buildGraph()
    {
        QValueList<KoFilterEntry> filters = KoFilterEntry::query(); // no constraint here - we want *all* :)
        QValueList<KoFilterEntry>::ConstIterator it = filters.begin();
        QValueList<KoFilterEntry>::ConstIterator end = filters.end();
        for ( ; it!=end; ++it ) {
            // First add the "starting points" to the dict
            QStringList::ConstIterator importIt = ( *it ).import.begin();
            QStringList::ConstIterator importEnd = ( *it ).import.end();
            for ( ; importIt!=importEnd; ++importIt ) {
                QCString key = ( *importIt ).latin1();  // latin1 is okay here (werner)
                // already there?
                if ( !m_vertices[ key ] )
                    m_vertices.insert( key, new Vertex( key ) );
            }

            // Now add end vertices (if neccessary) and create edges from
            // the start vertex
            QStringList::ConstIterator exportIt = ( *it ).export_.begin();
            QStringList::ConstIterator exportEnd = ( *it ).export_.end();
            for ( ; exportIt!=exportEnd; ++exportIt ) {
                // First make sure the export vertex is in place
                QCString key = ( *exportIt ).latin1();  // latin1 is okay here
                Vertex* exp = m_vertices[ key ];
                if ( !exp ) {
                    exp = new Vertex( key );
                    m_vertices.insert( key, exp );
                }
                // Then create the appropriate edges
                importIt = ( *it ).import.begin();
                KService::Ptr service( ( *it ).service() );
                QString library;
                if ( service )
                    library = service->library();
                int weight = ( *it ).weight;
                for ( ; importIt!=importEnd; ++importIt ) {
                    Vertex* imp = m_vertices[ ( *importIt ).latin1() ]; // has to exist
                    imp->addEdge( new Edge( exp, library, weight ) );
                }
            }
        }
    }

    void Graph::dumpAll() const
    {
        dumpQueue();
        dumpDict();
    }

    void Graph::dumpQueue() const
    {
        m_queue.dump();
    }

    void Graph::dumpDict() const
    {
        kdDebug() << "+++++++++ Graph::dumpDict +++++++++" << endl;
        QAsciiDictIterator<Vertex> it( m_vertices );
        for ( ; it.current(); ++it )
            it.current()->dump( "   " );
        kdDebug() << "+++++++++ Graph::dumpDict (done) +++++++++" << endl;
    }

} // namespace KOffice

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

#include <priorityqueue.h>

#include <limits.h> // UINT_MAX

namespace KOffice {

    Edge::Edge( Vertex* vertex, const QString& libName, unsigned int weight ) :
        m_vertex( vertex ), m_libName( libName ), m_weight( weight )
    {
    }

    void Edge::relax( const Vertex* predecessor, PriorityQueue<Vertex>& queue )
    {
        if ( !m_vertex || !predecessor )
            return;
        if ( m_vertex->setKey( predecessor->key() + m_weight ) ) {
            queue.keyDecreased( m_vertex ); // maintain the heap property
            m_vertex->setPredecessor( predecessor );
        }
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

    bool Vertex::setKey( unsigned int key )
    {
        if ( m_weight > key ) {
            m_weight=key;
            return true;
        }
        return false;
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
            e->relax( this, queue );
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
        shortestPath();
    }

    // Query the trader and create the vertices and edges representing
    // mime types and filters.
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
                for ( ; importIt!=importEnd; ++importIt )
                    m_vertices[ ( *importIt ).latin1() ]->addEdge( new Edge( exp, library, weight ) );
            }
        }
    }

    // As all edges (=filters) are required to have a positive weight
    // we can use Dijkstra's shortest path algorithm from Cormen's
    // "Introduction to Algorithms" (p. 527)
    // Note: I did some adaptions as our data structures are slightly
    // different from the ones used in the book. Further we simply stop
    // the algorithm is we don't find any node with a weight != Infinity
    // (==UINT_MAX), as this means that the remaining nodes in the queue
    // aren't connected anyway.
    void Graph::shortestPath()
    {
        // Is the requested start mime type valid?
        Vertex* from = m_vertices[ m_from ];
        if ( !from )
            return;

        // Inititalize start vertex
        from->setKey( 0 );

        // Fill the priority queue with all the vertices
        PriorityQueue<Vertex> queue( m_vertices );

        while ( !queue.isEmpty() ) {
            Vertex *min = queue.extractMinimum();
            // Did we already relax all connected vertices?
            if ( min->key() == UINT_MAX )
                break;
            min->relaxVertices( queue );
        }
        m_graphValid = true;
    }

    void Graph::dump() const
    {
        kdDebug() << "+++++++++ Graph::dump +++++++++" << endl;
        QAsciiDictIterator<Vertex> it( m_vertices );
        for ( ; it.current(); ++it )
            it.current()->dump( "   " );
        kdDebug() << "+++++++++ Graph::dump (done) +++++++++" << endl;
    }

} // namespace KOffice

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

#ifndef __koffice_filter_chain_h__
#define __koffice_filter_chain_h__

#include <qcstring.h>
#include <qasciidict.h>
#include <qptrlist.h>

class KoFilterManager;

// As we use quite generic classnames...
namespace KOffice
{
    class Vertex;
    template<class T> class PriorityQueue;

    class Edge
    {

    public:
        // creates a new edge to "vertex" with the given service (=filter).
        Edge( Vertex* vertex, const QString& libName, unsigned int weight );
        ~Edge() {}

        unsigned int weight() const { return m_weight; }
        QString libName() const { return m_libName; }
        const Vertex* vertex() const { return m_vertex; }

        // Relaxes the "connected" vertex (i.e. the weight of the
        // connected vertex = "predec.->key()" (parameter) + weight of this edge
        // As this will only be called once we calculate the weight
        // of the edge "on the fly"
        // Note: We have to pass the queue as we have to call keyDecreased :}
        void relax( const Vertex* predecessor, PriorityQueue<Vertex>& queue );

        // debugging
        void dump( const QCString& indent ) const;

    private:
        Edge( const Edge& rhs );
        Edge& operator=( const Edge& rhs );

        Vertex* m_vertex;
        QString m_libName;
        unsigned int m_weight;
    };


    class Vertex
    {

    public:
        Vertex( const QCString& mimeType );
        ~Vertex() {}

        QCString mimeType() const { return m_mimeType; }

        // current "weight" of the vertex - will be "relaxed" when
        // running the shortest path algorithm. Returns true if it
        // really was "relaxed"
        bool setKey( unsigned int key );
        unsigned int key() const { return m_weight; }

        // Position in the heap, needed for a fast keyDecreased operation
        void setIndex( int index ) { m_index=index; }
        int index() const { return m_index; }

        // predecessor on the way from the source to the destination,
        // needed for the shortest path algorithm
        void setPredecessor( const Vertex* predecessor ) { m_predecessor=predecessor; }
        const Vertex* predecessor() const { return m_predecessor; }

        // Adds an outgoing edge to the vertex, transfers ownership
        void addEdge( const Edge* edge );
        // Finds the lightest(!) edge pointing to the given vertex, if any (0 if not found)
        const Edge* findEdge( const Vertex* vertex ) const;

        // This method is called when we need to relax all "our" edges.
        // We need to pass the queue as we have to notify it about key changes - ugly :(
        void relaxVertices( PriorityQueue<Vertex>& queue );

        // debugging
        void dump( const QCString& indent ) const;

    private:
        Vertex( const Vertex& rhs );
        Vertex& operator=( const Vertex& rhs );

        QPtrList<Edge> m_edges;
        const Vertex* m_predecessor;
        QCString m_mimeType;
        unsigned int m_weight; // "key" inside the queue
        int m_index; // position inside the queue, needed for a fast keyDecreased()
    };


    class Graph
    {

    public:
        Graph( const QCString& from );
        ~Graph() {}

        bool isValid() const { return m_graphValid; }

        // debugging
        void dump() const;

    private:
        Graph( const Graph& rhs );
        Graph& operator=( const Graph& rhs );

        void buildGraph();
        void shortestPath();

        QAsciiDict<Vertex> m_vertices;
        QCString m_from;
        bool m_graphValid;
    };

} // namespace KOffice

// Just a toy class right now to test the basic functionality of
// the graph algorithm.
class KoFilterChain
{

public:
    // find a better name for the enum
    enum Status { OK, StupidError, FileNotFound };

    KoFilterChain( KoFilterManager* manager );
    KoFilterChain( const KoFilterChain& rhs );
    KoFilterChain& operator=( const KoFilterChain& rhs );
    ~KoFilterChain();

    // starts the filtering process
    Status invoke();

private:
    //Q(Value?)List<filter>...
};

#endif // __koffice_filter_chain_h__

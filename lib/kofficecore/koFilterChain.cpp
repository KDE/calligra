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

    Edge::Edge(Vertex* vertex, unsigned int weight) : m_vertex(vertex), m_weight(weight) {
    }

    void Edge::relax(unsigned int vertexWeight, PriorityQueue<Vertex>& queue) {
        if(!m_vertex)
            return;
        m_vertex->setKey(vertexWeight+m_weight);
        queue.keyDecreased(m_vertex); // maintain the heap property
    }

    void Edge::dump(const QCString& indent) const {
        if(m_vertex)
            kdDebug() << indent << "Edge -> '" << m_vertex->mimeType() << "' (" << m_weight << ")" << endl;
        else
            kdDebug() << indent << "Edge -> '(null)' (" << m_weight << ")" << endl;
    }


    Vertex::Vertex(const QCString& mimeType) : m_predecessor(0), m_mimeType(mimeType),
        m_weight(UINT_MAX), m_index(-1) {
        m_edges.setAutoDelete(true);  // we take ownership of added edges
    }

    void Vertex::addEdge(const Edge* edge) {
        if(!edge || edge->weight()==0)
            return;
        m_edges.append(edge);
    }

    const Edge* Vertex::findEdge(const Vertex* vertex) const {
        if(!vertex)
            return 0;
        QPtrListIterator<Edge> it(m_edges);
        for( ; it.current(); ++it) {
            if(it.current()->vertex()==vertex)
                return it.current();
        }
        return 0; // not found here...
    }

    void Vertex::relaxVertices(PriorityQueue<Vertex>& queue) {
        for(Edge *e=m_edges.first(); e; e=m_edges.next())
            e->relax(m_weight, queue);
    }

    void Vertex::dump(const QCString& indent) const {
        kdDebug() << indent << "Vertex: " << m_mimeType << " (" << m_weight << "):" << endl;
        QCString i(indent+"   ");
        QPtrListIterator<Edge> it(m_edges);
        for( ; it.current(); ++it)
            it.current()->dump(i);
    }


    Graph::Graph(const QCString& from) : m_from(from), m_graphValid(false) {
        m_vertices.setAutoDelete(true);
        buildGraph();
    }

    void Graph::buildGraph() {
        QValueList<KoFilterEntry> filters=KoFilterEntry::query(); // no constraint here - we want *all* :)
        kdDebug() << "##### found: " << filters.count() << endl;
    }

    void Graph::dumpAll() const {
        dumpQueue();
        dumpDict();
    }

    void Graph::dumpQueue() const {
        m_queue.dump();
    }

    void Graph::dumpDict() const {
        kdDebug() << "+++++++++ Graph::dumpDict +++++++++" << endl;
        QAsciiDictIterator<Vertex> it(m_vertices);
        for( ; it.current(); ++it)
            it.current()->dump("   ");
        kdDebug() << "+++++++++ Graph::dumpDict (done) +++++++++" << endl;
    }

} // namespace KOffice

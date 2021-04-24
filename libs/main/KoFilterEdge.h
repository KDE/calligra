/* This file is part of the Calligra libraries
   SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOFILTEREDGE_H
#define KOFILTEREDGE_H

#include "KoFilterChain.h"
#include "PriorityQueue_p.h"

namespace CalligraFilter {
/**
 * An internal class representing a filter (=edge) in the filter graph.
 * @internal
 */
class Edge
{

public:
    // creates a new edge to "vertex" with the given weight.
    Edge(Vertex* vertex, KoFilterEntry::Ptr filterEntry);
    ~Edge() {}

    unsigned int weight() const {
        return m_filterEntry ? m_filterEntry->weight : 0;
    }
    KoFilterEntry::Ptr filterEntry() const {
        return m_filterEntry;
    }
    const Vertex* vertex() const {
        return m_vertex;
    }

    // Relaxes the "connected" vertex (i.e. the weight of the
    // connected vertex = "predec.->key()" (parameter) + weight of this edge
    // As this will only be called once we calculate the weight
    // of the edge "on the fly"
    // Note: We have to pass the queue as we have to call keyDecreased :}
    void relax(const Vertex* predecessor, PriorityQueue<Vertex>& queue);

    // debugging
    void dump(const QByteArray& indent) const;

private:
    Edge(const Edge& rhs);
    Edge& operator=(const Edge& rhs);

    Vertex* m_vertex;
    KoFilterEntry::Ptr m_filterEntry;

    class Private;
    Private * const d;
};
}

#endif // KOFILTEREDGE_H

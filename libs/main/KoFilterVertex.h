/* This file is part of the Calligra libraries
   SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOFILTERVERTEX_H
#define KOFILTERVERTEX_H

#include "KoFilterChain.h"

namespace CalligraFilter
{
template<typename T>
class PriorityQueue;
/**
 * An internal class representing a mime type (=node, vertex) in the filter graph.
 * @internal
 */
class Vertex
{
public:
    explicit Vertex(const QByteArray &mimeType);
    ~Vertex();

    QByteArray mimeType() const
    {
        return m_mimeType;
    }

    // Current "weight" of the vertex - will be "relaxed" when
    // running the shortest path algorithm. Returns true if it
    // really has been "relaxed"
    bool setKey(unsigned int key);

    unsigned int key() const
    {
        return m_weight;
    }

    // Can be used to set the key back to "Infinity" (UINT_MAX)
    // and reset the predecessor of this vertex
    void reset();

    // Position in the heap, needed for a fast keyDecreased operation
    void setIndex(int index)
    {
        m_index = index;
    }

    int index() const
    {
        return m_index;
    }

    // predecessor on the way from the source to the destination,
    // needed for the shortest path algorithm
    void setPredecessor(const Vertex *predecessor)
    {
        m_predecessor = predecessor;
    }

    const Vertex *predecessor() const
    {
        return m_predecessor;
    }

    // Adds an outgoing edge to the vertex, transfers ownership
    void addEdge(Edge *edge);

    // Finds the lightest(!) edge pointing to the given vertex, if any (0 if not found)
    // This means it will always search the whole list of edges
    const Edge *findEdge(const Vertex *vertex) const;

    // This method is called when we need to relax all "our" edges.
    // We need to pass the queue as we have to notify it about key changes - ugly :(
    void relaxVertices(PriorityQueue<Vertex> &queue);

    // debugging
    void dump(const QByteArray &indent) const;

private:
    Vertex(const Vertex &rhs) = delete;
    Vertex &operator=(const Vertex &rhs) = delete;

    QList<Edge *> m_edges;
    const Vertex *m_predecessor;
    QByteArray m_mimeType;
    unsigned int m_weight; // "key" inside the queue
    int m_index; // position inside the queue, needed for a fast keyDecreased()

    class Private;
    Private *const d;
};

}
#endif // KOFILTERVERTEX_H

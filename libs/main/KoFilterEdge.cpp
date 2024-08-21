/* This file is part of the Calligra libraries
   SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoFilterEdge.h"
#include "KoFilterVertex.h"
#include "PriorityQueue_p.h"

namespace CalligraFilter
{

Edge::Edge(Vertex *vertex, KoFilterEntry::Ptr filterEntry)
    : m_vertex(vertex)
    , m_filterEntry(filterEntry)
    , d(nullptr)
{
}

void Edge::relax(const Vertex *predecessor, PriorityQueue<Vertex> &queue)
{
    if (!m_vertex || !predecessor || !m_filterEntry)
        return;
    if (m_vertex->setKey(predecessor->key() + m_filterEntry->weight)) {
        queue.keyDecreased(m_vertex); // maintain the heap property
        m_vertex->setPredecessor(predecessor);
    }
}

void Edge::dump(const QByteArray &indent) const
{
    if (m_vertex)
        debugFilter << indent << "Edge -> '" << m_vertex->mimeType() << "' (" << m_filterEntry->weight << ")" << Qt::endl;
    else
        debugFilter << indent << "Edge -> '(null)' (" << m_filterEntry->weight << ")" << Qt::endl;
}

}

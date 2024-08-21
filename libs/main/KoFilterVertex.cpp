/* This file is part of the Calligra libraries
   SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoFilterVertex.h"
#include "KoFilterEdge.h"
#include <limits.h> // UINT_MAX

namespace CalligraFilter
{

Vertex::Vertex(const QByteArray &mimeType)
    : m_predecessor(nullptr)
    , m_mimeType(mimeType)
    , m_weight(UINT_MAX)
    , m_index(-1)
    , d(nullptr)
{
}

Vertex::~Vertex()
{
    qDeleteAll(m_edges);
}

bool Vertex::setKey(unsigned int key)
{
    if (m_weight > key) {
        m_weight = key;
        return true;
    }
    return false;
}

void Vertex::reset()
{
    m_weight = UINT_MAX;
    m_predecessor = nullptr;
}

void Vertex::addEdge(Edge *edge)
{
    if (!edge || edge->weight() == 0)
        return;
    m_edges.append(edge);
}

const Edge *Vertex::findEdge(const Vertex *vertex) const
{
    if (!vertex)
        return nullptr;
    const Edge *edge = nullptr;
    foreach (Edge *e, m_edges) {
        if (e->vertex() == vertex && (!edge || e->weight() < edge->weight())) {
            edge = e;
        }
    }
    return edge;
}

void Vertex::relaxVertices(PriorityQueue<Vertex> &queue)
{
    foreach (Edge *e, m_edges) {
        e->relax(this, queue);
    }
}

void Vertex::dump(const QByteArray &indent) const
{
#ifdef NDEBUG
    Q_UNUSED(indent)
#else
    debugFilter << indent << "Vertex:" << m_mimeType << " (" << m_weight << "):";
    const QByteArray i(indent + "   ");
    foreach (Edge *edge, m_edges) {
        edge->dump(i);
    }
#endif
}

}

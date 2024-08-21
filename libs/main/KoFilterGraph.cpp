/* This file is part of the Calligra libraries
   SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoFilterGraph.h"

#include "KoDocument.h"
#include "KoDocumentEntry.h"
#include "KoFilterEntry.h"
#include "KoFilterManager.h" // KoFilterManager::filterAvailable, private API

#include "KoFilterChainLink.h"
#include "KoFilterEdge.h"
#include "KoFilterVertex.h"
#include "PriorityQueue_p.h"

#include <MainDebug.h>
#include <QMetaMethod>
#include <QPluginLoader>

#include <limits.h> // UINT_MAX

namespace CalligraFilter
{

Graph::Graph(const QByteArray &from)
    : m_from(from)
    , m_graphValid(false)
    , d(nullptr)
{
    buildGraph();
    shortestPaths(); // Will return after a single lookup if "from" is invalid (->no check here)
}

Graph::~Graph()
{
    foreach (Vertex *vertex, m_vertices) {
        delete vertex;
    }
    m_vertices.clear();
}

void Graph::setSourceMimeType(const QByteArray &from)
{
    if (from == m_from)
        return;
    m_from = from;
    m_graphValid = false;

    // Initialize with "infinity" ...
    foreach (Vertex *vertex, m_vertices) {
        vertex->reset();
    }
    // ...and re-run the shortest path search for the new source mime
    shortestPaths();
}

KoFilterChain::Ptr Graph::chain(const KoFilterManager *manager, QByteArray &to) const
{
    if (!isValid() || !manager)
        return KoFilterChain::Ptr();

    if (to.isEmpty()) { // if the destination is empty we search the closest Calligra part
        to = findCalligraPart();
        if (to.isEmpty()) // still empty? strange stuff...
            return KoFilterChain::Ptr();
    }

    const Vertex *vertex = m_vertices.value(to);
    if (!vertex || vertex->key() == UINT_MAX)
        return KoFilterChain::Ptr();

    KoFilterChain::Ptr ret(new KoFilterChain(manager));

    // Fill the filter chain with all filters on the path
    const Vertex *tmp = vertex->predecessor();
    while (tmp) {
        const Edge *const edge = tmp->findEdge(vertex);
        Q_ASSERT(edge);
        ret->prependChainLink(edge->filterEntry(), tmp->mimeType(), vertex->mimeType());
        vertex = tmp;
        tmp = tmp->predecessor();
    }
    return ret;
}

void Graph::dump() const
{
#ifndef NDEBUG
    debugFilter << "+++++++++ Graph::dump +++++++++";
    debugFilter << "From:" << m_from;
    foreach (Vertex *vertex, m_vertices) {
        vertex->dump("   ");
    }
    debugFilter << "+++++++++ Graph::dump (done) +++++++++";
#endif
}

// Query the trader and create the vertices and edges representing
// available mime types and filters.
void Graph::buildGraph()
{
    // Make sure that all available parts are added to the graph
    const QList<KoDocumentEntry> parts(KoDocumentEntry::query());

    foreach (const KoDocumentEntry &part, parts) {
        QJsonObject metaData = part.metaData();
        QStringList nativeMimeTypes = metaData.value("X-KDE-ExtraNativeMimeTypes").toString().split(',');
        nativeMimeTypes += metaData.value("X-KDE-NativeMimeType").toString();

        foreach (const QString &nativeMimeType, nativeMimeTypes) {
            const QByteArray key = nativeMimeType.toLatin1();
            if (!m_vertices.contains(key))
                m_vertices[key] = new Vertex(key);
        }
    }

    // no constraint here - we want *all* :)
    const QList<KoFilterEntry::Ptr> filters(KoFilterEntry::query());

    foreach (KoFilterEntry::Ptr filter, filters) {
        // First add the "starting points" to the dict
        foreach (const QString &import, filter->import) {
            const QByteArray key = import.toLatin1(); // latin1 is okay here (werner)
            // already there?
            if (!m_vertices.contains(key))
                m_vertices.insert(key, new Vertex(key));
        }

        // Are we allowed to use this filter at all?
        if (KoFilterManager::filterAvailable(filter)) {
            foreach (const QString &exportIt, filter->export_) {
                // First make sure the export vertex is in place
                const QByteArray key = exportIt.toLatin1(); // latin1 is okay here
                Vertex *exp = m_vertices.value(key);
                if (!exp) {
                    exp = new Vertex(key);
                    m_vertices.insert(key, exp);
                }
                // Then create the appropriate edges
                foreach (const QString &import, filter->import) {
                    m_vertices[import.toLatin1()]->addEdge(new Edge(exp, filter));
                }
            }
        } else
            debugFilter << "Filter:" << filter->fileName() << " doesn't apply.";
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
void Graph::shortestPaths()
{
    // Is the requested start mime type valid?
    Vertex *from = m_vertices.value(m_from);
    if (!from)
        return;

    // Inititalize start vertex
    from->setKey(0);

    // Fill the priority queue with all the vertices
    PriorityQueue<Vertex> queue(m_vertices);

    while (!queue.isEmpty()) {
        Vertex *min = queue.extractMinimum();
        // Did we already relax all connected vertices?
        if (min->key() == UINT_MAX)
            break;
        min->relaxVertices(queue);
    }
    m_graphValid = true;
}

QByteArray Graph::findCalligraPart() const
{
    // Here we simply try to find the closest Calligra mimetype
    const QList<KoDocumentEntry> parts(KoDocumentEntry::query());
    QList<KoDocumentEntry>::ConstIterator partIt(parts.constBegin());
    QList<KoDocumentEntry>::ConstIterator partEnd(parts.constEnd());

    const Vertex *v = nullptr;

    // Be sure that v gets initialized correctly
    while (!v && partIt != partEnd) {
        QJsonObject metaData = (*partIt).metaData();
        QStringList nativeMimeTypes = metaData.value("X-KDE-ExtraNativeMimeTypes").toString().split(',');
        nativeMimeTypes += metaData.value("X-KDE-NativeMimeType").toString();
        QStringList::ConstIterator it = nativeMimeTypes.constBegin();
        QStringList::ConstIterator end = nativeMimeTypes.constEnd();
        for (; !v && it != end; ++it)
            if (!(*it).isEmpty())
                v = m_vertices.value((*it).toLatin1());
        ++partIt;
    }
    if (!v)
        return "";

    // Now we try to find the "cheapest" Calligra vertex
    while (partIt != partEnd) {
        QJsonObject metaData = (*partIt).metaData();
        QStringList nativeMimeTypes = metaData.value("X-KDE-ExtraNativeMimeTypes").toString().split(',');
        nativeMimeTypes += metaData.value("X-KDE-NativeMimeType").toString();
        QStringList::ConstIterator it = nativeMimeTypes.constBegin();
        QStringList::ConstIterator end = nativeMimeTypes.constEnd();
        for (; !v && it != end; ++it) {
            QString key = *it;
            if (!key.isEmpty()) {
                Vertex *tmp = m_vertices.value(key.toLatin1());
                if (!v || (tmp && tmp->key() < v->key()))
                    v = tmp;
            }
        }
        ++partIt;
    }

    // It seems it already is a Calligra part
    if (v->key() == 0)
        return "";

    return v->mimeType();
}
}

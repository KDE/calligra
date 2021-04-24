/* This file is part of the Calligra libraries
   SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOFILTERGRAPH_H
#define KOFILTERGRAPH_H

#include "komain_export.h"
#include "KoFilterChain.h"
#include "KoFilterVertex.h"
#include <QByteArray>
#include <QHash>

namespace CalligraFilter {

/**
 * The main worker behind the scenes. Manages the creation of the graph,
 * processing the information in it, and creating the filter chains.
 * @internal
 * Only exported for unit tests.
 */
class KOMAIN_TEST_EXPORT Graph
{

public:
    explicit Graph(const QByteArray& from);
    ~Graph();

    bool isValid() const {
        return m_graphValid;
    }

    QByteArray sourceMimeType() const {
        return m_from;
    }
    void setSourceMimeType(const QByteArray& from);

    // Creates a chain from "from" to the "to" mimetype
    // If the "to" mimetype isEmpty() then we try to find the
    // closest Calligra mimetype and use that as destination.
    // After such a search "to" will contain the dest. mimetype (return value)
    // if the search was successful. Might return 0!
    KoFilterChain::Ptr chain(const KoFilterManager* manager, QByteArray& to) const;

    // debugging
    void dump() const;

private:
    Graph(const Graph& rhs);
    Graph& operator=(const Graph& rhs);

    void buildGraph();
    void shortestPaths();
    QByteArray findCalligraPart() const;

    QHash<QByteArray, CalligraFilter::Vertex*> m_vertices;
    QByteArray m_from;
    bool m_graphValid;

    class Private;
    Private * const d;
};

}
#endif // KOFILTERGRAPH_H

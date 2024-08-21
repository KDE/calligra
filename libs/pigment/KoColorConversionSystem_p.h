/*
 *  SPDX-FileCopyrightText: 2007-2008 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOCOLORCONVERSIONSYSTEM_P_H
#define KOCOLORCONVERSIONSYSTEM_P_H

#include "DebugPigment.h"
#include "KoColorConversionTransformationFactory.h"
#include "KoColorModelStandardIds.h"
#include "KoColorSpaceEngine.h"
#include "KoColorSpaceRegistry.h"

#include <QList>

struct KoColorConversionSystem::Node {
    Node()
        : isHdr(false)
        , isInitialized(false)
        , referenceDepth(0)
        , isGray(false)
        , crossingCost(1)
        , colorSpaceFactory(nullptr)
        , isEngine(false)
        , engine(nullptr)
    {
    }

    void init(const KoColorSpaceFactory *_colorSpaceFactory)
    {
        dbgPigment << "Initialise " << modelId << " " << depthId << " " << profileName;

        if (isInitialized) {
            dbgPigment << "Re-initializing node. Old factory" << colorSpaceFactory << "new factory" << _colorSpaceFactory;
        }
        isInitialized = true;

        if (_colorSpaceFactory) {
            isHdr = _colorSpaceFactory->isHdr();
            colorSpaceFactory = _colorSpaceFactory;
            referenceDepth = _colorSpaceFactory->referenceDepth();
            isGray = (_colorSpaceFactory->colorModelId() == GrayAColorModelID || _colorSpaceFactory->colorModelId() == GrayColorModelID);
        }
    }

    void init(const KoColorSpaceEngine *_engine)
    {
        Q_ASSERT(!isInitialized);
        isEngine = true;
        isInitialized = true;
        isHdr = true;
        engine = _engine;
    }

    QString id() const
    {
        return modelId + " " + depthId + " " + profileName;
    }

    QString modelId;
    QString depthId;
    QString profileName;
    bool isHdr;
    bool isInitialized;
    int referenceDepth;
    QList<Vertex *> outputVertexes;
    bool isGray;
    int crossingCost;
    const KoColorSpaceFactory *colorSpaceFactory;
    bool isEngine;
    const KoColorSpaceEngine *engine;
};
Q_DECLARE_TYPEINFO(KoColorConversionSystem::Node, Q_MOVABLE_TYPE);

struct KoColorConversionSystem::Vertex {
    Vertex(Node *_srcNode, Node *_dstNode)
        : srcNode(_srcNode)
        , dstNode(_dstNode)
        , factoryFromSrc(nullptr)
        , factoryFromDst(nullptr)
    {
    }

    ~Vertex()
    {
        if (factoryFromSrc == factoryFromDst) {
            delete factoryFromSrc;
        } else {
            delete factoryFromSrc;
            delete factoryFromDst;
        }
    }

    void setFactoryFromSrc(KoColorConversionTransformationFactory *factory)
    {
        factoryFromSrc = factory;
        initParameter(factoryFromSrc);
    }

    void setFactoryFromDst(KoColorConversionTransformationFactory *factory)
    {
        factoryFromDst = factory;
        if (!factoryFromSrc)
            initParameter(factoryFromDst);
    }

    void initParameter(KoColorConversionTransformationFactory *transfo)
    {
        conserveColorInformation = transfo->conserveColorInformation();
        conserveDynamicRange = transfo->conserveDynamicRange();
    }

    KoColorConversionTransformationFactory *factory()
    {
        if (factoryFromSrc)
            return factoryFromSrc;
        return factoryFromDst;
    }

    Node *srcNode;
    Node *dstNode;

    bool conserveColorInformation;
    bool conserveDynamicRange;

private:
    KoColorConversionTransformationFactory *factoryFromSrc; // Factory provided by the destination node
    KoColorConversionTransformationFactory *factoryFromDst; // Factory provided by the destination node
};

struct KoColorConversionSystem::NodeKey {
    NodeKey(const QString &_modelId, const QString &_depthId, const QString &_profileName)
        : modelId(_modelId)
        , depthId(_depthId)
        , profileName(_profileName)
    {
    }

    bool operator==(const KoColorConversionSystem::NodeKey &rhs) const
    {
        return modelId == rhs.modelId && depthId == rhs.depthId && profileName == rhs.profileName;
    }

    QString modelId;
    QString depthId;
    QString profileName;
};
Q_DECLARE_TYPEINFO(KoColorConversionSystem::NodeKey, Q_MOVABLE_TYPE);

struct KoColorConversionSystem::Path {
    Path()
        : respectColorCorrectness(true)
        , referenceDepth(0)
        , keepDynamicRange(true)
        , isGood(false)
        , cost(0)
    {
    }

    Node *startNode()
    {
        return (vertexes.first())->srcNode;
    }

    bool operator==(const Path &other) const
    {
        return other.vertexes == vertexes;
    }

    const Node *startNode() const
    {
        return (vertexes.first())->srcNode;
    }

    Node *endNode()
    {
        return (vertexes.last())->dstNode;
    }

    const Node *endNode() const
    {
        return (vertexes.last())->dstNode;
    }

    bool isEmpty() const
    {
        return vertexes.isEmpty();
    }

    void appendVertex(Vertex *v)
    {
        if (vertexes.empty()) {
            referenceDepth = v->srcNode->referenceDepth;
        }
        vertexes.append(v);
        if (!v->conserveColorInformation)
            respectColorCorrectness = false;
        if (!v->conserveDynamicRange)
            keepDynamicRange = false;
        referenceDepth = qMin(referenceDepth, v->dstNode->referenceDepth);
        cost += v->dstNode->crossingCost;
    }

    // Compress path to hide the Engine node and correctly select the factory
    typedef QPair<Node *, const KoColorConversionTransformationAbstractFactory *> node2factory;
    QList<node2factory> compressedPath() const
    {
        QList<node2factory> nodes;
        nodes.push_back(node2factory(vertexes.first()->srcNode, vertexes.first()->factory()));
        const KoColorConversionTransformationAbstractFactory *previousFactory = nullptr;
        foreach (Vertex *vertex, vertexes) { // Unless the node is the icc node, add it to the path
            Node *n = vertex->dstNode;
            if (n->isEngine) {
                previousFactory = n->engine;
            } else {
                nodes.push_back(node2factory(n, previousFactory ? previousFactory : vertex->factory()));
                previousFactory = nullptr;
            }
        }
        return nodes;
    }

    int length() const
    {
        return vertexes.size();
    }

    bool contains(Node *n) const
    {
        foreach (Vertex *v, vertexes) {
            if (v->srcNode == n || v->dstNode == n) {
                return true;
            }
        }
        return false;
    }

    QList<Vertex *> vertexes;
    bool respectColorCorrectness;
    int referenceDepth;
    bool keepDynamicRange;
    bool isGood;
    int cost;
};
Q_DECLARE_TYPEINFO(KoColorConversionSystem::Path, Q_MOVABLE_TYPE);

typedef QHash<KoColorConversionSystem::Node *, KoColorConversionSystem::Path> Node2PathHash;

uint qHash(const KoColorConversionSystem::NodeKey &key)
{
    return qHash(key.modelId) + qHash(key.depthId);
}

struct Q_DECL_HIDDEN KoColorConversionSystem::Private {
    QHash<NodeKey, Node *> graph;
    QList<Vertex *> vertexes;
    Node *alphaNode;
};

#define CHECK_ONE_AND_NOT_THE_OTHER(name)                                                                                                                      \
    if (path1.name && !path2.name) {                                                                                                                           \
        return true;                                                                                                                                           \
    }                                                                                                                                                          \
    if (!path1.name && path2.name) {                                                                                                                           \
        return false;                                                                                                                                          \
    }

struct PathQualityChecker {
    PathQualityChecker(int _referenceDepth, bool _ignoreHdr, bool _ignoreColorCorrectness)
        : referenceDepth(_referenceDepth)
        , ignoreHdr(_ignoreHdr)
        , ignoreColorCorrectness(_ignoreColorCorrectness)
    {
    }

    /// @return true if the path maximize all the criteria (except length)
    inline bool isGoodPath(const KoColorConversionSystem::Path &path) const
    {
        return (path.respectColorCorrectness || ignoreColorCorrectness) && (path.referenceDepth >= referenceDepth) && (path.keepDynamicRange || ignoreHdr);
    }

    /**
     * Compare two paths.
     */
    inline bool lessWorseThan(const KoColorConversionSystem::Path &path1, const KoColorConversionSystem::Path &path2) const
    {
        // There is no point in comparing two paths which doesn't start from the same node or doesn't end at the same node
        if (!ignoreHdr) {
            CHECK_ONE_AND_NOT_THE_OTHER(keepDynamicRange)
        }
        if (!ignoreColorCorrectness) {
            CHECK_ONE_AND_NOT_THE_OTHER(respectColorCorrectness)
        }
        if (path1.referenceDepth == path2.referenceDepth) {
            return path1.cost
                < path2.cost; // if they have the same cost, well anyway you have to choose one, and there is no point in keeping one and not the other
        }
        return path1.referenceDepth > path2.referenceDepth;
    }
    int referenceDepth;
    bool ignoreHdr;
    bool ignoreColorCorrectness;
};

#undef CHECK_ONE_AND_NOT_THE_OTHER

#endif

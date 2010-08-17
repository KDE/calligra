/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (c) 2010 Carlos Licea <carlos@kdab.com>
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "MsooXmlDiagramReader.h"

#define MSOOXML_CURRENT_NS "dgm"
#define MSOOXML_CURRENT_CLASS MsooXmlDiagramReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>
#include <QXmlQuery>
#include <QAbstractUriResolver>

namespace MSOOXML
{

/****************************************************************************************************
 * The data-model of the diagram. The data-model was designed after the way it's done in
 * oo.org to make it easier to reuse some of the logic and ideas.
 */

/// The AbstractNode is the base class to handle the diagram data-model.
class AbstractNode
{
    public:
        const char *m_tagName;
        explicit AbstractNode(const char *tagName) : m_tagName(tagName) {}
        virtual ~AbstractNode() {}
        virtual void readElement(MsooXmlDiagramReader* reader) {
            Q_UNUSED(reader);
        }
        virtual void readAll(MsooXmlDiagramReader* reader) {
            while (!reader->atEnd()) {
                QXmlStreamReader::TokenType tokenType = reader->readNext();
                if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
                if (!reader->isStartElement() && reader->qualifiedName() == m_tagName) break;
                readElement(reader);
            }
        }
};

/// A point in the data-model.
class PointNode : public AbstractNode
{
    public:
        QString m_modelId;
        QString m_type;
        QString m_cxnId;
        explicit PointNode() : AbstractNode("dgm:pt") {}
        virtual ~PointNode() {}
        virtual void readElement(MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:prSet")) {
                    //TODO
                }
                else if (reader->qualifiedName() == QLatin1String("dgm:spPr")) {
                    //TODO
                }
                else if (reader->qualifiedName() == QLatin1String("dgm:t")) {
                    //TODO
                }
            }
        }
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(modelId, m_modelId)
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            if (m_type.isEmpty()) m_type = "node";
            if (m_type == QLatin1String("parTrans") || m_type == QLatin1String("sibTrans"))
                TRY_READ_ATTR_WITHOUT_NS_INTO(cxnId, m_cxnId)
            else
                m_cxnId.clear();
            AbstractNode::readAll(reader);
        }
        QList<PointNode*> children() const { return m_children; }
        void addChild(PointNode* node) { m_children << node; }
    private:
        QList<PointNode*> m_children;
};

/// A list of points in the data-model.
class PointListNode : public AbstractNode
{
    public:
        explicit PointListNode() : AbstractNode("dgm:ptLst") {}
        virtual ~PointListNode() { qDeleteAll(m_points); }
        virtual void readElement(MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:pt")) {
                    PointNode *n = new PointNode;
                    n->readAll(reader);
                    m_points << n;
                }
            }
        }
        QList<PointNode*> points() const { return m_points; }
    private:
        QList<PointNode*> m_points;
};

/// A connection between two nodes in the data-model.
class ConnectionNode : public AbstractNode
{
    public:
        QString m_modelId;
        QString m_type;
        QString m_srcId;
        QString m_destId;
        QString m_presId;
        QString m_sibTransId;
        int m_srcOrd;
        int m_destOrd;
        explicit ConnectionNode() : AbstractNode("dgm:cxn"), m_srcOrd(0), m_destOrd(0) {}
        virtual ~ConnectionNode() {}
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(modelId, m_modelId)
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            if (m_type.isEmpty()) m_type = "parOf";
            TRY_READ_ATTR_WITHOUT_NS_INTO(srcId, m_srcId)
            TRY_READ_ATTR_WITHOUT_NS_INTO(destId, m_destId)
            TRY_READ_ATTR_WITHOUT_NS_INTO(presId, m_presId)
            TRY_READ_ATTR_WITHOUT_NS_INTO(sibTransId, m_sibTransId)
            TRY_READ_ATTR_WITHOUT_NS(srcOrd)
            TRY_READ_ATTR_WITHOUT_NS(destOrd)
            m_srcOrd = srcOrd.toInt();
            m_destOrd = destOrd.toInt();
            AbstractNode::readAll(reader);
        }
};

/// A list of connections in the data-model.
class ConnectionListNode : public AbstractNode
{
    public:
        explicit ConnectionListNode() : AbstractNode("dgm:cxnLst") {}
        virtual ~ConnectionListNode() { qDeleteAll(m_connections); }
        virtual void readElement(MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:cxn")) {
                    ConnectionNode *n = new ConnectionNode;
                    n->readAll(reader);
                    m_connections << n;
                }
            }
        }
        QList<ConnectionNode*> connections() const { return m_connections; }
    private:
        QList<ConnectionNode*> m_connections;
};

class AtomNode : public AbstractNode
{
    public:
        explicit AtomNode(const char *tagName) : AbstractNode(tagName) {}
        virtual ~AtomNode() { qDeleteAll(m_children); }
        virtual void readElement(MsooXmlDiagramReader* reader);
        QList<AtomNode*> children() const { return m_children; }
        void addChild(AtomNode* node) { m_children << node; }
    private:
        QList<AtomNode*> m_children;
};

class LayoutNode : public AtomNode
{
    public:
        QString m_name;
        explicit LayoutNode() : AtomNode("dgm:layoutNode") {}
        virtual ~LayoutNode() {}
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            //TRY_READ_ATTR_WITHOUT_NS_INTO(styleLbl, m_styleLbl)
            AtomNode::readAll(reader);
        }
};

class ShapeNode : public AtomNode
{
    public:
        QString m_type;
        QString m_name;
        explicit ShapeNode() : AtomNode("dgm:shape") {}
        virtual ~ShapeNode() {}
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            if (m_type.isEmpty()) m_type = "obj";
            //TRY_READ_ATTR_WITHOUT_NS_INTO(id, m_id)
            //TRY_READ_ATTR_WITHOUT_NS_INTO(idx, m_idx)
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            AtomNode::readAll(reader);
        }
};

class ChooseNode : public AtomNode
{
    public:
        explicit ChooseNode() : AtomNode("dgm:choose") {}
        virtual ~ChooseNode() {}
};

class ForEachNode : public AtomNode
{
    public:
        explicit ForEachNode() : AtomNode("dgm:forEach") {}
        virtual ~ForEachNode() {}
};

void AtomNode::readElement(MsooXmlDiagramReader* reader)
{
    if (reader->isStartElement()) {
        AtomNode *node = 0;

        if (reader->qualifiedName() == QLatin1String("dgm:layoutNode")) {
            node = new LayoutNode;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:shape")) {
            node = new ShapeNode;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:choose")) {
            node = new ChooseNode;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:forEach")) {
            node = new ForEachNode;
        }
        
        if (node) {
            addChild(node);
            node->readAll(reader);
        }
    }
}

}

/****************************************************************************************************
 * The reader-context and the reader itself. Note that there will be one reader-instance per xml-file
 * in a diagram. The reader-context is shared between the reader-instances for one diagram.
 */

using namespace MSOOXML;

MsooXmlDiagramReaderContext::MsooXmlDiagramReaderContext(KoStore* storeout)
    : MSOOXML::MsooXmlReaderContext()
    , m_storeout(storeout)
    , m_points(new PointListNode)
    , m_connections(new ConnectionListNode)
    , m_layout(new LayoutNode)
{
}

MsooXmlDiagramReaderContext::~MsooXmlDiagramReaderContext()
{
    delete m_points;
    delete m_connections;
    delete m_layout;
}

MsooXmlDiagramReader::MsooXmlDiagramReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , m_type(InvalidType)
{
}

MsooXmlDiagramReader::~MsooXmlDiagramReader()
{
}

KoFilter::ConversionStatus MsooXmlDiagramReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<MsooXmlDiagramReaderContext*>(context);
    Q_ASSERT(m_context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    if (qualifiedName() == QLatin1String("dgm:dataModel")) {
        m_type = DataModelType;
        
        while (!atEnd()) {
            QXmlStreamReader::TokenType tokenType = readNext();
            if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
            if (isStartElement()) {
                if (qualifiedName() == QLatin1String("dgm:ptLst")) { // list of points
                    m_context->m_points->readAll(this);
                }
                else if (qualifiedName() == QLatin1String("dgm:cxnLst")) { // list of connections
                    m_context->m_connections->readAll(this);
                }
            }
        }

        QMap<QString, PointNode*> pointMap;
        foreach(PointNode* node, m_context->m_points->points()) {
            if (!node->m_modelId.isEmpty()) {
                pointMap[node->m_modelId] = node;
            }
        }

        QMap<QString, PointNode*> pointTree;
        foreach(ConnectionNode* node, m_context->m_connections->connections()) {
            if (node->m_type != "parOf") continue;

            PointNode* source = 0;
            if (pointTree.contains(node->m_srcId)) {
                source = pointTree[node->m_srcId];
            } else {
                if (!pointMap.contains(node->m_srcId)) continue;
                source = pointMap[node->m_srcId];
                pointTree[node->m_srcId] = source;
            }

            if (!pointMap.contains(node->m_destId)) continue;
            PointNode* destination = pointMap[node->m_destId];
            source->addChild(destination);
            pointTree[node->m_destId] = destination;
        }

#if 0
        for(QMap<QString, PointNode*>::Iterator it = pointTree.begin(); it != pointTree.end(); ++it) {
            PointNode* node = (*it);
            kDebug()<<">"<< node->m_modelId;
            foreach(PointNode* n, node->children()) {
                kDebug()<<"  >"<< n->m_modelId;
                foreach(PointNode* n2, n->children()) {
                    kDebug()<<"    >"<< n2->m_modelId;
                }
            }
        }
#endif
    }
    else if (qualifiedName() == QLatin1String("dgm:layoutDef")) {
        m_type = LayoutDefType;

        while (!atEnd()) {
            QXmlStreamReader::TokenType tokenType = readNext();
            if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
            if (isStartElement()) {
                if (qualifiedName() == QLatin1String("dgm:layoutNode")) {
                    m_context->m_layout->readAll(this);
                }
                //ELSE_TRY_READ_IF(title)
                //ELSE_TRY_READ_IF(catLst)
                //ELSE_TRY_READ_IF(sampData)
                //ELSE_TRY_READ_IF(styleData)
            }
        }
        
#if 0
        device()->seek(0);
        const QString xml = QString::fromUtf8(device()->readAll());

        QXmlNamePool namepool;
        //QXmlName name1(namepool, "layoutDef", "http://schemas.openxmlformats.org/drawingml/2006/diagram", "dgm");
        QXmlQuery query(QXmlQuery::XSLT20, namepool);
        if (!query.setFocus(xml)) {
            return KoFilter::WrongFormat;
        }

        //query.bindVariable("inputDocument", ba);
        
        class UriResolver : public QAbstractUriResolver {
            public:
                UriResolver() : QAbstractUriResolver() {}
                virtual ~UriResolver() {}
                virtual QUrl resolve(const QUrl &relative, const QUrl &baseURI) const { return QString(); }
        };
        UriResolver resolver;
        query.setUriResolver(&resolver);

        query.setQuery(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<xsl:stylesheet\n"
                "xmlns:dgm=\"http://schemas.openxmlformats.org/drawingml/2006/diagram\"\n"
                "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"\n"
                "version=\"2.0\">\n"
            "<xsl:template match=\"/dgm:layoutDef\">\n"
                "abc\n"
            "</xsl:template>\n"
            "</xsl:stylesheet>\n"
        );
        Q_ASSERT(query.isValid());

        QByteArray result;
        QBuffer buffer(&result);
        buffer.open(QBuffer::ReadWrite);
        query.evaluateTo(&buffer);
        buffer.close();
        kDebug()<<"3>>>>>>"<<result;
#endif
    }
    else if (qualifiedName() == QLatin1String("dgm:styleDef")) {
        m_type = StyleDefType;
    }
    else if (qualifiedName() == QLatin1String("dgm:colorsDef")) {
        m_type = ColorsDefType;
    }
    else {
        return KoFilter::WrongFormat;
    }

    m_context = 0;
    m_type = InvalidType;
    return KoFilter::OK;
}


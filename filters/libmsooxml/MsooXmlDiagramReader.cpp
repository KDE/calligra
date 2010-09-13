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
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <QExplicitlySharedDataPointer>
//#include <QXmlQuery>
//#include <QAbstractUriResolver>
#include <typeinfo>

#include "MsooXmlDiagramReader_p.h"

using namespace MSOOXML;

MsooXmlDiagramReaderContext::MsooXmlDiagramReaderContext(KoGenStyles* styles)
    : MSOOXML::MsooXmlReaderContext()
    , m_styles(styles)
    , m_context(new Diagram::Context)
{
}

MsooXmlDiagramReaderContext::~MsooXmlDiagramReaderContext()
{
    delete m_context;
}

void MsooXmlDiagramReaderContext::saveIndex(KoXmlWriter* xmlWriter, const QRect &rect)
{
    // The root layout node always inherits the canvas dimensions by default.
    m_context->m_rootLayout->m_x = rect.x();
    m_context->m_rootLayout->m_y = rect.y();
    m_context->m_rootLayout->m_width = rect.width();
    m_context->m_rootLayout->m_height = rect.height();
    m_context->m_rootLayout->m_cx = 0;
    m_context->m_rootLayout->m_cy = 0;
    // Do the (re-)layout.
    m_context->m_rootLayout->layoutAtom(m_context);
    // Write the content.
    m_context->m_rootLayout->writeAtom(m_context, xmlWriter, m_styles);
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
        
        Diagram::PointListNode rootList;
        while (!atEnd()) {
            QXmlStreamReader::TokenType tokenType = readNext();
            if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
            if (isStartElement()) {
                if (qualifiedName() == QLatin1String("dgm:ptLst")) { // list of points
                    rootList.readAll(m_context->m_context, this);
                } else if (qualifiedName() == QLatin1String("dgm:cxnLst")) { // list of connections
                    m_context->m_context->m_connections->readAll(m_context->m_context, this);
                }
            }
        }
        
        QMap<QString, Diagram::PointNode*> pointMap;
        foreach(Diagram::AbstractNode* node, rootList.children()) {
            if(Diagram::PointNode* point = dynamic_cast<Diagram::PointNode*>(node))
                if (!point->m_modelId.isEmpty())
                    pointMap[point->m_modelId] = point;
        }

        QMap<QString, Diagram::PointNode*> pointTree;
        foreach(Diagram::AbstractNode* node, m_context->m_context->m_connections->children()) {
            if(Diagram::ConnectionNode* connection = dynamic_cast<Diagram::ConnectionNode*>(node)) {
                if (connection->m_type != "parOf") continue;

                Diagram::PointNode* source = 0;
                if (pointTree.contains(connection->m_srcId)) {
                    source = pointTree[connection->m_srcId];
                }
                else {
                    if (!pointMap.contains(connection->m_srcId)) continue;
                    source = pointMap[connection->m_srcId];
                    pointTree[connection->m_srcId] = source;
                }

                if (!pointMap.contains(connection->m_destId)) continue;

                Diagram::PointNode* destination = pointMap[connection->m_destId];
                rootList.removeChild(destination);
                source->addChild(destination);
                pointTree[connection->m_destId] = destination;
            }
        }

        Q_ASSERT(!m_context->m_context->m_rootPoint);
        foreach(Diagram::AbstractNode* node, rootList.children()) {
            if(Diagram::PointNode* pt = dynamic_cast<Diagram::PointNode*>(node)) {
                if(pt->m_type == QLatin1String("doc")) {
                    m_context->m_context->m_rootPoint = pt;
                    break;
                }
            }
        }
        Q_ASSERT(m_context->m_context->m_rootPoint);
        rootList.removeChild(m_context->m_context->m_rootPoint);
        m_context->m_context->setCurrentNode(m_context->m_context->m_rootPoint);

        //for(QMap<QString, Diagram::PointNode*>::Iterator it = pointTree.begin(); it != pointTree.end(); ++it) (*it)->dump(m_context->m_context, 0);
        //m_context->m_context->m_rootPoint->dump(0);
    }
    else if (qualifiedName() == QLatin1String("dgm:layoutDef")) {
        m_type = LayoutDefType;

        while (!atEnd()) {
            QXmlStreamReader::TokenType tokenType = readNext();
            if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
            if (isStartElement()) {
                if (qualifiedName() == QLatin1String("dgm:layoutNode")) {
                    m_context->m_context->m_rootLayout->readAll(m_context->m_context, this);
                }
                //ELSE_TRY_READ_IF(title)
                //ELSE_TRY_READ_IF(sampData)
                //ELSE_TRY_READ_IF(styleData)
            }
        }
        Q_ASSERT(m_context->m_context->m_rootPoint);
        
        //m_context->m_context->m_rootLayout->dump(m_context->m_context,0);
        m_context->m_context->m_rootLayout->build(m_context->m_context);
        //m_context->m_context->m_rootLayout->dump(m_context->m_context,0);
        //Q_ASSERT(false);
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
        //TODO
    }
    else if (qualifiedName() == QLatin1String("dgm:colorsDef")) {
        m_type = ColorsDefType;
        //TODO
    }
    else {
        return KoFilter::WrongFormat;
    }

    m_context = 0;
    m_type = InvalidType;
    return KoFilter::OK;
}


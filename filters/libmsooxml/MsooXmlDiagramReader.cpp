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
    m_context->m_rootLayout->m_values["l"] = rect.x();
    m_context->m_rootLayout->m_values["t"] = rect.y();
    m_context->m_rootLayout->m_values["w"] = rect.width();
    m_context->m_rootLayout->m_values["h"] = rect.height();
    m_context->m_rootLayout->m_values["ctrX"] = 0;
    m_context->m_rootLayout->m_values["ctrY"] = 0;
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
connection->dump(m_context->m_context,10);

                Diagram::PointNode* source = 0;
                if (pointTree.contains(connection->m_srcId)) {
                    source = pointTree[connection->m_srcId];
                } else {
                    if (!pointMap.contains(connection->m_srcId)) continue;
                    source = pointMap[connection->m_srcId];
                    pointTree[connection->m_srcId] = source;
                }

                if (!pointMap.contains(connection->m_destId)) continue;

                Diagram::PointNode* destination = pointMap[connection->m_destId];
                rootList.removeChild(destination);

                const bool isFirst = source->children().isEmpty();

                //FIXME specs are missing details how sibling-transitions are mapped for >2 child-nodes. Let's assume for now
                // that we only need to add in max 2 transitions, one for the sibling before and one for after the current node.
                
                // transition between the previous node with this node
                if(!isFirst) {
                    Diagram::PointNode* siblingTransition = new Diagram::PointNode;
                    //siblingTransition->m_modelId = destination->m_modelId;
                    siblingTransition->m_cxnId = connection->m_modelId;
                    siblingTransition->m_type = "sibTrans";
                    source->addChild(siblingTransition);
                }

                // transition between parent and child
                Diagram::PointNode* parentTransition = new Diagram::PointNode;
                //parentTransition->m_modelId = ;
                parentTransition->m_cxnId = connection->m_modelId;
                parentTransition->m_type = "parTrans";
                source->addChild(parentTransition);

                // attach the child node to the parent node
                source->addChild(destination);
                // remember the connection that was responsible for connecting the node with it's parent
                Q_ASSERT(destination->m_cxnId.isEmpty());
                destination->m_cxnId = connection->m_modelId;
                // remember for future reference
                pointTree[connection->m_destId] = destination;
                
                // transition between this node with the previous node
                if(!isFirst) {
                    Diagram::PointNode* siblingTransition = new Diagram::PointNode;
                    //siblingTransition->m_modelId = ;
                    siblingTransition->m_cxnId = connection->m_modelId;
                    siblingTransition->m_type = "sibTrans";
                    source->addChild(siblingTransition);
                }
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
        if(!m_context->m_context->m_rootPoint) {
            kWarning() << "Data-definition doesn't specify a root-node";
            return KoFilter::WrongFormat;
        }
        rootList.removeChild(m_context->m_context->m_rootPoint);
        m_context->m_context->setCurrentNode(m_context->m_context->m_rootPoint);
// kDebug()<<"1x.................................";
        //for(QMap<QString, Diagram::PointNode*>::Iterator it = pointTree.begin(); it != pointTree.end(); ++it) (*it)->dump(m_context->m_context, 0);
        m_context->m_context->m_rootPoint->dump(m_context->m_context, 0);
// kDebug()<<"2x.................................";
//         Q_ASSERT(false);
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
        if(!m_context->m_context->m_rootPoint) {
            kWarning() << "Layout-definition doesn't specify a root-point";
            return KoFilter::WrongFormat;
        }
        
        m_context->m_context->m_rootLayout->build(m_context->m_context);
        //m_context->m_context->m_rootLayout->dump(m_context->m_context,0);
        //Q_ASSERT(false);
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


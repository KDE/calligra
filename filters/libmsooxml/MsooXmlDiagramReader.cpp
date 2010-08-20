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
//#include <QXmlQuery>
#include <QAbstractUriResolver>
#include <typeinfo>

namespace MSOOXML { namespace Diagram {

/****************************************************************************************************
 * The data-model and layout logic of the diagram. The data-model was designed after the way it's
 * descripbed in the MSOOXML-specs and how it was done in oo.org.
 *
 * See also;
 * - http://wiki.services.openoffice.org/wiki/SmartArt
 * - http://msdn.microsoft.com/en-us/magazine/cc163470.aspx
 */

#define DEBUG_DUMP \
    qDebug() << QString("%1Dgm::%2::%3").arg(QString(' ').repeated(level)).arg(typeid(this).name()).arg(__FUNCTION__) << "atom=" << m_tagName
#define DEBUG_PROCESS \
    qDebug() << QString("Dgm::%1::%2").arg(typeid(this).name()).arg(__FUNCTION__) << "atom=" << m_tagName

/// The AbstractNode is the base class to handle the diagram data-model (content of data1.xml).
class AbstractNode
{
    public:
        const QString m_tagName;
        explicit AbstractNode(const QString &tagName) : m_tagName(tagName) {}
        virtual ~AbstractNode() {}
        virtual void dump(int level) {
            DEBUG_DUMP;
        }
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
        explicit PointNode() : AbstractNode("dgm:pt"), m_parent(0) {}
        virtual ~PointNode() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "modelId=" << m_modelId << "type=" << m_type << "cxnId=" << m_cxnId;
            foreach(PointNode* node, m_children)
                node->dump(level + 1);
        }
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
        PointNode* parent() const { return m_parent; } 
        QList<PointNode*> children() const { return m_children; }
        void addChild(PointNode* node) {
            node->m_parent = this;
            m_children << node;
        }
    private:
        PointNode* m_parent;
        QList<PointNode*> m_children;
};

/// A list of points in the data-model.
class PointListNode : public AbstractNode
{
    public:
        explicit PointListNode() : AbstractNode("dgm:ptLst") {}
        virtual ~PointListNode() { qDeleteAll(m_points); }
        virtual void dump(int level) {
            //DEBUG_DUMP;
            foreach(PointNode* node, m_points)
                node->dump(level + 1);
        }
        virtual void readElement(MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:pt")) {
                    PointNode *n = new PointNode;
                    m_points << n;
                    n->readAll(reader);
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
        virtual void dump(int level) {
            DEBUG_DUMP << "modelId=" << m_modelId << "type=" << m_type << "srcId=" << m_srcId << "destId=" << m_destId;
        }
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
        virtual void dump(int level) {
            //DEBUG_DUMP;
            foreach(ConnectionNode* node, m_connections)
                node->dump(level + 1);
        }
        virtual void readElement(MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:cxn")) {
                    ConnectionNode *n = new ConnectionNode;
                    m_connections << n;
                    n->readAll(reader);
                }
            }
        }
        QList<ConnectionNode*> connections() const { return m_connections; }
    private:
        QList<ConnectionNode*> m_connections;
};

/// The data model.
class DataModel
{
    public:
        PointListNode* m_points;
        ConnectionListNode* m_connections;
        explicit DataModel() : m_points(new PointListNode), m_connections(new ConnectionListNode) {}
        ~DataModel() { delete m_points; delete m_connections; }
        /*
        PointNode* dataModelPoint(const QString &axis) const {
            QStringList axislist = axis.split(' ', QString::SkipEmptyParts);
            //if (axislist.isEmpty()) return 0;
            // if(m_axis.contains("ancst")) // Ancestor
            // if(m_axis.contains("ancstOrSelf")) // Ancestor Or Self
            // if(m_axis.contains("ch")) // Child
            // if(m_axis.contains("des")) // Descendant
            // if(m_axis.contains("desOrSelf")) // Descendant Or Self
            // if(m_axis.contains("follow")) // Follow
            // if(m_axis.contains("followSib")) // Follow Sibling
            // if(m_axis.contains("none")) // None
            // if(m_axis.contains("par")) // Parent
            // if(m_axis.contains("preced")) // Preceding
            // if(m_axis.contains("precedSib")) // Preceding Sibling
            // if(m_axis.contains("root")) // Root
            // if(m_axis.contains("self")) // Self
        }
        */
};

class AbstractAtom;
class LayoutNode;
class Context
{
    public:
        KoOdfWriters* m_odfWriters;
        DataModel* m_datamodel;
        //QMap<LayoutNode*, AbstractAtom*> m_layoutDataMap;
        //QMap<AbstractAtom*, LayoutNode*> m_dataLayoutMap;
        ////AbstractAtom* m_item;
        //PointNode* m_item;
        Context(KoOdfWriters* odfWriters, DataModel* datamodel) : m_odfWriters(odfWriters), m_datamodel(datamodel)/*, m_item(0)*/ {}
        ~Context() {}
};

/// Base class for layout-operations (content of layout1.xml)
class AbstractAtom
{
    public:
        const QString m_tagName;
        explicit AbstractAtom(const QString &tagName) : m_tagName(tagName), m_parent(0) {}
        virtual ~AbstractAtom() { qDeleteAll(m_children); }
        virtual void dump(int level) {
            DEBUG_DUMP;
            foreach(AbstractAtom* node, m_children)
                node->dump(level + 1);
        }
        virtual void readElement(MsooXmlDiagramReader* reader);
        virtual void readAll(MsooXmlDiagramReader* reader) {
            while (!reader->atEnd()) {
                QXmlStreamReader::TokenType tokenType = reader->readNext();
                if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
                if (!reader->isStartElement() && reader->qualifiedName() == m_tagName) break;
                readElement(reader);
            }
        }
        virtual void processAtom(Context* context) = 0;
        AbstractAtom* parent() const { return m_parent; }
        QList<AbstractAtom*> children() const { return m_children; }
        void addChild(AbstractAtom* node) {
            node->m_parent = this;
            m_children << node;
        }
    protected:
        AbstractAtom* m_parent;
        QList<AbstractAtom*> m_children;
        
        void processAtomChildren(Context* context) {
            foreach(AbstractAtom* node, m_children)
                node->processAtom(context);
        }
};

class LayoutNode : public AbstractAtom
{
    public:
        QString m_name;
        explicit LayoutNode() : AbstractAtom("dgm:layoutNode")/*, m_dataModelPoint(0)*/ {}
        virtual ~LayoutNode() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "name=" << m_name;
            foreach(AbstractAtom* node, m_children)
                node->dump(level + 1);
        }
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            //TRY_READ_ATTR_WITHOUT_NS_INTO(styleLbl, m_styleLbl)
            AbstractAtom::readAll(reader);
        }
        virtual void processAtom(Context* context) {
            DEBUG_PROCESS << "name=" << m_name;
            //TODO
            processAtomChildren(context);
        }
};

class ShapeNode : public AbstractAtom
{
    public:
        QString m_type;
        QString m_blip;
        explicit ShapeNode() : AbstractAtom("dgm:shape") {}
        virtual ~ShapeNode() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "type=" << m_type << "blip=" << m_blip;
        }
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            //if (m_type.isEmpty()) m_type = "obj";
            TRY_READ_ATTR_WITHOUT_NS_INTO(blip, m_blip)
            AbstractAtom::readAll(reader);
        }
        virtual void processAtom(Context* context) {
            DEBUG_PROCESS << "type=" << m_type << "blip=" << m_blip;
            
            if (m_type == QLatin1String("ellipse")) {
static int iii=0;
if(++iii >= 2) return;
                KoXmlWriter *body = context->m_odfWriters->body;
/*
          <table:table-cell>
            <draw:g draw:name="Group 1" draw:z-index="0" table:end-cell-address="Sheet1.H29" table:end-x="0.6016in" table:end-y="0.1339in">

              <draw:custom-shape draw:name="_s1027" draw:style-name="gr1" draw:text-style-name="P1" svg:height="1.8701in" svg:width="1.9516in" svg:x="1.6319in" svg:y="0.8161in">
                <text:p/>
                <draw:enhanced-geometry draw:enhanced-path="U 10800 10800 10800 10800 0 360 Z N" draw:glue-points="10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163" draw:text-areas="3163 3163 18437 18437" draw:type="ellipse" svg:viewBox="0 0 21600 21600"/>
              </draw:custom-shape>

              <draw:custom-shape draw:name="_s1028" draw:style-name="gr2" draw:text-style-name="P3" svg:height="0.4705in" svg:width="0.5232in" svg:x="2.3461in" svg:y="0.163in">
                <text:p text:style-name="P2">
                  <text:span text:style-name="T1">
                    a
                  </text:span>
                </text:p>
                <draw:enhanced-geometry draw:enhanced-path="M 0 0 L 21600 0 21600 21600 0 21600 0 0 Z N" draw:type="rectangle" svg:viewBox="0 0 21600 21600"/>
              </draw:custom-shape>

              <draw:custom-shape draw:name="_s1029" draw:style-name="gr3" draw:text-style-name="P1" svg:height="1.8701in" svg:width="1.9516in" svg:x="2.2598in" svg:y="1.8748in">
                <text:p/>
                <draw:enhanced-geometry draw:enhanced-path="U 10800 10800 10800 10800 0 360 Z N" draw:glue-points="10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163" draw:text-areas="3163 3163 18437 18437" draw:type="ellipse" svg:viewBox="0 0 21600 21600"/>
              </draw:custom-shape>

              <draw:custom-shape draw:name="_s1030" draw:style-name="gr2" draw:text-style-name="P3" svg:height="0.4646in" svg:width="0.5232in" svg:x="4.2543in" svg:y="3.3626in">
                <text:p text:style-name="P2">
                  <text:span text:style-name="T1">
                    c
                  </text:span>
                </text:p>
                <draw:enhanced-geometry draw:enhanced-path="M 0 0 L 21600 0 21600 21600 0 21600 0 0 Z N" draw:type="rectangle" svg:viewBox="0 0 21600 21600"/>
              </draw:custom-shape>

              <draw:custom-shape draw:name="_s1031" draw:style-name="gr4" draw:text-style-name="P1" svg:height="1.8642in" svg:width="1.9575in" svg:x="0.9854in" svg:y="1.8689in">
                <text:p/>
                <draw:enhanced-geometry draw:enhanced-path="U 10800 10800 10800 10800 0 360 Z N" draw:glue-points="10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163" draw:text-areas="3163 3163 18437 18437" draw:type="ellipse" svg:viewBox="0 0 21600 21600"/>
              </draw:custom-shape>

              <draw:custom-shape draw:style-name="gr2" draw:text-style-name="P3" svg:height="0.4705in" svg:width="0.5169in" svg:x="0.4374in" svg:y="3.3685in">
                <text:p text:style-name="P2">
                  <text:span text:style-name="T1">
                    b
                  </text:span>
                </text:p>
                <draw:enhanced-geometry draw:enhanced-path="M 0 0 L 21600 0 21600 21600 0 21600 0 0 Z N" draw:type="rectangle" svg:viewBox="0 0 21600 21600"/>
              </draw:custom-shape>

            </draw:g>
          </table:table-cell>
*/

body->startElement("draw:g");
body->addAttribute("draw:name", "Group 1");
body->addAttribute("draw:z-index", "0");
body->addAttribute("table:end-cell-address", "Sheet1.H29");
body->addAttribute("table:end-x", "0.6016in");
body->addAttribute("table:end-y", "0.1339in");
                body->startElement("draw:custom-shape");
                body->addAttribute("draw:layer", "layout");
                body->addAttribute("draw:name", "_s4101");
                //body->addAttribute("draw:style-name", "gr1");
                //body->addAttribute("draw:text-style-name", "P2");
                body->addAttribute("svg:x", "10.358cm");
                body->addAttribute("svg:y", "6.606cm");
                /*if(m_width > 0) */body->addAttribute("svg:width", "4.684cm");
                /*if(m_height > 0) */body->addAttribute("svg:height", "4.683cm");
                
                body->startElement("text:p");
                body->endElement();
                
                body->startElement("draw:enhanced-geometry");
                body->addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N");
                body->addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
                body->addAttribute("draw:text-areas", "3163 3163 18437 18437");
                body->addAttribute("draw:type", "ellipse");
                body->addAttribute("svg:viewBox", "0 0 21600 21600");
                body->endElement();

                body->endElement(); // draw:custom-shape
body->endElement(); // draw:g
            }
            
        }
};

/// The algorithm used by the containing layout node. The algorithm defines the behavior of the layout node along with the behavior and layout of the nested layout nodes.
class AlgorithmNode : public AbstractAtom
{
    public:
        QString m_type; // composite, conn, cycle, hierChild, hierRoot, lin, pyra, snake, sp, tx
        explicit AlgorithmNode() : AbstractAtom("dgm:alg") {}
        virtual ~AlgorithmNode() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "type=" << m_type;
        }
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            AbstractAtom::readAll(reader);
        }
        virtual void processAtom(Context*) {
            //DEBUG_PROCESS;
            //TODO
        }
};

/// This element specifies a particular data model point which is to be mapped to the containing layout node.
class PresentationOfNode : public AbstractAtom
{
    public:
        QString m_axis;
        int m_count;
        QString m_hideLastTrans;
        QString m_dataPointType;
        int m_start;
        int m_step;
        explicit PresentationOfNode() : AbstractAtom("dgm:presOf"), m_count(0), m_start(0), m_step(0) {}
        virtual ~PresentationOfNode() {}
        virtual void dump(int level) {
            DEBUG_DUMP;
        }
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(axis, m_axis)
            TRY_READ_ATTR_WITHOUT_NS(cnt)
            m_count = cnt.toInt();
            TRY_READ_ATTR_WITHOUT_NS_INTO(hideLastTrans, m_hideLastTrans)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_dataPointType)
            TRY_READ_ATTR_WITHOUT_NS(st)
            m_start = st.toInt();
            TRY_READ_ATTR_WITHOUT_NS(step)
            m_step = step.toInt();
            AbstractAtom::readAll(reader);
        }
        virtual void processAtom(Context* context) {
            //DEBUG_PROCESS;
#if 0

            for(AbstractAtom* node = m_parent; node; node = node->parent()) {
                if (LayoutNode* layoutnode = dynamic_cast<LayoutNode*>(node)) {

/*
QList<PointNode*> fetchAxis(const QString &axis)
{
    QList<PointNode*> list;
    if(axis == QLatin1String("ancst")) { // Ancestor
        for(PointNode* n = m_item; n; n = n->parent()) list.append(n);
    } else if(axis == QLatin1String("ancstOrSelf")) { // Ancestor Or Self
        list.append(m_item);
        for(PointNode* n = m_item; n; n = n->parent()) list.append(n);
    } else if(axis == QLatin1String("ch")) { // Child
        list = m_item->children();
    } else if(axis == QLatin1String("des")) { // Descendant
        list = m_item->childrenRecursive();
    } else if(m_axis == QLatin1String("desOrSelf")) { // Descendant Or Self
        list.append(m_item);
        list = m_item->childrenRecursive();
    // } else if(m_axis == QLatin1String("follow")) { // Follow
    // } else if(m_axis == QLatin1String("followSib")) { // Follow Sibling
    } else if(m_axis == QLatin1String("par")) { // Parent
        if (m_item->parent()) list.append(m_item->parent());
    // } else if(m_axis == QLatin1String("preced")) { // Preceding
    // } else if(m_axis == QLatin1String("precedSib")) { // Preceding Sibling
    // } else if(m_axis == QLatin1String("root")) { // Root
    } else if(axis == QLatin1String("self")) { // Self
        list.append(m_item);
    }
    return list;
}
QList<PointNode*> filterAxis(const QString &type, QList<PointNode*> axis)
{
    
}
*/                    
                    
                    
                    if(m_axis.isEmpty()) {
                        // An empty presOf element (<presOf />) indicates that the layoutNode does not map to anything
                        // in the data model. These tags are used for shapes without text, as well as for layout nodes
                        // without shapes that are used by algorithms to lay out the actual shapes.
                        context->m_layoutDataMap[layoutnode] = 0;
                    }
                    else if(m_axis == QLatin1String("self")) { // Self
                        // A presOf with axis="self" associates the context item with the generated layout node. In
                        // the sample XML, this presOf puts the parent text into the parent text area shape.
                        //context->m_dataLayoutMap[context->m_item] = layoutnode;
                        context->m_layoutDataMap[layoutnode] = context->m_item;
                    }
                    else if(m_axis == QLatin1String("des")) { // Descendant
                        // A presOf with axis="des" and ptType="node" associates all of the descendants of the context
                        // item with the generated layout node. For the sample, this presOf puts all of the child text
                        // for a specific parent into the same text area shape.
                        //context->m_layoutDataMap[layoutnode] = context->m_item->children();
                        foreach(AbstractAtom *node, context->m_item->children())
                           context->m_dataLayoutMap[node] = layoutnode;
                    }
                        
                    // if(m_axis == QLatin1String("ancst")) // Ancestor
                    // if(m_axis == QLatin1String("ancstOrSelf")) // Ancestor Or Self
                    // if(m_axis == QLatin1String("ch")) // Child
                    // if(m_axis == QLatin1String("des")) // Descendant
                    // if(m_axis == QLatin1String("desOrSelf")) // Descendant Or Self
                    // if(m_axis == QLatin1String("follow")) // Follow
                    // if(m_axis == QLatin1String("followSib")) // Follow Sibling
                    // if(m_axis == QLatin1String("none")) // None
                    // if(m_axis == QLatin1String("par")) // Parent
                    // if(m_axis == QLatin1String("preced")) // Preceding
                    // if(m_axis == QLatin1String("precedSib")) // Preceding Sibling
                    // if(m_axis == QLatin1String("root")) // Root
                    // if(m_axis == QLatin1String("self")) // Self
                        
                    break;
                }
            }
#endif
        }
};

class IfNode : public AbstractAtom
{
    public:
        QString m_argument;
        QString m_axis;
        int m_count;
        QString m_function;
        QString m_hideLastTrans;
        QString m_name;
        QString m_operator;
        QString m_dataPointType;
        int m_start;
        int m_step;
        QString m_value;
        explicit IfNode(bool isTrue) : AbstractAtom(isTrue ? "dgm:if" : "dgm:else"), m_count(0), m_start(0), m_step(0), m_isTrue(isTrue) {}
        virtual ~IfNode() {}
        virtual void dump(int level) {
            DEBUG_DUMP;
            //DEBUG_DUMP << "argument=" << m_argument << "axis=" << m_axis << "count=" << m_count << "function=" << m_function << "hideLastTrans=" << m_hideLastTrans << "name=" << m_name << "operator=" << m_operator << "dataPointType=" << m_dataPointType << "start=" << m_start << "step=" << m_step << "value=" << m_value;
            foreach(AbstractAtom* node, m_children)
               node->dump(level + 1);
        }
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(arg, m_argument)
            TRY_READ_ATTR_WITHOUT_NS_INTO(axis, m_axis)
            TRY_READ_ATTR_WITHOUT_NS(cnt)
            m_count = cnt.toInt();
            TRY_READ_ATTR_WITHOUT_NS_INTO(func, m_function)
            TRY_READ_ATTR_WITHOUT_NS_INTO(hideLastTrans, m_hideLastTrans)
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            TRY_READ_ATTR_WITHOUT_NS_INTO(op, m_operator)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_dataPointType)
            TRY_READ_ATTR_WITHOUT_NS(st)
            m_start = st.toInt();
            TRY_READ_ATTR_WITHOUT_NS(step)
            m_step = step.toInt();
            TRY_READ_ATTR_WITHOUT_NS_INTO(val, m_value)
            AbstractAtom::readAll(reader);
        }
        virtual void processAtom(Context* context) {
            if (!testAtom()) return;
            DEBUG_PROCESS << "name=" << m_name;
            processAtomChildren(context);
        }
        bool testAtom() {
            //TODO check for the condition rather then always falling into the else-condition...
            return !m_isTrue;
        }
    private:
        bool m_isTrue;
};

/// The choose element wraps if/else blocks into a choose block.
class ChooseNode : public AbstractAtom
{
    public:
        QString m_name;
        explicit ChooseNode() : AbstractAtom("dgm:choose") {}
        virtual ~ChooseNode() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "name=" << m_name;
            foreach(AbstractAtom* node, m_children)
                if (static_cast<IfNode*>(node)->testAtom())
                    node->dump(level + 1);
        }
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            AbstractAtom::readAll(reader);
        }
        virtual void readElement(MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:if")) {
                    IfNode *n = new IfNode(true);
                    addChild(n);
                    n->readAll(reader);
                } else if (reader->qualifiedName() == QLatin1String("dgm:else")) {
                    IfNode *n = new IfNode(false);
                    addChild(n);
                    n->readAll(reader);
                }
            }
        }
        virtual void processAtom(Context* context) {
            //DEBUG_PROCESS;
            processAtomChildren(context);
        }
};

/// A looping structure, similar to a for loop in a programming language, which defines what data model points will use this layout node.
class ForEachNode : public AbstractAtom
{
    public:
        QString m_axis;
        int m_count;
        QString m_hideLastTrans;
        QString m_name;
        QString m_dataPointType;
        QString m_reference;
        int m_start;
        int m_step;
        explicit ForEachNode() : AbstractAtom("dgm:forEach"), m_count(0), m_start(0), m_step(0) {}
        virtual ~ForEachNode() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "axis=" << m_axis << "count=" << m_count << "hideLastTrans=" << m_hideLastTrans << "name=" << m_name << "dataPointType=" << m_dataPointType << "reference=" << m_reference << "start=" << m_start << "step=" << m_step;
            foreach(AbstractAtom* node, m_children)
                node->dump(level + 1);
        }
        virtual void readAll(MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(axis, m_axis)
            TRY_READ_ATTR_WITHOUT_NS(cnt)
            m_count = cnt.toInt();
            TRY_READ_ATTR_WITHOUT_NS_INTO(hideLastTrans, m_hideLastTrans)
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_dataPointType)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ref, m_reference)
            TRY_READ_ATTR_WITHOUT_NS(st)
            m_start = st.toInt();
            TRY_READ_ATTR_WITHOUT_NS(step)
            m_step = step.toInt();
            AbstractAtom::readAll(reader);
        }
        virtual void processAtom(Context* context) {
            DEBUG_PROCESS;

            //TODO handle forEach-conditions
            //processAtomChildren();
            
            
            
            foreach(AbstractAtom* node, m_children)
                node->processAtom(context);
        }
};

void AbstractAtom::readElement(MsooXmlDiagramReader* reader)
{
    if (reader->isStartElement()) {
        AbstractAtom *node = 0;

        if (reader->qualifiedName() == QLatin1String("dgm:layoutNode")) {
            node = new LayoutNode;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:shape")) {
            node = new ShapeNode;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:alg")) {
            node = new AlgorithmNode;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:presOf")) {
            node = new PresentationOfNode;
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

}} // namespace MSOOXML::Diagram

/****************************************************************************************************
 * The reader-context and the reader itself. Note that there will be one reader-instance per xml-file
 * in a diagram. The reader-context is shared between the reader-instances for one diagram.
 */

using namespace MSOOXML;

MsooXmlDiagramReaderContext::MsooXmlDiagramReaderContext(KoStore* storeout)
    : MSOOXML::MsooXmlReaderContext()
    , m_storeout(storeout)
    , m_data(new Diagram::DataModel)
    , m_layout(new Diagram::LayoutNode)
{
}

MsooXmlDiagramReaderContext::~MsooXmlDiagramReaderContext()
{
    delete m_data;
    delete m_layout;
}

void MsooXmlDiagramReaderContext::saveDiagram(KoOdfWriters* odfWriters)
{
    Diagram::Context context(odfWriters, m_data);
    m_layout->processAtom(&context);
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
                    m_context->m_data->m_points->readAll(this);
                }
                else if (qualifiedName() == QLatin1String("dgm:cxnLst")) { // list of connections
                    m_context->m_data->m_connections->readAll(this);
                }
            }
        }

        QMap<QString, Diagram::PointNode*> pointMap;
        foreach(Diagram::PointNode* node, m_context->m_data->m_points->points()) {
            if (!node->m_modelId.isEmpty()) {
                pointMap[node->m_modelId] = node;
            }
        }

        QMap<QString, Diagram::PointNode*> pointTree;
        foreach(Diagram::ConnectionNode* node, m_context->m_data->m_connections->connections()) {
            if (node->m_type != "parOf") continue;

            Diagram::PointNode* source = 0;
            if (pointTree.contains(node->m_srcId)) {
                source = pointTree[node->m_srcId];
            }
            else {
                if (!pointMap.contains(node->m_srcId)) continue;
                source = pointMap[node->m_srcId];
                pointTree[node->m_srcId] = source;
            }

            if (!pointMap.contains(node->m_destId)) continue;
            Diagram::PointNode* destination = pointMap[node->m_destId];
            source->addChild(destination);
            pointTree[node->m_destId] = destination;
        }

        /*
        "Dgm::PN7MSOOXML9PointNodeE::dump" atom= "dgm:pt" modelId= "{C535E748-2B36-46E9-932A-8C23010861CB}" type= "doc" cxnId= "" 
        " Dgm::PN7MSOOXML9PointNodeE::dump" atom= "dgm:pt" modelId= "{286997D8-7E78-41EA-BE45-94B55360F355}" type= "node" cxnId= "" 
        " Dgm::PN7MSOOXML9PointNodeE::dump" atom= "dgm:pt" modelId= "{4868A8F1-5981-4D39-8831-C095082821AB}" type= "node" cxnId= "" 
        " Dgm::PN7MSOOXML9PointNodeE::dump" atom= "dgm:pt" modelId= "{301CBC57-DAD9-40EA-ADC5-368C7122AA96}" type= "node" cxnId= ""
        */
        //for(QMap<QString, PointNode*>::Iterator it = pointTree.begin(); it != pointTree.end(); ++it) (*it)->dump(0);
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
                //ELSE_TRY_READ_IF(sampData)
                //ELSE_TRY_READ_IF(styleData)
            }
        }
        
        //m_context->m_layout->dump(0);
        //saveDiagram();
        // Q_ASSERT(false);
        
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


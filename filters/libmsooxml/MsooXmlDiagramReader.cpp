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
//#include <QAbstractUriResolver>
#include <typeinfo>

namespace MSOOXML { namespace Diagram {

/****************************************************************************************************
 * The following classes where designed after the way the dmg-namespace is described in the
 * MSOOXML-specs and how it was done in oo.org.
 *
 * See also;
 * - http://wiki.services.openoffice.org/wiki/SmartArt
 * - http://msdn.microsoft.com/en-us/magazine/cc163470.aspx
 */

#define DEBUG_DUMP \
    qDebug() << QString("%1Dgm::%2::%3").arg(QString(' ').repeated(level)).arg(typeid(this).name()).arg(__FUNCTION__) << "atom=" << m_tagName
#define DEBUG_PROCESS \
    qDebug() << QString("Dgm::%1::%2").arg(typeid(this).name()).arg(__FUNCTION__) << "atom=" << m_tagName

class PointListNode;
class ConnectionListNode;
class AbstractAtom;
class LayoutAtom;
class ConstraintAtom;

/// The evaluation context that is passed around and contains all kind of state-informations.
class Context
{
    public:
        PointListNode* m_points;
        ConnectionListNode* m_connections;
        LayoutAtom* m_rootLayout;
        LayoutAtom* m_parentLayout;
        QMap<QString, LayoutAtom*> m_layoutMap;
        explicit Context();
        ~Context();
};

/****************************************************************************************************
 * It follws the classes used within the data-model to build up a tree of nodes.
 */

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
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            Q_UNUSED(context);
            Q_UNUSED(reader);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            while (!reader->atEnd()) {
                QXmlStreamReader::TokenType tokenType = reader->readNext();
                if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
                if (!reader->isStartElement() && reader->qualifiedName() == m_tagName) break;
                readElement(context, reader);
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
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            Q_UNUSED(context);
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
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(modelId, m_modelId)
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            if (m_type.isEmpty()) m_type = "node";
            if (m_type == QLatin1String("parTrans") || m_type == QLatin1String("sibTrans"))
                TRY_READ_ATTR_WITHOUT_NS_INTO(cxnId, m_cxnId)
            else
                m_cxnId.clear();
            AbstractNode::readAll(context, reader);
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
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:pt")) {
                    PointNode *n = new PointNode;
                    m_points << n;
                    n->readAll(context, reader);
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
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
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
            AbstractNode::readAll(context, reader);
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
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:cxn")) {
                    ConnectionNode *n = new ConnectionNode;
                    m_connections << n;
                    n->readAll(context, reader);
                }
            }
        }
        QList<ConnectionNode*> connections() const { return m_connections; }
    private:
        QList<ConnectionNode*> m_connections;
};

/****************************************************************************************************
 * So much for the nodes. Now the atoms are following which are used to add some logic to
 * the data-model.
 */

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
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader);
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            while (!reader->atEnd()) {
                QXmlStreamReader::TokenType tokenType = reader->readNext();
                if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
                if (!reader->isStartElement() && reader->qualifiedName() == m_tagName) break;
                readElement(context, reader);
            }
        }
        virtual void processAtom(Context* context, KoXmlWriter* xmlWriter) = 0;
        //virtual int width() const = 0;
        //virtual int height() const = 0;
        AbstractAtom* parent() const { return m_parent; }
        QList<AbstractAtom*> children() const { return m_children; }
        void addChild(AbstractAtom* node) {
            node->m_parent = this;
            m_children << node;
        }
        //QList<AbstractAtom*> relationship(Context* context, const QString &rel) const;
        QList<ConstraintAtom*> constraints() const { return m_constraints; }
        void addConstraint(ConstraintAtom* constraint) { m_constraints << constraint; }
    protected:
        AbstractAtom* m_parent;
        QList<AbstractAtom*> m_children;
        QList<ConstraintAtom*> m_constraints;
        
        void processAtomChildren(Context* context, KoXmlWriter* xmlWriter) {
            foreach(AbstractAtom* node, m_children)
                node->processAtom(context, xmlWriter);
        }
};

/// The layout node is the basic building block of diagrams. The layout node is responsible for defining how shapes are arranged in a diagram and how the data maps to a particular shape in a diagram.
class LayoutAtom : public AbstractAtom
{
    public:
        QString m_name;
        //int left, top, width, height;
        explicit LayoutAtom() : AbstractAtom("dgm:layoutNode") /*, left(0), top(0), width(0), height(0)*/ {}
        virtual ~LayoutAtom() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "name=" << m_name;
            foreach(AbstractAtom* node, m_children)
                node->dump(level + 1);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            //TRY_READ_ATTR_WITHOUT_NS_INTO(styleLbl, m_styleLbl)
            context->m_layoutMap[m_name] = this;
            AbstractAtom::readAll(context, reader);
        }
        virtual void processAtom(Context* context, KoXmlWriter* xmlWriter) {
            DEBUG_PROCESS << "name=" << m_name;
            LayoutAtom* oldLayout = context->m_parentLayout;
            context->m_parentLayout = this;
            processAtomChildren(context, xmlWriter);
            context->m_parentLayout = oldLayout;
        }
};

/// Specify size and position of nodes, text values, and layout dependencies between nodes in a layout definition.
class ConstraintAtom : public AbstractAtom
{
    public:
        /// Factor used in a reference constraint or a rule in order to modify a referenced value by the factor defined.
        double m_fact;
        /// Specifies the axis of layout nodes to apply a constraint or rule to.
        QString m_for;
        /// Specifies the name of the layout node to apply a constraint or rule to.
        QString m_forName;
        /// The operator constraint used to evaluate the condition.
        QString m_op;
        /// Specifies the type of data point to select.
        QString m_ptType;
        /// The point type used int he referenced constraint.
        QString m_refPtType;
        /// Specifies the type of a reference constraint.
        QString m_refType;
        /// The for value of the referenced constraint.
        QString m_refFor;
        /// The name of the layout node referenced by a reference constraint.
        QString m_refForName;
        /// Specifies the constraint to apply to this layout node.
        QString m_type;
        /// Specifies an absolute value instead of reference another constraint.
        QString m_val;
        explicit ConstraintAtom() : AbstractAtom("dgm:constr") {}
        virtual ~ConstraintAtom() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "fact=" << m_fact << "for=" << m_for << "forName=" << m_forName << "refType=" << m_refType << "refFor=" << m_refFor << "refForName=" << m_refForName << "type=" << m_type;
        }
        virtual void readAll(Context*, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS(fact)
            m_fact = fact.isEmpty() ? 1.0 : fact.toDouble();
            TRY_READ_ATTR_WITHOUT_NS_INTO(for, m_for)
            TRY_READ_ATTR_WITHOUT_NS_INTO(forName, m_forName)
            TRY_READ_ATTR_WITHOUT_NS_INTO(op, m_op)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_ptType)
            TRY_READ_ATTR_WITHOUT_NS_INTO(refPtType, m_refPtType)
            
            TRY_READ_ATTR_WITHOUT_NS_INTO(refType, m_refType)
            TRY_READ_ATTR_WITHOUT_NS_INTO(refFor, m_refFor)
            TRY_READ_ATTR_WITHOUT_NS_INTO(refForName, m_refForName)
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            TRY_READ_ATTR_WITHOUT_NS_INTO(val, m_val)
            //AbstractAtom::readAll(context, reader);
        }
        virtual void processAtom(Context* context, KoXmlWriter* xmlWriter) {
            DEBUG_PROCESS << "fact=" << m_fact << "for=" << m_for << "forName=" << m_forName << "refType=" << m_refType << "refFor=" << m_refFor << "refForName=" << m_refForName << "type=" << m_type;
            LayoutAtom* layout = m_forName.isEmpty() ? context->m_parentLayout : context->m_layoutMap.value(m_forName);
            //Q_ASSERT(layout);
            if(!layout) return;
// foreach(AbstractAtom* a, layout->relationship(context, m_for)) {
//     a->addConstraint(this);
// }
        }
};

/// List of constraints.
class ConstraintListAtom : public AbstractAtom
{
    public:
        explicit ConstraintListAtom() : AbstractAtom("dgm:constrLst") {}
        virtual ~ConstraintListAtom() {}
        virtual void dump(int level) {
            foreach(AbstractAtom* node, m_children)
                node->dump(level + 1);
        }
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:constr")) {
                    ConstraintAtom* node = new ConstraintAtom;
                    addChild(node);
                    node->readAll(context, reader);
                }
            }
        }
        virtual void processAtom(Context* context, KoXmlWriter* xmlWriter) {
            DEBUG_PROCESS;
            //Q_ASSERT(context->m_parentLayout);
            foreach(AbstractAtom* node, m_children)
                node->processAtom(context, xmlWriter);
        }
};

class ShapeAtom : public AbstractAtom
{
    public:
        QString m_type;
        QString m_blip;
        explicit ShapeAtom() : AbstractAtom("dgm:shape") {}
        virtual ~ShapeAtom() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "type=" << m_type << "blip=" << m_blip;
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            //if (m_type.isEmpty()) m_type = "obj";
            TRY_READ_ATTR_WITHOUT_NS_INTO(blip, m_blip)
            AbstractAtom::readAll(context, reader);
        }
        virtual void processAtom(Context* context, KoXmlWriter* xmlWriter) {
            DEBUG_PROCESS << "type=" << m_type << "blip=" << m_blip;

            Q_ASSERT(context->m_parentLayout);
            
            if (m_type == QLatin1String("ellipse")) {
static int iii=0;
//if(++iii >= 2) return;

kDebug() << "###############################>>>>>>>>>>>>>>>>>>>>>" << context->m_parentLayout->m_name;


                xmlWriter->startElement("draw:custom-shape");
                xmlWriter->addAttribute("draw:layer", "layout");
                if (!context->m_parentLayout->m_name.isEmpty())
                    xmlWriter->addAttribute("draw:name", context->m_parentLayout->m_name);
                //xmlWriter->addAttribute("draw:style-name", "gr1");
                //xmlWriter->addAttribute("draw:text-style-name", "P2");
                xmlWriter->addAttribute("svg:x", QString("%1cm").arg(iii*3));//"10.358cm");
                xmlWriter->addAttribute("svg:y", QString("%1cm").arg(iii*3));//"6.606cm");
                /*if(m_width > 0) */xmlWriter->addAttribute("svg:width", "4.684cm");
                /*if(m_height > 0) */xmlWriter->addAttribute("svg:height", "4.683cm");
                
                xmlWriter->startElement("text:p");
                xmlWriter->endElement();
                
                xmlWriter->startElement("draw:enhanced-geometry");
                xmlWriter->addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N");
                xmlWriter->addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
                //xmlWriter->addAttribute("draw:text-areas", "3163 3163 18437 18437");
                xmlWriter->addAttribute("draw:type", "ellipse");
                xmlWriter->addAttribute("svg:viewBox", "0 0 21600 21600");
                xmlWriter->endElement();

                xmlWriter->endElement(); // draw:custom-shape
++iii;
            }

        }
};

/// The algorithm used by the containing layout node. The algorithm defines the behavior of the layout node along with the behavior and layout of the nested layout nodes.
class AlgorithmAtom : public AbstractAtom
{
    public:
        QString m_type; // composite, conn, cycle, hierChild, hierRoot, lin, pyra, snake, sp, tx
        explicit AlgorithmAtom() : AbstractAtom("dgm:alg") {}
        virtual ~AlgorithmAtom() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "type=" << m_type;
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            AbstractAtom::readAll(context, reader);
        }
        virtual void processAtom(Context*, KoXmlWriter*) {
            //DEBUG_PROCESS;
            //TODO
        }
};

/// This element specifies a particular data model point which is to be mapped to the containing layout node.
class PresentationOfAtom : public AbstractAtom
{
    public:
        QString m_axis;
        int m_count;
        QString m_hideLastTrans;
        QString m_dataPointType;
        int m_start;
        int m_step;
        explicit PresentationOfAtom() : AbstractAtom("dgm:presOf"), m_count(0), m_start(0), m_step(0) {}
        virtual ~PresentationOfAtom() {}
        virtual void dump(int level) {
            DEBUG_DUMP;
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
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
            AbstractAtom::readAll(context, reader);
        }
        virtual void processAtom(Context* context, KoXmlWriter* xmlWriter) {
            //DEBUG_PROCESS;
#if 0

            for(AbstractAtom* node = m_parent; node; node = node->parent()) {
                if (LayoutAtom* layoutnode = dynamic_cast<LayoutAtom*>(node)) {

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

/// The if element represents a condition that applies to all it's children.
class IfAtom : public AbstractAtom
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
        explicit IfAtom(bool isTrue) : AbstractAtom(isTrue ? "dgm:if" : "dgm:else"), m_count(0), m_start(0), m_step(0), m_isTrue(isTrue) {}
        virtual ~IfAtom() {}
        virtual void dump(int level) {
            DEBUG_DUMP;
            //DEBUG_DUMP << "argument=" << m_argument << "axis=" << m_axis << "count=" << m_count << "function=" << m_function << "hideLastTrans=" << m_hideLastTrans << "name=" << m_name << "operator=" << m_operator << "dataPointType=" << m_dataPointType << "start=" << m_start << "step=" << m_step << "value=" << m_value;
            foreach(AbstractAtom* node, m_children)
               node->dump(level + 1);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
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
            AbstractAtom::readAll(context, reader);
        }
        virtual void processAtom(Context* context, KoXmlWriter* xmlWriter) {
            if (!testAtom()) return;
            DEBUG_PROCESS << "name=" << m_name;
            processAtomChildren(context, xmlWriter);
        }
        bool testAtom() {
            //TODO check for the condition rather then always falling into the else-condition...
            return !m_isTrue;
        }
    private:
        bool m_isTrue;
};

/// The choose element wraps if/else blocks into a choose block.
class ChooseAtom : public AbstractAtom
{
    public:
        QString m_name;
        explicit ChooseAtom() : AbstractAtom("dgm:choose") {}
        virtual ~ChooseAtom() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "name=" << m_name;
            foreach(AbstractAtom* node, m_children)
                if (static_cast<IfAtom*>(node)->testAtom())
                    node->dump(level + 1);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            AbstractAtom::readAll(context, reader);
        }
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:if")) {
                    IfAtom *n = new IfAtom(true);
                    addChild(n);
                    n->readAll(context, reader);
                } else if (reader->qualifiedName() == QLatin1String("dgm:else")) {
                    IfAtom *n = new IfAtom(false);
                    addChild(n);
                    n->readAll(context, reader);
                }
            }
        }
        virtual void processAtom(Context* context, KoXmlWriter* xmlWriter) {
            //DEBUG_PROCESS;
            processAtomChildren(context, xmlWriter);
        }
};

/// A looping structure, similar to a for loop in a programming language, which defines what data model points will use this layout node.
class ForEachAtom : public AbstractAtom
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
        explicit ForEachAtom() : AbstractAtom("dgm:forEach"), m_count(0), m_start(0), m_step(0) {}
        virtual ~ForEachAtom() {}
        virtual void dump(int level) {
            DEBUG_DUMP << "axis=" << m_axis << "count=" << m_count << "hideLastTrans=" << m_hideLastTrans << "name=" << m_name << "dataPointType=" << m_dataPointType << "reference=" << m_reference << "start=" << m_start << "step=" << m_step;
            foreach(AbstractAtom* node, m_children)
                node->dump(level + 1);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
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
            AbstractAtom::readAll(context, reader);
        }
        virtual void processAtom(Context* context, KoXmlWriter* xmlWriter) {
            DEBUG_PROCESS;

            //TODO handle forEach-conditions
            foreach(AbstractAtom* node, m_children)
                node->processAtom(context, xmlWriter);
        }
};

Context::Context()
    : m_points(new PointListNode)
    , m_connections(new ConnectionListNode)
    , m_rootLayout(new Diagram::LayoutAtom)
    , m_parentLayout(m_rootLayout)
{
}

Context::~Context()
{
    delete m_points;
    delete m_connections;
    delete m_rootLayout;
}
        
void AbstractAtom::readElement(Context* context, MsooXmlDiagramReader* reader)
{
    if (reader->isStartElement()) {
        AbstractAtom *node = 0;

        if (reader->qualifiedName() == QLatin1String("dgm:layoutNode")) {
            node = new LayoutAtom;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:shape")) {
            node = new ShapeAtom;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:alg")) {
            node = new AlgorithmAtom;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:presOf")) {
            node = new PresentationOfAtom;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:choose")) {
            node = new ChooseAtom;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:forEach")) {
            node = new ForEachAtom;
        }
        else if (reader->qualifiedName() == QLatin1String("dgm:constrLst")) {
            node = new ConstraintListAtom;
        }
        
        if (node) {
            addChild(node);
            node->readAll(context, reader);
        }
    }
}

/*
/// ST_ConstraintRelationship
QList<AbstractAtom*> AbstractAtom::relationship(Context* context, const QString &rel) const
{
    QList<AbstractAtom*> result;
    //TODO implement all cases
    foreach(const QString& r, rel.split(' ', QString::SkipEmptyParts)) {
        //if(r.contains("ancst")) // Ancestor
        //if(r.contains("ancstOrSelf")) // Ancestor Or Self
        if(r.contains("ch")) result += m_children; // Child
        //if(r.contains("des")) // Descendant
        //if(r.contains("desOrSelf")) // Descendant Or Self
        //if(r.contains("follow")) // Follow
        //if(r.contains("followSib")) // Follow Sibling
        if(r.contains("par")) if(m_parent) result += m_parent; // Parent
        //if(r.contains("preced")) // Preceding
        //if(r.contains("precedSib")) // Preceding Sibling
        if(r.contains("root")) result += context->m_rootLayout; // Root
        if(r.contains("self")) result += const_cast<AbstractAtom*>(this); // Self
    }
    return result;
}
*/

}} // namespace MSOOXML::Diagram

/****************************************************************************************************
 * The reader-context and the reader itself. Note that there will be one reader-instance per xml-file
 * in a diagram. The reader-context is shared between the reader-instances for one diagram.
 */

using namespace MSOOXML;

MsooXmlDiagramReaderContext::MsooXmlDiagramReaderContext()
    : MSOOXML::MsooXmlReaderContext()
    , m_context(new Diagram::Context)
{
}

MsooXmlDiagramReaderContext::~MsooXmlDiagramReaderContext()
{
    delete m_context;
}

void MsooXmlDiagramReaderContext::saveIndex(KoXmlWriter* xmlWriter)
{
    m_context->m_rootLayout->processAtom(m_context, xmlWriter);
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
                    m_context->m_context->m_points->readAll(m_context->m_context, this);
                }
                else if (qualifiedName() == QLatin1String("dgm:cxnLst")) { // list of connections
                    m_context->m_context->m_connections->readAll(m_context->m_context, this);
                }
            }
        }

        QMap<QString, Diagram::PointNode*> pointMap;
        foreach(Diagram::PointNode* node, m_context->m_context->m_points->points()) {
            if (!node->m_modelId.isEmpty()) {
                pointMap[node->m_modelId] = node;
            }
        }

        QMap<QString, Diagram::PointNode*> pointTree;
        foreach(Diagram::ConnectionNode* node, m_context->m_context->m_connections->connections()) {
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
                    m_context->m_context->m_rootLayout->readAll(m_context->m_context, this);
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


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
    qDebug() << QString("%1Dgm::%2::%3").arg(QString(' ').repeated(level)).arg(typeid(this).name()).arg(__FUNCTION__) << this << "atom=" << m_tagName
#define DEBUG_WRITE \
    qDebug() << QString("Dgm::%1::%2").arg(typeid(this).name()).arg(__FUNCTION__) << "atom=" << m_tagName

class AbstractNode;
class PointNode;
class PointListNode;
class ConnectionListNode;
class AbstractAtom;
class LayoutNodeAtom;
class ConstraintAtom;
class AlgorithmAtom;

/// The evaluation context that is passed around and contains all kind of state-informations.
class Context
{
    public:
        PointListNode* m_rootPoint;
        ConnectionListNode* m_connections;
        LayoutNodeAtom* m_rootLayout;
        LayoutNodeAtom* m_parentLayout;
        QMap<QString, LayoutNodeAtom*> m_layoutMap;
        AbstractNode* m_currentNode; // the moving context node
        explicit Context();
        ~Context();
};

/****************************************************************************************************
 * It follws the classes used within the data-model to build up a tree of data-nodes.
 */

/// The AbstractNode is the base class to handle the diagram data-model (content of data1.xml).
class AbstractNode
{
    public:
        const QString m_tagName;
        explicit AbstractNode(const QString &tagName) : m_tagName(tagName), m_parent(0) {}
        virtual ~AbstractNode() { qDeleteAll(m_children); }
        virtual void dump(Context* context, int level) {
            foreach(AbstractNode* node, m_children)
                node->dump(context, level + 1);
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
        AbstractNode* parent() const { return m_parent; } 
        QList<AbstractNode*> children() const { return m_children; }
        void addChild(AbstractNode* node) {
            Q_ASSERT(!node->m_parent);
            node->m_parent = this;
            m_children.append(node);
        }
        void removeChild(AbstractNode* node) {
            Q_ASSERT(node->m_parent == this);
            node->m_parent = 0;
            m_children.removeAll(node);
        }
        QList<AbstractNode*> descendant() const {
            QList<AbstractNode*> list = m_children;
            foreach(AbstractNode* node, m_children)
                foreach(AbstractNode* n, node->descendant())
                    list.append(n);
            return list;
        }
        QList<AbstractNode*> peers() const {
            QList<AbstractNode*> list;
            if (m_parent)
                foreach(AbstractNode* node, m_parent->m_children)
                    if(node != this)
                        list.append(node);
            return list;
        }                
    protected:
        AbstractNode* m_parent;
        QList<AbstractNode*> m_children;
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
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "modelId=" << m_modelId << "type=" << m_type << "cxnId=" << m_cxnId;
            AbstractNode::dump(context, level);
        }
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            Q_UNUSED(context);
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:prSet")) {
                    //TODO
                } else if (reader->qualifiedName() == QLatin1String("dgm:spPr")) {
                    //TODO
                } else if (reader->qualifiedName() == QLatin1String("dgm:t")) {
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
};

/// A list of points in the data-model.
class PointListNode : public AbstractNode
{
    public:
        explicit PointListNode() : AbstractNode("dgm:ptLst") {}
        virtual ~PointListNode() {}
        virtual void dump(Context* context, int level) {
            //DEBUG_DUMP;
            AbstractNode::dump(context, level);
        }
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:pt")) {
                    PointNode *n = new PointNode;
                    addChild(n);
                    n->readAll(context, reader);
                }
            }
        }
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
        virtual void dump(Context* context, int level) {
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
        virtual ~ConnectionListNode() {}
        virtual void dump(Context* context, int level) {
            //DEBUG_DUMP;
            AbstractNode::dump(context, level);
        }
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:cxn")) {
                    ConnectionNode *n = new ConnectionNode;
                    addChild(n);
                    n->readAll(context, reader);
                }
            }
        }
};

/****************************************************************************************************
 * So much for the nodes. Now the atoms are following which are used to add some logic to the
 * data-model and they do provide the functionality to build up a hierarchical layout tree.
 */

/// Base class for layout-operations (content of layout1.xml)
class AbstractAtom : public AbstractNode
{
    public:
        explicit AbstractAtom(const QString &tagName) : AbstractNode(tagName) {}
        virtual ~AbstractAtom() {}
        virtual void dump(Context* context, int level) {
            AbstractNode::dump(context, level);
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
        virtual void layoutAtom(Context* context) {
            foreach(AbstractNode* node, m_children)
                if(AbstractAtom* atom = dynamic_cast<AbstractAtom*>(node))
                    atom->layoutAtom(context);
        }
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter) {
            foreach(AbstractNode* node, m_children)
                if(AbstractAtom* atom = dynamic_cast<AbstractAtom*>(node))
                    atom->writeAtom(context, xmlWriter);
        }
        //virtual int width() const = 0;
        //virtual int height() const = 0;
        //QList<AbstractAtom*> relationship(Context* context, const QString &rel) const;
        //QList<ConstraintAtom*> constraints() const { return m_constraints; }
        //void addConstraint(ConstraintAtom* constraint) { m_constraints << constraint; }

    protected:
        QList<AbstractNode*> fetchAxis(Context* context, const QString& axis) {
            QList<AbstractNode*> list;
            if(axis == QLatin1String("ancst")) { // Ancestor
               for(AbstractNode* n = context->m_currentNode; n; n = n->parent())
                   list.append(n);
            } else if(axis == QLatin1String("ancstOrSelf")) { // Ancestor Or Self
               for(AbstractNode* n = context->m_currentNode; n; n = n->parent())
                   list.append(n);
               list.append(context->m_currentNode);
            } else if(axis == QLatin1String("ch")) { // Child
               list = context->m_currentNode->children();
            } else if(axis == QLatin1String("des")) { // Descendant
               list = context->m_currentNode->descendant();
            } else if(axis == QLatin1String("desOrSelf")) { // Descendant Or Self
               list = context->m_currentNode->descendant();
               list.append(context->m_currentNode);
            } else if(axis == QLatin1String("follow")) { // Follow
                foreach(AbstractNode* peer, context->m_currentNode->peers()) {
                    list.append(peer);
                    foreach(AbstractNode* n, peer->descendant())
                        list.append(n);
                }
            } else if(axis == QLatin1String("followSib")) { // Follow Sibling
                list = context->m_currentNode->peers();
            } else if(axis == QLatin1String("par")) { // Parent
               if (context->m_currentNode->parent()) list.append(context->m_currentNode->parent());
            } else if(axis == QLatin1String("preced")) { // Preceding
                //TODO
            } else if(axis == QLatin1String("precedSib")) { // Preceding Sibling
                //TODO
            } else if(axis == QLatin1String("root")) { // Root
                list.append(context->m_rootPoint);
            } else if(axis == QLatin1String("self")) { // Self
                list.append(context->m_currentNode);
            }
            return list;
        }
        
        QList<AbstractNode*> filterAxis(const QList<AbstractNode*> &list, const QString &ptType) const {
            QList<AbstractNode*> result;
            foreach(AbstractNode* node, list)
                if(PointNode* pt = dynamic_cast<PointNode*>(node))
                    if(ptType == pt->m_type || ptType == "all" || (ptType == "nonAsst" && pt->m_type != "asst" ) || (ptType == "nonNorm" && pt->m_type != "norm"))
                        result.append(pt);
            return result;
        }
        
        QList<AbstractNode*> foreachAxis(const QList<AbstractNode*> &list, int start, int count, int step) const {
            QList<AbstractNode*> result;
            const int _start = qMax(0, start - 1);
            const int _step = qMax(1, step);
            const int _count = qMin(list.count(), (count + _start) * _step);
            for(int i = _start; i < _count; i += _step) {
                result.append(list[i]);
            }
            return result;
        }
        
};

/// The layout node is the basic building block of diagrams. The layout node is responsible for defining how shapes are arranged in a diagram and how the data maps to a particular shape in a diagram.
class LayoutNodeAtom : public AbstractAtom
{
    public:
        QString m_name;
        int m_x, m_y, m_width, m_height;
        explicit LayoutNodeAtom() : AbstractAtom("dgm:layoutNode"), m_x(0), m_y(0), m_width(-1), m_height(-1), m_algorithm(0) {}
        virtual ~LayoutNodeAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "name=" << m_name << "constraintsCount=" << m_constraints.count();
            AbstractAtom::dump(context, level);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            //TRY_READ_ATTR_WITHOUT_NS_INTO(styleLbl, m_styleLbl)
            context->m_layoutMap[m_name] = this;
            LayoutNodeAtom* oldLayout = context->m_parentLayout;
            context->m_parentLayout = this;
            AbstractAtom::readAll(context, reader);
            context->m_parentLayout = oldLayout;
        }
        virtual void layoutAtom(Context* context);
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter) {
            DEBUG_WRITE << "name=" << m_name;
            LayoutNodeAtom* oldLayout = context->m_parentLayout;
            context->m_parentLayout = this;
            AbstractAtom::writeAtom(context, xmlWriter);
            context->m_parentLayout = oldLayout;
        }
        QList<ConstraintAtom*> constraints() const { return m_constraints; }
        void addConstraint(ConstraintAtom* constraint) { m_constraints << constraint; }
        void setAlgorithm(AlgorithmAtom* algorithm) { m_algorithm = algorithm; }
        QList<AbstractNode*> axis() const { return m_axis; }
        void setAxis(Context* context, const QString& axis, const QString& ptType) { m_axis = filterAxis(fetchAxis(context, axis), ptType); }
    private:
        QList<ConstraintAtom*> m_constraints;
        AlgorithmAtom* m_algorithm;
        QList<AbstractNode*> m_axis;
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
        virtual void dump(Context* context, int level) {
            QString s = QString("fact=%1 ").arg(m_fact);
            if(!m_for.isEmpty()) s += QString("for=%1 ").arg(m_for);
            if(!m_forName.isEmpty()) s += QString("forName=%1 ").arg(m_forName);
            if(!m_op.isEmpty()) s += QString("op=%1 ").arg(m_op);
            if(!m_ptType.isEmpty()) s += QString("ptType=%1 ").arg(m_ptType);
            if(!m_refPtType.isEmpty()) s += QString("refPtType=%1 ").arg(m_refPtType);
            if(!m_refType.isEmpty()) s += QString("refType=%1 ").arg(m_refType);
            if(!m_refFor.isEmpty()) s += QString("refFor=%1 ").arg(m_refFor);
            if(!m_refForName.isEmpty()) s += QString("refForName=%1 ").arg(m_refForName);
            if(!m_type.isEmpty()) s += QString("type=%1 ").arg(m_type);
            if(!m_val.isEmpty()) s += QString("val=%1 ").arg(m_val);
            DEBUG_DUMP << s;
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
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
        virtual void layoutAtom(Context* context) {
            LayoutNodeAtom* layout = m_forName.isEmpty() ? context->m_parentLayout : context->m_layoutMap.value(m_forName);
            if(layout) layout->addConstraint(this);
        }
};

/// List of constraints.
class ConstraintListAtom : public AbstractAtom
{
    public:
        explicit ConstraintListAtom() : AbstractAtom("dgm:constrLst") {}
        virtual ~ConstraintListAtom() {}
        virtual void dump(Context* context, int level) {
            AbstractAtom::dump(context, level);
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
};

/// The shape displayed by the containing layout node. Not all layout nodes display shapes.
class ShapeAtom : public AbstractAtom
{
    public:
        QString m_type;
        QString m_blip;
        bool m_hideGeom;
        int m_x, m_y, m_width, m_height;
        explicit ShapeAtom() : AbstractAtom("dgm:shape"), m_hideGeom(false), m_x(0), m_y(0), m_width(-1), m_height(-1) {}
        virtual ~ShapeAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "type=" << m_type << "blip=" << m_blip;
            AbstractAtom::dump(context, level);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            //if (m_type.isEmpty()) m_type = "obj";
            TRY_READ_ATTR_WITHOUT_NS_INTO(blip, m_blip)
            TRY_READ_ATTR_WITHOUT_NS(hideGeom)
            m_hideGeom = hideGeom.toInt();
            AbstractAtom::readAll(context, reader);
        }
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter) {
            DEBUG_WRITE << "type=" << m_type << "blip=" << m_blip;
            Q_ASSERT(context->m_parentLayout);
            
            if (m_type == QLatin1String("ellipse")) {
static int iii=-1;
++iii;
                xmlWriter->startElement("draw:custom-shape");
                xmlWriter->addAttribute("draw:layer", "layout");
                if (!context->m_parentLayout->m_name.isEmpty())
                    xmlWriter->addAttribute("draw:name", context->m_parentLayout->m_name);
                //xmlWriter->addAttribute("draw:style-name", "gr1");
                //xmlWriter->addAttribute("draw:text-style-name", "P2");
                xmlWriter->addAttribute("svg:x", QString("%1cm").arg(iii));//"10.358cm");
                xmlWriter->addAttribute("svg:y", QString("%1cm").arg(iii));//"6.606cm");
                /*if(m_width >= 0)*/ xmlWriter->addAttribute("svg:width", "1.684cm");//"4.684cm");
                /*if(m_height >= 0)*/ xmlWriter->addAttribute("svg:height", "1.683cm");//"4.683cm");
                
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
            } else if (m_type == QLatin1String("rect")) {
                //TODO
            }

        }
};

/// The algorithm used by the containing layout node. The algorithm defines the behavior of the layout node along with the behavior and layout of the nested layout nodes.
class AlgorithmAtom : public AbstractAtom
{
    public:
        QString m_type; // composite, conn, cycle, hierChild, hierRoot, lin, pyra, snake, sp, tx
        QList< QPair<QString,QString> > m_params; // list of type=value parameters that modify the default behavior of the algorithm.
        explicit AlgorithmAtom() : AbstractAtom("dgm:alg") {}
        virtual ~AlgorithmAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "type=" << m_type;
            AbstractAtom::dump(context, level);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
            context->m_parentLayout->setAlgorithm(this);
            AbstractAtom::readAll(context, reader);
        }
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            Q_UNUSED(context);
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:param")) {
                    const QXmlStreamAttributes attrs(reader->attributes());
                    TRY_READ_ATTR_WITHOUT_NS(type)
                    TRY_READ_ATTR_WITHOUT_NS(val)
                    m_params << QPair<QString,QString>(type, val);
                }
            }
        }
};

/// This element specifies a particular data model point which is to be mapped to the containing layout node.
class PresentationOfAtom : public AbstractAtom
{
    public:
        QString m_axis; // This determines how to navigate through the data model, setting the context node as it moves. 
        QString m_ptType; // dataPointType
        int m_count;
        QString m_hideLastTrans;
        int m_start;
        int m_step;
        explicit PresentationOfAtom() : AbstractAtom("dgm:presOf"), m_count(0), m_start(0), m_step(0) {}
        virtual ~PresentationOfAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "axis=" << m_axis << "ptType=" << m_ptType << "count=" << m_count << "start=" << m_start << "step=" << m_step << "hideLastTrans=" << m_hideLastTrans;
            AbstractAtom::dump(context, level);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(axis, m_axis)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_ptType)
            TRY_READ_ATTR_WITHOUT_NS(cnt)
            m_count = cnt.toInt();
            TRY_READ_ATTR_WITHOUT_NS_INTO(hideLastTrans, m_hideLastTrans)
            TRY_READ_ATTR_WITHOUT_NS(st)
            m_start = st.toInt();
            TRY_READ_ATTR_WITHOUT_NS(step)
            m_step = step.toInt();
            context->m_parentLayout->setAxis(context, m_axis, m_ptType);
            AbstractAtom::readAll(context, reader);
        }
        //virtual void layoutAtom(Context* context) {
        //    AbstractAtom::layoutAtom(context);
        //}
};

/// The if element represents a condition that applies to all it's children.
class IfAtom : public AbstractAtom
{
    public:
        QString m_argument;
        QString m_axis;
        QString m_function;
        QString m_hideLastTrans;
        QString m_name;
        QString m_operator;
        QString m_ptType;
        int m_start;
        int m_step;
        int m_count;
        QString m_value;
        explicit IfAtom(bool isTrue) : AbstractAtom(isTrue ? "dgm:if" : "dgm:else"), m_start(0), m_step(0), m_count(0), m_isTrue(isTrue) {}
        virtual ~IfAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP;
            //DEBUG_DUMP << "argument=" << m_argument << "axis=" << m_axis << "count=" << m_count << "function=" << m_function << "hideLastTrans=" << m_hideLastTrans << "name=" << m_name << "operator=" << m_operator << "dataPointType=" << m_ptType << "start=" << m_start << "step=" << m_step << "value=" << m_value;
            AbstractAtom::dump(context, level);
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
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_ptType)
            TRY_READ_ATTR_WITHOUT_NS(st)
            m_start = st.toInt();
            TRY_READ_ATTR_WITHOUT_NS(step)
            m_step = step.toInt();
            TRY_READ_ATTR_WITHOUT_NS_INTO(val, m_value)
            AbstractAtom::readAll(context, reader);
        }
        bool testAtom(Context* context) {
            //TODO handle m_argument=="var"
            QList<AbstractNode*> axis = foreachAxis(filterAxis(fetchAxis(context, m_axis), m_ptType), m_start, m_count, m_step);
            bool istrue = false;
            if(m_function == "cnt") { // Specifies a count.
                istrue = axis.count() == m_value.toInt();
            } else if(m_function == "depth") { // Specifies the depth.
                //int depth = 0;
                //for(AbstractNode* n = context->m_currentNode; n; n = n->parent(), ++depth);
                //istrue = depth == m_value.toInt();
                //TODO
                kWarning()<<"TODO func=depth";
            } else if(m_function == "maxDepth") { // Defines the maximum depth.
                //int depth = 0;
                //for(AbstractNode* n = context->m_currentNode; n; n = n->parent(), ++depth);
                //istrue = depth <= m_value.toInt();
                //TODO
                kWarning()<<"TODO func=maxDepth";
            } else if(m_function == "pos") { // Retrieves the position of the node in the specified set of nodes.
                //int index = axis.indexOf(context->m_currentNode)+1;
                //istrue = index == m_value.toInt();
                //TODO
                kWarning()<<"TODO func=pos";
            } else if(m_function == "posEven") { // Returns 1 if the specified node is at an even numbered position in the data model.
                //int index = axis.indexOf(context->m_currentNode)+1;
                //istrue = index>=1 ? index % 2 == 0 : false;
                //TODO
                kWarning()<<"TODO func=posEven";
            } else if(m_function == "posOdd") { // Returns 1 if the specified node is in an odd position in the data model.
                //int index = axis.indexOf(context->m_currentNode)+1;
                //istrue = index>=1 ? index % 2 != 0 : false;
                //TODO
                kWarning()<<"TODO func=posOdd";
            } else if(m_function == "revPos") { // Reverse position function.
                //int index = axis.indexOf(context->m_currentNode)+1;
                //istrue = axis.count()-index == m_value.toInt();
                //TODO
                kWarning()<<"TODO func=revPos";
            } else if(m_function == "var") { // Used to reference a variable.
                //TODO
                kWarning()<<"TODO func=var";
            }
            return istrue || !m_isTrue;
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
        virtual void dump(Context* context, int level) {
            //DEBUG_DUMP << "name=" << m_name;
            foreach(AbstractNode* node, m_children)
                if(IfAtom* atom = dynamic_cast<IfAtom*>(node))
                    if(atom->testAtom(context))
                        atom->dump(context, level);
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
};

/// A looping structure, similar to a for loop in a programming language, which defines what data model points will use this layout node.
class ForEachAtom : public AbstractAtom
{
    public:
        QString m_axis;
        QString m_hideLastTrans;
        QString m_name;
        QString m_ptType;
        QString m_reference;
        int m_start;
        int m_step;
        int m_count;
        explicit ForEachAtom() : AbstractAtom("dgm:forEach"), m_start(0), m_step(0), m_count(0) {}
        virtual ~ForEachAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "axis=" << m_axis << "count=" << m_count << "hideLastTrans=" << m_hideLastTrans << "name=" << m_name << "ptType=" << m_ptType << "reference=" << m_reference << "start=" << m_start << "step=" << m_step;
            foreach(AbstractNode* node, m_children)
                node->dump(context, level + 1);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(axis, m_axis)
            TRY_READ_ATTR_WITHOUT_NS(cnt)
            m_count = cnt.toInt();
            TRY_READ_ATTR_WITHOUT_NS_INTO(hideLastTrans, m_hideLastTrans)
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_ptType)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ref, m_reference)
            TRY_READ_ATTR_WITHOUT_NS(st)
            m_start = st.toInt();
            TRY_READ_ATTR_WITHOUT_NS(step)
            m_step = step.toInt();
            AbstractAtom::readAll(context, reader);
        }
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter) {
            DEBUG_WRITE;
            QList<AbstractNode*> axis = foreachAxis(filterAxis(fetchAxis(context, m_axis), m_ptType), m_start, m_count, m_step);
            foreach(AbstractNode* pt, axis) {
                context->m_currentNode = pt;
                foreach(AbstractNode* node, m_children)
                    if(AbstractAtom* atom = dynamic_cast<AbstractAtom*>(node))
                        atom->writeAtom(context, xmlWriter);
            }
        }
};

Context::Context()
    : m_rootPoint(new PointListNode())
    , m_connections(new ConnectionListNode)
    , m_rootLayout(new Diagram::LayoutNodeAtom)
    , m_parentLayout(m_rootLayout)
    , m_currentNode(m_rootPoint)
{
}

Context::~Context()
{
    delete m_rootPoint;
    delete m_connections;
    delete m_rootLayout;
}
        
void AbstractAtom::readElement(Context* context, MsooXmlDiagramReader* reader)
{
    if (reader->isStartElement()) {
        AbstractAtom *node = 0;

        if (reader->qualifiedName() == QLatin1String("dgm:layoutNode")) {
            node = new LayoutNodeAtom;
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

void LayoutNodeAtom::layoutAtom(Context* context)
{
    LayoutNodeAtom* oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;

    QString algType;
    QList< QPair<QString,QString> > params;
    if(m_algorithm) {
        algType = m_algorithm->m_type;
        params = m_algorithm->m_params;
    }

    if(algType == QLatin1String("composite")) {
        // The composite algorithm specifies the size and position for all child layout nodes. You can use it to create
        // graphics with a predetermined layout or in combination with other algorithms to create more complex shapes.
        //TODO
    } else if(algType == QLatin1String("conn")) {
        // The connector algorithm lays out and routes connecting lines, arrows, and shapes between layout nodes.
        //TODO
    } else if(algType == QLatin1String("cycle")) {
        // The cycle algorithm lays out child layout nodes around a circle or portion of a circle using equal angle spacing.
        //TODO
    } else if(algType == QLatin1String("hierChild")) {
        // The hierarchy child algorithm works with the hierRoot algorithm to create hierarchical tree layouts. This
        // algorithm aligns and positions its child layout nodes in a linear path under the hierRoot layout node.
        //TODO
    } else if(algType == QLatin1String("hierRoot")) {
        // The hierarchy root algorithm works with the hierChild
        // algorithm to create hierarchical tree layouts. The
        // hierRoot algorithm aligns and positions the hierRoot
        // layout node in relation to the hierChild layout nodes.
        //TODO
    } else if(algType == QLatin1String("lin")) {
        // The linear algorithm lays out child layout nodes along a linear path.
        //TODO
    } else if(algType == QLatin1String("pyra")) {
        // The pyramid algorithm lays out child layout nodes along a vertical path and works with the trapezoid
        // shape to create a pyramid.
        //TODO
    } else if(algType == QLatin1String("snake")) {
        // The snake algorithm lays out child layout nodes along a linear path in two dimensions, allowing the linear
        // flow to continue across multiple rows or columns.
        //TODO
    } else if(algType == QLatin1String("sp")) {
        // The space algorithm is used to specify a minimum space between other layout nodes or as an indication
        // to do nothing with the layout node’s size and position.
        m_x = m_y = 0;
        m_width = m_height = -1;
    } else if(algType == QLatin1String("tx")) {
        // The text algorithm sizes text to fit inside a shape and controls its margins and alignment.
        //TODO
    } else {
        //TODO
    }
    
    kDebug() << "################# name=" << m_name << "algType=" << algType << "constraintCount=" << m_constraints.count();
    //this->dump(context, 2);
    //kDebug() << "####/CONSTRAINTS-START";
    //foreach(ConstraintAtom* c, m_constraints) c->dump(context, 2);
    //kDebug() << "####/CONSTRAINTS-END";

    foreach(AbstractNode* node, m_children)
        if(AbstractAtom* atom = dynamic_cast<AbstractAtom*>(node))
            atom->layoutAtom(context);
    
    //Q_ASSERT(m_name!="circ1");
    context->m_parentLayout = oldLayout;
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
    m_context->m_rootLayout->writeAtom(m_context, xmlWriter);
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
                    m_context->m_context->m_rootPoint->readAll(m_context->m_context, this);
                } else if (qualifiedName() == QLatin1String("dgm:cxnLst")) { // list of connections
                    m_context->m_context->m_connections->readAll(m_context->m_context, this);
                }
            }
        }

        QMap<QString, Diagram::PointNode*> pointMap;
        foreach(Diagram::AbstractNode* node, m_context->m_context->m_rootPoint->children()) {
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
                m_context->m_context->m_rootPoint->removeChild(destination);
                source->addChild(destination);
                pointTree[connection->m_destId] = destination;
            }
        }

        //for(QMap<QString, Diagram::PointNode*>::Iterator it = pointTree.begin(); it != pointTree.end(); ++it) (*it)->dump(0);
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

        //m_context->m_context->m_rootPoint->dump(0);
        //m_context->m_context->m_layout->dump(0);
        //kDebug()<<"...............................................................................";
        m_context->m_context->m_rootLayout->layoutAtom(m_context->m_context);
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


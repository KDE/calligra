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
        AbstractNode* m_rootPoint;
        ConnectionListNode* m_connections;
        LayoutNodeAtom* m_rootLayout;
        LayoutNodeAtom* m_parentLayout;
        QMap<QString, LayoutNodeAtom*> m_layoutMap;
        explicit Context();
        ~Context();
        AbstractNode* currentNode() const { return m_currentNode; }
        void setCurrentNode(AbstractNode* node);
    private:
        AbstractNode* m_currentNode; // the moving context node
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
        virtual void readElement(Context*, MsooXmlDiagramReader*) {
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
        virtual void readElement(Context*, MsooXmlDiagramReader* reader) {
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
        virtual void dump(Context*, int level) {
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
class AbstractAtom
{
    public:
        const QString m_tagName;
        explicit AbstractAtom(const QString &tagName) : m_tagName(tagName), m_parent(0) {}
        virtual ~AbstractAtom() { qDeleteAll(m_children); }
        virtual void dump(Context* context, int level) {
            foreach(AbstractAtom* atom, m_children)
                atom->dump(context, level + 1);
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
        virtual void readDone(Context* context) {
            foreach(AbstractAtom* atom, m_children)
                atom->readDone(context);
        }
        virtual void layoutAtom(Context* context) {
            foreach(AbstractAtom* atom, m_children)
                atom->layoutAtom(context);
        }
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
            foreach(AbstractAtom* atom, m_children)
                atom->writeAtom(context, xmlWriter, styles);
        }
        AbstractAtom* parent() const { return m_parent; } 
        QList<AbstractAtom*> children() const { return m_children; }
        template<class T> QList<T*> children(bool recursivly = false) const {
            QList<T*> results;
            foreach(AbstractAtom* atom, m_children)
                if(T* a = dynamic_cast<T*>(atom))
                    results.append(a);
            if(recursivly) {
                const int count = results.count();
                for(int i = 0; i < count; ++i)
                    foreach(T* a, results[i]->children<T>(recursivly))
                        results.append(a);
            }
            return results;
        }
        void addChild(AbstractAtom* node) {
            Q_ASSERT(!node->m_parent);
            node->m_parent = this;
            m_children.append(node);
        }
        void removeChild(AbstractAtom* node) {
            Q_ASSERT(node->m_parent == this);
            node->m_parent = 0;
            m_children.removeAll(node);
        }

    protected:
        AbstractAtom* m_parent;
        QList<AbstractAtom*> m_children;

        QList<AbstractNode*> fetchAxis(Context* context, const QString& _axis, const QString &_ptType, const QString& _start, const QString& _count, const QString& _step) const {
            const QStringList axisList = _axis.split(' ', QString::SkipEmptyParts);

            const QStringList typeList = _ptType.split(' ', QString::SkipEmptyParts);
            Q_ASSERT(axisList.count() <= 1 || axisList.count() == typeList.count());
            
            const QStringList startList = _start.split(' ', QString::SkipEmptyParts);
            const QStringList countList = _count.split(' ', QString::SkipEmptyParts);
            const QStringList stepList = _step.split(' ', QString::SkipEmptyParts);
            Q_ASSERT(startList.count() == countList.count() || startList.isEmpty() || countList.isEmpty());
            Q_ASSERT(countList.count() == stepList.count() || countList.isEmpty() || stepList.isEmpty());
            Q_ASSERT(startList.count() == stepList.count() || startList.isEmpty() || stepList.isEmpty());

            QList<AbstractNode*> result;
            for(int i = 0; i < axisList.count(); ++i) {
                const QString axis = axisList[i];
                QList<AbstractNode*> list;
                if(axis == QLatin1String("ancst")) { // Ancestor
                    for(AbstractNode* n = context->currentNode(); n; n = n->parent())
                        list.append(n);
                } else if(axis == QLatin1String("ancstOrSelf")) { // Ancestor Or Self
                    for(AbstractNode* n = context->currentNode(); n; n = n->parent())
                        list.append(n);
                    list.append(context->currentNode());
                } else if(axis == QLatin1String("ch")) { // Child
                    foreach(AbstractNode* n, context->currentNode()->children())
                        list.append(n);
                } else if(axis == QLatin1String("des")) { // Descendant
                    foreach(AbstractNode* n, context->currentNode()->descendant())
                        list.append(n);
                } else if(axis == QLatin1String("desOrSelf")) { // Descendant Or Self
                    foreach(AbstractNode* n, context->currentNode()->descendant())
                        list.append(n);
                    list.append(context->currentNode());
                } else if(axis == QLatin1String("follow")) { // Follow
                    foreach(AbstractNode* peer, context->currentNode()->peers()) {
                        list.append(peer);
                        foreach(AbstractNode* n, peer->descendant())
                            list.append(n);
                    }
                } else if(axis == QLatin1String("followSib")) { // Follow Sibling
                    foreach(AbstractNode* n, context->currentNode()->peers())
                        list.append(n);
                } else if(axis == QLatin1String("par")) { // Parent
                    if (context->currentNode()->parent())
                        list.append(context->currentNode()->parent());
                } else if(axis == QLatin1String("preced")) { // Preceding
                    //TODO
                } else if(axis == QLatin1String("precedSib")) { // Preceding Sibling
                    //TODO
                } else if(axis == QLatin1String("root")) { // Root
                    list.append(context->m_rootPoint);
                } else if(axis == QLatin1String("self")) { // Self
                    list.append(context->currentNode());
                }

                // optionally filter the list
                if(i < typeList.count()) {
                    QList<AbstractNode*> _list = list;
                    list.clear();
                    const QString ptType = typeList[i];
                    foreach(AbstractNode* node, _list) {
                        if(PointNode* pt = dynamic_cast<PointNode*>(node)) {
                            if(ptType == pt->m_type || ptType == "all" || (ptType == "nonAsst" && pt->m_type != "asst" ) || (ptType == "nonNorm" && pt->m_type != "norm")) {
                                list.append(pt);
                            }
                        }
                    }
                }

                // evaluate optional forEach-conditions
                if(i < startList.count() || i < countList.count() || i < stepList.count()) {
                    const int start = i < startList.count() ? startList[i].toInt() : 1;
                    const int count = i < countList.count() ? countList[i].toInt() : 0;
                    const int step =  i < stepList.count()  ? stepList[i].toInt()  : 1;
                    list = foreachAxis(context, list, start, count, step);
                }

                // transfer the resulting list to the result-list.
                foreach(AbstractNode* node, list) {
                    result.append(node);
                }
            }

            return result;
        }

    private:
        QList<AbstractNode*> foreachAxis(Context*, const QList<AbstractNode*> &list, int start, int count, int step) const {
            QList<AbstractNode*> result;
            const int _start = qMax(0, start - 1);
            const int _step = qMax(1, step);
            for(int i = _start; i < list.count(); i += _step) {
                result.append(list[i]);
                if(/*count > 0 &&*/ result.count() == count) break;
            }
            return result;
        }
};

/// The layout node is the basic building block of diagrams. The layout node is responsible for defining how shapes are arranged in a diagram and how the data maps to a particular shape in a diagram.
class LayoutNodeAtom : public AbstractAtom
{
    public:
        QString m_name;
        int m_x, m_y, m_width, m_height, m_cx, m_cy;
        qreal m_factX, m_factY, m_factWidth, m_factHeight;
        qreal m_ctrX, m_ctrY;
        explicit LayoutNodeAtom() : AbstractAtom("dgm:layoutNode"), m_x(-1), m_y(-1), m_width(-1), m_height(-1), m_cx(-1), m_cy(-1), m_factX(1.0), m_factY(1.0), m_factWidth(1.0), m_factHeight(1.0), m_ctrX(1.0), m_ctrY(1.0), m_algorithm(0), m_needsRelayout(true), m_childNeedsRelayout(true) {}
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
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles);
        QList<ConstraintAtom*> constraints() const { return m_constraints; }
        void addConstraint(ConstraintAtom* constraint) { m_constraints.append(constraint); setNeedsRelayout(true); }
        void setAlgorithm(AlgorithmAtom* algorithm) { m_algorithm = algorithm; setNeedsRelayout(true); }
        QList<AbstractNode*> axis() const { return m_axis; }
        void setAxis(const QList<AbstractNode*> &axis) { m_axis = axis; setNeedsRelayout(true); }
        void setNeedsRelayout(bool needsRelayout) {
            if(needsRelayout == m_needsRelayout) return;
            m_needsRelayout = needsRelayout;
            if(m_needsRelayout) // let parent-layouts know that we need a relayout
                for(AbstractAtom* parent = m_parent; parent; parent = parent->parent())
                    if(LayoutNodeAtom* parentLayoutAtom = dynamic_cast<LayoutNodeAtom*>(parent))
                        parentLayoutAtom->m_childNeedsRelayout = true;
        }
    private:
        QList<ConstraintAtom*> m_constraints;
        AlgorithmAtom* m_algorithm;
        QList<AbstractNode*> m_axis;
        bool m_needsRelayout, m_childNeedsRelayout;
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
        QString m_value;
        explicit ConstraintAtom() : AbstractAtom("dgm:constr") {}
        virtual ~ConstraintAtom() {}
        virtual void dump(Context*, int level) {
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
            if(!m_value.isEmpty()) s += QString("val=%1 ").arg(m_value);
            DEBUG_DUMP << s;
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
            TRY_READ_ATTR_WITHOUT_NS_INTO(val, m_value)
            //AbstractAtom::readAll(context, reader);
        }
        virtual void readDone(Context* context) {
            LayoutNodeAtom* layout = m_forName.isEmpty() ? context->m_parentLayout : context->m_layoutMap.value(m_forName);
            if(layout) layout->addConstraint(this);
            AbstractAtom::readDone(context);
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
        int m_x, m_y, m_width, m_height, m_cx, m_cy;
        qreal m_factX, m_factY, m_factWidth, m_factHeight;
        qreal m_ctrX, m_ctrY;
        explicit ShapeAtom() : AbstractAtom("dgm:shape"), m_hideGeom(false), m_x(-1), m_y(-1), m_width(-1), m_height(-1), m_cx(-1), m_cy(-1), m_factX(1.0), m_factY(1.0), m_factWidth(1.0), m_factHeight(1.0), m_ctrX(1.0), m_ctrY(1.0) {}
        virtual ~ShapeAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "type=" << m_type << "blip=" << m_blip << "x=" << m_x << "y=" << m_y << "width=" << m_width << "height=" << m_height;
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

        //TODO use filters/libmso/ODrawToOdf.h
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
            Q_ASSERT(context->m_parentLayout);
            if(m_type.isEmpty()) return;
            
            const int x = m_x * m_factX;
            const int y = m_y * m_factY;
            const int w = m_width * m_factWidth;
            const int h = m_height * m_factHeight;
            const int cx = m_cx * m_ctrX;
            const int cy = m_cy * m_ctrY;
            DEBUG_WRITE << "### type=" << m_type << "blip=" << m_blip << x+cx << y+cy << w << h;

            xmlWriter->startElement("draw:custom-shape");
            xmlWriter->addAttribute("draw:layer", "layout");
            if (!context->m_parentLayout->m_name.isEmpty())
                xmlWriter->addAttribute("draw:name", context->m_parentLayout->m_name);

            KoGenStyle style = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
            style.addProperty("draw:fill", "solid" /*none*/, KoGenStyle::GraphicType);
            style.addProperty("draw:fill-color", "#6666ff"); //TODO needs to handle colors1.xml ...
            style.addProperty("draw:opacity", "50%");
            const QString styleName = styles->insert(style);
            xmlWriter->addAttribute("draw:style-name", styleName);
            //xmlWriter->addAttribute("draw:text-style-name", "P2");

            xmlWriter->addAttribute("svg:x", QString("%1px").arg(x+cx));
            xmlWriter->addAttribute("svg:y", QString("%1px").arg(y+cy));
            xmlWriter->addAttribute("svg:width", QString("%1px").arg(w));
            xmlWriter->addAttribute("svg:height", QString("%1px").arg(h));

            xmlWriter->startElement("text:p");
            xmlWriter->endElement();

            if (m_type == QLatin1String("ellipse")) {
                xmlWriter->startElement("draw:enhanced-geometry");
                xmlWriter->addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N");
                xmlWriter->addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
                xmlWriter->addAttribute("draw:type", "ellipse");
                xmlWriter->addAttribute("svg:viewBox", "0 0 21600 21600");
                xmlWriter->endElement();
            } else /*if (m_type == QLatin1String("rect"))*/ {
                //TODO
                kWarning() << "TODO shape type=" << m_type;
            }

            xmlWriter->endElement(); // draw:custom-shape
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
        virtual void readElement(Context*, MsooXmlDiagramReader* reader) {
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
        QString m_count;
        QString m_hideLastTrans;
        QString m_start;
        QString m_step;
        explicit PresentationOfAtom() : AbstractAtom("dgm:presOf") {}
        virtual ~PresentationOfAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "axis=" << m_axis << "ptType=" << m_ptType << "count=" << m_count << "start=" << m_start << "step=" << m_step << "hideLastTrans=" << m_hideLastTrans;
            AbstractAtom::dump(context, level);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(axis, m_axis)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_ptType)
            TRY_READ_ATTR_WITHOUT_NS_INTO(cnt, m_count)
            TRY_READ_ATTR_WITHOUT_NS_INTO(hideLastTrans, m_hideLastTrans)
            TRY_READ_ATTR_WITHOUT_NS_INTO(st, m_start)
            TRY_READ_ATTR_WITHOUT_NS_INTO(step, m_step)
            AbstractAtom::readAll(context, reader);
        }
        virtual void layoutAtom(Context* context) {
            QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
            context->m_parentLayout->setAxis(axis);
            //AbstractAtom::layoutAtom(context);
        }
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
        QString m_start;
        QString m_step;
        QString m_count;
        QString m_value;
        explicit IfAtom(bool isTrue) : AbstractAtom(isTrue ? "dgm:if" : "dgm:else"), m_isTrue(isTrue) {}
        virtual ~IfAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP<<"name="<<m_name;
            //DEBUG_DUMP << "name=" << m_name << "argument=" << m_argument << "axis=" << m_axis << "count=" << m_count << "function=" << m_function << "hideLastTrans=" << m_hideLastTrans << "operator=" << m_operator << "dataPointType=" << m_ptType << "start=" << m_start << "step=" << m_step << "value=" << m_value;
            AbstractAtom::dump(context, level);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(arg, m_argument)
            TRY_READ_ATTR_WITHOUT_NS_INTO(axis, m_axis)
            TRY_READ_ATTR_WITHOUT_NS_INTO(cnt, m_count)
            TRY_READ_ATTR_WITHOUT_NS_INTO(func, m_function)
            TRY_READ_ATTR_WITHOUT_NS_INTO(hideLastTrans, m_hideLastTrans)
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            TRY_READ_ATTR_WITHOUT_NS_INTO(op, m_operator)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_ptType)
            TRY_READ_ATTR_WITHOUT_NS_INTO(st, m_start)
            TRY_READ_ATTR_WITHOUT_NS_INTO(step, m_step)
            TRY_READ_ATTR_WITHOUT_NS_INTO(val, m_value)
            AbstractAtom::readAll(context, reader);
        }
        bool isTrue() const { return m_isTrue; } // is true or false?
        bool testAtom(Context* context) {
            //TODO handle m_argument=="var"
            QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
            QString funcValue;
            if(m_function == "cnt") { // Specifies a count.
                funcValue = QString::number(axis.count());
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
                if(m_argument == QLatin1String("animLvl")) { // Specifies the animation level
                    //TODO
                } else if(m_argument == QLatin1String("animOne")) { // Specifies animate as one.
                    //TODO
                } else if(m_argument == QLatin1String("bulEnabled")) { // Specifies bullets enabled.
                    //TODO
                } else if(m_argument == QLatin1String("chMax")) { // The maximum number of children.
                    //TODO
                } else if(m_argument == QLatin1String("chPref")) { // The preferred number of children.
                    //TODO
                } else if(m_argument == QLatin1String("dir")) { // Specifies the direction of the diagram.
                    //TODO another case missing in the specs: What are the possible directions and where are they defined?
                    funcValue = "norm";
                } else if(m_argument == QLatin1String("hierBranch")) { // The hierarchy branch.
                    //TODO
                } else if(m_argument == QLatin1String("none")) { // Unknown variable type.
                    //TODO
                } else if(m_argument == QLatin1String("orgChart")) { // Algorithm that lays out an org chart.
                    //TODO
                } else if(m_argument == QLatin1String("resizeHandles")) { // Specifies the resize handles.
                    //TODO
                } else {
                    kWarning()<<"Unexpected argument="<<m_argument<<"name="<<m_name;
                }
            }

            bool istrue = false;
            if(m_isTrue && !funcValue.isNull()) {
                if(m_operator == "equ") {
                    istrue = funcValue == m_value;
                } else {
                    bool isInt;
                    const int funcValueInt = funcValue.toInt(&isInt);
                    const int valueInt = isInt ? m_value.toInt(&isInt) : 0;
                    if(!isInt) {
                        // right, that's untested atm since I didn't found a single document that does it and the specs don't cover
                        // such "details" anyways so it seems. So, if you run into this then it's up to you to fix it :)
                        kWarning()<<"TODO figure out how non-integer comparision is expected to work";
                    }

                    if(m_operator == QLatin1String("gt")) {
                        istrue = isInt ? funcValueInt > valueInt : funcValue > m_value;
                    } else if(m_operator == QLatin1String("gte")) {
                        istrue = isInt ? funcValueInt >= valueInt : funcValue >= m_value;
                    } else if(m_operator == QLatin1String("lt")) {
                        istrue = isInt ? funcValueInt < valueInt : funcValue < m_value;
                    } else if(m_operator == QLatin1String("lte")) {
                        istrue = isInt ? funcValueInt <= valueInt : funcValue <= m_value;
                    } else if(m_operator == QLatin1String("neq")) {
                        istrue = isInt ? funcValueInt != valueInt : funcValue != m_value;
                    } else {
                        kWarning()<<"Unexpected operator="<<m_operator<<"name="<<m_name;
                    }
                }
                //kDebug()<<"name="<<m_name<<"value1="<<funcValue<<"value2="<<m_value<<"operator="<<m_operator<<"istrue="<<istrue;
            }

            return istrue;
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
            foreach(AbstractAtom* atom, atomsMatchingToCondition(context))
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
        virtual void layoutAtom(Context* context) {
            foreach(AbstractAtom* atom, atomsMatchingToCondition(context))
                atom->layoutAtom(context);
        }
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
            DEBUG_WRITE;
            foreach(AbstractAtom* atom, atomsMatchingToCondition(context))
                atom->writeAtom(context, xmlWriter, styles);
        }
    private:
        QList<AbstractAtom*> atomsMatchingToCondition(Context* context) const {
            QList<AbstractAtom*> ifResult;
            QList<AbstractAtom*> elseResult;
            foreach(AbstractAtom* atom, m_children) {
                if(IfAtom* ifatom = dynamic_cast<IfAtom*>(atom)) {
                    if(ifatom->isTrue()) {
                        if(ifatom->testAtom(context))
                            ifResult.append(ifatom);
                    } else {
                        elseResult.append(ifatom);
                    }
                }
            }
            return ifResult.isEmpty() ? elseResult : ifResult;
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
        QString m_start;
        QString m_step;
        QString m_count;
        explicit ForEachAtom() : AbstractAtom("dgm:forEach") {}
        virtual ~ForEachAtom() {}
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "axis=" << m_axis << "count=" << m_count << "hideLastTrans=" << m_hideLastTrans << "name=" << m_name << "ptType=" << m_ptType << "reference=" << m_reference << "start=" << m_start << "step=" << m_step;
            foreach(AbstractAtom* atom, m_children)
                atom->dump(context, level + 1);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(axis, m_axis)
            TRY_READ_ATTR_WITHOUT_NS_INTO(cnt, m_count)
            TRY_READ_ATTR_WITHOUT_NS_INTO(hideLastTrans, m_hideLastTrans)
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_ptType)
            TRY_READ_ATTR_WITHOUT_NS_INTO(ref, m_reference)
            TRY_READ_ATTR_WITHOUT_NS_INTO(st, m_start)
            TRY_READ_ATTR_WITHOUT_NS_INTO(step, m_step)
            AbstractAtom::readAll(context, reader);
        }
        virtual void layoutAtom(Context* context) {
            QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
            foreach(AbstractNode* node, axis) {
                Q_ASSERT(dynamic_cast<PointNode*>(node));
                foreach(AbstractAtom* atom, m_children)
                    atom->layoutAtom(context);
            }
        }
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
            DEBUG_WRITE;
            QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
            foreach(AbstractNode* node, axis) {
                Q_ASSERT(dynamic_cast<PointNode*>(node));
                foreach(AbstractAtom* atom, m_children)
                    atom->writeAtom(context, xmlWriter, styles);
            }
        }
};

Context::Context()
    : m_rootPoint(0)
    , m_connections(new ConnectionListNode)
    , m_rootLayout(new Diagram::LayoutNodeAtom)
    , m_parentLayout(m_rootLayout)
    , m_currentNode(0)
{
}

Context::~Context()
{
    delete m_rootPoint;
    delete m_connections;
    delete m_rootLayout;
}
 
void Context::setCurrentNode(AbstractNode* node)
{
    Q_ASSERT(dynamic_cast<PointNode*>(node));
    m_currentNode = node;
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

    // initially inherit the layout from the parent.
    m_x = oldLayout->m_x;
    m_y = oldLayout->m_y;
    m_width = oldLayout->m_width;
    m_height = oldLayout->m_height;
    m_cx = oldLayout->m_cx;
    m_cy = oldLayout->m_cy;
    m_factX = oldLayout->m_factX;
    m_factY = oldLayout->m_factY;
    m_factWidth = oldLayout->m_factWidth;
    m_factHeight = oldLayout->m_factHeight;
    m_ctrX = oldLayout->m_ctrX;
    m_ctrY = oldLayout->m_ctrY;

    /*
    if(m_childNeedsRelayout) {
        m_childNeedsRelayout = false;
        foreach(AbstractAtom* atom, m_children) {
            atom->layoutAtom(context);
        }
    }
    */

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
        //TODO
    } else if(algType == QLatin1String("tx")) {
        // The text algorithm sizes text to fit inside a shape and controls its margins and alignment.
        //TODO
    } else {
        //TODO
    }

    // layout ourself if requested
    if(m_needsRelayout) {
        m_needsRelayout = false;

        //QStringList axisnames;
        //foreach(AbstractNode* n, axis()) axisnames.append(n->m_tagName);
        //kDebug() << "################# name=" << m_name << "algType=" << algType << "constraintCount=" << m_constraints.count() << "axis=" << axisnames;
        //this->dump(context, 2);

        // evaluate the constraints responsible for positioning and sizing.
        //kDebug() << "####/CONSTRAINTS-START"<<m_name;
        foreach(ConstraintAtom* c, m_constraints) {
            c->dump(context, 2);

            int value = -1;
            if(!c->m_value.isEmpty()) {
                bool ok;
                value = c->m_value.toInt(&ok);
                Q_ASSERT(ok);
            } else {
                LayoutNodeAtom* ref = c->m_refForName.isEmpty() ? this : context->m_layoutMap.value(c->m_refForName);
                Q_ASSERT(ref);
                //Q_ASSERT(!ref->m_needsRelayout);
                //Q_ASSERT(!ref->m_childNeedsRelayout);
                if(ref->m_needsRelayout || ref->m_childNeedsRelayout) {
                    ref->layoutAtom(context);
                    m_needsRelayout = true;
                    layoutAtom(context); // restart from the beginning since things may have completly changed now
                    context->m_parentLayout = oldLayout;
                    return;
                }

                if(!c->m_refType.isEmpty()) {
                    if(c->m_refType == "l") {
                        value = ref->m_x;
                    } else if(c->m_refType == "t") {
                        value = ref->m_y;
                    } else if(c->m_refType == "w") {
                        value = ref->m_width;
                    } else if(c->m_refType == "h") {
                        value = ref->m_height;
                    } else {
                        kWarning() << "Unhandled constraint reference-type=" << c->m_refType;
                    }
                } else {
                    //TODO
                }
            }

            //Q_ASSERT(c->m_for.isEmpty() == c->m_forName.isEmpty() || c->m_type=="primFontSz");

            if(c->m_type == QLatin1String("l")) {
                if(value >= 0) m_x = value;
                m_factX = (m_factX + c->m_fact) / 2.0;
            } else if(c->m_type == QLatin1String("t")) {
                if(value >= 0) m_y = value;
                m_factY = (m_factY + c->m_fact) / 2.0;
            } else if(c->m_type == QLatin1String("w")) {
                if(value >= 0) m_width = value;
                m_factWidth = (m_factWidth + c->m_fact) / 2.0;
            } else if(c->m_type == QLatin1String("h")) {
                if(value >= 0) m_height = value;
                m_factHeight = (m_factHeight + c->m_fact) / 2.0;
            } else if(c->m_type == QLatin1String("ctrX")) {
                if(value >= 0) m_cx = value;
                m_ctrX = (m_ctrX + c->m_fact) / 2.0;
            } else if(c->m_type == QLatin1String("ctrY")) {
                if(value >= 0) m_cy = value;
                m_ctrY = (m_ctrY + c->m_fact) / 2.0;
            } else if(c->m_type == QLatin1String("lMarg")) {
                //TODO
            } else if(c->m_type == QLatin1String("tMarg")) {
                //TODO
            } else if(c->m_type == QLatin1String("rMarg")) {
                //TODO
            } else if(c->m_type == QLatin1String("bMarg")) {
                //TODO
            } else if(c->m_type == QLatin1String("primFontSz")) {
                //TODO
            } else {
                kWarning() << "Unhandled constraint type=" << c->m_type;
            }

            if(c->m_for == QLatin1String("ch")) {
                foreach(ShapeAtom* shape, children<ShapeAtom>()) {
                    if(m_x >= 0) shape->m_x = m_x;
                    if(m_y >= 0) shape->m_y = m_y;
                    if(m_width >= 0) shape->m_width = m_width;
                    if(m_height >= 0) shape->m_height = m_height;
                    if(m_cx >= 0) shape->m_cx = m_cx;
                    if(m_cy >= 0) shape->m_cy = m_cy;
                    shape->m_factX = m_factX;
                    shape->m_factY = m_factY;
                    shape->m_factWidth = m_factWidth;
                    shape->m_factHeight = m_factHeight;
                    shape->m_ctrX = m_ctrX;
                    shape->m_ctrY = m_ctrY;
                    //shape->dump(context,10);
                }
                //if(m_x >= 0 || m_y >= 0 || m_width >= 0 || m_height >= 0) kDebug()<<m_x<<m_y<<m_width<<m_height;
                //Q_ASSERT(m_x < 0 && m_y < 0 && m_width < 0 && m_height < 0);
            }
        }
        //kDebug() << "####/CONSTRAINTS-END";
    }

    // layout the children again if still requested
    if(m_childNeedsRelayout) {
        m_childNeedsRelayout = false;
        foreach(AbstractAtom* atom, m_children) {
            atom->layoutAtom(context);
        }
    }

    //TODO evaluate rules too

    context->m_parentLayout = oldLayout;
}

void LayoutNodeAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles)
{
    QString algType;
    QList< QPair<QString,QString> > params;
    if(m_algorithm) {
        algType = m_algorithm->m_type;
        params = m_algorithm->m_params;
    }
    DEBUG_WRITE << "name=" << m_name << "algType=" << algType << "params=" << params;

    LayoutNodeAtom* oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;

    AbstractAtom::writeAtom(context, xmlWriter, styles);
    context->m_parentLayout = oldLayout;
}

}} // namespace MSOOXML::Diagram

/****************************************************************************************************
 * The reader-context and the reader itself. Note that there will be one reader-instance per xml-file
 * in a diagram. The reader-context is shared between the reader-instances for one diagram.
 */

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
    // The root layout node always inherits the canvas dimensions by default
    m_context->m_rootLayout->m_x = rect.x();
    m_context->m_rootLayout->m_y = rect.y();
    m_context->m_rootLayout->m_width = rect.width();
    m_context->m_rootLayout->m_height = rect.height();
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
        
        //m_context->m_context->m_rootPoint->dump(m_context->m_context,0);
        m_context->m_context->m_rootLayout->readDone(m_context->m_context);
        
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


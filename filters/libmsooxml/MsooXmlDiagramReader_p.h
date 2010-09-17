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

#ifndef MSOOXMLXMLDIAGRAMREADER_P_H
#define MSOOXMLXMLDIAGRAMREADER_P_H

#include <cmath>

namespace MSOOXML { namespace Diagram {

/****************************************************************************************************
 * The following classes where designed after the way the dmg-namespace is described in the
 * MSOOXML-specs and how it was done in oo.org.
 *
 * See also;
 * - http://wiki.services.openoffice.org/wiki/SmartArt
 * - http://msdn.microsoft.com/en-us/magazine/cc163470.aspx
 * - http://msdn.microsoft.com/en-us/library/dd439435(v=office.12).aspx
 * - http://msdn.microsoft.com/en-us/library/dd439443(v=office.12).aspx
 * - http://msdn.microsoft.com/en-us/library/dd439454(v=office.12).aspx
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
        /// The "doc" root node.
        AbstractNode* m_rootPoint;
        /// A list of connections between nodes.
        ConnectionListNode* m_connections;
        /// The root layout node.
        QExplicitlySharedDataPointer<LayoutNodeAtom> m_rootLayout;
        /// The current parent layout node. This will change during walking through the layout nodes.
        QExplicitlySharedDataPointer<LayoutNodeAtom> m_parentLayout;
        /// A identifier=>LayoutNodeAtom map used to access the layouts by there unique identifiers.
        QMap<QString, QExplicitlySharedDataPointer<LayoutNodeAtom> > m_layoutMap;

        explicit Context();
        ~Context();
        AbstractNode* currentNode() const { return m_currentNode; }
        void setCurrentNode(AbstractNode* node) { m_currentNode = node; }
    private:
        /// the moving context node
        AbstractNode* m_currentNode;
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
                if(!reader->isStartElement() && reader->qualifiedName() == m_tagName) break;
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
            DEBUG_DUMP << "type=" << m_type << "modelId=" << m_modelId << "cxnId=" << m_cxnId;
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
class AbstractAtom : public QSharedData
{
    public:
        const QString m_tagName;
        explicit AbstractAtom(const QString &tagName) : QSharedData(), m_tagName(tagName) {}
        virtual ~AbstractAtom() {}
        virtual AbstractAtom* clone() = 0;
        virtual void dump(Context* context, int level) {
            foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
                atom->dump(context, level + 1);
            }
        }
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader);
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            while (!reader->atEnd()) {
                QXmlStreamReader::TokenType tokenType = reader->readNext();
                if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
                if(!reader->isStartElement() && reader->qualifiedName() == m_tagName) break;
                readElement(context, reader);
            }
        }
        virtual void build(Context* context) {
            foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
                atom->build(context);
        }
        virtual void layoutAtom(Context* context) {
            foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
                atom->layoutAtom(context);
        }
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
            foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
                atom->writeAtom(context, xmlWriter, styles);
        }
        QExplicitlySharedDataPointer<AbstractAtom> parent() const { return m_parent; } 
        QVector< QExplicitlySharedDataPointer<AbstractAtom> > children() const { return m_children; }
        void addChild(AbstractAtom* node) {
            addChild(QExplicitlySharedDataPointer<AbstractAtom>(node));
        }
        void addChild(QExplicitlySharedDataPointer<AbstractAtom> node) {
            node->m_parent = this;
            m_children.append(node);
        }
        void removeChild(QExplicitlySharedDataPointer<AbstractAtom> node) {
            const int index = m_children.indexOf(node);
            Q_ASSERT(index >= 0);
            m_children.remove(index);
            node->m_parent = QExplicitlySharedDataPointer<AbstractAtom>();
        }
    protected:
        QExplicitlySharedDataPointer<AbstractAtom> m_parent;
        QVector< QExplicitlySharedDataPointer<AbstractAtom> > m_children;

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
            result << context->currentNode();
            for(int i = 0; i < axisList.count(); ++i) {
                result = fetchAxis(context, result, axisList[i], typeList.value(i), startList.value(i), countList.value(i), stepList.value(i));
            }
            return result;
        }
    private:
        QList<AbstractNode*> fetchAxis(Context* context, QList<AbstractNode*> list, const QString& axis, const QString &ptType, const QString& start, const QString& count, const QString& step) const {
            QList<AbstractNode*> result;

            // fill the result-list according to the defined axis value
            foreach(AbstractNode* node, list) {
                if(axis == QLatin1String("ancst")) { // Ancestor
                    for(AbstractNode* n = node; n; n = n->parent())
                        result.append(n);
                } else if(axis == QLatin1String("ancstOrSelf")) { // Ancestor Or Self
                    for(AbstractNode* n = node; n; n = n->parent())
                        result.append(n);
                    result.append(node);
                } else if(axis == QLatin1String("ch")) { // Child
                    foreach(AbstractNode* n, node->children())
                        result.append(n);
                } else if(axis == QLatin1String("des")) { // Descendant
                    foreach(AbstractNode* n, node->descendant())
                        result.append(n);
                } else if(axis == QLatin1String("desOrSelf")) { // Descendant Or Self
                    foreach(AbstractNode* n, node->descendant())
                        result.append(n);
                    result.append(node);
                } else if(axis == QLatin1String("follow")) { // Follow
                    foreach(AbstractNode* peer, node->peers()) {
                        result.append(peer);
                        foreach(AbstractNode* n, peer->descendant())
                            result.append(n);
                    }
                } else if(axis == QLatin1String("followSib")) { // Follow Sibling
                    foreach(AbstractNode* n, node->peers())
                        result.append(n);
                } else if(axis == QLatin1String("par")) { // Parent
                    if (AbstractNode* n = node->parent())
                        result.append(n);
                } else if(axis == QLatin1String("preced")) { // Preceding
                    kWarning()<<"TODO preced";
                    //TODO
                } else if(axis == QLatin1String("precedSib")) { // Preceding Sibling
                    kWarning()<<"TODO precedSib";
                    //TODO
                } else if(axis == QLatin1String("root")) { // Root
                    result.append(context->m_rootPoint);
                } else if(axis == QLatin1String("self")) { // Self
                    result.append(node);
                }
            }
            
            // optionally filter the list
            if(!ptType.isEmpty()) {
                QList<AbstractNode*> list = result;
                result.clear();
                foreach(AbstractNode* node, list) {
                    if(PointNode* pt = dynamic_cast<PointNode*>(node)) {
                        if(ptType == pt->m_type || ptType == "all" || (ptType == "nonAsst" && pt->m_type != "asst" ) || (ptType == "nonNorm" && pt->m_type != "norm")) {
                            result.append(pt);
                        }
                    }
                }
            }

            // evaluate optional forEach-conditions
            if(!start.isEmpty() || !count.isEmpty() || !step.isEmpty()) {
                const int _start = start.isEmpty() ? 1 : start.toInt();
                const int _count = count.isEmpty() ? 0 : count.toInt();
                const int _step =  step.isEmpty() ? 1 : step.toInt();
                result = foreachAxis(context, result, _start, _count, _step);
            }

            return result;
        }

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
        QMap<QString, qreal> m_values; // map that contains values like l,t,w,h,ctrX and ctrY for positioning the layout
        QMap<QString, qreal> m_factors;
        QMap<QString, int> m_countFactors;
        explicit LayoutNodeAtom() : AbstractAtom("dgm:layoutNode"), m_algorithm(0), m_needsRelayout(true), m_childNeedsRelayout(true), m_firstLayout(true) {}
        virtual ~LayoutNodeAtom() {}
        virtual LayoutNodeAtom* clone();
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "name=" << m_name << "constraintsCount=" << m_constraints.count() << "variables=" << m_variables << "values=" << finalValues();
            AbstractAtom::dump(context, level);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            //TRY_READ_ATTR_WITHOUT_NS_INTO(styleLbl, m_styleLbl)
            QExplicitlySharedDataPointer<LayoutNodeAtom> ptr(this);
            context->m_layoutMap[m_name] = ptr;
            QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
            context->m_parentLayout = ptr;
            AbstractAtom::readAll(context, reader);
            context->m_parentLayout = oldLayout;
        }
        virtual void build(Context* context) {
            //TODO what do to with the axis? How to use them in the layout itself?
            QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
            context->m_parentLayout = this;
            AbstractAtom::build(context);
            context->m_parentLayout = oldLayout;
        }
        virtual void layoutAtom(Context* context);
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles);
        QList< QExplicitlySharedDataPointer<ConstraintAtom> > constraints() const { return m_constraints; }
        void addConstraint(QExplicitlySharedDataPointer<ConstraintAtom> constraint) { m_constraints.append(constraint); setNeedsRelayout(true); }
        void setAlgorithm(QExplicitlySharedDataPointer<AlgorithmAtom> algorithm) { m_algorithm = algorithm; setNeedsRelayout(true); }
        //QList<AbstractNode*> axis() const { return m_axis; }
        void setAxis(const QList<AbstractNode*> &axis) { m_axis = axis; setNeedsRelayout(true); }
        void setNeedsRelayout(bool needsRelayout) {
            if(needsRelayout == m_needsRelayout) return;
            m_needsRelayout = needsRelayout;
            if(m_needsRelayout) // let parent-layouts know that we need a relayout
                for(QExplicitlySharedDataPointer<AbstractAtom> parent = m_parent; parent.data(); parent = parent->parent())
                    if(LayoutNodeAtom* parentLayoutAtom = dynamic_cast<LayoutNodeAtom*>(parent.data()))
                        parentLayoutAtom->m_childNeedsRelayout = true;
        }
        QString variable(const QString &name, bool checkParents = false) const {
            if(m_variables.contains(name))
                return m_variables[name];
            if(checkParents)
                for(QExplicitlySharedDataPointer<AbstractAtom> parent = m_parent; parent.data(); parent = parent->parent())
                    if(LayoutNodeAtom* parentLayoutAtom = dynamic_cast<LayoutNodeAtom*>(parent.data()))
                        return parentLayoutAtom->variable(name, checkParents);
            return QString();
        }
        QMap<QString, QString> variables() const { return m_variables; }
        void setVariable(const QString &name, const QString &value) { m_variables[name] = value; }
        QMap<QString, qreal> finalValues() const {
            //TODO cache
            QMap<QString, qreal> result = m_values;
            for(QMap<QString, qreal>::iterator it = result.begin(); it != result.end(); ++it) {
                if(m_factors.contains(it.key())) {
                    //TODO figure our why the +1.0 and +1 provides better results and find a better way that makes more sense
                    result[it.key()] = it.value() * ((m_factors[it.key()]+1.0) / qreal(m_countFactors[it.key()]+1.0));
                }
            }
            return result;
        }
    private:
        QList< QExplicitlySharedDataPointer<ConstraintAtom> > m_constraints;
        QExplicitlySharedDataPointer<AlgorithmAtom> m_algorithm;
        QList<AbstractNode*> m_axis;
        bool m_needsRelayout, m_childNeedsRelayout;
        QMap<QString, QString> m_variables;
        bool m_firstLayout;
};

/// Specify size and position of nodes, text values, and layout dependencies between nodes in a layout definition.
class ConstraintAtom : public AbstractAtom
{
    public:
        /// Factor used in a reference constraint or a rule in order to modify a referenced value by the factor defined.
        qreal m_fact;
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
        explicit ConstraintAtom() : AbstractAtom("dgm:constr"), m_fact(1.0) {}
        virtual ~ConstraintAtom() {}
        virtual ConstraintAtom* clone() {
            ConstraintAtom* atom = new ConstraintAtom;
            atom->m_fact = m_fact;
            atom->m_for = m_for;
            atom->m_forName = m_forName;
            atom->m_op = m_op;
            atom->m_ptType = m_ptType;
            atom->m_refPtType = m_refPtType;
            atom->m_refType = m_refType;
            atom->m_refFor = m_refFor;
            atom->m_refForName = m_refForName;
            atom->m_type = m_type;
            atom->m_value = m_value;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
                atom->addChild(a->clone());
            return atom;
        }
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
        virtual void build(Context* context) {
            QExplicitlySharedDataPointer<LayoutNodeAtom> layout = m_forName.isEmpty() ? context->m_parentLayout : context->m_layoutMap.value(m_forName);
            Q_ASSERT(layout);
            if(layout) {
                QExplicitlySharedDataPointer<ConstraintAtom> ptr(this);
                m_parent->removeChild(QExplicitlySharedDataPointer<AbstractAtom>(this));
                layout->addConstraint(ptr);
            }
            AbstractAtom::build(context);
        }
};

/// List of constraints.
class ConstraintListAtom : public AbstractAtom
{
    public:
        explicit ConstraintListAtom() : AbstractAtom("dgm:constrLst") {}
        virtual ~ConstraintListAtom() {}
        virtual ConstraintListAtom* clone() {
            ConstraintListAtom* atom = new ConstraintListAtom;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
                atom->addChild(a->clone());
            return atom;
        }
        virtual void dump(Context* context, int level) {
            AbstractAtom::dump(context, level);
        }
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            if (reader->isStartElement()) {
                if (reader->qualifiedName() == QLatin1String("dgm:constr")) {
                    QExplicitlySharedDataPointer<AbstractAtom> node(new ConstraintAtom);
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
        explicit ShapeAtom() : AbstractAtom("dgm:shape"), m_hideGeom(false) {}
        virtual ~ShapeAtom() {}
        virtual ShapeAtom* clone() {
            ShapeAtom* atom = new ShapeAtom;
            atom->m_type = m_type;
            atom->m_blip = m_blip;
            atom->m_hideGeom = m_hideGeom;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
                atom->addChild(a->clone());
            return atom;
        }
        virtual void dump(Context* context, int level) {
            QExplicitlySharedDataPointer<LayoutNodeAtom> l = context->m_parentLayout;
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

        //TODO use filters/libmso/ODrawToOdf.h
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
            Q_ASSERT(context->m_parentLayout);
            if(m_type.isEmpty()) return;
//if(m_hideGeom) return; // skip for now

            QMap<QString, qreal> values = context->m_parentLayout->finalValues();
            Q_ASSERT(values.contains("l"));
            Q_ASSERT(values.contains("t"));
            Q_ASSERT(values.contains("w"));
            Q_ASSERT(values.contains("h"));
            Q_ASSERT(values.contains("ctrX"));
            Q_ASSERT(values.contains("ctrY"));
            qreal x  = values["l"];
            qreal y  = values["t"];
            qreal w  = values["w"];
            qreal h  = values["h"];
            qreal cx = values["ctrX"];
            qreal cy = values["ctrY"];

            //TODO can spacing between the siblings applied by shriking the shapes or is it needed to apply them along the used algorithm?
            qreal sibSp = values.value("sibSp");
            if(sibSp > 0.0) {
                //Q_ASSERT(w >= sibSp);
                //Q_ASSERT(h >= sibSp);
                if(w >= sibSp && h >= sibSp) {
                    x += sibSp;
                    y += sibSp;
                    w -= sibSp;
                    h -= sibSp;
                } else {
                    kWarning()<<"Sibling spacing is bigger then width/height! Skipping sibSp-value! width="<<w<<"height="<<h<<"sibSp="<<sibSp;
                    dump(context,10);
                }
            }
            
            DEBUG_WRITE << "type=" << m_type << "blip=" << m_blip << "hideGeom=" << m_hideGeom << "geometry=" << x+cx << y+cy << w << h;
            Q_ASSERT(x >= 0);
            Q_ASSERT(y >= 0);
            Q_ASSERT(w >= 0);
            Q_ASSERT(h >= 0);
            Q_ASSERT(cx >= 0);
            Q_ASSERT(cy >= 0);

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

kDebug()<<"################### context"<<context->m_parentLayout->m_name<<x+cx<<y+cy<<w<<h;
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
            } else if (m_type == QLatin1String("cycle")) {
                xmlWriter->startElement("draw:enhanced-geometry");
                xmlWriter->addAttribute("draw:enhanced-path", "M 0 414114 C 0 304284 43630 198953 121292 121291 198954 43630 304285 0 414115 0 523945 0 629276 43630 706938 121292 784599 198954 828229 304285 828229 414115 828229 523945 784599 629276 706938 706938 629277 784599 523945 828229 414115 828229 304285 828229 198954 784599 121292 706938 43631 629276 1 523945 1 414115 L 0 414114 Z N");
                xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90, -90, -90, -90, -90, -90");
                xmlWriter->addAttribute("draw:glue-points", "-90, -90, -90, -90, -90, -90, -90, -90, -90, -90");
                xmlWriter->addAttribute("draw:type", "?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f27 ?f28 ?f29 ?f30 ?f27 ?f31 ?f25 ?f32 ?f23 ?f31 ?f33 ?f30 ?f21 ?f22");
                xmlWriter->addAttribute("draw:text-areas", "?f34 ?f36 ?f35 ?f37");
                xmlWriter->addAttribute("draw:type", "circle");
                xmlWriter->addAttribute("svg:viewBox", "0 0 828228 828228");
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
        /// The used layout-algorithm.
        enum Algorithm {
            UnknownAlg, ///< Unknown algorithm. This should happen...
            CompositeAlg, ///< The composite algorithm specifies the size and position for all child layout nodes. You can use it to create graphics with a predetermined layout or in combination with other algorithms to create more complex shapes.
            ConnectorAlg, ///< The connector algorithm lays out and routes connecting lines, arrows, and shapes between layout nodes.
            CycleAlg, ///< The cycle algorithm lays out child layout nodes around a circle or portion of a circle using equal angle spacing.
            HierChildAlg, ///< The hierarchy child algorithm works with the hierRoot algorithm to create hierarchical tree layouts. This algorithm aligns and positions its child layout nodes in a linear path under the hierRoot layout node.
            HierRootAlg, ///< The hierarchy root algorithm works with the hierChild algorithm to create hierarchical tree layouts. The hierRoot algorithm aligns and positions the hierRoot layout node in relation to the hierChild layout nodes.
            LinearAlg, ///< The linear algorithm lays out child layout nodes along a linear path.
            PyramidAlg, ///< The pyramid algorithm lays out child layout nodes along a vertical path and works with the trapezoid shape to create a pyramid.
            SnakeAlg, ///< The snake algorithm lays out child layout nodes along a linear path in two dimensions, allowing the linear flow to continue across multiple rows or columns.
            SpaceAlg, ///< The space algorithm is used to specify a minimum space between other layout nodes or as an indication to do nothing with the layout node’s size and position.
            TextAlg ///< The text algorithm sizes text to fit inside a shape and controls its margins and alignment.
        };
        Algorithm m_type;
        QList< QPair<QString,QString> > m_params; // list of type=value parameters that modify the default behavior of the algorithm.
        explicit AlgorithmAtom() : AbstractAtom("dgm:alg"), m_type(UnknownAlg) {}
        virtual ~AlgorithmAtom() {}
        virtual AlgorithmAtom* clone() {
            AlgorithmAtom* atom = new AlgorithmAtom;
            atom->m_type = m_type;
            atom->m_params = m_params;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
                atom->addChild(a->clone());
            return atom;
        }
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "type=" << m_type;
            AbstractAtom::dump(context, level);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS(type)
            if(type == QLatin1String("composite")) m_type = CompositeAlg;
            else if(type == QLatin1String("conn")) m_type = ConnectorAlg;
            else if(type == QLatin1String("cycle")) m_type = CycleAlg;
            else if(type == QLatin1String("hierChild")) m_type = HierChildAlg;
            else if(type == QLatin1String("hierRoot")) m_type = HierRootAlg;
            else if(type == QLatin1String("lin")) m_type = LinearAlg;
            else if(type == QLatin1String("pyra")) m_type = PyramidAlg;
            else if(type == QLatin1String("snake")) m_type = SnakeAlg;
            else if(type == QLatin1String("sp")) m_type = SpaceAlg;
            else if(type == QLatin1String("tx")) m_type = TextAlg;
            else m_type = UnknownAlg;
            QExplicitlySharedDataPointer<AlgorithmAtom> ptr(this);
            context->m_parentLayout->setAlgorithm(ptr);
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
        virtual PresentationOfAtom* clone() {
            PresentationOfAtom* atom = new PresentationOfAtom;
            atom->m_axis = m_axis;
            atom->m_ptType = m_ptType;
            atom->m_count = m_count;
            atom->m_hideLastTrans = m_hideLastTrans;
            atom->m_start = m_start;
            atom->m_step = m_step;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
                atom->addChild(a->clone());
            return atom;
        }
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
        virtual void build(Context* context) {
            QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
            context->m_parentLayout->setAxis(axis);
            AbstractAtom::build(context);
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
        virtual IfAtom* clone() {
            IfAtom* atom = new IfAtom(m_isTrue);
            atom->m_argument = m_argument;
            atom->m_axis = m_axis;
            atom->m_function = m_function;
            atom->m_hideLastTrans = m_hideLastTrans;
            atom->m_name = m_name;
            atom->m_operator = m_operator;
            atom->m_ptType = m_ptType;
            atom->m_start = m_start;
            atom->m_step = m_step;
            atom->m_count = m_count;
            atom->m_value = m_value;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
                atom->addChild(a->clone());
            return atom;
        }
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
            QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
            QString funcValue;
            if(m_function == "cnt") { // Specifies a count.
                funcValue = QString::number(axis.count());
            } else if(m_function == "depth") { // Specifies the depth.
                //int depth = 0;
                //for(AbstractNode* n = context->m_currentNode; n; n = n->parent(), ++depth);
                //funcValue = depth;
                //TODO
                kWarning()<<"TODO func=depth";
            } else if(m_function == "maxDepth") { // Defines the maximum depth.
                //int depth = 0;
                //for(AbstractNode* n = context->m_currentNode; n; n = n->parent(), ++depth);
                //funcValue = depth;
                //TODO
                kWarning()<<"TODO func=maxDepth";
            } else if(m_function == "pos") { // Retrieves the position of the node in the specified set of nodes.
                //int index = axis.indexOf(context->m_currentNode)+1;
                //istrue = index;
                //TODO
                kWarning()<<"TODO func=pos";
            } else if(m_function == "posEven") { // Returns 1 if the specified node is at an even numbered position in the data model.
                //int index = axis.indexOf(context->m_currentNode)+1;
                //funcValue = index>=1 && index % 2 == 0 ? 1 : 0;
                //TODO
                kWarning()<<"TODO func=posEven";
            } else if(m_function == "posOdd") { // Returns 1 if the specified node is in an odd position in the data model.
                //int index = axis.indexOf(context->m_currentNode)+1;
                //funcValue = index>=1 && index % 2 != 0 = 1 : 0;
                //TODO
                kWarning()<<"TODO func=posOdd";
            } else if(m_function == "revPos") { // Reverse position function.
                //int index = axis.indexOf(context->m_currentNode)+1;
                //istrue = axis.count()-index;
                //TODO
                kWarning()<<"TODO func=revPos";
            } else if(m_function == "var") { // Used to reference a variable.
                if(m_argument == QLatin1String("animLvl")) { // Specifies the animation level
                    //TODO
                    kWarning()<<"TODO m_function=var m_argument=animLvl";
                } else if(m_argument == QLatin1String("animOne")) { // Specifies animate as one.
                    //TODO
                    kWarning()<<"TODO m_function=var m_argument=animOne";
                } else if(m_argument == QLatin1String("bulEnabled")) { // Specifies bullets enabled.
                    //TODO
                    kWarning()<<"TODO m_function=var m_argument=bulEnabled";
                } else if(m_argument == QLatin1String("chMax")) { // The maximum number of children.
                    //TODO
                    kWarning()<<"TODO m_function=var m_argument=chMax";
                } else if(m_argument == QLatin1String("chPref")) { // The preferred number of children.
                    //TODO
                    kWarning()<<"TODO m_function=var m_argument=chPref";
                } else if(m_argument == QLatin1String("dir")) { // Specifies the direction of the diagram.
                    const QString dirval = context->m_parentLayout->variable("dir", true /* check parents */);
                    funcValue = dirval.isEmpty() ? "norm" : dirval;
                } else if(m_argument == QLatin1String("hierBranch")) { // The hierarchy branch.
                    //TODO
                    kWarning()<<"TODO m_function=var m_argument=hierBranch";
                } else if(m_argument == QLatin1String("none")) { // Unknown variable type.
                    //TODO
                    kWarning()<<"TODO m_function=var m_argument=none";
                } else if(m_argument == QLatin1String("orgChart")) { // Algorithm that lays out an org chart.
                    //TODO
                    kWarning()<<"TODO m_function=var m_argument=orgChart";
                } else if(m_argument == QLatin1String("resizeHandles")) { // Specifies the resize handles.
                    //TODO
                    kWarning()<<"TODO m_function=var m_argument=resizeHandles";
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
            }

            kDebug()<<"name="<<m_name<<"value1="<<funcValue<<"value2="<<m_value<<"operator="<<m_operator<<"istrue="<<istrue;
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
        virtual ChooseAtom* clone() {
            ChooseAtom* atom = new ChooseAtom;
            atom->m_name = m_name;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
                atom->addChild(a->clone());
            return atom;
        }
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "name=" << m_name;
            AbstractAtom::dump(context, level);
        }
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
            AbstractAtom::readAll(context, reader);
        }
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader) {
            if(reader->isStartElement()) {
                if(reader->qualifiedName() == QLatin1String("dgm:if")) {
                    QExplicitlySharedDataPointer<AbstractAtom> n(new IfAtom(true));
                    addChild(n);
                    n->readAll(context, reader);
                } else if(reader->qualifiedName() == QLatin1String("dgm:else")) {
                    QExplicitlySharedDataPointer<AbstractAtom> n(new IfAtom(false));
                    addChild(n);
                    n->readAll(context, reader);
                }
            }
        }
        virtual void build(Context* context) {
            QVector< QExplicitlySharedDataPointer<AbstractAtom> > ifResult;
            QVector< QExplicitlySharedDataPointer<AbstractAtom> > elseResult;
            while(!m_children.isEmpty()) {
                QExplicitlySharedDataPointer<AbstractAtom> atom = m_children.first();
                m_children.remove(0); // detach child
                IfAtom* ifatom = static_cast<IfAtom*>(atom.data());
                if(ifatom->isTrue()) {
                    if(ifatom->testAtom(context)) {
                        ifResult.append(atom);
                    }
                } else {
                    elseResult.append(atom);
                }
            }
kDebug()<<"###############"<<m_name;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, ifResult.isEmpty() ? elseResult : ifResult) {
                foreach(QExplicitlySharedDataPointer<AbstractAtom> a, atom->children()) {
                    atom->removeChild(a);
                    m_parent->addChild(a);
                    a->build(context);
                }
            }

            // once everything is done we can detach ourself from our parent
            QExplicitlySharedDataPointer<AbstractAtom> ptr(this);
            m_parent->removeChild(ptr);
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
        virtual ForEachAtom* clone() {
            ForEachAtom* atom = new ForEachAtom;
            atom->m_axis = m_axis;
            atom->m_hideLastTrans = m_hideLastTrans;
            atom->m_name = m_name;
            atom->m_ptType = m_ptType;
            atom->m_reference = m_reference;
            atom->m_start = m_start;
            atom->m_step = m_step;
            atom->m_count = m_count;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
                atom->addChild(a->clone());
            return atom;
        }
        virtual void dump(Context* context, int level) {
            DEBUG_DUMP << "axis=" << m_axis << "count=" << m_count << "hideLastTrans=" << m_hideLastTrans << "name=" << m_name << "ptType=" << m_ptType << "reference=" << m_reference << "start=" << m_start << "step=" << m_step;
            foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
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
        virtual void build(Context* context) {
#if 0
            QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
            AbstractNode* oldCurrentNode = context->currentNode();
            foreach(AbstractNode* node, axis) {
                context->setCurrentNode(node); // move on to the next node
                foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
                    QExplicitlySharedDataPointer<AbstractAtom> atomCopy(atom->clone());
                    m_parent->addChild(atomCopy);
                    Q_ASSERT(atomCopy->parent().data() == parent().data());
                    atomCopy->build(context);
                }
            }
            context->setCurrentNode(oldCurrentNode);

            // everything done so we can detach ourself now since we are not needed any longer
            QExplicitlySharedDataPointer<AbstractAtom> ptr(this);
QExplicitlySharedDataPointer<AbstractAtom> prevParent=m_parent;
            m_parent->removeChild(ptr);
            m_children.clear(); // just to be sure
#else
            typedef QPair<AbstractNode*, QList<QExplicitlySharedDataPointer<AbstractAtom> > > NodePair;
            QList<NodePair> newChildren;

            QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
            foreach(AbstractNode* node, axis) {
                QList<QExplicitlySharedDataPointer<AbstractAtom> > list;
                foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
                    QExplicitlySharedDataPointer<AbstractAtom> atomCopy(atom->clone());
                    list.append(atomCopy);
                }
                newChildren.append(NodePair(node, list));
            }
            
            AbstractNode* oldCurrentNode = context->currentNode();
            foreach(NodePair p, newChildren) {
                context->setCurrentNode(p.first); // move on to the next node
                foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, p.second) {
                    m_parent->addChild(atom);
                    atom->build(context);
                }
            }
            context->setCurrentNode(oldCurrentNode);

            // once everything is done we can detach ourself from our parent
            QExplicitlySharedDataPointer<AbstractAtom> ptr(this);
            m_parent->removeChild(ptr);
#endif
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
}
 
void AbstractAtom::readElement(Context* context, MsooXmlDiagramReader* reader)
{
    if (reader->isStartElement()) {
        AbstractAtom *node = 0;

        if (reader->qualifiedName() == QLatin1String("dgm:layoutNode")) {
            node = new LayoutNodeAtom;
        } else if (reader->qualifiedName() == QLatin1String("dgm:shape")) {
            node = new ShapeAtom;
        } else if (reader->qualifiedName() == QLatin1String("dgm:alg")) {
            node = new AlgorithmAtom;
        } else if (reader->qualifiedName() == QLatin1String("dgm:presOf")) {
            node = new PresentationOfAtom;
        } else if (reader->qualifiedName() == QLatin1String("dgm:choose")) {
            node = new ChooseAtom;
        } else if (reader->qualifiedName() == QLatin1String("dgm:forEach")) {
            node = new ForEachAtom;
        } else if (reader->qualifiedName() == QLatin1String("dgm:constrLst")) {
            node = new ConstraintListAtom;
        } else if (reader->qualifiedName() == QLatin1String("dgm:varLst")) {
            while (!reader->atEnd()) {
                QXmlStreamReader::TokenType tokenType = reader->readNext();
                if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
                if(!reader->isStartElement() && reader->qualifiedName() == "dgm:varLst") break;
                if(reader->isStartElement()) {
                    const QXmlStreamAttributes attrs(reader->attributes());
                    TRY_READ_ATTR_WITHOUT_NS(val)
                    context->m_parentLayout->setVariable(reader->name().toString(), val);
                }
            }
        } else {
            kWarning()<<"TODO"<<reader->qualifiedName();
        }
        
        if (node) {
            QExplicitlySharedDataPointer<AbstractAtom> ptr(node);
            addChild(ptr);
            ptr->readAll(context, reader);
        }
    }
}

// template<class U, class T> QList<U*> filterItems(Context* context, const QList<T*> &items) const
// {
//     QList<U*> results;
//     foreach(T* atom, items) {
//         if(ChooseAtom* chooseatom = dynamic_cast<ChooseAtom*>(atom)) {
//             QList<U*> children = filterItems<U, AbstractAtom>(context, chooseatom->atomsMatchingToCondition(context));
//             foreach(U* a, children)
//                 results.append(a);
//         } else if(ForEachAtom* foreachatom = dynamic_cast<ForEachAtom*>(atom)) {
//             QList<U*> children = filterItems<U, AbstractAtom>(context, foreachatom->children());
//             foreach(U* a, children)
//                 results.append(a);
//         } else if(U* a = dynamic_cast<U*>(atom)) {
//             results.append(a);
//         }
//     }
//     return results;
// }

LayoutNodeAtom* LayoutNodeAtom::clone()
{
    LayoutNodeAtom* atom = new LayoutNodeAtom;
    atom->m_name = m_name;
    atom->m_values = m_values;
    atom->m_factors = m_factors;
    atom->m_countFactors = m_countFactors;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone());
    foreach(QExplicitlySharedDataPointer<ConstraintAtom> a, m_constraints)
        atom->addConstraint(QExplicitlySharedDataPointer<ConstraintAtom>(a->clone()));
    atom->m_algorithm = QExplicitlySharedDataPointer<AlgorithmAtom>(m_algorithm->clone());
    atom->setAxis(m_axis);
    atom->m_needsRelayout = m_needsRelayout;
    atom->m_childNeedsRelayout = m_childNeedsRelayout;
    atom->m_variables = m_variables;
    atom->m_firstLayout = m_firstLayout;
    return atom;
}

void LayoutNodeAtom::layoutAtom(Context* context)
{
    kDebug() << "name=" << m_name;

    QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;
    
    AbstractNode* oldCurrentNode = context->currentNode();

    // Initially take over the layout coordinates from the parent layout. Constraints and rules attached to this
    // layout and that are evaluated later in this method will then make sure to change this accordingly.
    if(m_firstLayout) {
        m_firstLayout = false;
        m_values = oldLayout->m_values;
        m_factors = oldLayout->m_factors;
        m_countFactors = oldLayout->m_countFactors;

        QMap<QString, qreal> values = oldLayout->finalValues();
        Q_ASSERT(values.contains("l"));
        Q_ASSERT(values.contains("t"));
        Q_ASSERT(values.contains("w"));
        Q_ASSERT(values.contains("h"));
        Q_ASSERT(values.contains("ctrX"));
        Q_ASSERT(values.contains("ctrY"));
        Q_ASSERT(values["l"] >= 0.0);
        Q_ASSERT(values["t"] >= 0.0);
        Q_ASSERT(values["w"] >= 0.0);
        Q_ASSERT(values["h"] >= 0.0);
        Q_ASSERT(values["ctrX"] >= 0.0);
        Q_ASSERT(values["ctrY"] >= 0.0);
    }

    AlgorithmAtom::Algorithm algorithm = AlgorithmAtom::UnknownAlg;
    QList< QPair<QString,QString> > params;
    if(m_algorithm) {
        algorithm = m_algorithm->m_type;
        params = m_algorithm->m_params;
    }

    // layout ourself if requested
    if(m_needsRelayout) {
        m_needsRelayout = false;
        m_childNeedsRelayout = true;

        //QStringList axisnames;
        //foreach(AbstractNode* n, m_axis) axisnames.append(n->m_tagName);
        //kDebug() << "################# name=" << m_name << "algorithm=" << algorithm << "constraintCount=" << m_constraints.count() << "axis=" << axisnames;
        //this->dump(context, 2);

        // evaluate the constraints responsible for positioning and sizing.
        //if(!m_constraints.isEmpty()) kDebug() << "Constraints for LayoutNodeAtom="<<m_name;
        foreach(QExplicitlySharedDataPointer<ConstraintAtom> c, m_constraints) {
            c->dump(context, 2);

            qreal value = -1.0;
            if(!c->m_value.isEmpty()) {
                bool ok;
                value = c->m_value.toDouble(&ok);
                Q_ASSERT(ok);
            } else {
                LayoutNodeAtom* ref = c->m_refForName.isEmpty() ? this : context->m_layoutMap.value(c->m_refForName).data();
                Q_ASSERT(ref);
                //Q_ASSERT(!ref->m_needsRelayout);
                //Q_ASSERT(!ref->m_childNeedsRelayout);
                if(ref != this && (ref->m_needsRelayout || ref->m_childNeedsRelayout)) {
                    ref->layoutAtom(context);
                    Q_ASSERT(!ref->m_needsRelayout && !ref->m_childNeedsRelayout);
                    m_needsRelayout = m_childNeedsRelayout = true;
                    layoutAtom(context); // restart from the beginning since things may have completly changed now for us
                    Q_ASSERT(!m_needsRelayout);
                    Q_ASSERT(!m_childNeedsRelayout);
                    context->setCurrentNode(oldCurrentNode);
                    context->m_parentLayout = oldLayout;
                    return;
                }

                if(!c->m_refType.isEmpty()) {
                    QMap<QString, qreal> values = ref->finalValues();
                    Q_ASSERT(values.contains(c->m_refType));
                    value = values[c->m_refType];
                    Q_ASSERT(value >= 0.0);
                } else {
                    //FIXME no refType and no value defined means use the default-value but the specs don't say what
                    //the default values are. So, it's needed someone figures out what it means if standalone tags
                    //like <dgm:constr type="connDist"/> are defined.
                    kDebug()<<"TODO figure out defaults for constraints";
                    c->dump(context,10);
                    value = 0.0;
                }
            }

            if(c->m_type.isEmpty()) {
                kWarning() << "Unhandled constraint with empty type";
                continue;
            }

            if(value >= 0.0)
                m_values[c->m_type] = value;
            m_factors[c->m_type] += c->m_fact;
            m_countFactors[c->m_type] += 1;
        }
    }

    // layout the children if still requested
    if(m_childNeedsRelayout) {
        m_childNeedsRelayout = false;
        QList<LayoutNodeAtom*> childLayouts;
        foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
            atom->layoutAtom(context);
            if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data()))
                childLayouts.append(l);
        }

        Q_ASSERT(m_algorithm);
        QList< QPair<qreal,qreal> > m_startCoordinates;
        switch(m_algorithm->m_type) {
            case AlgorithmAtom::UnknownAlg:
                kWarning() << "Unknown algorithm defined. Using composite algorithm.";
                break;
            case AlgorithmAtom::CompositeAlg:
                // nothing to do since composite means that the constraints define everything needed and
                // we don't need to do anything additionally...
                break;
            case AlgorithmAtom::ConnectorAlg:
                //TODO
                kWarning() << "TODO implement AlgorithmAtom::ConnectorAlg";
                break;
            case AlgorithmAtom::CycleAlg: {
                qreal w = finalValues()["w"];
                qreal h = finalValues()["h"];
                const qreal rx = w / 2.0;
                const qreal ry = h / 2.0;
                Q_ASSERT(!childLayouts.isEmpty());
                const int num = 360 / childLayouts.count();
                for(int degree = 0; degree <= 360; degree += num) {
                    const qreal radian = (degree - 90) * (3.14 / 180);
                    const qreal x = rx + cos(radian) * rx;
                    const qreal y = ry + sin(radian) * ry;
                    Q_ASSERT(x >= 0);
                    Q_ASSERT(y >= 0);
                    m_startCoordinates.append(QPair<qreal,qreal>(x, y));
                }
            } break;
            case AlgorithmAtom::HierChildAlg:
                //TODO
                kWarning() << "TODO implement AlgorithmAtom::HierChildAlg";
                break;
            case AlgorithmAtom::HierRootAlg:
                //TODO
                kWarning() << "TODO implement AlgorithmAtom::HierRootAlg";
                break;
            case AlgorithmAtom::LinearAlg:
                //TODO
                kWarning() << "TODO implement AlgorithmAtom::LinearAlg";
                break;
            case AlgorithmAtom::PyramidAlg:
                //TODO
                kWarning() << "TODO implement AlgorithmAtom::PyramidAlg";
                break;
            case AlgorithmAtom::SnakeAlg:
                //TODO
                kWarning() << "TODO implement AlgorithmAtom::SnakeAlg";
                break;
            case AlgorithmAtom::SpaceAlg:
                //TODO
                kWarning() << "TODO implement AlgorithmAtom::SpaceAlg";
                break;
            case AlgorithmAtom::TextAlg:
                //TODO
                kWarning() << "TODO implement AlgorithmAtom::TextAlg";
                break;
        }

        foreach(LayoutNodeAtom* l, childLayouts) {
            if(!m_startCoordinates.isEmpty()) {
                QPair<qreal,qreal> coordinates = m_startCoordinates.takeFirst();
                l->m_values["ctrX"] += coordinates.first;
                l->m_values["ctrY"] += coordinates.second;
//kDebug()<<"w="<<m_values.value("w")<<"final-w="<<finalValues().value("w")<<"sibSp="<<m_values.value("sibSp")<<"final-sibSp="<<finalValues().value("sibSp");
//Q_ASSERT(false);
            }
        }
        //dump(context,10);
    }

    //TODO evaluate rules too

    context->setCurrentNode(oldCurrentNode);
    context->m_parentLayout = oldLayout;
}

void LayoutNodeAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles)
{
    AlgorithmAtom::Algorithm algorithm = AlgorithmAtom::UnknownAlg;
    QList< QPair<QString,QString> > params;
    if(m_algorithm) {
        algorithm = m_algorithm->m_type;
        params = m_algorithm->m_params;
    }
    DEBUG_WRITE << "name=" << m_name << "algorithm=" << algorithm << "params=" << params;

    QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;

    AbstractAtom::writeAtom(context, xmlWriter, styles);
    context->m_parentLayout = oldLayout;
}

}} // namespace MSOOXML::Diagram

#endif

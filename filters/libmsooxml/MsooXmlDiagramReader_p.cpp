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

#include "MsooXmlDiagramReader_p.h"

#include <typeinfo>
#include <QDebug>
#include <QXmlStreamReader>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>

#define MSOOXML_CURRENT_NS "dgm"
#define MSOOXML_CURRENT_CLASS MsooXmlDiagramReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader.h>
#include <MsooXmlReader_p.h>
#include <MsooXmlCommonReader.h>
#include <MsooXmlUtils.h>
#include <MsooXmlDiagramReader.h>

namespace MSOOXML { namespace Diagram {

#define DEBUG_DUMP \
    qDebug() << QString("%1Dgm::%2::%3").arg(QString(' ').repeated(level)).arg(typeid(this).name()).arg(__FUNCTION__) << this << "atom=" << m_tagName
#define DEBUG_WRITE \
    qDebug() << QString("Dgm::%1::%2").arg(typeid(this).name()).arg(__FUNCTION__) << "atom=" << m_tagName

}}

using namespace MSOOXML::Diagram;

/****************************************************************************************************/

Context::Context()
    : m_rootPoint(0)
    , m_connections(new ConnectionListNode)
    , m_rootLayout(new Diagram::LayoutNodeAtom)
    , m_parentLayout(m_rootLayout)
    , m_currentNode(0) {
}

Context::~Context() {
    delete m_rootPoint;
    delete m_connections;
}
        
AbstractNode* Context::currentNode() const { return m_currentNode; }
void Context::setCurrentNode(AbstractNode* node) { m_currentNode = node; }

/****************************************************************************************************/

AbstractNode::AbstractNode(const QString &tagName) : m_tagName(tagName), m_parent(0) {}
AbstractNode::~AbstractNode() { qDeleteAll(children()); }

void AbstractNode::dump(Context* context, int level) {
    foreach(AbstractNode* node, children())
        node->dump(context, level + 1);
}

void AbstractNode::readElement(Context*, MsooXmlDiagramReader*) {
}

void AbstractNode::readAll(Context* context, MsooXmlDiagramReader* reader) {
    while (!reader->atEnd()) {
        QXmlStreamReader::TokenType tokenType = reader->readNext();
        if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
        if(!reader->isStartElement() && reader->qualifiedName() == m_tagName) break;
        readElement(context, reader);
    }
}

AbstractNode* AbstractNode::parent() const { return m_parent; } 

QList<AbstractNode*> AbstractNode::children() const {
    if(m_cachedChildren.isEmpty()) {
        const int count = m_appendedChildren.count()+m_orderedChildren.count();
        for(int i = 0, k = -1; i < count; ++i) {
            if(m_orderedChildren.contains(i)) {
                foreach(AbstractNode* n, m_orderedChildren[i])
                    m_cachedChildren.append(n);
            } else {
                m_cachedChildren.append(m_appendedChildren[++k]);
            }
        }
    }
    return m_cachedChildren;
}

void AbstractNode::insertChild(int index, AbstractNode* node) {
    //Q_ASSERT(!m_orderedChildren.contains(index));
    Q_ASSERT(!m_orderedChildrenReverse.contains(node));
    Q_ASSERT(!m_appendedChildren.contains(node));
    Q_ASSERT(!node->m_parent);
    node->m_parent = this;
    if(m_orderedChildren.contains(index))
        m_orderedChildren[index].append(node);
    else
        m_orderedChildren[index] = QList<AbstractNode*>() << node;
    m_orderedChildrenReverse[node] = index;
    m_cachedChildren.clear();
}

void AbstractNode::addChild(AbstractNode* node) {
    Q_ASSERT(!node->m_parent);
    Q_ASSERT(!m_orderedChildrenReverse.contains(node));
    node->m_parent = this;
    m_appendedChildren.append(node);
    m_cachedChildren.clear();
}

void AbstractNode::removeChild(AbstractNode* node) {
    Q_ASSERT(node->m_parent == this);
    node->m_parent = 0;
    if(m_orderedChildrenReverse.contains(node)) {
        int index = m_orderedChildrenReverse.take(node);
        QList<AbstractNode*> nodes = m_orderedChildren[index];
        nodes.removeAll(node);
        m_orderedChildren[index] = nodes;
    } else {
        m_appendedChildren.removeAll(node);
    }
    m_cachedChildren.clear();
}

QList<AbstractNode*> AbstractNode::descendant() const {
    QList<AbstractNode*> list = children();
    foreach(AbstractNode* node, children())
        foreach(AbstractNode* n, node->descendant())
            list.append(n);
    return list;
}

QList<AbstractNode*> AbstractNode::peers() const {
    QList<AbstractNode*> list;
    if (m_parent)
        foreach(AbstractNode* node, m_parent->children())
            if(node != this)
                list.append(node);
    return list;
}                

/****************************************************************************************************/

void PointNode::dump(Context* context, int level) {
    DEBUG_DUMP << "type=" << m_type << "modelId=" << m_modelId << "cxnId=" << m_cxnId;
    AbstractNode::dump(context, level);
}

void PointNode::readElement(Context* context, MsooXmlDiagramReader* reader) {
    if (reader->isStartElement()) {
        if (reader->qualifiedName() == QLatin1String("dgm:prSet")) {
            //TODO
        } else if (reader->qualifiedName() == QLatin1String("dgm:spPr")) {
            //TODO
        } else if (reader->qualifiedName() == QLatin1String("dgm:t")) {
            readTextBody(context, reader);
        }
    }
}

void PointNode::readAll(Context* context, MsooXmlDiagramReader* reader) {
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

void PointNode::readTextBody(Context*, MsooXmlDiagramReader* reader) {
    m_text.clear();
    enum { Start, Paragraph, TextRun } s;
    s = Start;
    while (!reader->atEnd()) {
        reader->readNext();
        if(reader->isEndElement() && reader->qualifiedName() == QLatin1String("dgm:t")) break;
        switch(s) {
            case Start:
                if (reader->isStartElement() && reader->qualifiedName() == QLatin1String("a:p")) s = Paragraph;
                break;
            case Paragraph:
                if (reader->qualifiedName() == QLatin1String("a:r")) // text run
                s = reader->isStartElement() ? TextRun : Start;
            break;
            case TextRun:
                if (reader->qualifiedName() == QLatin1String("a:t")) {
                    if(reader->isStartElement()) {
                        if(!m_text.isEmpty()) m_text += ' '; // concat multiple strings into one result
                        m_text += reader->readElementText();
                    } else
                        s = Paragraph;
                }
                break;
        }
        
    }
}

/****************************************************************************************************/

void PointListNode::dump(Context* context, int level) {
    //DEBUG_DUMP;
    AbstractNode::dump(context, level);
}

/****************************************************************************************************/

void PointListNode::readElement(Context* context, MsooXmlDiagramReader* reader) {
    if (reader->isStartElement()) {
        if (reader->qualifiedName() == QLatin1String("dgm:pt")) {
            PointNode *n = new PointNode;
            addChild(n);
            n->readAll(context, reader);
        }
    }
}

void ConnectionNode::dump(Context*, int level) {
    DEBUG_DUMP << "modelId=" << m_modelId << "type=" << m_type << "srcId=" << m_srcId << "destId=" << m_destId;
    //AbstractNode::dump(context, level);
}

void ConnectionNode::readElement(Context* context, MsooXmlDiagramReader* reader) {
    if (reader->isStartElement()) {
        if (reader->qualifiedName() == QLatin1String("dgm:cxn")) {
            ConnectionNode *n = new ConnectionNode;
            addChild(n);
            n->readAll(context, reader);
        }
    }
}

void ConnectionNode::readAll(Context* context, MsooXmlDiagramReader* reader) {
    const QXmlStreamAttributes attrs(reader->attributes());
    TRY_READ_ATTR_WITHOUT_NS_INTO(modelId, m_modelId)
    TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
    if (m_type.isEmpty()) m_type = "parOf";
    TRY_READ_ATTR_WITHOUT_NS_INTO(srcId, m_srcId)
    TRY_READ_ATTR_WITHOUT_NS_INTO(destId, m_destId)
    TRY_READ_ATTR_WITHOUT_NS_INTO(presId, m_presId)
    TRY_READ_ATTR_WITHOUT_NS_INTO(parTransId, m_parTransId)
    TRY_READ_ATTR_WITHOUT_NS_INTO(sibTransId, m_sibTransId)
    TRY_READ_ATTR_WITHOUT_NS(srcOrd)
    TRY_READ_ATTR_WITHOUT_NS(destOrd)
    m_srcOrd = srcOrd.toInt();
    m_destOrd = destOrd.toInt();
    AbstractNode::readAll(context, reader);
}

/****************************************************************************************************/

void ConnectionListNode::dump(Context* context, int level) {
    //DEBUG_DUMP;
    AbstractNode::dump(context, level);
}

void ConnectionListNode::readElement(Context* context, MsooXmlDiagramReader* reader) {
    if (reader->isStartElement()) {
        if (reader->qualifiedName() == QLatin1String("dgm:cxn")) {
            ConnectionNode *n = new ConnectionNode;
            addChild(n);
            n->readAll(context, reader);
        }
    }
}

/****************************************************************************************************/

AbstractAtom::AbstractAtom(const QString &tagName) : QSharedData(), m_tagName(tagName) {}
AbstractAtom::~AbstractAtom() {}

void AbstractAtom::dump(Context* context, int level) {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
        atom->dump(context, level + 1);
    }
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
        } else if (reader->qualifiedName() == QLatin1String("dgm:ruleLst")) {
            node = new RuleListAtom;
        } else {
            kDebug()<<"TODO atom="<<m_tagName<<"qualifiedName="<<reader->qualifiedName();
        }
        
        if (node) {
            QExplicitlySharedDataPointer<AbstractAtom> ptr(node);
            addChild(ptr);
            ptr->readAll(context, reader);
        }
    }
}

void AbstractAtom::readAll(Context* context, MsooXmlDiagramReader* reader) {
    while (!reader->atEnd()) {
        QXmlStreamReader::TokenType tokenType = reader->readNext();
        if(tokenType == QXmlStreamReader::Invalid || tokenType == QXmlStreamReader::EndDocument) break;
        if(!reader->isStartElement() && reader->qualifiedName() == m_tagName) break;
        readElement(context, reader);
    }
}

void AbstractAtom::build(Context* context) {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
        atom->build(context);
}

void AbstractAtom::layoutAtom(Context* context) {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
        atom->layoutAtom(context);
}

void AbstractAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
        atom->writeAtom(context, xmlWriter, styles);
}

QExplicitlySharedDataPointer<AbstractAtom> AbstractAtom::parent() const { return m_parent; } 
QVector< QExplicitlySharedDataPointer<AbstractAtom> > AbstractAtom::children() const { return m_children; }

void AbstractAtom::addChild(AbstractAtom* node) {
    addChild(QExplicitlySharedDataPointer<AbstractAtom>(node));
}

void AbstractAtom::addChild(QExplicitlySharedDataPointer<AbstractAtom> node) {
    node->m_parent = this;
    m_children.append(node);
}

void AbstractAtom::removeChild(QExplicitlySharedDataPointer<AbstractAtom> node) {
    const int index = m_children.indexOf(node);
    Q_ASSERT(index >= 0);
    m_children.remove(index);
    node->m_parent = QExplicitlySharedDataPointer<AbstractAtom>();
}

QList<AbstractNode*> AbstractAtom::fetchAxis(Context* context, const QString& _axis, const QString &_ptType, const QString& _start, const QString& _count, const QString& _step) const {
    const QStringList axisList = _axis.split(' ', QString::SkipEmptyParts);
    const QStringList typeList = _ptType.split(' ', QString::SkipEmptyParts);
    const QStringList startList = _start.split(' ', QString::SkipEmptyParts);
    const QStringList countList = _count.split(' ', QString::SkipEmptyParts);
    const QStringList stepList = _step.split(' ', QString::SkipEmptyParts);
    QList<AbstractNode*> result;
    result << context->currentNode();
    for(int i = 0; i < axisList.count(); ++i) {
        result = fetchAxis(context, result, axisList[i], typeList.value(i), startList.value(i), countList.value(i), stepList.value(i));
    }
    return result;
}

QList<AbstractNode*> AbstractAtom::fetchAxis(Context* context, QList<AbstractNode*> list, const QString& axis, const QString &ptType, const QString& start, const QString& count, const QString& step) const {
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

QList<AbstractNode*> AbstractAtom::foreachAxis(Context*, const QList<AbstractNode*> &list, int start, int count, int step) const {
    QList<AbstractNode*> result;
    const int _start = qMax(0, start - 1);
    const int _step = qMax(1, step);
    for(int i = _start; i < list.count(); i += _step) {
        result.append(list[i]);
        if(/*count > 0 &&*/ result.count() == count) break;
    }
    return result;
}

/****************************************************************************************************/

AlgorithmAtom* AlgorithmAtom::clone() {
    AlgorithmAtom* atom = new AlgorithmAtom;
    atom->m_type = m_type;
    atom->m_params = m_params;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone());
    return atom;
}

void AlgorithmAtom::dump(Context* context, int level) {
    DEBUG_DUMP << "type=" << m_type;
    AbstractAtom::dump(context, level);
}

void AlgorithmAtom::readAll(Context* context, MsooXmlDiagramReader* reader) {
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

void AlgorithmAtom::readElement(Context*, MsooXmlDiagramReader* reader) {
    if (reader->isStartElement()) {
        if (reader->qualifiedName() == QLatin1String("dgm:param")) {
            const QXmlStreamAttributes attrs(reader->attributes());
            TRY_READ_ATTR_WITHOUT_NS(type)
            TRY_READ_ATTR_WITHOUT_NS(val)
            m_params[type] = val;
        }
    }
}

/****************************************************************************************************/

LayoutNodeAtom* LayoutNodeAtom::clone() {
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
    atom->m_axis = m_axis;
    atom->m_rotateAngle = m_rotateAngle;
    atom->m_needsReinit = m_needsReinit;
    atom->m_needsRelayout = m_needsRelayout;
    atom->m_childNeedsRelayout = m_childNeedsRelayout;
    atom->m_variables = m_variables;
    atom->m_firstLayout = m_firstLayout;
    return atom;
}

void LayoutNodeAtom::dump(Context* context, int level) {
    DEBUG_DUMP << "name=" << m_name << "constraintsCount=" << m_constraints.count() << "variables=" << m_variables << "values=" << finalValues();
    AbstractAtom::dump(context, level);
}

void LayoutNodeAtom::readAll(Context* context, MsooXmlDiagramReader* reader) {
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

void LayoutNodeAtom::build(Context* context) {
    //TODO what do to with the axis? How to use them in the layout itself?
    QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;
    AbstractAtom::build(context);
    context->m_parentLayout = oldLayout;
}

void LayoutNodeAtom::layoutAtom(Context* context) {
    AlgorithmBase *algbase = 0;

    switch(m_algorithm->m_type) {
        case AlgorithmAtom::UnknownAlg: kWarning() << "Layout with name=" << m_name << "defines an unknown algorithm."; break;
        case AlgorithmAtom::CompositeAlg: algbase = new CompositeAlgorithm; break;
        case AlgorithmAtom::ConnectorAlg: algbase = new ConnectorAlgorithm; break;
        case AlgorithmAtom::CycleAlg: algbase = new CycleAlgorithm; break;
        case AlgorithmAtom::HierChildAlg: kWarning() << "TODO AlgorithmAtom::HierChildAlg"; break;
        case AlgorithmAtom::HierRootAlg: kWarning() << "TODO AlgorithmAtom::HierRootAlg"; break;
        case AlgorithmAtom::LinearAlg: algbase = new LinearAlgorithm; break;
        case AlgorithmAtom::PyramidAlg: kWarning() << "TODO AlgorithmAtom::PyramidAlg"; break;
        case AlgorithmAtom::SnakeAlg: kWarning() << "TODO AlgorithmAtom::SnakeAlg"; break;
        case AlgorithmAtom::SpaceAlg: algbase = new SpaceAlg; break;
        case AlgorithmAtom::TextAlg: algbase = new TextAlgorithm; break;
        //default: break;
    }

    if(algbase) {
        QExplicitlySharedDataPointer<LayoutNodeAtom> thisPtr(this);
        algbase->doInit(context, thisPtr);
    }

    if(m_needsRelayout) {
        m_needsRelayout = false;
        m_childNeedsRelayout = true;
        if(algbase) {
            algbase->doLayout();
        }
    }
    
    if(m_childNeedsRelayout) {
        m_childNeedsRelayout = false;
        if(algbase) {
            algbase->doLayoutChildren();
        }
    }
    
    delete algbase;
}

void LayoutNodeAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
    AlgorithmAtom::Algorithm algorithm = AlgorithmAtom::UnknownAlg;
    QMap<QString, QString> params;
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

QList< QExplicitlySharedDataPointer<ConstraintAtom> > LayoutNodeAtom::constraints() const {
    return m_constraints;
}

void LayoutNodeAtom::addConstraint(QExplicitlySharedDataPointer<ConstraintAtom> constraint) {
    m_constraints.append(constraint);
    setNeedsRelayout(true);
}

QExplicitlySharedDataPointer<AlgorithmAtom> LayoutNodeAtom::algorithm() const {
    return m_algorithm;
}

void LayoutNodeAtom::setAlgorithm(QExplicitlySharedDataPointer<AlgorithmAtom> algorithm) {
    m_algorithm = algorithm;
    setNeedsRelayout(true);
}

QList<AbstractNode*> LayoutNodeAtom::axis() const { return m_axis; }

void LayoutNodeAtom::setAxis(Context* context, const QList<AbstractNode*> &axis) {
    Q_UNUSED(context);
    m_axis = axis;
    /*
    QExplicitlySharedDataPointer<LayoutNodeAtom> ptr(this);
    foreach(AbstractNode* node, axis) {
        PointNode* n = dynamic_cast<PointNode*>(node);
        Q_ASSERT(n);
        if(n->m_type != "node") continue;
        Q_ASSERT(!n->m_modelId.isEmpty());
        Q_ASSERT(!context->m_pointLayoutMap.contains(n->m_modelId));
        context->m_pointLayoutMap[n->m_modelId] = ptr;
    }
    */
    setNeedsRelayout(true);
}

void LayoutNodeAtom::setNeedsReinit(bool needsReinit) {
    if(m_needsReinit == needsReinit) return;
    m_needsReinit = needsReinit;
    if(m_needsReinit) // if we need to be re-initialized then our children need to be too
        foreach(QExplicitlySharedDataPointer<AbstractAtom> child, children())
            if(LayoutNodeAtom* childLayoutAtom = dynamic_cast<LayoutNodeAtom*>(child.data()))
                childLayoutAtom->setNeedsReinit(true);
}

void LayoutNodeAtom::setNeedsRelayout(bool needsRelayout) {
    if(needsRelayout == m_needsRelayout) return;
    m_needsRelayout = needsRelayout;
    if(m_needsRelayout) // let parent-layouts know that we need a relayout
        for(QExplicitlySharedDataPointer<AbstractAtom> parent = m_parent; parent.data(); parent = parent->parent())
            if(LayoutNodeAtom* parentLayoutAtom = dynamic_cast<LayoutNodeAtom*>(parent.data()))
                parentLayoutAtom->m_childNeedsRelayout = true;
}

AlgorithmAtom::Algorithm LayoutNodeAtom::algorithmType() const { return m_algorithm ? m_algorithm->m_type : AlgorithmAtom::UnknownAlg; }
QMap<QString,QString> LayoutNodeAtom::algorithmParams() const { return m_algorithm ? m_algorithm->m_params : QMap<QString,QString>(); }

QString LayoutNodeAtom::algorithmParam(const QString &name, const QString &defaultValue) const {
    return m_algorithm && m_algorithm->m_params.contains(name) ? m_algorithm->m_params[name] : defaultValue;
}

QString LayoutNodeAtom::variable(const QString &name, bool checkParents) const {
    if(m_variables.contains(name))
        return m_variables[name];
    if(checkParents)
        for(QExplicitlySharedDataPointer<AbstractAtom> parent = m_parent; parent.data(); parent = parent->parent())
            if(LayoutNodeAtom* parentLayoutAtom = dynamic_cast<LayoutNodeAtom*>(parent.data()))
                return parentLayoutAtom->variable(name, checkParents);
    return QString();
}

QMap<QString, QString> LayoutNodeAtom::variables() const { return m_variables; }
void LayoutNodeAtom::setVariable(const QString &name, const QString &value) { m_variables[name] = value; }

QMap<QString, qreal> LayoutNodeAtom::finalValues() const {
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

QPair<LayoutNodeAtom*,LayoutNodeAtom*> LayoutNodeAtom::neighbors() const
{
    LayoutNodeAtom* parentLayout = dynamic_cast<LayoutNodeAtom*>(parent().data());
    Q_ASSERT(parentLayout);
    QList<LayoutNodeAtom*> siblingLayouts;
    int myindex = -1;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, parentLayout->children()) {
        if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data())) {
            if(l == this) myindex = siblingLayouts.count();
            siblingLayouts.append(l);
        }
    }
    Q_ASSERT(myindex >= 0); // our parent should know about us else something is fundamental broken
    if(siblingLayouts.count() < 3) // if we don't have enough neighbors then abort and return NULL for both
        return QPair<LayoutNodeAtom*,LayoutNodeAtom*>(0,0);
    LayoutNodeAtom* srcAtom = siblingLayouts[ myindex>=1 ? myindex-1 : siblingLayouts.count()-1 ]; // warps around the list
    LayoutNodeAtom* dstAtom = siblingLayouts[ myindex+1<siblingLayouts.count() ? myindex+1 : 0 ];
    return QPair<LayoutNodeAtom*,LayoutNodeAtom*>(srcAtom,dstAtom);
}

qreal LayoutNodeAtom::distanceTo(LayoutNodeAtom* otherAtom) const
{
    QMap<QString, qreal> srcValues = this->m_values;
    QMap<QString, qreal> dstValues = otherAtom->m_values;
    QMap<QString, qreal> srcFactors = this->m_factors;
    QMap<QString, qreal> dstFactors = otherAtom->m_factors;
    QMap<QString, int> srcCountFactors = this->m_countFactors;
    QMap<QString, int> dstCountFactors = otherAtom->m_countFactors;
    qreal srcX = srcValues["l"] + srcValues["ctrX"];
    qreal srcY = srcValues["t"] + srcValues["ctrY"];
    qreal dstX = dstValues["l"] + dstValues["ctrX"];
    qreal dstY = dstValues["t"] + dstValues["ctrY"];
    qreal diffX = dstX - srcX;
    qreal diffY = dstY - srcY;
    return sqrt(diffX*diffX + diffY*diffY);
}

/****************************************************************************************************/

ConstraintAtom* ConstraintAtom::clone() {
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

void ConstraintAtom::dump(Context*, int level) {
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

void ConstraintAtom::readAll(Context*, MsooXmlDiagramReader* reader) {
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

void ConstraintAtom::build(Context* context) {
    QExplicitlySharedDataPointer<LayoutNodeAtom> layout = m_forName.isEmpty() ? context->m_parentLayout : context->m_layoutMap.value(m_forName);
    if(layout) {
        QExplicitlySharedDataPointer<ConstraintAtom> ptr(this);
        m_parent->removeChild(QExplicitlySharedDataPointer<AbstractAtom>(this));
        layout->addConstraint(ptr);
    }
    AbstractAtom::build(context);
}

ConstraintListAtom* ConstraintListAtom::clone() {
    ConstraintListAtom* atom = new ConstraintListAtom;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone());
    return atom;
}

void ConstraintListAtom::dump(Context* context, int level) {
    AbstractAtom::dump(context, level);
}

void ConstraintListAtom::readElement(Context* context, MsooXmlDiagramReader* reader) {
    if (reader->isStartElement()) {
        if (reader->qualifiedName() == QLatin1String("dgm:constr")) {
            QExplicitlySharedDataPointer<AbstractAtom> node(new ConstraintAtom);
            addChild(node);
            node->readAll(context, reader);
        }
    }
}

/****************************************************************************************************/

RuleAtom* RuleAtom::clone()
{
    RuleAtom* atom = new RuleAtom;
    atom->m_fact = m_fact;
    atom->m_for = m_for;
    atom->m_forName = m_forName;
    atom->m_max = m_max;
    atom->m_ptType = m_ptType;
    atom->m_type = m_type;
    atom->m_value = m_value;
    return atom;
}

void RuleAtom::dump(Context*, int level)
{
    QString s = QString("fact=%1 ").arg(m_fact);
    if(!m_for.isEmpty()) s += QString("for=%1 ").arg(m_for);
    if(!m_forName.isEmpty()) s += QString("forName=%1 ").arg(m_forName);
    if(!m_max.isEmpty()) s += QString("max=%1 ").arg(m_max);
    if(!m_ptType.isEmpty()) s += QString("ptType=%1 ").arg(m_ptType);
    if(!m_type.isEmpty()) s += QString("type=%1 ").arg(m_type);
    if(!m_value.isEmpty()) s += QString("val=%1 ").arg(m_value);
    DEBUG_DUMP << s;
}

void RuleAtom::readElement(Context*, MsooXmlDiagramReader* reader)
{
    const QXmlStreamAttributes attrs(reader->attributes());
    TRY_READ_ATTR_WITHOUT_NS_INTO(fact, m_fact)
    TRY_READ_ATTR_WITHOUT_NS_INTO(for, m_for)
    TRY_READ_ATTR_WITHOUT_NS_INTO(forName, m_forName)
    TRY_READ_ATTR_WITHOUT_NS_INTO(max, m_max)
    TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_ptType)
    TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
    TRY_READ_ATTR_WITHOUT_NS_INTO(val, m_value)
}
        
RuleListAtom* RuleListAtom::clone()
{
    RuleListAtom* atom = new RuleListAtom;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone());
    return atom;
}

void RuleListAtom::dump(Context* context, int level)
{
    AbstractAtom::dump(context, level);
}

void RuleListAtom::readElement(Context* context, MsooXmlDiagramReader* reader)
{
    if (reader->isStartElement()) {
        if (reader->qualifiedName() == QLatin1String("dgm:rule")) {
            QExplicitlySharedDataPointer<AbstractAtom> node(new RuleAtom);
            addChild(node);
            node->readAll(context, reader);
        }
    }
}

/****************************************************************************************************/

ShapeAtom* ShapeAtom::clone() {
    ShapeAtom* atom = new ShapeAtom;
    atom->m_type = m_type;
    atom->m_blip = m_blip;
    atom->m_hideGeom = m_hideGeom;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone());
    return atom;
}

void ShapeAtom::dump(Context* context, int level) {
    QExplicitlySharedDataPointer<LayoutNodeAtom> l = context->m_parentLayout;
    DEBUG_DUMP << "type=" << m_type << "hideGeom=" << m_hideGeom << "blip=" << m_blip;
    AbstractAtom::dump(context, level);
}

void ShapeAtom::readAll(Context* context, MsooXmlDiagramReader* reader) {
    const QXmlStreamAttributes attrs(reader->attributes());
    TRY_READ_ATTR_WITHOUT_NS_INTO(type, m_type)
    //if (m_type.isEmpty()) m_type = "obj";
    TRY_READ_ATTR_WITHOUT_NS_INTO(blip, m_blip)
    TRY_READ_ATTR_WITHOUT_NS(hideGeom)
    m_hideGeom = hideGeom.toInt();
    AbstractAtom::readAll(context, reader);
}

//TODO use filters/libmso/ODrawToOdf.h
void ShapeAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
    Q_ASSERT(context->m_parentLayout);
    if(m_type.isEmpty() || m_hideGeom) return;

    QMap<QString,QString> params = context->m_parentLayout->algorithmParams();

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
        Q_ASSERT(w >= sibSp);
        Q_ASSERT(h >= sibSp);
        if(w >= sibSp && h >= sibSp) {
            x += sibSp;
            y += sibSp;
            w -= sibSp;
            h -= sibSp;
        } else {
            kWarning()<<"Sibling spacing is bigger then width/height! Skipping sibSp-value! width="<<w<<"height="<<h<<"sibSp="<<sibSp;
            context->m_parentLayout->dump(context, 10);
        }
    }
    
    DEBUG_WRITE << "type=" << m_type << "blip=" << m_blip << "hideGeom=" << m_hideGeom << "geometry=" << x+cx << y+cy << w << h;
    Q_ASSERT(x >= 0.0);
    Q_ASSERT(y >= 0.0);
    Q_ASSERT(w > 0.0);
    Q_ASSERT(h > 0.0);
    Q_ASSERT(cx >= 0.0);
    Q_ASSERT(cy >= 0.0);

    xmlWriter->startElement("draw:custom-shape");
    xmlWriter->addAttribute("draw:layer", "layout");
    if (!context->m_parentLayout->m_name.isEmpty())
        xmlWriter->addAttribute("draw:name", context->m_parentLayout->m_name);

    KoGenStyle style = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    style.addProperty("draw:fill", "solid" /*none*/, KoGenStyle::GraphicType);
    style.addProperty("draw:opacity", "50%");

    const int m_svgX = x+cx;
    const int m_svgY = y+cy;
    const int rotateAngle = context->m_parentLayout->m_rotateAngle; //0=right 45=bottom 90=left 135=top 180=right
    if(rotateAngle == 0) {
        xmlWriter->addAttribute("svg:x", QString("%1px").arg(m_svgX));
        xmlWriter->addAttribute("svg:y", QString("%1px").arg(m_svgY));
    }
    xmlWriter->addAttribute("svg:width", QString("%1px").arg(w));
    xmlWriter->addAttribute("svg:height", QString("%1px").arg(h));
    if(rotateAngle != 0) {
        QMatrix matrix;
        matrix.translate(m_svgX + 0.5 * w, m_svgY + 0.5 * h);
        matrix.rotate(rotateAngle);
        matrix.translate(-0.5 * w, -0.5 * h);
        xmlWriter->addAttribute("draw:transform", QString("matrix(%1 %2 %3 %4 %5pt %6pt)").arg(matrix.m11()).arg(matrix.m12()).arg(matrix.m21()).arg(matrix.m22()).arg(matrix.dx()) .arg(matrix.dy()));
    }

    kDebug()<<"################### context"<<context->m_parentLayout->m_name<<m_svgX<<m_svgY<<w<<h<<rotateAngle;

    if (m_type == QLatin1String("conn")) {
        // xmlWriter->addAttribute("draw:transform", "translate(-0.12039in -0.15285in) rotate(-0.62832) translate(3.12798in 0.91174in)");
        // xmlWriter->addAttribute("draw:transform", "translate(-0.12039in -0.15285in) rotate(-5.02655) translate(3.47562in 1.95573in)");
        // xmlWriter->addAttribute("draw:transform", "");
        // xmlWriter->addAttribute("draw:transform", "translate(-0.12039in -0.15285in) rotate(-1.25664) translate(1.69526in 1.9687in)");
        // xmlWriter->addAttribute("draw:transform", "translate(-0.12039in -0.15285in) rotate(-5.65487) translate(2.02766in 0.91975in)");
        /*
        <draw:custom-shape draw:id="id1" draw:style-name="a5" draw:transform="translate(-0.12039in -0.15285in) rotate(-0.62832) translate(3.12798in 0.91174in)" svg:height="0.30569in" svg:width="0.24079in">
            <draw:enhanced-geometry draw:enhanced-path="M 0 55905 L 110087 55905 110087 0 220174 139764 110087 279527 110087 223622 0 223622 0 55905 Z N"            draw:glue-point-leaving-directions="-90, -90, -90, -90, -90, -90, -90, -90" draw:glue-points="?f16 ?f17 ?f18 ?f17 ?f18 ?f19 ?f20 ?f21 ?f18 ?f22 ?f18 ?f23 ?f16 ?f23 ?f16 ?f17" draw:text-areas="?f24 ?f26 ?f25 ?f27" draw:type="non-primitive" svg:viewBox="0 0 220174 279527" xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0">
        <draw:custom-shape draw:id="id3" draw:style-name="a11" draw:transform="translate(-0.12039in -0.15285in) rotate(-5.02655) translate(3.47562in 1.95573in)" svg:height="0.30569in" svg:width="0.24079in">
            <draw:enhanced-geometry draw:enhanced-path="M 220174 223622 L 110087 223622 110087 279527 0 139763 110087 0 110087 55905 220174 55905 220174 223622 Z N" draw:glue-point-leaving-directions="-90, -90, -90, -90, -90, -90, -90, -90" draw:glue-points="?f16 ?f17 ?f18 ?f17 ?f18 ?f19 ?f20 ?f21 ?f18 ?f22 ?f18 ?f23 ?f16 ?f23 ?f16 ?f17" draw:text-areas="?f24 ?f26 ?f25 ?f27" draw:type="non-primitive" svg:viewBox="0 0 220174 279527" xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0">
        <draw:custom-shape draw:id="id5" draw:style-name="a17" svg:height="0.3057in" svg:width="0.24079in" svg:x="2.46976in" svg:y="2.45612in">
            <draw:enhanced-geometry draw:enhanced-path="M 220174 223622 L 110087 223622 110087 279527 0 139763 110087 0 110087 55905 220174 55905 220174 223622 Z N" draw:glue-point-leaving-directions="-90, -90, -90, -90, -90, -90, -90, -90" draw:glue-points="?f16 ?f17 ?f18 ?f17 ?f18 ?f19 ?f20 ?f21 ?f18 ?f22 ?f18 ?f23 ?f16 ?f23 ?f16 ?f17" draw:text-areas="?f24 ?f26 ?f25 ?f27" draw:type="non-primitive" svg:viewBox="0 0 220174 279527" xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0">
        <draw:custom-shape draw:id="id7" draw:style-name="a23" draw:transform="translate(-0.12039in -0.15285in) rotate(-1.25664) translate(1.69526in 1.9687in)" svg:height="0.3057in" svg:width="0.24079in">
            <draw:enhanced-geometry draw:enhanced-path="M 220174 223622 L 110087 223622 110087 279527 0 139763 110087 0 110087 55905 220174 55905 220174 223622 Z N" draw:glue-point-leaving-directions="-90, -90, -90, -90, -90, -90, -90, -90" draw:glue-points="?f16 ?f17 ?f18 ?f17 ?f18 ?f19 ?f20 ?f21 ?f18 ?f22 ?f18 ?f23 ?f16 ?f23 ?f16 ?f17" draw:text-areas="?f24 ?f26 ?f25 ?f27" draw:type="non-primitive" svg:viewBox="0 0 220174 279527" xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0">
        <draw:custom-shape draw:id="id9" draw:style-name="a29" draw:transform="translate(-0.12039in -0.15285in) rotate(-5.65487) translate(2.02766in 0.91975in)" svg:height="0.30569in" svg:width="0.24079in">
            <draw:enhanced-geometry draw:enhanced-path="M 0 55905 L 110087 55905 110087 0 220174 139764 110087 279527 110087 223622 0 223622 0 55905 Z N"            draw:glue-point-leaving-directions="-90, -90, -90, -90, -90, -90, -90, -90" draw:glue-points="?f16 ?f17 ?f18 ?f17 ?f18 ?f19 ?f20 ?f21 ?f18 ?f22 ?f18 ?f23 ?f16 ?f23 ?f16 ?f17" draw:text-areas="?f24 ?f26 ?f25 ?f27" draw:type="non-primitive" svg:viewBox="0 0 220174 279527" xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0">
        */
        /*
        QList<AbstractNode*> axis = context->m_parentLayout->axis();
        foreach(AbstractNode* n, axis) n->dump(context,10);        
        Q_ASSERT(axis.count() == 1);
        Q_ASSERT(static_cast<PointNode*>(axis.first())->m_type == "sibTrans");
        const QString cxnId = static_cast<PointNode*>(axis.first())->m_cxnId;
        Q_ASSERT(!static_cast<PointNode*>(axis.first())->m_cxnId.isEmpty());
        ConnectionNode* connection = 0;
        foreach(AbstractNode* node, context->m_connections->children()) {
            ConnectionNode* n = dynamic_cast<ConnectionNode*>(node);
            if(n && n->m_modelId == cxnId) {
                connection = n;
                break;
            }
        }
        Q_ASSERT(connection);
        //connection->m_srcId;
        //connection->m_destId;
        */
        style.addProperty("draw:fill-color", "#9999ff");
    } else {
        style.addProperty("draw:fill-color", "#3333ff");
    }

    const QString styleName = styles->insert(style);
    xmlWriter->addAttribute("draw:style-name", styleName);
    //xmlWriter->addAttribute("draw:text-style-name", "P2");

    QList<PointNode*> textlist;
    foreach(AbstractNode* n, context->m_parentLayout->axis()) {
        if(PointNode* pn = dynamic_cast<PointNode*>(n))
            if(!pn->m_text.isEmpty())
                textlist.append(pn);
                
    }
    if(!textlist.isEmpty()) {
        xmlWriter->startElement("text:p");
        foreach(PointNode* pn, textlist) {
            xmlWriter->startElement("text:span");
            xmlWriter->addTextNode(pn->m_text);
            xmlWriter->endElement();
        }
        xmlWriter->endElement();
    }

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
    } else if (m_type == QLatin1String("rect")) {
        xmlWriter->startElement("draw:enhanced-geometry");
        xmlWriter->addAttribute("draw:enhanced-path", "M 0 0 L 1393031 0 1393031 557212 0 557212 0 0 Z N");
        xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90");
        xmlWriter->addAttribute("draw:glue-points", "?f12 ?f13 ?f14 ?f13 ?f14 ?f15 ?f12 ?f15 ?f12 ?f13");
        xmlWriter->addAttribute("draw:text-areas", "?f16 ?f18 ?f17 ?f19");
        xmlWriter->addAttribute("draw:type", "non-primitive");
        xmlWriter->addAttribute("svg:viewBox", "0 0 1393031 557212");
        xmlWriter->endElement();
    } else if (m_type == QLatin1String("roundRect")) {
        xmlWriter->startElement("draw:enhanced-geometry");
        xmlWriter->addAttribute("draw:enhanced-path", "M 0 97707 C 0 71793 10294 46941 28618 28618 46942 10294 71794 0 97707 0 L 804191 0 C 830105 0 854957 10294 873280 28618 891604 46942 901898 71794 901898 97707 L 901898 488526 C 901898 514440 891604 539292 873280 557615 854956 575939 830104 586233 804191 586233 L 97707 586233 C 71793 586233 46941 575939 28618 557615 10294 539291 0 514439 0 488526 L 0 97707 Z N");
        xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90");
        xmlWriter->addAttribute("draw:glue-points", "?f20 ?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f25 ?f27 ?f23 ?f28 ?f21 ?f28 ?f29 ?f27 ?f30 ?f26 ?f31 ?f24 ?f31 ?f22 ?f30 ?f20 ?f29 ?f20 ?f21");
        xmlWriter->addAttribute("draw:text-areas", "?f32 ?f34 ?f33 ?f35");
        xmlWriter->addAttribute("draw:type", "non-primitive");
        xmlWriter->addAttribute("svg:viewBox", "0 0 901898 586233");
        xmlWriter->endElement();
    } else if (m_type == QLatin1String("conn")) { // Connection shape type
        enum EndStyle { Arrow, Auto, NoArrow };
        EndStyle endstyle = Arrow;
        if(params.contains("endSty")) {
            const QString endStyle = params["endSty"];
            if(endStyle == "auto") {
                //TODO specs say that the algorithm needs to define the style but it misses details how...
            } else if(endStyle == "noArr") {
                endstyle = NoArrow;
            }
        }
        if(endstyle == NoArrow) { // just a connecting line without arrow
            xmlWriter->startElement("draw:enhanced-geometry");
            xmlWriter->addAttribute("draw:enhanced-path", "M 1627875 92938 A ?f54 ?f55 ?f56 ?f57 1627875 92938 ?f51 ?f53  W ?f58 ?f59 ?f60 ?f61 1627875 92938 ?f51 ?f53 N");
            //xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90");
            //xmlWriter->addAttribute("draw:glue-points", "?f20 ?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f25 ?f27 ?f23 ?f28 ?f21 ?f28 ?f29 ?f27 ?f30 ?f26 ?f31 ?f24 ?f31 ?f22 ?f30 ?f20 ?f29 ?f20 ?f21");
            xmlWriter->addAttribute("draw:text-areas", "?f11 ?f13 ?f12 ?f14");
            xmlWriter->addAttribute("draw:type", "non-primitive");
            xmlWriter->addAttribute("svg:viewBox", "0 0 2341473 2341473");
            xmlWriter->endElement();
        } else { // arrow-right
            xmlWriter->startElement("draw:enhanced-geometry");
            xmlWriter->addAttribute("draw:enhanced-path", "M 0 55905 L 110087 55905 110087 0 220174 139764 110087 279527 110087 223622 0 223622 0 55905 Z N");
            xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90, -90, -90, -90");
            xmlWriter->addAttribute("draw:glue-points", "?f16 ?f17 ?f18 ?f17 ?f18 ?f19 ?f20 ?f21 ?f18 ?f22 ?f18 ?f23 ?f16 ?f23 ?f16 ?f17");
            xmlWriter->addAttribute("draw:text-areas", "?f24 ?f26 ?f25 ?f27");
            xmlWriter->addAttribute("draw:type", "non-primitive");
            xmlWriter->addAttribute("svg:viewBox", "0 0 220174 279527");
            xmlWriter->endElement();
        }
    } else {
        kWarning() << "TODO shape type=" << m_type;
        //Q_ASSERT_X(false, __FUNCTION__, QString("Handle shape of type=%1").arg(m_type).toUtf8());
    }

    xmlWriter->endElement(); // draw:custom-shape
}

/****************************************************************************************************/

PresentationOfAtom* PresentationOfAtom::clone() {
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

void PresentationOfAtom::dump(Context* context, int level) {
    DEBUG_DUMP << "axis=" << m_axis << "ptType=" << m_ptType << "count=" << m_count << "start=" << m_start << "step=" << m_step << "hideLastTrans=" << m_hideLastTrans;
    AbstractAtom::dump(context, level);
}

void PresentationOfAtom::readAll(Context* context, MsooXmlDiagramReader* reader) {
    const QXmlStreamAttributes attrs(reader->attributes());
    TRY_READ_ATTR_WITHOUT_NS_INTO(axis, m_axis)
    TRY_READ_ATTR_WITHOUT_NS_INTO(ptType, m_ptType)
    TRY_READ_ATTR_WITHOUT_NS_INTO(cnt, m_count)
    TRY_READ_ATTR_WITHOUT_NS_INTO(hideLastTrans, m_hideLastTrans)
    TRY_READ_ATTR_WITHOUT_NS_INTO(st, m_start)
    TRY_READ_ATTR_WITHOUT_NS_INTO(step, m_step)
    AbstractAtom::readAll(context, reader);
}

void PresentationOfAtom::build(Context* context) {
    QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
    context->m_parentLayout->setAxis(context, axis);
    AbstractAtom::build(context);
}

/****************************************************************************************************/

IfAtom* IfAtom::clone() {
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

void IfAtom::dump(Context* context, int level) {
    DEBUG_DUMP<<"name="<<m_name;
    //DEBUG_DUMP << "name=" << m_name << "argument=" << m_argument << "axis=" << m_axis << "count=" << m_count << "function=" << m_function << "hideLastTrans=" << m_hideLastTrans << "operator=" << m_operator << "dataPointType=" << m_ptType << "start=" << m_start << "step=" << m_step << "value=" << m_value;
    AbstractAtom::dump(context, level);
}

void IfAtom::readAll(Context* context, MsooXmlDiagramReader* reader) {
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

bool IfAtom::isTrue() const { return m_isTrue; } // is true or false?

bool IfAtom::testAtom(Context* context) {
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

    //kDebug()<<"name="<<m_name<<"value1="<<funcValue<<"value2="<<m_value<<"operator="<<m_operator<<"istrue="<<istrue;
    return istrue;
}

/****************************************************************************************************/

ChooseAtom* ChooseAtom::clone() {
    ChooseAtom* atom = new ChooseAtom;
    atom->m_name = m_name;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone());
    return atom;
}

void ChooseAtom::dump(Context* context, int level) {
    DEBUG_DUMP << "name=" << m_name;
    AbstractAtom::dump(context, level);
}

void ChooseAtom::readAll(Context* context, MsooXmlDiagramReader* reader) {
    const QXmlStreamAttributes attrs(reader->attributes());
    TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
    AbstractAtom::readAll(context, reader);
}

void ChooseAtom::readElement(Context* context, MsooXmlDiagramReader* reader) {
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

void ChooseAtom::build(Context* context) {
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

/****************************************************************************************************/

ForEachAtom* ForEachAtom::clone() {
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

void ForEachAtom::dump(Context* context, int level) {
    DEBUG_DUMP << "axis=" << m_axis << "count=" << m_count << "hideLastTrans=" << m_hideLastTrans << "name=" << m_name << "ptType=" << m_ptType << "reference=" << m_reference << "start=" << m_start << "step=" << m_step;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
        atom->dump(context, level + 1);
}

void ForEachAtom::readAll(Context* context, MsooXmlDiagramReader* reader) {
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

void ForEachAtom::build(Context* context) {
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
}

/****************************************************************************************************/

AlgorithmBase::AlgorithmBase() : m_context(0), m_oldCurrentNode(0) {}

AlgorithmBase::~AlgorithmBase() {
    if(m_context) {
        m_context->m_parentLayout = m_parentLayout;
        m_context->setCurrentNode(m_oldCurrentNode);
    }
}

Context* AlgorithmBase::context() const { return m_context; }
LayoutNodeAtom* AlgorithmBase::layout() const { return m_layout.data(); }
LayoutNodeAtom* AlgorithmBase::parentLayout() const { return m_parentLayout.data(); }

QList<LayoutNodeAtom*> AlgorithmBase::childLayouts() const
{
    QList<LayoutNodeAtom*> result;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_layout->children()) {
        if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data())) {
            result.append(l);
        }
    }
    return result;
}

void AlgorithmBase::setNodePosition(LayoutNodeAtom* l, qreal x, qreal y, qreal w, qreal h) {
    l->m_values["l"] = parentLayout()->m_values["l"];
    l->m_values["t"] = parentLayout()->m_values["t"];
    l->m_values["w"] = w;
    l->m_values["h"] = h;
    l->m_factors["l"] = l->m_factors["t"] = 1.0;
    l->m_countFactors["l"] = l->m_countFactors["t"] = 1;
    l->m_values["ctrX"] = x;
    l->m_values["ctrY"] = y;
    l->m_needsReinit = false; // we initialized things above already
    l->m_needsRelayout = true; // but we clearly need a layout now
    l->m_childNeedsRelayout = true; // and our children need to be relayouted too now
}

void AlgorithmBase::doInit(Context* context, QExplicitlySharedDataPointer<LayoutNodeAtom> layout) {
    m_context = context;
    m_layout = layout;
    m_parentLayout = m_context->m_parentLayout;
    m_context->m_parentLayout = m_layout;
    m_oldCurrentNode = m_context->currentNode();
    virtualDoInit();
}

void AlgorithmBase::doLayout() {
    virtualDoLayout();
}

void AlgorithmBase::doLayoutChildren() {
    virtualDoLayoutChildren();
}

void AlgorithmBase::virtualDoInit() {
    if(layout()->m_needsReinit) {
        layout()->m_needsReinit = false; // initialization done
        layout()->m_values = parentLayout()->m_values;
        layout()->m_factors = parentLayout()->m_factors;
        layout()->m_countFactors = parentLayout()->m_countFactors;
    }
    QMap<QString, qreal> values = parentLayout()->finalValues();
    Q_ASSERT(values["l"] >= 0.0);
    Q_ASSERT(values["t"] >= 0.0);
    Q_ASSERT(values["w"] > 0.0);
    Q_ASSERT(values["h"] > 0.0);
    Q_ASSERT(values["ctrX"] >= 0.0);
    Q_ASSERT(values["ctrY"] >= 0.0);
}

void AlgorithmBase::virtualDoLayout() {
    // QStringList axisnames;
    // foreach(AbstractNode* n, m_axis) axisnames.append(n->m_tagName);
    // kDebug() << "################# name=" << m_name << "algorithm=" << algorithm << "constraintCount=" << m_constraints.count() << "axis=" << axisnames;
    //this->dump(context, 10);

    // evaluate the constraints responsible for positioning and sizing.
    foreach(QExplicitlySharedDataPointer<ConstraintAtom> c, layout()->constraints()) {
        c->dump(context(), 2);

        qreal value = -1.0;
        if(!c->m_value.isEmpty()) {
            bool ok;
            value = c->m_value.toDouble(&ok);
            if(!ok) {
                kWarning() << "Layout with name=" << layout()->m_name << "defines none-double value=" << c->m_value;
                continue;
            }
        } else {
            QExplicitlySharedDataPointer<LayoutNodeAtom> ref = c->m_refForName.isEmpty() ? m_layout : context()->m_layoutMap.value(c->m_refForName);
            if(ref && ref != m_layout && (ref->m_needsReinit || ref->m_needsRelayout || ref->m_childNeedsRelayout)) {
                ref->layoutAtom(context());
                Q_ASSERT(!ref->m_needsReinit && !ref->m_needsRelayout && !ref->m_childNeedsRelayout);
            }

            QMap<QString, qreal> values = ref->finalValues();
            if(!c->m_refType.isEmpty()) {
                if(!values.contains(c->m_refType)) {
                    kWarning() << "Layout with name=" << layout()->m_name << "defines constraint for non-existing refType=" << c->m_refType;
                    continue;
                }
                value = values[c->m_refType];
            } else {
                // If there are no refType and no value defined then a default-value needs to be used.
                // See also http://social.msdn.microsoft.com/Forums/en/os_binaryfile/thread/7c823650-7913-4e63-970f-1c5dab3450c4
                if (c->m_type == "primFontSz") {
                    value = 36;
                } else if (c->m_type == "l") {
                    value = 0.0;
                } else if (c->m_type == "t") {
                    value = 0.0;
                //} else if (c->m_type == "w") {
                    //value = ;
                //} else if (c->m_type == "h") {
                    //value = ;
                } else if (c->m_type == "tMarg") {
                    value = values.contains("primFontSz") ? values["primFontSz"] * 0.56 : 0.0;
                } else if (c->m_type == "lMarg") {
                    value = values.contains("primFontSz") ? values["primFontSz"] * 0.40 : 0.0;
                } else if (c->m_type == "rMarg") {
                    value = values.contains("primFontSz") ? values["primFontSz"] * 0.42 : 0.0;
                } else if (c->m_type == "bMarg") {
                    value = values.contains("primFontSz") ? values["primFontSz"] * 0.60 : 0.0;
                } else if (c->m_type == "connDist") {
                    QPair<LayoutNodeAtom*,LayoutNodeAtom*> neighbors = layout()->neighbors();
                    LayoutNodeAtom* srcAtom = neighbors.first;
                    LayoutNodeAtom* dstAtom = neighbors.second;
                    value = (srcAtom && dstAtom) ? srcAtom->distanceTo(dstAtom) : 0.0;
                //} else if (c->m_type == "begPad") {
                    //value = ;
                //} else if (c->m_type == "endPad") {
                    //value = ;
                //} else if (c->m_type == "userA") {
                    //value = ;
                } else {
                    kDebug() << "TODO figure out defaults for constraint=" << c->m_type << "at layout=" << layout()->m_name;
                    //c->dump(context(),10);
                    //Q_ASSERT_X(false, __FUNCTION__, QString("Set defaults for constraint=%1").arg(c->m_type).toUtf8());
                }
            }
        }

//TODO 1) "op" isn't supported, 2) what happens if for=ch is defined but no forName? 3) etc.
#if 0
        QList<LayoutNodeAtom*> layouts;
        layouts << layout();
        if(!c->m_for.isEmpty() && c->m_forName.isEmpty()) {
            if(c->m_for == "ch") { // Child
            layouts.clear();
            foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, layout()->children())
                if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data()))
                    layouts.append(l);
            } else if(c->m_for == "des") { // Descendant
                //layouts.clear();
            } else if(c->m_for == "self") { // Ourself
                // not needed to do anything cause layouts contains our layout() already.
            }
        }
        foreach(LayoutNodeAtom* l, layouts) {
            if(value >= 0.0) l->m_values[c->m_type] = value;
            l->m_factors[c->m_type] += c->m_fact;
            l->m_countFactors[c->m_type] += 1;
        }  
#else
        if(value >= 0.0) {
            layout()->m_values[c->m_type] = value;
        }
        layout()->m_factors[c->m_type] += c->m_fact;
        layout()->m_countFactors[c->m_type] += 1;
#endif
    }
}

void AlgorithmBase::virtualDoLayoutChildren() {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, layout()->children()) {
        atom->layoutAtom(context());
    }
}

/****************************************************************************************************/

void ConnectorAlgorithm::virtualDoLayoutChildren() {
    // Get a list of all child-layouts of our parent to apply the connector-algorithm on our direct
    // neighbors. Also while on it also determinate our own position in that list.
    QPair<LayoutNodeAtom*,LayoutNodeAtom*> neighbors = layout()->neighbors();
    LayoutNodeAtom* srcAtom = neighbors.first;
    LayoutNodeAtom* dstAtom = neighbors.second;
    Q_ASSERT(srcAtom && dstAtom);
    if(!srcAtom || !dstAtom) {
        kWarning() << "The ConnectorAlgorithm attached to the layout with name=" << layout()->m_name << "cannot be applied cause if missing neighbors.";
        return;
    }

    QMap<QString, qreal> srcValues = srcAtom->m_values;
    QMap<QString, qreal> dstValues = dstAtom->m_values;
    QMap<QString, qreal> srcFactors = srcAtom->m_factors;
    QMap<QString, qreal> dstFactors = dstAtom->m_factors;
    QMap<QString, int> srcCountFactors = srcAtom->m_countFactors;
    QMap<QString, int> dstCountFactors = dstAtom->m_countFactors;
    qreal srcX = srcValues["l"]+srcValues["ctrX"];
    qreal srcY = srcValues["t"]+srcValues["ctrY"];
    qreal srcW = srcValues["w"];
    qreal srcH = srcValues["h"];
    qreal dstX = dstValues["l"]+dstValues["ctrX"];
    qreal dstY = dstValues["t"]+dstValues["ctrY"];
    qreal dstW = dstValues["w"];
    qreal dstH = dstValues["h"];
#if 0
    Q_ASSERT(srcX > 0.0);
    Q_ASSERT(srcY > 0.0);
    Q_ASSERT(dstX > 0.0);
    Q_ASSERT(dstY > 0.0);
#endif
    qreal srcCX = srcX + srcW/2.0;
    qreal srcCY = srcY + srcH/2.0;
    qreal dstCX = dstX + dstW/2.0;
    qreal dstCY = dstY + dstH/2.0;
    qreal angle = atan2(dstCY-srcCY,dstCX-srcCX)*180/M_PI;

    layout()->m_rotateAngle = angle / 2.0;
    
    AlgorithmBase::virtualDoLayoutChildren();
}

/****************************************************************************************************/

void CycleAlgorithm::virtualDoInit() {
    AlgorithmBase::virtualDoInit();
}

void CycleAlgorithm::virtualDoLayout() {
    AlgorithmBase::virtualDoLayout();

    QList<LayoutNodeAtom*> childs = childLayouts();
    Q_ASSERT(!childs.isEmpty());
    
    // Specifies the angle at which the first shape is placed. Angles are in degrees, measured clockwise from a line pointing straight upward from the center of the cycle.
    int startAngel = layout()->algorithmParam("stAng", "90").toInt() - 90; //-45;
    // Specifies the angle the cycle spans. Final shapealign text is placed at stAng+spanAng, unless spanAng=360. In that case, the algorithm places the text so that shapes do not overlap.
    int spanAngel = layout()->algorithmParam("spanAng", "360").toInt();
    // Specifies where to place nodes in relation to the center circle.
    bool firstNodeInCenter = layout()->algorithmParam("ctrShpMap", "none") == "fNode";

    LayoutNodeAtom* nodeInCenter = firstNodeInCenter ? childs.takeFirst() : 0;
    const int childsCount = qMax(1, childs.count());

    const qreal w = layout()->finalValues()["w"];
    const qreal h = layout()->finalValues()["h"];
    const qreal rx = w / 2.0;
    const qreal ry = h / 2.0;
    qreal num = 360.0 / childsCount;
    const bool inverse = startAngel > spanAngel;
    if(inverse) num = -num;

#if 1
    qreal dw = ( (2.0 * M_PI * rx) / childsCount );
    qreal dh = ( (2.0 * M_PI * ry) / childsCount );
    dw *= 0.9; dh *= 0.9; //TODO proper handle margins, spacings, etc.
#else
    qreal dw = w;
    qreal dh = h;
#endif

    if(nodeInCenter) {
        setNodePosition(nodeInCenter, rx, ry, dw, dh);
    }
    for(qreal degree = startAngel; (!childs.isEmpty()) && (inverse ? degree > spanAngel : degree <= spanAngel); degree -= num) {
        const qreal radian = (degree - 90.0) * (M_PI / 180.0);
        const qreal x = rx + cos(radian) * rx;
        const qreal y = ry + sin(radian) * ry;
        LayoutNodeAtom* l = childs.takeFirst();
        setNodePosition(l, x, y, dw, dh);
    }
}

void CycleAlgorithm::virtualDoLayoutChildren() {
    AlgorithmBase::virtualDoLayoutChildren();
}

/****************************************************************************************************/

void LinearAlgorithm::virtualDoLayout()
{
    QString direction = layout()->algorithmParam("linDir", "fromL");
    const qreal w = layout()->finalValues()["w"];
    const qreal h = layout()->finalValues()["h"];
    
    QList<LayoutNodeAtom*> childs = childLayouts();
    Q_ASSERT(!childs.isEmpty());
    const int childsCount = qMax(1, childs.count());

    int x, y, mx, my;
    x = y = mx = my = 0;
    if(direction == "fromL") {
        mx = w / childsCount;
    } else if(direction == "fromR") {
        x = w;
        mx = -(w / childsCount);
    } else if(direction == "fromT") {
        my = h / childsCount;
    } else if(direction == "fromB") {
        y = h;
        my = -(h / childsCount);
    }

    qreal dw = w / childsCount;
    qreal dh = h / childsCount;

    foreach(LayoutNodeAtom* l, childs) {
        setNodePosition(l, x, y, dw, dh);
        x += mx;
        y += my;
    }
}

/****************************************************************************************************/

void SpaceAlg::virtualDoLayout() {
    AlgorithmBase::virtualDoLayout();
}

/****************************************************************************************************/

void TextAlgorithm::virtualDoLayout() {
    AlgorithmBase::virtualDoLayout();
}

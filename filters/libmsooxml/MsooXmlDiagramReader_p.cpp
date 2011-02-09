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
#include <QTextStream>
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

#include <algorithm>

namespace MSOOXML { namespace Diagram {

#define DEBUG_DUMP \
    qDebug() << QString("%1Dgm::%2::%3").arg(QString(' ').repeated(level)).arg(typeid(this).name()).arg(__FUNCTION__) << this << "atom=" << m_tagName
#define DEBUG_WRITE \
    qDebug() << QString("Dgm::%1::%2").arg(typeid(this).name()).arg(__FUNCTION__) << "atom=" << m_tagName

}}

using namespace MSOOXML::Diagram;
using namespace MSOOXML;

/****************************************************************************************************/

Context::Context()
    : m_rootPoint(0)
    , m_connections(new ConnectionListNode)
    , m_rootLayout(new Diagram::LayoutNodeAtom)
    , m_parentLayout(m_rootLayout)
    , m_currentNode(0) {
}
//TODO make sure this is no memoryleak
Context::~Context() {
    //delete m_rootPoint;
    //delete m_connections;
}
        
AbstractNode* Context::currentNode() const { return m_currentNode; }
void Context::setCurrentNode(AbstractNode* node) { m_currentNode = node; }

qreal ValueCache::rectValue( const QString& name ) const
{
    Q_ASSERT( rect.isValid() );
    if ( name == "l")
        return rect.left();
    else if ( name == "r" )
        return rect.right();
    else if ( name == "w" )
        return rect.width();
    else if ( name == "h" )
        return rect.height();
    else if ( name == "t" )
        return rect.top();
    else if ( name == "b" )
        return rect.bottom();
    else if ( name == "ctrX" )
        return rect.center().rx();
    else if ( name == "ctrY" )
        return rect.center().ry();
    else
        return -1;
}
void ValueCache::setRectValue( const QString& name, qreal value )
{
    Q_ASSERT( rect.isValid() );
    if ( name == "l")
    {
        rect.moveLeft( value );
    }
    else if ( name == "r" )
        rect.setRight( value );
    else if ( name == "w" )
    {
        //Q_ASSERT( value > 0 );
        if ( value <  0 )
        {
            rect.setLeft( rect.right() + value );
            negativeWidth = true;
        }
        else
        {
            rect.setWidth( value );
            Q_ASSERT( negativeWidth == false );
            negativeWidth = false;
        }
    }
    else if ( name == "h" )
    {
        Q_ASSERT( value > 0 );
        rect.setHeight(value );
    }
    else if ( name == "t" )
        rect.moveTop( value );
    else if ( name == "b" )
        rect.setBottom( value );
    else if ( name == "ctrX" )
        rect.moveCenter( QPointF( rect.center().x() + value, rect.center().y() ) );
    else if ( name == "ctrY" )
        rect.moveCenter( QPointF( rect.center().x(), rect.center().y() + value ) );
    else
        Q_ASSERT( false );
    Q_ASSERT( rect.isValid() );
    Q_ASSERT( rect.left() >= 0 );
    Q_ASSERT( rect.top() >= 0 );
    unmodified = false;
}
//void setValue( const QString& name, qreal value );

/****************************************************************************************************/

AbstractNode::AbstractNode(const QString &tagName) : m_tagName(tagName), m_parent(0) {}
AbstractNode::~AbstractNode() { qDeleteAll(children()); }

void AbstractNode::dump(Context* context, int level) {    
    foreach(AbstractNode* node, children())
        node->dump(context, level + 1);    
}

void AbstractNode::dump( QTextStream& device ) {
  foreach(AbstractNode* node, children())
  {
    node->dump( device );
  }
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

static int addedChildsCounter2 = 0;

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
    LayoutNodeAtom* layNode = dynamic_cast< LayoutNodeAtom* >( node );
    if ( layNode && layNode->m_name == "Name0" )
    {
        ++addedChildsCounter2;
        Q_ASSERT( layNode->m_name != "compositeNode" );
    }
}

void AbstractNode::addChild(AbstractNode* node) {
    Q_ASSERT(!node->m_parent);
    Q_ASSERT(!m_orderedChildrenReverse.contains(node));
    node->m_parent = this;
    m_appendedChildren.append(node);
    m_cachedChildren.clear();
    LayoutNodeAtom* layNode = dynamic_cast< LayoutNodeAtom* >( this );
    if ( layNode && layNode->m_name == "Name0" )
    {
        ++addedChildsCounter2;
        Q_ASSERT( layNode->m_name != "compositeNode" );
    }
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
            prSet[ QLatin1String( "dgm:prSet" ) ] = reader->attributes().value( "phldrT" ).toString();
        } else if (reader->qualifiedName() == QLatin1String("dgm:spPr")) {
            //TODO
        } else if (reader->qualifiedName() == QLatin1String("dgm:t")) {
            readTextBody(context, reader);
        }
    }
}

void MSOOXML::Diagram::ConnectionNode::dump(QTextStream& device)
{
    foreach(AbstractNode* node, peers() )
    {
        ConnectionNode* connNode = dynamic_cast< ConnectionNode* > ( node );
        PointNode* pointNode = dynamic_cast< PointNode* > ( node );
        if ( connNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << connNode->m_tagName << connNode->m_modelId << "\"\n";
        else if ( pointNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << pointNode->m_tagName << pointNode->m_modelId << "\"\n";
    }
    foreach(AbstractNode* node, children())
    {
        ConnectionNode* connNode = dynamic_cast< ConnectionNode* > ( node );
        PointNode* pointNode = dynamic_cast< PointNode* > ( node );
        if ( connNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << connNode->m_tagName << connNode->m_modelId << "\"\n";
        else if ( pointNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << pointNode->m_tagName << pointNode->m_modelId << "\"\n";
    }
    MSOOXML::Diagram::AbstractNode::dump(device);
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
    //m_text.clear();
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
    if ( m_text.isEmpty() )
        m_text = prSet.value( QLatin1String("dgm:prSet") );
}

/****************************************************************************************************/

void PointListNode::dump(Context* context, int level) {
    //DEBUG_DUMP;
    AbstractNode::dump(context, level);
}

void PointListNode::dump( QTextStream& device )
{
    AbstractNode::dump( device );
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

void MSOOXML::Diagram::PointNode::dump(QTextStream& device)
{
    foreach(AbstractNode* node, peers() )
    {
        ConnectionNode* connNode = dynamic_cast< ConnectionNode* > ( node );
        PointNode* pointNode = dynamic_cast< PointNode* > ( node );
        if ( connNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << connNode->m_tagName << connNode->m_modelId << "\"[label=\"" << /*m_tagName << m_modelId << " " <<*/ m_text << "\"]\n";
        else if ( pointNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << pointNode->m_tagName << pointNode->m_modelId << "\"[label=\"" << /*m_tagName << m_modelId << " " <<*/ m_text << "\"]\n";
    }
    foreach(AbstractNode* node, children())
    {
        ConnectionNode* connNode = dynamic_cast< ConnectionNode* > ( node );
        PointNode* pointNode = dynamic_cast< PointNode* > ( node );
        if ( connNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << connNode->m_tagName << connNode->m_modelId << "\"[label=\"" << /*m_tagName << m_modelId << " " <<*/ m_text << "\"]\n";
        else if ( pointNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << pointNode->m_tagName << pointNode->m_modelId << "\"[label=\"" << /*m_tagName << m_modelId << " " <<*/ m_text << "\"]\n";
    }
    MSOOXML::Diagram::AbstractNode::dump(device);
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

void ConnectionListNode::dump( QTextStream& device ) {
    //DEBUG_DUMP;
    AbstractNode::dump( device );
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
            node = new ListAtom(reader->qualifiedName());
        } else if (reader->qualifiedName() == QLatin1String("dgm:ruleLst")) {
            node = new ListAtom(reader->qualifiedName());
        } else if (reader->qualifiedName() == QLatin1String("dgm:adjLst")) {
            node = new ListAtom(reader->qualifiedName());
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
    //typedef QList< QExplicitlySharedDataPointer< AbstractAtom > > SharedAtomList;
//     for( int i = 0; i < m_children.count(); ++i )
//         m_children[ i ]->build( context );
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
    {
      atom->build(context);
    }
}

void AbstractAtom::layoutAtom(Context* context) {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
    {
      if ( LayoutNodeAtom* layAtom = dynamic_cast< LayoutNodeAtom* >( atom.data() ) )
            layAtom->setNeedsRelayout( true );
        atom->layoutAtom(context);
    }
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

static int addedChildsCounter = 0;

void AbstractAtom::addChild(QExplicitlySharedDataPointer<AbstractAtom> node) {
    node->m_parent = this;
    LayoutNodeAtom* layNode = dynamic_cast< LayoutNodeAtom* >( this );
    if ( layNode && layNode->m_name == "Name0" )
    {
        ++addedChildsCounter;
        Q_ASSERT( layNode->m_name != "compositeNode" );
    }
    m_children.append(node);
}

void AbstractAtom::insertChild(int index, AbstractAtom* node) {
    insertChild(index, QExplicitlySharedDataPointer<AbstractAtom>(node));
}

void AbstractAtom::insertChild(int index, QExplicitlySharedDataPointer<AbstractAtom> node) {
    LayoutNodeAtom* layNode = dynamic_cast< LayoutNodeAtom* >( this );
    if ( layNode && layNode->m_name == "Name0" )
    {
        ++addedChildsCounter;
        Q_ASSERT( layNode->m_name != "compositeNode" );
    }
    node->m_parent = this;
    if ( index < m_children.count() )
      m_children.insert(index, node);
    else
      m_children.append( node );
}

void AbstractAtom::removeChild(QExplicitlySharedDataPointer<AbstractAtom> node) {
    const int index = m_children.indexOf(node);
    LayoutNodeAtom* layNode = dynamic_cast< LayoutNodeAtom* >( this );
    if ( layNode && layNode->m_name == "Name0" )
        --addedChildsCounter;
    /*
    
    int i = 0;
    const int count = m_children.count();
    Q_ASSERT( node != m_children[ 0 ] );
    */
//     bool found = false;
//     Q_FOREACH( QExplicitlySharedDataPointer<AbstractAtom> mnode, m_children )
//     {        
//         if ( mnode == node )
//             found = true;
//     }
//     int i = m_children.count();
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
            {
                result.append(n);
            }
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
    DEBUG_DUMP << "type=" << typeAsString() << "params=" << m_params;
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

QString AlgorithmAtom::typeAsString() const {
    QString s;
    switch(m_type) {
        case UnknownAlg: s = "Unknown"; break;
        case CompositeAlg: s = "Composite"; break;
        case ConnectorAlg: s = "Connector"; break;
        case CycleAlg: s = "Cycle"; break;
        case HierChildAlg: s = "HierChild"; break;
        case HierRootAlg: s = "HierRoot"; break;
        case LinearAlg: s = "Linear"; break;
        case PyramidAlg: s = "Pyramid"; break;
        case SnakeAlg: s = "Snake"; break;
        case SpaceAlg: s = "Space"; break;
        case TextAlg: s = "Text"; break;
    }
    return s;
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

class ConstraintPredicate
{
public:
    bool operator()( const QExplicitlySharedDataPointer<MSOOXML::Diagram::AbstractAtom> &value )
    {
        ListAtom *atom = dynamic_cast< ListAtom* >( value.data() );
        if ( !atom )
            return true;
        foreach( QExplicitlySharedDataPointer<AbstractAtom> val, atom->children() )
            if ( dynamic_cast< ConstraintAtom* >( val.data() ) )
                return false;
        
        return true;
    }
};

void LayoutNodeAtom::build(Context* context) {
    //TODO what do to with the axis? How to use them in the layout itself?
    QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;
    context->m_layoutPointMap[ this ] = context->currentNode();
    typedef QVector< QExplicitlySharedDataPointer<AbstractAtom> > AtomPList;
    AtomPList::iterator it = std::stable_partition( m_children.begin(), m_children.end(), ConstraintPredicate() );
    //Q_ASSERT( it == m_children.end() );
    //AtomPList constraintList;
    std::copy( it, m_children.end(), std::back_inserter( m_constraintsToBuild ) );
    m_children.erase( it, m_children.end() );
    AbstractAtom::build(context);
    foreach( QExplicitlySharedDataPointer<AbstractAtom> constr, m_constraintsToBuild )
    {
        constr->build( context );
    }
    m_constraintsToBuild.clear();
    context->m_parentLayout = oldLayout;
}

void LayoutNodeAtom::layoutAtom(Context* context) {
    delete m_algorithmImpl;
    m_algorithmImpl = 0;

    QExplicitlySharedDataPointer<AlgorithmAtom> alg = algorithm();
    switch(alg ? alg->m_type : AlgorithmAtom::UnknownAlg) {
        case AlgorithmAtom::UnknownAlg:
            kWarning() << "Layout with name=" << m_name << "defines an unknown algorithm.";
            // fall through and use the composite-algorithm
        case AlgorithmAtom::CompositeAlg: m_algorithmImpl = new CompositeAlgorithm; break;
        case AlgorithmAtom::ConnectorAlg: m_algorithmImpl = new ConnectorAlgorithm; break;
        case AlgorithmAtom::CycleAlg: m_algorithmImpl = new CycleAlgorithm; break;
        case AlgorithmAtom::HierChildAlg: m_algorithmImpl = new HierarchyAlgorithm(false); break;
        case AlgorithmAtom::HierRootAlg: m_algorithmImpl = new HierarchyAlgorithm(true); break;
        case AlgorithmAtom::LinearAlg: m_algorithmImpl = new LinearAlgorithm; break;
        case AlgorithmAtom::PyramidAlg: m_algorithmImpl = new LinearAlgorithm; break;
        case AlgorithmAtom::SnakeAlg: m_algorithmImpl = new SnakeAlgorithm; break;
        case AlgorithmAtom::SpaceAlg: m_algorithmImpl = new SpaceAlg; break;
        case AlgorithmAtom::TextAlg: m_algorithmImpl = new TextAlgorithm; break;
    }

    if(m_algorithmImpl) {
        QExplicitlySharedDataPointer<LayoutNodeAtom> thisPtr(this);
        m_algorithmImpl->doInit(context, thisPtr);
    }
    
    if(m_needsRelayout) {
        m_needsRelayout = false;
        m_childNeedsRelayout = true;
        if(m_algorithmImpl) {
            m_algorithmImpl->doLayout();
        }
    }
    
    if(m_childNeedsRelayout) {
        m_childNeedsRelayout = false;
        if(m_algorithmImpl) {
            m_algorithmImpl->doLayoutChildren();
        }
    }
    
}

void LayoutNodeAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
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

AbstractAlgorithm* LayoutNodeAtom::algorithmImpl() const {
    return m_algorithmImpl;
}

QExplicitlySharedDataPointer<AlgorithmAtom> LayoutNodeAtom::algorithm() const {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> child, children())
        if(AlgorithmAtom* alg = dynamic_cast<AlgorithmAtom*>(child.data()))
            return QExplicitlySharedDataPointer<AlgorithmAtom>(alg);
    return QExplicitlySharedDataPointer<AlgorithmAtom>();
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
            {
                childLayoutAtom->setNeedsReinit(true);
            }
}

void LayoutNodeAtom::setNeedsRelayout(bool needsRelayout) {
    if(needsRelayout == m_needsRelayout) return;
    m_needsRelayout = needsRelayout;
    if(m_needsRelayout) // let parent-layouts know that we need a relayout
        if(QExplicitlySharedDataPointer<LayoutNodeAtom> p = parentLayout())
            p->m_childNeedsRelayout = true;
}

AlgorithmAtom::Algorithm LayoutNodeAtom::algorithmType() const {
    if(QExplicitlySharedDataPointer<AlgorithmAtom> alg = algorithm())
        return alg->m_type;
    return AlgorithmAtom::UnknownAlg;
}

QMap<QString,QString> LayoutNodeAtom::algorithmParams() const {
    if(QExplicitlySharedDataPointer<AlgorithmAtom> alg = algorithm())
        return alg->m_params;
    return QMap<QString,QString>();
}

QString LayoutNodeAtom::algorithmParam(const QString &name, const QString &defaultValue) const {
    QMap<QString,QString> params = algorithmParams();
    return params.contains(name) ? params[name] : defaultValue;
}

QString LayoutNodeAtom::variable(const QString &name, bool checkParents) const {
    if(m_variables.contains(name))
        return m_variables[name];
    if(checkParents)
        if(QExplicitlySharedDataPointer<LayoutNodeAtom> p = parentLayout())
            return p->variable(name, checkParents);
    return QString();
}

QMap<QString, QString> LayoutNodeAtom::variables() const { return m_variables; }
void LayoutNodeAtom::setVariable(const QString &name, const QString &value) { m_variables[name] = value; }

QMap<QString, qreal> LayoutNodeAtom::finalValues() const {
    //TODO cache
    ValueCache result = m_values;
    //QMap<QString, qreal> result = m_values;
    for( QMap< QString, qreal>::const_iterator it = m_factors.constBegin(); it != m_factors.constEnd(); ++it )
    {
        result[ it.key() ] = result[ it.key() ] * it.value() / qreal ( m_countFactors[ it.key() ] );        
    }
//     for(QMap<QString, qreal>::iterator it = result.begin(); it != result.end(); ++it) {
//         if(m_factors.contains(it.key())) {
//             result[it.key()] = it.value() * ((m_factors[it.key()]) / qreal(m_countFactors[it.key()]));
//         }
//     }
    QMap< QString, qreal > res = result;
    if ( result.hasNegativeWidth() )
            res[ "w" ] = -res[ "w" ];
    return res;
}

QExplicitlySharedDataPointer<LayoutNodeAtom> LayoutNodeAtom::parentLayout() const {
    LayoutNodeAtom* p = 0;
    for(QExplicitlySharedDataPointer<AbstractAtom> a = parent(); a && !p; a = a->parent())
        p = dynamic_cast<LayoutNodeAtom*>(a.data());
    return QExplicitlySharedDataPointer<LayoutNodeAtom>(p);
}

QList< QExplicitlySharedDataPointer<LayoutNodeAtom> > LayoutNodeAtom::childrenLayouts() const {
    QList< QExplicitlySharedDataPointer<LayoutNodeAtom> > result;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, children())
        if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data()))
            result.append(QExplicitlySharedDataPointer<LayoutNodeAtom>(l));
    return result;
}

QList< QExplicitlySharedDataPointer<LayoutNodeAtom> > LayoutNodeAtom::descendantLayouts() const {
    QList< QExplicitlySharedDataPointer<LayoutNodeAtom> > result = childrenLayouts();
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, children())
        if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data()))
            foreach(QExplicitlySharedDataPointer<LayoutNodeAtom> atom, l->descendantLayouts())
                result.append(atom);
    return result;
}
        
QPair<LayoutNodeAtom*,LayoutNodeAtom*> LayoutNodeAtom::neighbors() const
{
    QExplicitlySharedDataPointer<LayoutNodeAtom> parentlayout = parentLayout();
    Q_ASSERT(parentlayout);
    QList<LayoutNodeAtom*> siblingLayouts;
    int myindex = -1;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, parent()->children()) {
        if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data())) {
            if(l == this) myindex = siblingLayouts.count();
            siblingLayouts.append(l);
        }
    }
    Q_ASSERT(myindex >= 0); // our parent should know about us else something is fundamental broken
    if(siblingLayouts.count() < 3) // if we don't have enough neighbors then abort and return NULL for both
        return QPair<LayoutNodeAtom*,LayoutNodeAtom*>(0,0);

    // Look if our index is the first or last in the list and if that's the case then wrap around the list
    // if the used algorithm-type is expected to produced a "circle".
    int srcIndex = myindex - 1;
    int dstIndex = myindex + 1;
    if(srcIndex < 0) {
        if(parentlayout->algorithmType() != AlgorithmAtom::CycleAlg)
            return QPair<LayoutNodeAtom*,LayoutNodeAtom*>(0,0);
        srcIndex = siblingLayouts.count()-1;
    }
    if(dstIndex < siblingLayouts.count()) {
        --myindex;
    } else {
        if(parentlayout->algorithmType() != AlgorithmAtom::CycleAlg)
            return QPair<LayoutNodeAtom*,LayoutNodeAtom*>(0,0);
        dstIndex = 0;
    }
    
    LayoutNodeAtom* srcAtom = siblingLayouts[srcIndex];
    LayoutNodeAtom* dstAtom = siblingLayouts[dstIndex];
    return QPair<LayoutNodeAtom*,LayoutNodeAtom*>(srcAtom,dstAtom);
}

qreal LayoutNodeAtom::distanceTo(LayoutNodeAtom* otherAtom) const
{
    //TODO specs are missing details from which exact point to calc the distance from...
#if 0
    QMap<QString, qreal> srcValues = this->m_values;
    QMap<QString, qreal> dstValues = otherAtom->m_values;
    //QMap<QString, qreal> srcFactors = this->m_factors;
    //QMap<QString, qreal> dstFactors = otherAtom->m_factors;
    //QMap<QString, int> srcCountFactors = this->m_countFactors;
    //QMap<QString, int> dstCountFactors = otherAtom->m_countFactors;
#else
    QMap<QString, qreal> srcValues = this->finalValues();
    QMap<QString, qreal> dstValues = otherAtom->finalValues();
#endif
    qreal srcX = srcValues["l"];// + srcValues["ctrX"];
    qreal srcY = srcValues["t"];// + srcValues["ctrY"];
    qreal dstX = dstValues["l"];// + dstValues["ctrX"];
    qreal dstY = dstValues["t"];// + dstValues["ctrY"];
    // qreal srcX = srcValues["l"] + srcValues["ctrX"] + srcValues["w"] / 2;
    // qreal srcY = srcValues["t"] + srcValues["ctrY"] + srcValues["h"] / 2;
    // qreal dstX = dstValues["l"] + dstValues["ctrX"] + dstValues["w"] / 2;
    // qreal dstY = dstValues["t"] + dstValues["ctrY"] + dstValues["h"] / 2;
    qreal diffX = dstX - srcX;
    qreal diffY = dstY - srcY;
    //qreal diffX = dstX - srcX - srcValues["w"]/2 - dstValues["w"] / 2;
    //qreal diffY = dstY - srcY - srcValues["h"]/2 - dstValues["h"] / 2;
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
    QString s;
    if(!m_fact.isEmpty()) s += QString("fact=%1 ").arg(m_fact);
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
    TRY_READ_ATTR_WITHOUT_NS_INTO(fact, m_fact)
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

void ConstraintAtom::build(Context* context)
{
//     context->m_parentLayout->addConstraint( QExplicitlySharedDataPointer<ConstraintAtom>( this ) );    
    QExplicitlySharedDataPointer<ConstraintAtom> ptr(this);
    QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > affectedLayouts;
    QVector< AbstractNode* > childDataPoints;
    QVector< AbstractNode* > refChildDataPoints;
    QVector< ConstraintAtom* > addedConstraints;
    bool constraintedWasApplied = false;
    if ( !m_for.isEmpty() && m_for != "ch" && m_for != "self" )
    {
        kWarning() << "constraint for " << m_for << " is not supported";
        return;
    }
    if ( m_for == "ch" )
    {
        foreach( AbstractNode* atom, context->currentNode()->children() )
        {
            PointNode *ptNode = dynamic_cast< PointNode* >( atom );
            if ( !ptNode )
                continue;
            if ( m_ptType.isEmpty() || m_ptType == ptNode->m_type )
                childDataPoints.append( atom );
            if ( m_refPtType.isEmpty() || m_refPtType == ptNode->m_type )
                refChildDataPoints.append( atom );
        }
        if ( m_ptType.isEmpty() )
            childDataPoints.append( context->currentNode() );
        if ( m_refPtType.isEmpty() )
            refChildDataPoints.append( context->currentNode() );
        Q_ASSERT( !childDataPoints.isEmpty() );
        //Q_ASSERT( childDataPoints.count() == refChildDataPoints.count() );
        
        //QVector< AbstractNode* >::iterator it = childDataPoints.constBegin();
        Q_ASSERT( context->m_parentLayout->childrenLayouts().count() > 0 );
        foreach( QExplicitlySharedDataPointer<LayoutNodeAtom> curChild, context->m_parentLayout->childrenLayouts() )
        {
            if ( curChild )
            {
                if ( !m_forName.isEmpty() )
                {
                    if ( curChild->m_name != m_forName )
                        continue;
                }
                if ( !childDataPoints.contains( context->m_layoutPointMap[ curChild.data() ] ) )
                    continue;
                QExplicitlySharedDataPointer<ConstraintAtom> clonedPtr( ptr->clone() );
                addedConstraints.append( clonedPtr.data() );
                curChild->addConstraint( clonedPtr );       
                constraintedWasApplied = true;
            }
        }        
    }
    else if ( m_for == "self" || m_for.isEmpty() )
    {
        //addedConstraints.append( ptr.data() );
        context->m_parentLayout->addConstraint( ptr );
        constraintedWasApplied = true;
    }
    typedef QList <QExplicitlySharedDataPointer <MSOOXML::Diagram::LayoutNodeAtom > >  LayoutNodeList;
    LayoutNodeList childList = context->m_parentLayout->childrenLayouts();
    foreach( ConstraintAtom* constraint, addedConstraints )
    {
      //foreach( QExplicitlySharedDataPointer<LayoutNodeAtom> curChild, context->m_parentLayout->childrenLayouts() )
        for( LayoutNodeList::iterator it = childList.begin(); it != childList.end(); ++it )        
        {
            QExplicitlySharedDataPointer<LayoutNodeAtom> curChild = *it;
            if ( !constraint->m_refForName.isEmpty() )
            {
                if ( curChild->m_name != constraint->m_refForName )
                    continue;
            }
            if ( !refChildDataPoints.contains( context->m_layoutPointMap[ curChild.data() ] ) )
                continue;
            constraint->m_referencedLayout = curChild.data();
            childList.erase( it );
        }
    }
    
//     QExplicitlySharedDataPointer<LayoutNodeAtom> layout = m_forName.isEmpty() ? context->m_parentLayout : context->m_layoutMap.value(m_forName);
//     if(layout) {
//         QExplicitlySharedDataPointer<ConstraintAtom> ptr(this);
//         m_parent->removeChild(QExplicitlySharedDataPointer<AbstractAtom>(this));
//         layout->addConstraint(ptr);
//     }
    Q_ASSERT( constraintedWasApplied );
    AbstractAtom::build(context);
}

void ConstraintAtom::applyConstraint( QExplicitlySharedDataPointer<LayoutNodeAtom> atom )
{
    Q_ASSERT( atom );
    qreal value = -1.0;
    if( !m_value.isEmpty() )
    {
        bool ok;
        qreal v = m_value.toDouble( &ok );
        if ( ok )
        {
            value = v;
            atom->m_values[ m_type ] = value;
            atom->setNeedsRelayout( true );
        }
        else
            kWarning() << "Layout with name=" << atom->m_name << "defines none-double value=" << m_value;
    }
    else if ( m_fact.isEmpty() )
    {
        QMap<QString, qreal> values;
        if ( m_referencedLayout )
        {
              values = m_referencedLayout->finalValues();
        } 
        else
        {
            values = atom->finalValues();
        }
        if( !m_refType.isEmpty() )
        {
            if( values.contains( m_refType ) )
                value = values[ m_refType ];
            if ( value < 0.0 )
            {
                AbstractAlgorithm* r = m_referencedLayout && m_referencedLayout->algorithmImpl() ? m_referencedLayout->algorithmImpl() : atom->algorithmImpl();
                Q_ASSERT( r );
                
                value = r->defaultValue( m_refType, values );      
                Q_ASSERT_X(value >= 0.0, __FUNCTION__, QString("type=%1 refType=%2").arg( m_type ).arg( m_refType ).toLocal8Bit());
            }
            atom->m_values[ m_type ] = value;
            atom->setNeedsRelayout( true );
        } else {
            if ( value >= 0.0 )
            {
                atom->m_values[ m_type ] = value;
                atom->setNeedsRelayout( true );
            }
            else
            {
                AbstractAlgorithm* r = m_referencedLayout && m_referencedLayout->algorithmImpl() ? m_referencedLayout->algorithmImpl() : atom->algorithmImpl();
                Q_ASSERT( r );
                
                value = r->defaultValue( m_refType, values );
                if  ( value >= 0.0 )
                {
                    atom->m_values[ m_type ] = value;
                    atom->setNeedsRelayout( true );
                }
                else
                {
                    ;//Q_ASSERT( false );
                }
            }
        }
    }
    else
    {
        bool ok;
        qreal v = m_fact.toDouble( &ok );
        if ( ok )
        {
            atom->m_factors[ m_type ] += v;
            atom->m_countFactors[ m_type ] += 1;
            atom->setNeedsRelayout( true );
            
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
    QString s;
    if(!m_fact.isEmpty()) s += QString("fact=%1 ").arg(m_fact);
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

/****************************************************************************************************/

AdjustAtom* AdjustAtom::clone()
{
    AdjustAtom* atom = new AdjustAtom;
    atom->m_index = m_index;
    atom->m_value = m_value;
    return atom;
}

void AdjustAtom::dump(Context*, int level)
{
    DEBUG_DUMP << "index=" << m_index << "value=" << m_value;
}

void AdjustAtom::readElement(Context*, MsooXmlDiagramReader* reader)
{
    const QXmlStreamAttributes attrs(reader->attributes());
    TRY_READ_ATTR_WITHOUT_NS(idx)
    m_index = idx.toInt();
    TRY_READ_ATTR_WITHOUT_NS(val)
    m_value = val.toDouble();
}

/****************************************************************************************************/

ListAtom* ListAtom::clone() {
    ListAtom* atom = new ListAtom(m_tagName);
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone());
    return atom;
}

void ListAtom::dump(Context* context, int level) {
    AbstractAtom::dump(context, level);
}

void ListAtom::readElement(Context* context, MsooXmlDiagramReader* reader) {
    if (reader->isStartElement()) {
        QExplicitlySharedDataPointer<AbstractAtom> node;
        if (reader->qualifiedName() == QLatin1String("dgm:constr")) {
            node = QExplicitlySharedDataPointer<AbstractAtom>(new ConstraintAtom);
        } else if (reader->qualifiedName() == QLatin1String("dgm:rule")) {
            node = QExplicitlySharedDataPointer<AbstractAtom>(new RuleAtom);
        } else if (reader->qualifiedName() == QLatin1String("dgm:adj")) {
            node = QExplicitlySharedDataPointer<AbstractAtom>(new AdjustAtom);
        }
        if(node) {
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

// void ShapeAtom::build(Context* context) {
//     QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
//     context->m_parentLayout->setAxis(context, axis);
//     AbstractAtom::build(context);
// }

//TODO use filters/libmso/ODrawToOdf.h
void ShapeAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
    Q_ASSERT(context->m_parentLayout);
    if(m_type.isEmpty() || m_hideGeom) return;

    QMap<QString,QString> params = context->m_parentLayout->algorithmParams();

    QMap<QString, qreal> values = context->m_parentLayout->finalValues();
    //Q_ASSERT(values.contains("l"));
    //Q_ASSERT(values.contains("t"));

    //if(!values.contains("w")) values["w"]=100;
    //if(!values.contains("h")) values["h"]=100;
    Q_ASSERT(values.contains("w"));
    Q_ASSERT(values.contains("h"));
    
    //Q_ASSERT(values.contains("ctrX"));
    //Q_ASSERT(values.contains("ctrY"));
    qreal x  = values.value("l");
    qreal y  = values.value("t");
    qreal w  = values.value("w");
    qreal h  = values.value("h");
    qreal cx = values.value("ctrX");
    qreal cy = values.value("ctrY");
    {
    QFile shapeListFile( "shapeList.txt" );
    QString cxt = QString::number( cx );
    QString cyt = QString::number( cy );
    QString wt = QString::number( w );
    QString ht = QString::number( h );
    QString lbrack = "(";
    QString rbrack = ")\n";
    QString space = " ";
//     qDebug() << context->m_rootLayout->finalValues();
//     Q_ASSERT( false );
    
    shapeListFile.open( QFile::WriteOnly | QFile::Append );
    shapeListFile.write( lbrack.toLatin1() );
    shapeListFile.write( cxt.toLatin1() );
    shapeListFile.write( space.toLatin1() );
    shapeListFile.write( cyt.toLatin1() );
    shapeListFile.write( space.toLatin1() );
    shapeListFile.write( wt.toLatin1() );
    shapeListFile.write( space.toLatin1() );
    shapeListFile.write( ht.toLatin1() );
    shapeListFile.write( rbrack.toLatin1() );
    shapeListFile.close();
    }

#if 0
    //TODO can spacing between the siblings applied by shriking the shapes or is it needed to apply them along the used algorithm?
    if(values.contains("sibSp")) {
        qreal sibSp = values["sibSp"];
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
#endif

    DEBUG_WRITE << "type=" << m_type << "blip=" << m_blip << "hideGeom=" << m_hideGeom << "geometry=" << x+cx << y+cy << w << h;
    Q_ASSERT(x >= 0.0);
    Q_ASSERT(y >= 0.0);
    Q_ASSERT(w > 0.0);
    Q_ASSERT(h > 0.0);
    Q_ASSERT(cx >= 0.0);
    Q_ASSERT(cy >= 0.0);

    xmlWriter->startElement("draw:custom-shape");
    //xmlWriter->addAttribute("draw:layer", "layout");

    if (!context->m_parentLayout->m_name.isEmpty())
        xmlWriter->addAttribute("draw:name", context->m_parentLayout->m_name);

    KoGenStyle style = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    style.addProperty("draw:fill", "solid" /*none*/, KoGenStyle::GraphicType);
    style.addProperty("draw:opacity", "50%");
    style.addProperty("draw:textarea-horizontal-align", "center");
    style.addProperty("draw:textarea-vertical-align", "middle");
    style.addProperty("fo:wrap-option", "wrap");

    //const qreal m_svgX = x + qMax(0.0, qMin(w, cx - w/2));
    //const qreal m_svgY = y + qMax(0.0, qMin(h, cy - h/2));
    const qreal m_svgX = x/* + cx*/;
    const qreal m_svgY = y/* + cy*/;

    const qreal rotateAngle = context->m_parentLayout->m_rotateAngle; //0=right 45=bottom 90=left 135=top 180=right
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

    if (m_type == QLatin1String("conn")) {
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

    //xmlWriter->startElement("svg:desc");
    //xmlWriter->endElement();

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
        //xmlWriter->addAttribute("svg:viewBox", "0 0 21600 21600");
        xmlWriter->endElement();
    } else if (m_type == QLatin1String("cycle")) {
        xmlWriter->startElement("draw:enhanced-geometry");
        xmlWriter->addAttribute("draw:enhanced-path", "M 0 414114 C 0 304284 43630 198953 121292 121291 198954 43630 304285 0 414115 0 523945 0 629276 43630 706938 121292 784599 198954 828229 304285 828229 414115 828229 523945 784599 629276 706938 706938 629277 784599 523945 828229 414115 828229 304285 828229 198954 784599 121292 706938 43631 629276 1 523945 1 414115 L 0 414114 Z N");
        xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90, -90, -90, -90, -90, -90");
        xmlWriter->addAttribute("draw:glue-points", "?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f27 ?f28 ?f29 ?f30 ?f27 ?f31 ?f25 ?f32 ?f23 ?f31 ?f33 ?f30 ?f21 ?f22");
        xmlWriter->addAttribute("draw:type", "circle");
        //xmlWriter->addAttribute("draw:text-areas", "?f34 ?f36 ?f35 ?f37");
        //xmlWriter->addAttribute("svg:viewBox", "0 0 828228 828228");
        xmlWriter->endElement();
    } else if (m_type == QLatin1String("rect")) {
        xmlWriter->startElement("draw:enhanced-geometry");
        xmlWriter->addAttribute("draw:enhanced-path", "M 0 0 L 1393031 0 1393031 557212 0 557212 0 0 Z N");
        xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90");
        xmlWriter->addAttribute("draw:glue-points", "?f12 ?f13 ?f14 ?f13 ?f14 ?f15 ?f12 ?f15 ?f12 ?f13");
        xmlWriter->addAttribute("draw:type", "non-primitive");
        //xmlWriter->addAttribute("draw:text-areas", "?f16 ?f18 ?f17 ?f19");
        //xmlWriter->addAttribute("svg:viewBox", "0 0 1393031 557212");
        xmlWriter->endElement();
    } else if (m_type == QLatin1String("roundRect")) {
        xmlWriter->startElement("draw:enhanced-geometry");
        xmlWriter->addAttribute("draw:enhanced-path", "M 0 97707 C 0 71793 10294 46941 28618 28618 46942 10294 71794 0 97707 0 L 804191 0 C 830105 0 854957 10294 873280 28618 891604 46942 901898 71794 901898 97707 L 901898 488526 C 901898 514440 891604 539292 873280 557615 854956 575939 830104 586233 804191 586233 L 97707 586233 C 71793 586233 46941 575939 28618 557615 10294 539291 0 514439 0 488526 L 0 97707 Z N");
        xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90");
        xmlWriter->addAttribute("draw:glue-points", "?f20 ?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f25 ?f27 ?f23 ?f28 ?f21 ?f28 ?f29 ?f27 ?f30 ?f26 ?f31 ?f24 ?f31 ?f22 ?f30 ?f20 ?f29 ?f20 ?f21");
        xmlWriter->addAttribute("draw:type", "non-primitive");
        //xmlWriter->addAttribute("draw:text-areas", "?f32 ?f34 ?f33 ?f35");
        //xmlWriter->addAttribute("svg:viewBox", "0 0 901898 586233");
        xmlWriter->endElement();
    } else if (m_type == QLatin1String("pie")) {
        xmlWriter->startElement("draw:enhanced-geometry");
        xmlWriter->addAttribute("draw:enhanced-path", "M 1152145 0 C 1563767 0 1944120 219599 2149931 576074 2355741 932549 2355740 1371744 2149929 1728219 L 1152144 1152144 C 1152144 768096 1152145 384048 1152145 0 Z N");
        xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90");
        xmlWriter->addAttribute("draw:glue-points", "?f16 ?f17 ?f18 ?f19 ?f20 ?f21 ?f22 ?f23 ?f16 ?f17");
        xmlWriter->addAttribute("draw:type", "non-primitive");
        //xmlWriter->addAttribute("draw:text-areas", "?f24 ?f26 ?f25 ?f27");
        //xmlWriter->addAttribute("svg:viewBox", "0 0 2304288 2304288");
        xmlWriter->endElement();
    } else if (m_type == QLatin1String("diamond")) {
        xmlWriter->startElement("draw:enhanced-geometry");
        xmlWriter->addAttribute("draw:enhanced-path", "M ?f0 ?f7 L ?f11 ?f2 ?f1 ?f7 ?f11 ?f3 Z N");
        //xmlWriter->addAttribute("draw:path-stretchpoint-x", "21600");
        //xmlWriter->addAttribute("draw:path-stretchpoint-y", "21600");
        xmlWriter->addAttribute("draw:type", "non-primitive");
        //xmlWriter->addAttribute("draw:text-areas", "?f10 ?f6 ?f12 ?f13");
        //xmlWriter->addAttribute("svg:viewBox", "0 0 21600 21600");
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "left"); xmlWriter->addAttribute("draw:name", "f0"); xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "right");  xmlWriter->addAttribute("draw:name", "f1");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "top");  xmlWriter->addAttribute("draw:name", "f2");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "bottom");  xmlWriter->addAttribute("draw:name", "f3");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f3 - ?f2");  xmlWriter->addAttribute("draw:name", "f4");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f4 / 2");  xmlWriter->addAttribute("draw:name", "f5");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f4 / 4");  xmlWriter->addAttribute("draw:name", "f6");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f2 + ?f5");  xmlWriter->addAttribute("draw:name", "f7");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f1 - ?f0");  xmlWriter->addAttribute("draw:name", "f8");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f8 / 2");  xmlWriter->addAttribute("draw:name", "f9");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f8 / 4");  xmlWriter->addAttribute("draw:name", "f10");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f0 + ?f9");  xmlWriter->addAttribute("draw:name", "f11");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f8 * 3 / 4");  xmlWriter->addAttribute("draw:name", "f12");xmlWriter->endElement();
        xmlWriter->startElement("draw:equation"); xmlWriter->addAttribute("draw:formula", "?f4 * 3 / 4");  xmlWriter->addAttribute("draw:name", "f13");xmlWriter->endElement();
        xmlWriter->endElement();
    } else if (m_type == QLatin1String("trapezoid")) {
        xmlWriter->startElement("draw:enhanced-geometry");
        xmlWriter->addAttribute("draw:enhanced-path", "M 0 914400 L 761997 0 762003 0 1524000 914400 0 914400 Z N");
        xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90");
        xmlWriter->addAttribute("draw:glue-points", "?f14 ?f15 ?f16 ?f17 ?f18 ?f17 ?f19 ?f15 ?f14 ?f15");
        xmlWriter->addAttribute("draw:type", "non-primitive");
        //xmlWriter->addAttribute("draw:text-areas", "?f20 ?f22 ?f21 ?f23");
        //xmlWriter->addAttribute("svg:viewBox", "0 0 1524000 914400");
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
            xmlWriter->addAttribute("draw:type", "non-primitive");
            //xmlWriter->addAttribute("draw:text-areas", "?f11 ?f13 ?f12 ?f14");
            //xmlWriter->addAttribute("svg:viewBox", "0 0 2341473 2341473");
            xmlWriter->endElement();
        } else { // arrow-right
            xmlWriter->startElement("draw:enhanced-geometry");
            xmlWriter->addAttribute("draw:enhanced-path", "M 0 55905 L 110087 55905 110087 0 220174 139764 110087 279527 110087 223622 0 223622 0 55905 Z N");
            xmlWriter->addAttribute("draw:glue-point-leaving-directions", "-90, -90, -90, -90, -90, -90, -90, -90");
            xmlWriter->addAttribute("draw:glue-points", "?f16 ?f17 ?f18 ?f17 ?f18 ?f19 ?f20 ?f21 ?f18 ?f22 ?f18 ?f23 ?f16 ?f23 ?f16 ?f17");
            xmlWriter->addAttribute("draw:type", "non-primitive");
            //xmlWriter->addAttribute("draw:text-areas", "?f24 ?f26 ?f25 ?f27");
            //xmlWriter->addAttribute("svg:viewBox", "0 0 220174 279527");
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
        //for(AbstractNode* n = context->currentNode(); n; n = n->parent(), ++depth);
        //funcValue = depth;
        //TODO
        kWarning()<<"TODO func=depth";
    } else if(m_function == "maxDepth") { // Defines the maximum depth.
        //int depth = 0;
        //for(AbstractNode* n = context->currentNode(); n; n = n->parent(), ++depth);
        //funcValue = depth;
        //TODO
        kWarning()<<"TODO func=maxDepth";
    } else if(m_function == "pos") { // Retrieves the position of the node in the specified set of nodes.
        const int position = axis.indexOf(context->currentNode()) + 1;
        funcValue = QString::number(position);
        //TODO 1-based? what index for not-found?
        kWarning()<<"TODO func=pos funcValue="<<funcValue;
    } else if(m_function == "posEven") { // Returns 1 if the specified node is at an even numbered position in the data model.
        //const int position = axis.indexOf(context->currentNode())+1;
        //funcValue = position>=1 && position % 2 == 0 ? 1 : 0;
        //TODO
        kWarning()<<"TODO func=posEven";
    } else if(m_function == "posOdd") { // Returns 1 if the specified node is in an odd position in the data model.
        //const int position = axis.indexOf(context->currentNode())+1;
        //funcValue = position>=1 && position % 2 != 0 = 1 : 0;
        //TODO
        kWarning()<<"TODO func=posOdd";
    } else if(m_function == "revPos") { // Reverse position function.
        const int position = axis.indexOf(context->currentNode()) + 1;
        funcValue = axis.count()-position;
        //TODO lastIndexOf? 1-based? what index for not-found?
        kWarning()<<"TODO func=revPos";
    } else if(m_function == "var") { // Used to reference a variable.
        funcValue = context->m_parentLayout->variable(m_argument, true /* check parents */);
        if(funcValue.isEmpty()) { // if not defined then use default variable-values
            if(m_argument == QLatin1String("dir")) { // Specifies the direction of the diagram.
                funcValue = "norm";
            } else {
                kWarning()<<"TODO figure out default for variable="<<m_argument;
            }
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

    int index = m_parent->children().count();
    QVector < QExplicitlySharedDataPointer<AbstractAtom> > childs = m_parent->children();
    for ( int i = 0; i < childs.count(); ++i )
        if ( childs[ i ].data() == this )
            index = i + 1;
    typedef QVector< QExplicitlySharedDataPointer< AbstractAtom > > AtomPList;
    //AtomPList constraintList;    
    Q_FOREACH( QExplicitlySharedDataPointer<AbstractAtom> atom, ifResult.isEmpty() ? elseResult : ifResult )
    {
        AtomPList listResult = atom->children();
        AtomPList::iterator it = std::stable_partition( listResult.begin(), listResult.end(), ConstraintPredicate() );    
        std::copy( it, listResult.end(), std::back_inserter( context->m_parentLayout->m_constraintsToBuild ) );
        listResult.erase( it, listResult.end() );
        Q_FOREACH( QExplicitlySharedDataPointer<AbstractAtom> a, listResult/*atom->children()*/ )
        {
            atom->removeChild( a );
            m_parent->insertChild( index, a );
            ++index;
            a->build( context );
        }
    }
//     foreach( QExplicitlySharedDataPointer<AbstractAtom> constr, constraintList )
//     {
//         constr->build( context );
//     }

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
            LayoutNodeAtom* layNodeAtom = dynamic_cast< LayoutNodeAtom* >( atom.data() );
            if ( layNodeAtom )
            {
                QList< AbstractNode* >  currentAxis;
                currentAxis << node;
                context->m_layoutPointMap[ layNodeAtom ] = node;
                layNodeAtom->setAxis( context, currentAxis );
            }
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

AbstractAlgorithm::AbstractAlgorithm() : m_context(0), m_oldCurrentNode(0) {}

AbstractAlgorithm::~AbstractAlgorithm() {
    if(m_context) {
        m_context->m_parentLayout = m_parentLayout;
        m_context->setCurrentNode(m_oldCurrentNode);
    }
    qDeleteAll( doubleLayoutContext );
}

Context* AbstractAlgorithm::context() const { return m_context; }
LayoutNodeAtom* AbstractAlgorithm::layout() const { return m_layout.data(); }
LayoutNodeAtom* AbstractAlgorithm::parentLayout() const { return m_parentLayout.data(); }

QList<LayoutNodeAtom*> AbstractAlgorithm::childLayouts() const
{
    QList<LayoutNodeAtom*> result;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_layout->children()) {
        if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data())) {
            result.append(l);
        }
    }
    return result;
}

void AbstractAlgorithm::setNodePosition(LayoutNodeAtom* l, qreal x, qreal y, qreal w, qreal h) {
    QStringList removeList;
    l->m_values["l"] = parentLayout()->finalValues()["l"] + x;
    l->m_values["t"] = parentLayout()->finalValues()["t"] + y;
    removeList << "l" << "t";
    if (w >= 0.0) {
        l->m_values["w"] = w;
        removeList << "w";
    }
    if (h >= 0.0) {
        l->m_values["h"] = h;
        removeList << "h";
    }
    //l->m_values["ctrX"] = 0.0;
    //l->m_values["ctrY"] = 0.0;
    //l->m_values["r"] = l->m_values["l"] + l->m_values["w"];
    //l->m_values.remove("ctrX");
    //l->m_values.remove("ctrY");
    removeList << "ctrX" << "ctrY";
//     foreach(const QString &s, removeList) {
//         //l->m_factors[s] = 1.0;
//         //l->m_countFactors[s] = 1;
//         l->m_factors.remove(s);
//         l->m_countFactors.remove(s);
//     }
    l->m_needsReinit = false; // we initialized things above already
    l->m_needsRelayout = true; // but we clearly need a layout now
    l->m_childNeedsRelayout = true; // and our children need to be relayouted too now
}

qreal AbstractAlgorithm::defaultValue(const QString& type, const QMap<QString, qreal>& values) {
    qreal value = virtualGetDefaultValue(type, values);
    if(value < 0.0) {
        // If the layout-algorithm doesn't define a default-value then use one of the common default-values.
        // See also http://social.msdn.microsoft.com/Forums/en/os_binaryfile/thread/7c823650-7913-4e63-970f-1c5dab3450c4
        if (type == "primFontSz") {
            value = 36;
//         } else if (type == "tMarg") {
//             Q_ASSERT(values.contains("primFontSz"));
//             value = values.value("primFontSz") * 0.56;
//         } else if (type == "lMarg") {
//             Q_ASSERT(values.contains("primFontSz"));
//             value = values.value("primFontSz") * 0.40;
//         } else if (type == "rMarg") {
//             Q_ASSERT(values.contains("primFontSz"));
//             value = values.value("primFontSz") * 0.42;
//         } else if (type == "bMarg") {
//             Q_ASSERT(values.contains("primFontSz"));
//             value = values.value("primFontSz") * 0.60;
        } else if (type.startsWith("user")) { // userA, userB, userC, etc.
            bool ok;
            const qreal v = layout()->variable(type, true /* checkParents */).toDouble(&ok);
            value = ok ? v : 0.0;
        }
    }
    return value;
}

void AbstractAlgorithm::doInit(Context* context, QExplicitlySharedDataPointer<LayoutNodeAtom> layout) {
    m_context = context;
    m_layout = layout;
    m_parentLayout = m_context->m_parentLayout;
    m_context->m_parentLayout = m_layout;
    m_oldCurrentNode = m_context->currentNode();
    virtualDoInit();
}

void AbstractAlgorithm::doLayout() {
    virtualDoLayout();
}

void AbstractAlgorithm::doLayoutChildren() {
    virtualDoLayoutChildren();
}

qreal AbstractAlgorithm::virtualGetDefaultValue(const QString&, const QMap<QString, qreal>&) {
    return -1.0;
}

void AbstractAlgorithm::virtualDoInit() {
    if(layout()->m_needsReinit) {
        layout()->m_needsReinit = false; // initialization done
        //layout()->m_values = parentLayout()->m_values;
        //layout()->m_factors = parentLayout()->m_factors;
        //layout()->m_countFactors = parentLayout()->m_countFactors;
    }
    //QMap<QString, qreal> values = parentLayout()->finalValues();
    //Q_ASSERT(values["l"] >= 0.0);
    //Q_ASSERT(values["t"] >= 0.0);
    //Q_ASSERT(values["w"] > 0.0);
    //Q_ASSERT(values["h"] > 0.0);
    //Q_ASSERT(values["ctrX"] >= 0.0);
    //Q_ASSERT(values["ctrY"] >= 0.0);
}

// http://msdn.microsoft.com/en-us/library/dd439461(v=office.12).aspx
void AbstractAlgorithm::virtualDoLayout() {
    Q_ASSERT( layout() );
    Q_ASSERT( !name().isEmpty() );
    const QString __name = name();
    kDebug() << "layout=" << layout()->m_name << "algorithm=" << __name;//name();
    
    // Specifies the aspect ratio (width to height) of the composite node to use when determining child constraints. A value of 0 specifies to
    // leave the width and height constraints unaltered. The algorithm may temporarily shrink one dimension to achieve the specified ratio.
    // For example, if a composite node has a width constraint of 20 and height constraint of 10, and if the value of ar is 1.5, the composite
    // algorithm uses a width value of 15 to calculate the composite node’s child constraints. However, the algorithm does not propagate this
    // value to other nodes.
    qreal aspectRatio = layout()->algorithmParam("ar", "0").toDouble();
    if(aspectRatio != 0.0) {
        layout()->m_values["w"] = layout()->finalValues()["h"] * aspectRatio;
    }
    QList< QExplicitlySharedDataPointer< LayoutNodeAtom > > allChilds = layout()->childrenLayouts();
    if ( context()->m_parentLayout->m_name == "circ1" )
        Q_FOREACH( QExplicitlySharedDataPointer< ConstraintAtom > atom, layout()->constraints() )
            atom->dump( context(), 2 );
    Q_FOREACH( QExplicitlySharedDataPointer< ConstraintAtom > atom, layout()->constraints() )
        atom->applyConstraint( QExplicitlySharedDataPointer< LayoutNodeAtom > ( layout() ) );
    Q_FOREACH( QExplicitlySharedDataPointer< LayoutNodeAtom > curChild, allChilds )
    {
        setNodePosition( curChild.data(), layout()->finalValues()[ "l" ], layout()->finalValues()[ "t" ], layout()->finalValues()[ "w" ], layout()->finalValues()[ "h" ] );        
    }    
#if 0
    return;

    // evaluate the constraints responsible for positioning and sizing.
    foreach(QExplicitlySharedDataPointer<ConstraintAtom> c, layout()->constraints()) {
        c->dump(context(), 2);

        qreal value = -1.0;
        if(!c->m_value.isEmpty()) {
            bool ok;
            qreal v = c->m_value.toDouble(&ok);
            if(ok) value = v; else kWarning() << "Layout with name=" << layout()->m_name << "defines none-double value=" << c->m_value;
        } else {
#if 0
#if 0
            QExplicitlySharedDataPointer<LayoutNodeAtom> ref = c->m_refForName.isEmpty() ? m_layout : context()->m_layoutMap.value(c->m_refForName);
            if(ref && ref != m_layout && (ref->m_needsReinit || ref->m_needsRelayout || ref->m_childNeedsRelayout)) {
                ref->layoutAtom(context());
                Q_ASSERT(!ref->m_needsReinit);
                Q_ASSERT(!ref->m_needsRelayout);
                Q_ASSERT(!ref->m_childNeedsRelayout);
                if(ref->m_needsReinit || ref->m_needsRelayout || ref->m_childNeedsRelayout)
                    continue;
            }

            QMap<QString, qreal> values = ref->finalValues();
            QString type = c->m_refType.isEmpty() ? c->m_type : c->m_refType;
            if(values.contains(type)) {
                value = values[type];
            } else {
                // if the layout doesn't know about such a type then look if one of his parent-layouts does.
                for(QExplicitlySharedDataPointer<LayoutNodeAtom> a = ref->parentLayout(); a; a = a->parentLayout()) {
                    values = a->finalValues();
                    if(values.contains(type)) {
                        value = values[type];
                        break;
                    }
                }
                if (value < 0.0) {
                    // if the type is still unknown then look if the layout-algorithm defines a default value for it.
                    value = defaultValue(type, values);
                }
                kDebug()<<typeid(this).name()<<c->m_type<<value;
                Q_ASSERT_X(value >= 0.0, __FUNCTION__, QString("No known value for the referenced type=%1.").arg(type).toUtf8());
            }
#else
            QMap<QString, qreal> values;
            if (!c->m_refForName.isEmpty()) {
                QExplicitlySharedDataPointer<LayoutNodeAtom> ref = context()->m_layoutMap.value(c->m_refForName);
                Q_ASSERT(ref);
                Q_ASSERT(ref != m_layout);
                if (ref && ref != m_layout && (ref->m_needsReinit || ref->m_needsRelayout || ref->m_childNeedsRelayout)) {
                    ref->layoutAtom(context());
                    Q_ASSERT(!ref->m_needsReinit);
                    Q_ASSERT(!ref->m_needsRelayout);
                    Q_ASSERT(!ref->m_childNeedsRelayout);
                }
                values = ref->finalValues();
            } else {
                values = m_layout->finalValues();
            }
            if(!c->m_refType.isEmpty()) {
                if(values.contains(c->m_refType)) {
                    value = values[c->m_refType];
                    //kDebug()<<"1-AAAAAAAAAAAAAAA name="<<m_layout->m_name<<"refForName="<<c->m_refForName<<"type="<<c->m_type<<"refType="<<c->m_refType<<"value="<<value;
                    //Probably  don't use addConstraint with forName cause we need the sender for the references?
                }
                if (value < 0.0) {
                    value = defaultValue(c->m_refType, values); //TODO maybe ref->defaultValue(...) ?
                }
            } else {
                //value = defaultValue(c->m_type, values);
            }
#endif
        }
//TODO 1) "op" isn't supported
        if (value >= 0.0) {
            layout()->m_values[c->m_type] = value;
            //kDebug()<<"2-AAAAAAAAAAAAAAA name="<<m_layout->m_name<<"refForName="<<c->m_refForName<<"type="<<c->m_type<<"refType="<<c->m_refType<<"value="<<value<<"finalValues="<<layout()->finalValues()[c->m_type];
        }
        if (!c->m_fact.isEmpty()) {
            bool ok;
            qreal v = c->m_fact.toDouble(&ok);
            if (ok) {
                layout()->m_factors[c->m_type] += v;
                layout()->m_countFactors[c->m_type] += 1;
            }
        }
    }
#else
            QMap<QString, qreal> values;
            QExplicitlySharedDataPointer<LayoutNodeAtom> ref;
            if (!c->m_refForName.isEmpty()) {
                ref = context()->m_layoutMap.value(c->m_refForName);
                Q_ASSERT(ref);
//                Q_ASSERT(ref != m_layout);
                if (ref && ref != m_layout && (ref->m_needsReinit || ref->m_needsRelayout || ref->m_childNeedsRelayout)) {
                    ref->layoutAtom(context());
                    Q_ASSERT(!ref->m_needsReinit);
                    Q_ASSERT(!ref->m_needsRelayout);
                    Q_ASSERT(!ref->m_childNeedsRelayout);
                }
                values = ref->finalValues();
            } else {
                values = m_layout->finalValues();
            }
            if(!c->m_refType.isEmpty()) {
                if(values.contains(c->m_refType))
                    value = values[c->m_refType];
                if (value < 0.0) {
                    //Q_ASSERT( ! (ref && ref->algorithmImpl()) );
                    AbstractAlgorithm* r = this;
                    value = r->defaultValue(c->m_refType, values);
                    Q_ASSERT_X(value >= 0.0, __FUNCTION__, QString("type=%1 refType=%2").arg(c->m_type).arg(c->m_refType).toLocal8Bit());
                }
            } else {
                //if (value < 0.0) { value = defaultValue(c->m_type, values); Q_ASSERT(value >= 0.0); }
            }
        }
        if (value >= 0.0) {
            layout()->m_values[c->m_type] = value;
        }
        if (!c->m_fact.isEmpty()) {
            bool ok;
            qreal v = c->m_fact.toDouble(&ok);
            if (ok) {
                layout()->m_factors[c->m_type] += v;
                layout()->m_countFactors[c->m_type] += 1;
            }
        }
    }
#endif
#endif
}

void AbstractAlgorithm::applyConstraints()
{
    foreach(QExplicitlySharedDataPointer<ConstraintAtom> c, layout()->constraints()) {
        c->dump(context(), 2);
        
        qreal value = -1.0;
        if(!c->m_value.isEmpty()) {
            bool ok;
            qreal v = c->m_value.toDouble(&ok);
            if(ok) value = v; else kWarning() << "Layout with name=" << layout()->m_name << "defines none-double value=" << c->m_value;
        } else if ( c->m_fact.isEmpty() ) {
          QMap<QString, qreal> values;
            QExplicitlySharedDataPointer<LayoutNodeAtom> ref;
            if (!c->m_refForName.isEmpty()) {
                ref = context()->m_layoutMap.value(c->m_refForName);
                Q_ASSERT(ref);
                if (ref && ref != m_layout && (ref->m_needsReinit || ref->m_needsRelayout || ref->m_childNeedsRelayout)) {
                    ref->layoutAtom(context());
                    Q_ASSERT(!ref->m_needsReinit);
                    Q_ASSERT(!ref->m_needsRelayout);
                    Q_ASSERT(!ref->m_childNeedsRelayout);
                }
                values = ref->finalValues();
            } else {
                values = m_layout->finalValues();
            }
            if(!c->m_refType.isEmpty()) {
                if(values.contains(c->m_refType))
                    value = values[c->m_refType];
                if (value < 0.0) {
                    //Q_ASSERT( ! (ref && ref->algorithmImpl()) );
                    //AbstractAlgorithm* r = this;
                    //AbstractAlgorithm* r = layout()->algorithmImpl();
                    AbstractAlgorithm* r = ref && ref->algorithmImpl() ? ref->algorithmImpl() : this;
                    
                    value = r->defaultValue(c->m_refType, values);
                    Q_ASSERT_X(value >= 0.0, __FUNCTION__, QString("type=%1 refType=%2").arg(c->m_type).arg(c->m_refType).toLocal8Bit());
                }
            } else {
                //if (value < 0.0) { value = defaultValue(c->m_type, values); Q_ASSERT(value >= 0.0); }
            }
        }
        if (value >= 0.0) {
            if ( c->m_forName.isEmpty() ) {
                layout()->m_factors.remove(c->m_type);
                Q_ASSERT( layout()->m_name != "space" || c->m_type != "w" );
                layout()->m_countFactors.remove(c->m_type);
                layout()->m_values[c->m_type] = value;
                layout()->setNeedsRelayout( true );
            } else {
                QExplicitlySharedDataPointer<LayoutNodeAtom> forLayout = context()->m_layoutMap[ c->m_forName ];
                Q_ASSERT( forLayout->m_name != "space" || c->m_type != "w" );
                forLayout->m_factors.remove(c->m_type);
                forLayout->m_countFactors.remove(c->m_type);
                if ( forLayout ) {
                    forLayout->m_values[c->m_type] = value;
                    forLayout->setNeedsRelayout( true );
                } else {
                    Q_ASSERT( false );
                }
            }
        }
        if (!c->m_fact.isEmpty()) {
            bool ok;
            qreal v = c->m_fact.toDouble(&ok);
            if (ok) {
                if ( c->m_forName.isEmpty() ) {
                    layout()->m_factors[c->m_type] += v;
                    layout()->m_countFactors[c->m_type] += 1;
                    layout()->setNeedsRelayout( true );
                } else {
                    QExplicitlySharedDataPointer<LayoutNodeAtom> forLayout = context()->m_layoutMap[ c->m_forName ];
                    Q_ASSERT( forLayout->m_name == c->m_forName );
                    if ( forLayout ) {
                        forLayout->m_factors[c->m_type] += v;
                        forLayout->m_countFactors[c->m_type] += 1;
                        forLayout->setNeedsRelayout( true );
                    } else {
                        Q_ASSERT( false );
                    }
                }
            }
        };
    }    
}


void AbstractAlgorithm::virtualDoLayoutChildren() {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, layout()->children()) {
        if ( LayoutNodeAtom* layAtom = dynamic_cast< LayoutNodeAtom* >( atom.data() ) )
            layAtom->setNeedsRelayout( true );
        atom->layoutAtom(context());
    }
}

/****************************************************************************************************/

// http://msdn.microsoft.com/en-us/library/dd439461(v=office.12).aspx
qreal CompositeAlgorithm::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) {
    Q_UNUSED(values);
    qreal value = -1.0;
    if (type == "w" || type == "h") {
        value = 100;
    } else if (type == "l" || type == "t") {
        value = 0;
    } else if (type == "wOff" || type == "hOff" || type == "lOff" || type == "ctrXOff" || type == "rOff" || type == "tOff" || type == "ctrYOff" || type == "bOff") {
        value = 0;
    }
    return value;
}

/****************************************************************************************************/

qreal ConnectorAlgorithm::connectorDistance() const
{
    QPair<LayoutNodeAtom*,LayoutNodeAtom*> neighbors = layout()->neighbors();
    LayoutNodeAtom* srcAtom = neighbors.first;
    LayoutNodeAtom* dstAtom = neighbors.second;
    return (srcAtom && dstAtom) ? srcAtom->distanceTo(dstAtom) : 0.0;
}

qreal ConnectorAlgorithm::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) {
    qreal value = -1.0;
    if (type == "w" || type == "h") {
        value = 100;
    } else if (type == "connDist") {
        value = connectorDistance();
    } else if (type == "stemThick") {
        value = values.value("h") * 0.60;
    } else if (type == "begMarg" || type == "endMarg") {
        value = 3.175;
    } else if (type == "begPad") {
        //Q_ASSERT(values.contains("connDist")); // can happen
        //value = values.value("connDist") * 0.22;
        //value = (values.contains("connDist") ? values.value("connDist") : connectorDistance()) * 0.22;
        value = connectorDistance() * 0.22;
    } else if (type == "endPad") {
        value = connectorDistance() * 0.25;
    } else if (type == "bendDist") {
        value = connectorDistance() * 0.50;
    } else if (type == "hArH") {
        value = values.value("h") * 1.00;
    } else if (type == "wArH") {
        value = values.value("h") * 0.50;
    } else if (type == "diam") {
        value = connectorDistance() * 1.00;
    }
    return value;
}

void ConnectorAlgorithm::virtualDoLayoutChildren() {
    // Get a list of all child-layouts of our parent to apply the connector-algorithm on our direct
    // neighbors. Also while on it also determinate our own position in that list.
    QPair<LayoutNodeAtom*,LayoutNodeAtom*> neighbors = layout()->neighbors();
    LayoutNodeAtom* srcAtom = neighbors.first;
    LayoutNodeAtom* dstAtom = neighbors.second;
    if(!srcAtom || !dstAtom) {
        if(layout()->parent()) {
            // If there is no source- or destination to connect with then hide our layout by detaching it.
            layout()->parent()->removeChild(QExplicitlySharedDataPointer<AbstractAtom>(layout()));
        }
        return;
    }
    
    // Beginning and end points defines different connection sites available on a node. This can be one of the following values;
    // * auto       Specifies that the algorithm will determine the best connection site to use.
    // * bCtr       Specifies that the bottom, center connection site is to be used.
    // * bL         Specifies that the bottom, left connection site is to be used.
    // * bR         Specifies that the bottom right connection site is to be used.
    // * ctr        Specifies that the center connection site is to be used.
    // * midL       Specifies that the middle left connection site is to be used.
    // * midR       Specifies that the middle right connection site is to be used.
    // * radial     Specifies connections along a radial path to support the use of connections in cycle diagrams.
    // * tCtr       Specifies that the top center connection site is to be used.
    // * tL         Specifies that the top left connection site is to be used.
    // * tR         Specifies that the top right connection site is to be used.
    QString begPts = layout()->algorithmParam("begPts");
    QString endPts = layout()->algorithmParam("endPts");
    //if (!begPts.isEmpty() && !endPts.isEmpty()) kDebug()<<"begPts="<<begPts<<"endPts="<<endPts;

    ValueCache srcValues = srcAtom->m_values;
    QMap<QString, qreal> dstValues = dstAtom->m_values;
    QMap<QString, qreal> srcFactors = srcAtom->m_factors;
    QMap<QString, qreal> dstFactors = dstAtom->m_factors;
    QMap<QString, int> srcCountFactors = srcAtom->m_countFactors;
    QMap<QString, int> dstCountFactors = dstAtom->m_countFactors;
    qreal srcX = srcValues["l"];//+srcValues["ctrX"];
    qreal srcY = srcValues["t"];//+srcValues["ctrY"];
    qreal srcW = srcValues["w"];
    qreal srcH = srcValues["h"];
    qreal dstX = dstValues["l"];//+dstValues["ctrX"];
    qreal dstY = dstValues["t"];//+dstValues["ctrY"];
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
    layout()->m_rotateAngle = atan2(dstCY - srcCY, dstCX - srcCX) * 180 / M_PI;
    
    AbstractAlgorithm::virtualDoLayoutChildren();
}

/****************************************************************************************************/

qreal CycleAlgorithm::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>&) {
    qreal value = -1.0;
      if (type == "w" || type == "h") {
        value = 100;
    } else if (type == "diam") {
        value = 0;
    } else if (type == "sibSp") {
          value = 0;
    } else if (type == "sp") {
        value = 0;
    }
    return value;
}

// http://msdn.microsoft.com/en-us/library/dd439451(v=office.12).aspx
void CycleAlgorithm::virtualDoLayout() {
    AbstractAlgorithm::virtualDoLayout();

    QList<LayoutNodeAtom*> childs = childLayouts();
    Q_ASSERT(!childs.isEmpty());

    // Specifies the angle at which the first shape is placed. Angles are in degrees, measured clockwise from a line pointing straight upward from the center of the cycle.
    int startAngel = layout()->algorithmParam("stAng", "0").toInt();
    // Specifies the angle the cycle spans. Final shapealign text is placed at stAng+spanAng, unless spanAng=360. In that case, the algorithm places the text so that shapes do not overlap.
    int spanAngel = layout()->algorithmParam("spanAng", "360").toInt();

    // Specifies where to place nodes in relation to the center circle.
    bool firstNodeInCenter = layout()->algorithmParam("ctrShpMap", "none") == "fNode";

    LayoutNodeAtom* nodeInCenter = firstNodeInCenter ? childs.takeFirst() : 0;
    const qreal childsCount = qMax(1, childs.count());

    //const qreal w = layout()->finalValues()["w"];
    //const qreal h = layout()->finalValues()["h"];
    const qreal w = layout()->finalValues()["w"];
    const qreal h = layout()->finalValues()["h"];

    const qreal rx = w / 2.0;
    const qreal ry = h / 2.0;
    qreal num = 360.0 / childsCount;
    const bool inverse = startAngel > spanAngel;
    if(inverse) num = -num;

    qreal spacing = 0.0;//layout()-> "sibSp"] * childsCount;
    qreal dw = ( (2.0 * M_PI * rx - spacing) / childsCount );
    qreal dh = ( (2.0 * M_PI * ry - spacing) / childsCount );
    
    if(nodeInCenter) {
        setNodePosition(nodeInCenter, rx, ry, -1, -1); //dw, dh);
    }

    //for(qreal degree = startAngel; (!childs.isEmpty()) && (inverse ? degree > spanAngel : degree <= spanAngel); degree -= num) {
    for(qreal degree = startAngel; (!childs.isEmpty()) && (inverse ? degree > spanAngel : degree <= spanAngel); degree += num) {
        const qreal radian = (degree - 90.0) * (M_PI / 180.0);
        const qreal x = rx + cos(radian) * rx;
        const qreal y = ry + sin(radian) * ry;
        LayoutNodeAtom* l = childs.takeFirst();
        setNodePosition(l, x, y, dw, dh);
    }
}

/****************************************************************************************************/


void LinearAlgorithm::virtualDoLayout()
{
    Q_FOREACH( QExplicitlySharedDataPointer< ConstraintAtom > atom, layout()->constraints() )
        atom->applyConstraint( QExplicitlySharedDataPointer< LayoutNodeAtom > ( layout() ) );
    QString direction = layout()->algorithmParam("linDir", "fromL");
    const qreal lMarg = layout()->finalValues()[ "lMarg" ];
    const qreal rMarg = layout()->finalValues()[ "rMarg" ];
    const qreal tMarg = layout()->finalValues()[ "tMarg" ];
    const qreal bMarg = layout()->finalValues()[ "bMarg" ];
    const qreal w = layout()->finalValues()["w"] - lMarg - rMarg;
    const qreal h = layout()->finalValues()["h"] - bMarg - tMarg;
    Q_ASSERT( lMarg == 0 );
    
    QList<LayoutNodeAtom*> childs = childLayouts();
     
    Q_ASSERT(!childs.isEmpty());
    const qreal childsCount = qMax(1, childs.count());
    // first passthrough
    int x, y, mx, my;
    x = y = mx = my = 0;
    if(direction == "fromL") {
        mx = w / childsCount;
        x = lMarg;
    } else if(direction == "fromR") {
        x = lMarg + w;
        mx = -(w / childsCount);
    } else if(direction == "fromT") {
        my = h / childsCount;
    } else if(direction == "fromB") {
        y = h;
        my = -(h / childsCount);
    }
    
    const int constX = x;
    const int constY = y;

    qreal dw = w / childsCount;
    qreal dh = h / childsCount;
    foreach(LayoutNodeAtom* l, childs) {
        setNodePosition(l, x, y, dw, dh);
        x += mx;
        y += my;
    }   
    Context* childContext = new Context( *context() );
    doubleLayoutContext.append( childContext );
    childContext->m_parentLayout = this->layout();
    foreach ( LayoutNodeAtom* layNode, childs )
    {                
        childContext->m_parentLayout = this->layout();
        layNode->layoutAtom( childContext );
    }
    //applyConstraints();
    //second passthrough
        
    // calculate weights
    qreal summedWidth = 0;
    qreal summedHeight = 0;
    qreal totalSumWidth = 0;
    qreal totalSumHeight = 0;
    //int numOfRelevantChildren = 0;
    // TODO if spaceAlg is posititive or creates any spaces it has to count, will be a horror to calculate it
    foreach ( LayoutNodeAtom* layNode, childs )
    {        
        QMap< QString, qreal > vals = layNode->finalValues();
        //Q_ASSERT( layNode->m_name != "space" );
        if ( layNode->algorithmType() != AlgorithmAtom::SpaceAlg )
        {
            summedHeight += vals[ "h" ];
            summedWidth += vals[ "w" ];
        }
        totalSumWidth += vals[ "w" ];
        totalSumHeight += vals[ "h" ];
        
        
    }
    qreal currentX = constX;
    qreal currentY = constY;
    qreal currentWidth = 0;
    qreal currentHeight = 0;
    const int xFactor = mx >=  0 ? 1 : -1;
    const int yFactor = my >= 0 ? 1 : -1;
    foreach(LayoutNodeAtom* l, childs) {
        QMap< QString, qreal > values = l->finalValues();
        if ( l->algorithmType() != AlgorithmAtom::SpaceAlg )
        {
            currentWidth = l->finalValues()[ "w" ] / summedWidth * w;
            currentHeight = l->finalValues()[ "h" ] / summedHeight * h;
            setNodePosition(l, currentX, currentY, currentWidth, currentHeight);
            
            if ( direction == "fromR" || direction == "fromL" )
                currentX = currentX + xFactor * l->finalValues()[ "w" ];
            else
                currentY = currentY + yFactor * l->finalValues()[ "h" ];
        }
        else
        {
            currentWidth = l->finalValues()[ "w" ] / totalSumWidth * w;
            if ( l->m_values.hasNegativeWidth() )
                currentWidth = -currentWidth;
            //Q_ASSERT( l->m_factors["w"] > 0 );
            currentHeight = l->finalValues()[ "h" ] / totalSumHeight * h;
            if ( direction == "fromR" || direction == "fromL" )
                currentX += currentWidth;
            else
                currentY += currentHeight;
        }
    }
    
}

/****************************************************************************************************/

qreal SnakeAlgorithm::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) {
    Q_UNUSED(values);
    qreal value = -1.0;
    if (type == "w" || type == "h") {
        value = 100;
    } else if (type == "alignOff" || type == "sp" || type == "begPad" || type == "endPad") {
        value = 0;
    }
    return value;
}

// http://msdn.microsoft.com/en-us/library/dd439436(v=office.12).aspx
void SnakeAlgorithm::virtualDoLayout() {
    // Specifies from which corner the snake grows. For example, if the algorithm uses a top left value, the snake grows from the top left.
    const QString growDirection = layout()->algorithmParam("grDir", "tL");
    // Specifies whether nodes are arranged in rows or columns.
    const QString flowDirection = layout()->algorithmParam("flowDir");
    // Specifies the direction of the subsequent row or column. For example, if the algorithm initially places the nodes from left to right, revDir places the nodes in the next row from right to left. However if the algorithm uses contDir, the nodes on the next row are arranged from left to right.
    const QString continueDirection = layout()->algorithmParam("contDir");
    // Specifies the offset.
    const QString offset = layout()->algorithmParam("off");

    QList<LayoutNodeAtom*> childs = childLayouts();
    Q_ASSERT(!childs.isEmpty());
    //const qreal childsCount = qMax(1, childs.count());

    const qreal w = layout()->finalValues()["w"];
    const qreal h = layout()->finalValues()["h"];

    bool inRows = flowDirection != "column";
    bool inSameDirection = continueDirection != "revDir";
    
    enum { TopLeft, TopRight, BottomLeft, BottomRight } direction = TopLeft;
    if(growDirection == "tR") {
        direction = TopRight;
    } else if(growDirection == "bL") {
        direction = BottomLeft;
    } else if(growDirection == "bR") {
        direction = BottomRight;
    }

    //TODO is hardcoding correct here? The specs say default is 100...
    qreal dw = 100;
    qreal dh = 100;
    qreal x = 0;
    qreal y = 0;
    qreal mx = 110;
    qreal my = 110;

    //TODO use direction
    foreach(LayoutNodeAtom* l, childs) {
        if(l->algorithmType() == AlgorithmAtom::SpaceAlg) continue; // specs says 'or does nothing' but not under which conditions :-/
        setNodePosition(l, x, y, dw, dh);
        if(!inSameDirection) inRows = !inRows;
        if(inRows) {
            y += my;
            if(y+my > h) {
                x += mx;
                y = 0;
            }
        } else {
            x += mx;
            if(x+mx > w) {
                x = 0;
                y += my;
            }
        }
    }
}

/****************************************************************************************************/

qreal HierarchyAlgorithm::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>&) {
    qreal value = -1.0;
    if (type == "w" || type == "h") {
        value = 100;
    } else if (m_isRoot && (type == "alignOff" || type == "sp")) {
        value = 0;
    } else if (!m_isRoot && (type == "sibSp" || type == "secSibSp")) {
        value = 0;
    }
    return value;
}

// http://msdn.microsoft.com/en-us/library/dd439442(v=office.12).aspx
// http://msdn.microsoft.com/en-us/library/dd439449(v=office.12).aspx
void HierarchyAlgorithm::virtualDoLayout() {
    kDebug()<<"TODO Implement algorithm isRoot="<<m_isRoot;
    AbstractAlgorithm::virtualDoLayout();
}

/****************************************************************************************************/

qreal PyramidAlgorithm::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>&) {
    qreal value = -1.0;
    if (type == "w" || type == "h") {
        value = 100;
    } else if (type == "pyraAcctRatio") {
        value = 0.33;
    }
    return value;
}

void PyramidAlgorithm::virtualDoLayout() {
    kDebug()<<"TODO Implement algorithm";
    AbstractAlgorithm::virtualDoLayout();
}

/****************************************************************************************************/

//NOTE I start to assume that the parent layout-algorithms are also resposible for setting defaults at children
//layout-algorithms. If that's the case then the question is how/where that happens. To bad the specs are
//missing the most basic informations :-(
qreal SpaceAlg::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) {
    Q_UNUSED(values);
    qreal value = -1.0;
    if (type == "w" || type == "h") {
        kDebug()<<"TODO type="<<type;
        value = 100; //TODO what default value is expected here?
    } else if (type == "sibSp") {
        kDebug()<<"TODO type="<<type;
        value = 0; //TODO what default value is expected here?
    }
    return value;
}

void SpaceAlg::virtualDoLayout() {
    // just don't do anything cause the space-algorithm is just a placeholder-algorithm
    AbstractAlgorithm::virtualDoLayout();
}

/****************************************************************************************************/

qreal TextAlgorithm::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) {
    qreal value = -1.0;
    if (type == "w" || type == "h") {
        value = 100;
    } else if (type == "primFontSz" || type == "secFontSize") {
        value = 100;
    } else if (type == "tMarg") {
        value = values.value("primFontSz") * 0.78;
    } else if (type == "bMarg") {
        value = values.value("primFontSz") * 0.60;
    } else if (type == "lMarg") {
        value = values.value("primFontSz") * 0.42;
    } else if (type == "rMarg") {
        value = values.value("primFontSz") * 0.42;
    }/* else if ( type == "r" && values.contains( "w" ) ) {
        value = values["l"] + values["w"];
    } else if ( type == "l" && values.contains( "r" ) && values.contains( "w" ) ) {
        value = values["r"] - values["w"];
    } else if ( type == "l" ) {
        value =  0;
    }*/
    return value;
}

void TextAlgorithm::virtualDoLayout() {
    //TODO implement the text-layout logic
    AbstractAlgorithm::virtualDoLayout();
}

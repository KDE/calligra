/*
 * This file is part of Office 2007 Filters for Calligra
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
#include <iterator>
#include <QXmlStreamReader>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoShapeSavingContext.h>
#include "MsooXmlDebug.h"

#define MSOOXML_CURRENT_NS "dgm"
#define MSOOXML_CURRENT_CLASS MsooXmlDiagramReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader.h>
#include <MsooXmlReader_p.h>
#include <MsooXmlCommonReader.h>
#include <MsooXmlUtils.h>
#include <MsooXmlDiagramReader.h>

#include <QTextStream>

#include <algorithm>

namespace MSOOXML { namespace Diagram {

//#define ASSERT_X(condition, errormessage) Q_ASSERT_X(condition, __FUNCTION__, errormessage)
#define ASSERT_X(condition, errormessage) 
#define DEBUG_DUMP debugMsooXml << QString("%1%2").arg(QString(' ').repeated(level*2)).arg(m_tagName)
//#define DEBUG_DUMP debugMsooXml << QString("%1Dgm::%2::%3").arg(QString(' ').repeated(level)).arg(typeid(this).name()).arg(__FUNCTION__) << this << "atom=" << m_tagName
#define DEBUG_WRITE debugMsooXml << QString("Dgm::%1::%2").arg(typeid(this).name()).arg(__FUNCTION__) << "atom=" << m_tagName

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

Context::~Context() {
    //TODO make sure this is no memoryleak
    //delete m_rootPoint;
    //delete m_connections;
}
        
AbstractNode* Context::currentNode() const {
    return m_currentNode;
}

void Context::setCurrentNode(AbstractNode* node) {
    m_currentNode = node;
}

ValueCache::ValueCache() : m_rect( QRectF( 0.0f, 0.0f, 100.0f, 100.0f ) ), m_unmodified( true ), m_negativeWidth( false ), m_negativeHeight( false ) {}

bool ValueCache::hasNegativeWidth() const {
    return m_negativeWidth;
}

bool ValueCache::hasNegativeHeight() const {
    return m_negativeHeight;
}

qreal ValueCache::value( const QString& name, bool *valid ) const {
    if ( valid )
        *valid = true;
    if ( isRectValue( name ) )
        return rectValue( name );
    if ( valid && ! m_mapping.contains( name ) )
        *valid = false;
    return m_mapping[ name ];
}

bool ValueCache::valueExists( const QString& name ) {
    return isRectValue( name ) || m_mapping.contains( name );
}

void ValueCache::setValue( const QString& name, qreal value ) {
    if ( isRectValue( name ) )
        setRectValue( name, value );
    else
        m_mapping[ name ] = value;
}

qreal ValueCache::operator[]( const QString& name ) const  {
    return value( name );
}

ValueCache::ResultWrapper ValueCache::operator[]( const char* name ) {
    return ResultWrapper( this, QString::fromLatin1( name ) );
}

ValueCache::ResultWrapper ValueCache::operator[]( const QString& name ) {
    return ResultWrapper( this, name );
}

ValueCache::operator QMap< QString, qreal >() const {
    QMap < QString, qreal > result = m_mapping;
    result[ "l" ] = m_rect.left();
    result[ "r" ] = m_rect.right();
    result[ "t" ] = m_rect.top();
    result[ "b" ] = m_rect.bottom();
    result[ "w" ] = m_rect.width();
    result[ "h" ] = m_rect.height();
    result[ "ctrX" ] = m_rect.center().rx();
    result[ "ctrY" ] = m_rect.center().ry();
    return result;
}

bool ValueCache::isRectValue( const QString& name ) const {
    return name == "l" || name == "r" || name == "w" || name == "h" || name == "t" || name == "b" || name == "ctrX" || name == "ctrY";
}

qreal ValueCache::rectValue( const QString& name ) const {
    if ( name == "l")
        return m_rect.left();
    if ( name == "r" )
        return m_rect.right();
    if ( name == "w" )
        return m_rect.width();
    if ( name == "h" )
        return m_rect.height();
    if ( name == "t" )
        return m_rect.top();
    if ( name == "b" )
        return m_rect.bottom();
    if ( name == "ctrX" )
        return m_rect.center().rx();
    if ( name == "ctrY" )
        return m_rect.center().ry();
    return 0.0;
}
#include <limits>
void ValueCache::setRectValue( const QString& name, qreal value ) {
    if ( name == "l") {
        m_rect.moveLeft( value );
    } else if ( name == "r" ) {
        m_rect.moveRight( value );
    } else if ( name == "w" ) {
		m_rect.setWidth( value );
    } else if ( name == "h" ) {
        //TODO this is a hack, as its not really described how to handle infinite values during layouting
        if ( value != std::numeric_limits<qreal>::infinity() )
            m_rect.setHeight( value );
        else
            m_rect.setHeight( m_rect.width() );
    } else if ( name == "t" ) {
        m_rect.moveTop( value );
    } else if ( name == "b" ) {
        m_rect.moveBottom( value );
    } else if ( name == "ctrX" ) {
        m_rect.moveCenter( QPointF( value, m_rect.center().y() ) );
    } else if ( name == "ctrY" ) {
        m_rect.moveCenter( QPointF( m_rect.center().x(), value ) );
    } else {
        ASSERT_X( false, QString("TODO unhandled name=%1 value=%2").arg(name).arg(value).toLocal8Bit() );
    }
    m_unmodified = false;
}

/****************************************************************************************************/

AbstractNode::AbstractNode(const QString &tagName) : m_tagName(tagName), m_parent(0) {}
AbstractNode::~AbstractNode() { qDeleteAll(children()); }

void AbstractNode::dump(Context* context, int level) {    
    foreach(AbstractNode* node, children())
        node->dump(context, level + 1);    
}

void AbstractNode::dump( QTextStream& device ) {
    foreach(AbstractNode* node, children())
        node->dump( device );
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

AbstractNode* AbstractNode::parent() const {
    return m_parent;
}

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
    //LayoutNodeAtom* layNode = dynamic_cast< LayoutNodeAtom* >( node );
}

void AbstractNode::addChild(AbstractNode* node) {
    Q_ASSERT(!node->m_parent);
    Q_ASSERT(!m_orderedChildrenReverse.contains(node));
    node->m_parent = this;
    m_appendedChildren.append(node);
    m_cachedChildren.clear();
    //LayoutNodeAtom* layNode = dynamic_cast< LayoutNodeAtom* >( this );
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

void MSOOXML::Diagram::ConnectionNode::dump(QTextStream& device) {
    foreach(AbstractNode* node, peers() ) {
        ConnectionNode* connNode = dynamic_cast< ConnectionNode* > ( node );
        PointNode* pointNode = dynamic_cast< PointNode* > ( node );
        if ( connNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << connNode->m_tagName << connNode->m_modelId << "\"\n";
        else if ( pointNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << pointNode->m_tagName << pointNode->m_modelId << "\"\n";
    }
    foreach(AbstractNode* node, children()) {
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

void PointListNode::dump( QTextStream& device ) {
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

void MSOOXML::Diagram::PointNode::dump(QTextStream& device) {
    foreach(AbstractNode* node, peers() ) {
        ConnectionNode* connNode = dynamic_cast< ConnectionNode* > ( node );
        PointNode* pointNode = dynamic_cast< PointNode* > ( node );
        if ( connNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << connNode->m_tagName << connNode->m_modelId << "\"[label=\"" << /*m_tagName << m_modelId << " " <<*/ m_text << "\"]\n";
        else if ( pointNode )
            device << "\"" << m_tagName << m_modelId << "\" -> \"" << pointNode->m_tagName << pointNode->m_modelId << "\"[label=\"" << /*m_tagName << m_modelId << " " <<*/ m_text << "\"]\n";
    }
    foreach(AbstractNode* node, children()) {
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
    //DEBUG_DUMP;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
        atom->dump(context, level + 1);
    }
}

void AbstractAtom::readElement(Context* context, MsooXmlDiagramReader* reader) {
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
        } else if (reader->qualifiedName() == QLatin1String("dgm:adj")) {
            node = new AdjustAtom;
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
            debugMsooXml<<"TODO atom="<<m_tagName<<"qualifiedName="<<reader->qualifiedName();
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
    //for( int i = 0; i < m_children.count(); ++i ) m_children[ i ]->build( context );
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
        atom->build(context);
    }
}

void AbstractAtom::finishBuild(Context* context) {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
        atom->finishBuild(context);
    }
}

void AbstractAtom::layoutAtom(Context* context) {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
        if (LayoutNodeAtom* layAtom = dynamic_cast< LayoutNodeAtom* >( atom.data() ))
            layAtom->setNeedsRelayout( true );
        atom->layoutAtom(context);
    }
}

void AbstractAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children)
        atom->writeAtom(context, xmlWriter, styles);
}

QExplicitlySharedDataPointer<LayoutNodeAtom> AbstractAtom::parentLayout() const {
    LayoutNodeAtom* p = 0;
    for(QExplicitlySharedDataPointer<AbstractAtom> a = parent(); a && !p; a = a->parent())
        p = dynamic_cast<LayoutNodeAtom*>(a.data());
    return QExplicitlySharedDataPointer<LayoutNodeAtom>(p);
}

QExplicitlySharedDataPointer<AbstractAtom> AbstractAtom::parent() const {
    return m_parent;
} 

QVector< QExplicitlySharedDataPointer<AbstractAtom> > AbstractAtom::children() const {
    return m_children;
}

int AbstractAtom::indexOfChild(AbstractAtom* node) const {
    for ( int i = 0; i < m_children.count(); ++i )
        if ( m_children[ i ].data() == node )
            return i;
    return -1;
}

void AbstractAtom::addChild(AbstractAtom* node) {
    addChild(QExplicitlySharedDataPointer<AbstractAtom>(node));
}

void AbstractAtom::addChild(QExplicitlySharedDataPointer<AbstractAtom> node) {
    node->m_parent = this;
    m_children.append(node);
}

void AbstractAtom::insertChild(int index, AbstractAtom* node) {
    insertChild(index, QExplicitlySharedDataPointer<AbstractAtom>(node));
}

void AbstractAtom::insertChild(int index, QExplicitlySharedDataPointer<AbstractAtom> node) {
    node->m_parent = this;
    if ( index < m_children.count() )
      m_children.insert(index, node);
    else
      m_children.append( node );
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
    Q_ASSERT(context->currentNode());
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
        } else if(axis == QLatin1String("ch")) { // Children
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
            warnMsooXml<<"TODO preced";
            //TODO
        } else if(axis == QLatin1String("precedSib")) { // Preceding Sibling
            warnMsooXml<<"TODO precedSib";
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

AlgorithmAtom* AlgorithmAtom::clone(Context* context) {
    AlgorithmAtom* atom = new AlgorithmAtom;
    atom->m_type = m_type;
    atom->m_params = m_params;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone(context));
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

LayoutNodeAtom* LayoutNodeAtom::clone(Context* context) {
    LayoutNodeAtom* atom = new LayoutNodeAtom;
    atom->m_name = m_name;
    atom->m_values = m_values;
    atom->m_factors = m_factors;
    atom->m_countFactors = m_countFactors;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone(context));
    atom->m_rotateAngle = m_rotateAngle;
    atom->m_needsReinit = m_needsReinit;
    atom->m_needsRelayout = m_needsRelayout;
    atom->m_childNeedsRelayout = m_childNeedsRelayout;
    atom->m_variables = m_variables;
    atom->m_firstLayout = m_firstLayout;
    atom->setAxis(context, axis( context ));
    return atom;
}

void LayoutNodeAtom::dump(Context* context, int level) {
    QStringList list;
    foreach(AbstractNode* n, axis( context ))
        if(PointNode* p = dynamic_cast<PointNode*>(n))
            list.append( QString("modelId=%1 type=%2 cxnId=%3").arg(p->m_modelId).arg(p->m_type).arg(p->m_cxnId) );
        else
            list.append( QString("tagName=%1").arg(n->m_tagName) );
    //DEBUG_DUMP << "name=" << m_name << "variables=" << m_variables << "values=" << finalValues();
    DEBUG_DUMP << "name=" << m_name << list;
    AbstractAtom::dump(context, level);
}

void LayoutNodeAtom::readAll(Context* context, MsooXmlDiagramReader* reader) {
    const QXmlStreamAttributes attrs(reader->attributes());
    TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_name)
    //TRY_READ_ATTR_WITHOUT_NS_INTO(styleLbl, m_styleLbl)
    QExplicitlySharedDataPointer<LayoutNodeAtom> ptr(this);
    QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
    context->m_parentLayout = ptr;
    AbstractAtom::readAll(context, reader);
    context->m_parentLayout = oldLayout;
}

/*
class ConstraintPredicate
{
    public:
        bool operator()( const QExplicitlySharedDataPointer<MSOOXML::Diagram::AbstractAtom> &value ) {
            ListAtom *atom = dynamic_cast< ListAtom* >( value.data() );
            if ( !atom )
                return true;
            foreach( QExplicitlySharedDataPointer<AbstractAtom> val, atom->children() )
                if ( dynamic_cast< ConstraintAtom* >( val.data() ) )
                    return false;
            return true;
        }
};
*/

void LayoutNodeAtom::build(Context* context) {
#if 0
    QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;
    context->m_layoutPointMap[ this ] = context->currentNode();
    typedef QVector< QExplicitlySharedDataPointer<AbstractAtom> > AtomPList;
    AtomPList::iterator it = std::stable_partition( m_children.begin(), m_children.end(), ConstraintPredicate() );
    std::copy( it, m_children.end(), std::back_inserter( m_constraintsToBuild ) );
    m_children.erase( it, m_children.end() );
    AbstractAtom::build(context);
    foreach( QExplicitlySharedDataPointer<AbstractAtom> constr, m_constraintsToBuild )
        constr->build( context );
    m_constraintsToBuild.clear();
    context->m_parentLayout = oldLayout;
#else
    QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;
    AbstractNode* oldCurrentNode = context->currentNode();

    AbstractAtom::build(context);

    context->setCurrentNode(oldCurrentNode);
    context->m_parentLayout = oldLayout;
#endif
}

void LayoutNodeAtom::finishBuild(Context* context) {
    QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;
    AbstractAtom::finishBuild(context);
    context->m_parentLayout = oldLayout;
    
    delete m_algorithmImpl;
    m_algorithmImpl = 0;
    QExplicitlySharedDataPointer<AlgorithmAtom> alg = algorithm();
    switch(alg ? alg->m_type : AlgorithmAtom::UnknownAlg) {
        case AlgorithmAtom::UnknownAlg:
            warnMsooXml << "Layout with name=" << m_name << "defines an unknown algorithm.";
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
}

void LayoutNodeAtom::layoutAtom(Context* context) {
    if(m_algorithmImpl) {
        m_algorithmImpl->doInit(context, QExplicitlySharedDataPointer<LayoutNodeAtom>(this));
    }
    if(m_needsRelayout && m_algorithmImpl) {
        m_needsRelayout = false;
        m_childNeedsRelayout = true;
        m_algorithmImpl->doLayout();
    }
    if(m_childNeedsRelayout && m_algorithmImpl) {
        m_childNeedsRelayout = false;
        m_algorithmImpl->doLayoutChildren();
    }
}

void LayoutNodeAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
    QExplicitlySharedDataPointer<LayoutNodeAtom> oldLayout = context->m_parentLayout;
    context->m_parentLayout = this;
    AbstractAtom::writeAtom(context, xmlWriter, styles);
    context->m_parentLayout = oldLayout;
}

QList< QExplicitlySharedDataPointer<ConstraintAtom> > LayoutNodeAtom::constraints() const {
    QList< QExplicitlySharedDataPointer<ConstraintAtom> > result;
    foreach( QExplicitlySharedDataPointer<AbstractAtom> atom, m_children )
    {
        ConstraintAtom* constraintAtom = dynamic_cast< ConstraintAtom* >( atom.data() );
        if ( constraintAtom ) {
            result.append( QExplicitlySharedDataPointer<ConstraintAtom>(constraintAtom));
        } else if (ListAtom *list = dynamic_cast< ListAtom* >( atom.data() ) ) {
            foreach( QExplicitlySharedDataPointer<AbstractAtom> val, list->children() ) {
                constraintAtom =  dynamic_cast< ConstraintAtom* >( val.data() );
                if ( constraintAtom )
                    result.append(QExplicitlySharedDataPointer<ConstraintAtom>(constraintAtom));
            }
        }
    }
    return result;
}

QList< QExplicitlySharedDataPointer<ShapeAtom> > LayoutNodeAtom::shapes() const {
    QList< QExplicitlySharedDataPointer<ShapeAtom> > result;
    foreach( QExplicitlySharedDataPointer<AbstractAtom> atom, m_children ) {
        ShapeAtom* shapeAtom = dynamic_cast< ShapeAtom* >( atom.data() );
        if ( shapeAtom ) {
            result.append(QExplicitlySharedDataPointer<ShapeAtom>(dynamic_cast< ShapeAtom* >( atom.data() )));
        } else if (ListAtom *list = dynamic_cast< ListAtom* >( atom.data() ) ) {
            foreach( QExplicitlySharedDataPointer<AbstractAtom> val, list->children() ) {
                shapeAtom = dynamic_cast< ShapeAtom* >( val.data() );
                if ( shapeAtom )
                    result.append(QExplicitlySharedDataPointer<ShapeAtom>(shapeAtom));
            }
        }
    }
    return result;
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

QList<AbstractNode*> LayoutNodeAtom::axis(Context* context) const {
    return context->m_layoutPointMap.values(this);
}

void LayoutNodeAtom::setAxis(Context* context, const QList<AbstractNode*> &axis) {
    Q_UNUSED(context);
    // first remove the previous axis
    foreach(AbstractNode* node, context->m_layoutPointMap.values(this)) {
        context->m_pointLayoutMap.remove(node, this);
    }
    context->m_layoutPointMap.remove(this);
    Q_ASSERT(!context->m_pointLayoutMap.values().contains(this));
    Q_ASSERT(!context->m_layoutPointMap.keys().contains(this));
    // then set the new axis
    foreach(AbstractNode* node, axis) {
        context->m_layoutPointMap.insertMulti(this, node);
        context->m_pointLayoutMap.insertMulti(node, this);
    }
    // job done, new layout needed
    setNeedsRelayout(true);
}

void LayoutNodeAtom::setAxis(Context* context, PresentationOfAtom* atom) {
    setAxis(context, fetchAxis(context, atom->m_axis, atom->m_ptType, atom->m_start, atom->m_count, atom->m_step));
}

void LayoutNodeAtom::setNeedsReinit(bool needsReinit) {
    if(m_needsReinit == needsReinit) return;
    m_needsReinit = needsReinit;
    if(m_needsReinit) // if we need to be re-initialized then our children need to be too
        foreach(QExplicitlySharedDataPointer<AbstractAtom> child, children())
            if(LayoutNodeAtom* childLayoutAtom = dynamic_cast<LayoutNodeAtom*>(child.data())) {
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
    //debugMsooXml << m_name;
    ValueCache result = m_values;
	//QMap< QString, qreal > print = m_values;
	//debugMsooXml << "prefinal: " << print;
	//debugMsooXml << "final values";
    for( QMap< QString, qreal>::const_iterator it = m_factors.constBegin(); it != m_factors.constEnd(); ++it ) {
        result[ it.key() ] = result[ it.key() ] * it.value() / qreal ( m_countFactors[ it.key() ] );        
		//debugMsooXml << "key " << it.key() << " value: " << it.value() << " count: " << m_countFactors[ it.key() ];
    }
    //debugMsooXml << "end of final values";
    return result;
}

QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > LayoutNodeAtom::fetchLayouts(Context* context, const QString &forAxis, const QString &forName, const QString &ptType) const {
    QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > list;
    if ( forAxis == "self" || forAxis.isEmpty() ) {
        list.append( QExplicitlySharedDataPointer<LayoutNodeAtom>(const_cast<LayoutNodeAtom*>(this)) );
    } else {
        if ( forAxis == "ch" ) { // Children
            list = childrenLayouts();
        } else if ( forAxis == "des" ) { // Descendant
            list = descendantLayouts();
        } else {
            ASSERT_X(false, QString("Unsupported forAxis '%1'").arg( forAxis ).toLocal8Bit());
        }
    }
    QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > result;
    foreach(const QExplicitlySharedDataPointer<LayoutNodeAtom> &l, list) {
        if (!forName.isEmpty() && forName != l->m_name) {
            continue;
        }
        if (!ptType.isEmpty()) {
            bool ptTypeMatches = false;
            foreach(AbstractNode* node, l->axis( context )) {
                if ( PointNode *ptNode = dynamic_cast< PointNode* >( node ) ) {
                    if (ptType != ptNode->m_type)
                        continue;
                } else if ( ConnectionNode *connNode = dynamic_cast< ConnectionNode* >( node ) ) {
                    if (ptType != connNode->m_type)
                        continue;
                }
                ptTypeMatches = true;
                break;
            }
            if (!ptTypeMatches) {
                continue;
            }
        }
        result.append(l);
    }
    return result;
}

QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > LayoutNodeAtom::childrenLayouts() const {
    QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > result;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, children())
        if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data()))
            result.append(QExplicitlySharedDataPointer<LayoutNodeAtom>(l));
    return result;
}

QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > LayoutNodeAtom::descendantLayouts() const {
    QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > result = childrenLayouts();
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, children())
        if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data()))
            foreach(QExplicitlySharedDataPointer<LayoutNodeAtom> atom, l->descendantLayouts())
                result.append(atom);
    return result;
}
        
QPair<LayoutNodeAtom*,LayoutNodeAtom*> LayoutNodeAtom::neighbors() const {
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
        srcIndex = siblingLayouts.count() - 1;
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

QSizeF LayoutNodeAtom::childrenUsedSize() const {
    qreal w = 0;
    qreal h = 0;
    foreach( const QExplicitlySharedDataPointer<LayoutNodeAtom> &l, childrenLayouts() ) {
        QMap< QString, qreal > vals = l->finalValues();
        if ( l->algorithmType() != AlgorithmAtom::SpaceAlg ) {
            h += vals[ "h" ];
            w += vals[ "w" ];
        }
    }
    return QSizeF(w, h);
}

QSizeF LayoutNodeAtom::childrenTotalSize() const {
    qreal w = 0;
    qreal h = 0;
    foreach( const QExplicitlySharedDataPointer<LayoutNodeAtom> &l, childrenLayouts() ) {
        QMap< QString, qreal > vals = l->finalValues();
        h += vals[ "h" ];
        w += vals[ "w" ];
    }
    return QSizeF(w, h);
}

qreal LayoutNodeAtom::distanceTo(LayoutNodeAtom* otherAtom) const {
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

ConstraintAtom* ConstraintAtom::clone(Context* context) {
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
        atom->addChild(a->clone(context));
    return atom;
}

QString ConstraintAtom::dump() const {
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
    return s.trimmed();
}

void ConstraintAtom::dump(Context*, int level) {
    DEBUG_DUMP << dump();
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

void ConstraintAtom::build(Context* context) {
    AbstractAtom::build(context);
}

void ConstraintAtom::finishBuild(Context* context) {
#if 0
    QExplicitlySharedDataPointer<ConstraintAtom> ptr(this);
    QVector< QExplicitlySharedDataPointer<ConstraintAtom> > addedConstraints;

    // first evaluate on which layouts this constraint should be applied.
    if ( m_for == "self" || m_for.isEmpty() ) {
        // nothing to do cause this constraint is already attached to the correct layout.
        Q_ASSERT( context->m_parentLayout->constraints().contains(ptr) );
    } else {
        // We need to select the chosen data-points and determinate the layoutNotes which are connected with
        // them to look where we need to move this constraint to.
        QList<AbstractNode*> nodes;
        if ( m_for == "ch" ) { // Children
            nodes = context->currentNode()->children();
        } else if ( m_for == "des" ) { // Descendant
            nodes = context->currentNode()->descendant();
        } else {
            Q_ASSERT_X(false, __FUNCTION__, QString("Constraint with unhandled 'for' %1").arg( dump() ).toLocal8Bit());
        }

        QVector< AbstractNode* > childDataPoints;
        foreach( AbstractNode* node, nodes ) {
            if ( !m_ptType.isEmpty() ) {
                if ( PointNode *ptNode = dynamic_cast< PointNode* >( node ) ) {
                    if (m_ptType != ptNode->m_type)
                        continue;
                } else if ( ConnectionNode *connNode = dynamic_cast< ConnectionNode* >( node ) ) {
                    if (m_ptType != connNode->m_type)
                        continue;
                }
            }
            childDataPoints.append( node );
        }

        /*TODO why the following? how does that make sense?
        if ( m_ptType.isEmpty() )
            childDataPoints.append( context->currentNode() );
        if ( m_refPtType.isEmpty() )
            refChildDataPoints.append( context->currentNode() );
        */

        Q_ASSERT( !childDataPoints.isEmpty() );

        bool constraintedWasApplied = false;
        foreach(AbstractNode* node, childDataPoints) {
            foreach(LayoutNodeAtom* a, context->m_pointLayoutMap.values(node)) {
                if ( !m_forName.isEmpty() && a->m_name != m_forName )
                    continue;

                QExplicitlySharedDataPointer<ConstraintAtom> clonedPtr( ptr->clone(context) );
                a->addChild(clonedPtr);
                addedConstraints.append(clonedPtr);
                constraintedWasApplied = true;
            }
        }
        if (!constraintedWasApplied) dump(0,2);
        Q_ASSERT_X(constraintedWasApplied, __FUNCTION__, QString("Constraint could not be applied %1").arg( dump() ).toLocal8Bit());

        // this constraint is handled now and we can detach it
        Q_ASSERT( context->m_parentLayout->constraints().contains(ptr) );
        parent()->removeChild(ptr);
    }
    // and now evaluated the referenced layout definitions
    if ( m_refFor == "self" || m_refFor.isEmpty() ) {
        /*
        foreach(QExplicitlySharedDataPointer<ConstraintAtom> constraint, addedConstraints) {
            constraint->m_referencedLayouts.append( context->m_parentLayout );
        }
        */
    } else  {
        QList<AbstractNode*> nodes;
        if ( m_refFor == "ch" ) { // Children
            nodes = context->currentNode()->children();
        } else if ( m_refFor == "des" ) { // Descendant
            nodes = context->currentNode()->descendant();
        } else {
            Q_ASSERT_X(false, __FUNCTION__, QString("Constraint with unhandled 'refFor' %1").arg( dump() ).toLocal8Bit());
        }

        QVector< AbstractNode* > childDataPoints;
        foreach( AbstractNode* node, nodes ) {
            if ( !m_refPtType.isEmpty() ) {
                if ( PointNode *ptNode = dynamic_cast< PointNode* >( node ) ) {
                    if (m_refPtType != ptNode->m_type)
                        continue;
                } else if ( ConnectionNode *connNode = dynamic_cast< ConnectionNode* >( node ) ) {
                    if (m_refPtType != connNode->m_type)
                        continue;
                }
            }
            childDataPoints.append( node );
        }

        Q_ASSERT( !childDataPoints.isEmpty() );

        bool referenceWasApplied = false;
        foreach(AbstractNode* node, childDataPoints) {
            Q_ASSERT(context->m_pointLayoutMap.contains(node));
            foreach(LayoutNodeAtom* a, context->m_pointLayoutMap.values(node)) {
                if ( !m_refForName.isEmpty() && a->m_name != m_refForName )
                    continue;

                QExplicitlySharedDataPointer<LayoutNodeAtom> aPtr( a );
                foreach(QExplicitlySharedDataPointer<ConstraintAtom> constraint, addedConstraints) {
                    constraint->m_referencedLayouts.append( aPtr );
                }
                referenceWasApplied = true;
            }
        }
        Q_ASSERT_X(referenceWasApplied, __FUNCTION__, QString("Reference of constraint could not be applied %1").arg( dump() ).toLocal8Bit());
    }
#else
    Q_UNUSED(context);
#endif
}

void ConstraintAtom::applyConstraint(Context* context, LayoutNodeAtom* atom) {
    // Following block shows how we tried to determinate the layouts using there data-points. But that seems to be
    // wrong (with me07_basic_radial.xlsx) cause 'for' and 'refFor' are referring to the layout-tree and not the
    // data-tree which can be rather different.
#if 0
    QExplicitlySharedDataPointer<ConstraintAtom> ptr(this);
    QList< LayoutNodeAtom* > applyLayouts;
    QList< LayoutNodeAtom* > referencedLayouts;
    if ( m_for == "self" || m_for.isEmpty() ) {
        applyLayouts.append( atom /* context->m_parentLayout.data() */ );
    } else {
        QList<AbstractNode*> nodes;
        if ( m_for == "ch" ) { // Children
            nodes = context->currentNode()->children();
        } else if ( m_for == "des" ) { // Descendant
            nodes = context->currentNode()->descendant();
        } else {
            Q_ASSERT_X(false, __FUNCTION__, QString("Constraint with unhandled 'for' %1").arg( dump() ).toLocal8Bit());
        }
        QVector< AbstractNode* > childDataPoints;
        foreach( AbstractNode* node, nodes ) {
            if ( !m_ptType.isEmpty() ) {
                if ( PointNode *ptNode = dynamic_cast< PointNode* >( node ) ) {
                    if (m_ptType != ptNode->m_type)
                        continue;
                } else if ( ConnectionNode *connNode = dynamic_cast< ConnectionNode* >( node ) ) {
                    if (m_ptType != connNode->m_type)
                        continue;
                } else {
                    Q_ASSERT_X(false, __FUNCTION__, QString("Unhandled ptType=%1 for node=%2").arg(m_ptType).arg(node->m_tagName).toLocal8Bit());
                }
            }
            childDataPoints.append( node );
        }
        Q_ASSERT_X(!childDataPoints.isEmpty(), __FUNCTION__, QString("No data-points selected for constraint %1").arg(dump()).toLocal8Bit());
        foreach(AbstractNode* node, childDataPoints) {
            foreach(LayoutNodeAtom* a, context->m_pointLayoutMap.values(node)) {
                if ( m_forName.isEmpty() || a->m_name == m_forName )
                    applyLayouts.append( a );
            }
        }
        Q_ASSERT_X(!applyLayouts.isEmpty(), __FUNCTION__, QString("Failed to determinate the layout on which to apply the constraint %1").arg( dump() ).toLocal8Bit());
    }
    if ( m_refFor == "self" || m_refFor.isEmpty() ) {
        referencedLayouts.append( atom /* context->m_parentLayout.data() */ );
    } else  {
        QList<AbstractNode*> nodes;
        if ( m_refFor == "ch" ) { // Children
            nodes = context->currentNode()->children();
        } else if ( m_refFor == "des" ) { // Descendant
            nodes = context->currentNode()->descendant();
        } else {
            Q_ASSERT_X(false, __FUNCTION__, QString("Constraint with unhandled 'refFor' %1").arg( dump() ).toLocal8Bit());
        }
        QVector< AbstractNode* > childDataPoints;
        foreach( AbstractNode* node, nodes ) {
            if ( !m_refPtType.isEmpty() ) {
                if ( PointNode *ptNode = dynamic_cast< PointNode* >( node ) ) {
                    if (m_refPtType != ptNode->m_type)
                        continue;
                } else if ( ConnectionNode *connNode = dynamic_cast< ConnectionNode* >( node ) ) {
                    if (m_refPtType != connNode->m_type)
                        continue;
                } else {
                    Q_ASSERT_X(false, __FUNCTION__, QString("Unhandled ptType=%1 for node=%2").arg(m_ptType).arg(node->m_tagName).toLocal8Bit());
                }
            }
            childDataPoints.append( node );
        }
        Q_ASSERT_X(!childDataPoints.isEmpty(), __FUNCTION__, QString("No data-points selected for constraint %1").arg(dump()).toLocal8Bit());
        foreach(AbstractNode* node, childDataPoints)
            foreach(LayoutNodeAtom* a, context->m_pointLayoutMap.values(node))
                if ( m_refForName.isEmpty() || a->m_name == m_refForName )
                    referencedLayouts.append(a);
        Q_ASSERT_X(!referencedLayouts.isEmpty(), __FUNCTION__, QString("Failed to determinate the referenced layouts for the constraint %1").arg( dump() ).toLocal8Bit());
    }
#else
    QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > applyLayouts = atom->fetchLayouts(context, m_for, m_forName, m_ptType);
    QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > referencedLayouts = atom->fetchLayouts(context, m_refFor, m_refForName, m_refPtType);

    ASSERT_X(!applyLayouts.isEmpty(), QString("Failed to determinate the layouts for the constraint %1").arg( dump() ).toLocal8Bit());
    ASSERT_X(!referencedLayouts.isEmpty(), QString("Failed to determinate the referenced layouts for the constraint %1").arg( dump() ).toLocal8Bit());
	debugMsooXml << dump();

    foreach(const QExplicitlySharedDataPointer<LayoutNodeAtom> &applyLayout, applyLayouts) {
		debugMsooXml << "AppLayout: " << applyLayout->m_name;
        if( !m_value.isEmpty() ) {
            bool ok;
            qreal value = m_value.toDouble( &ok );
            ASSERT_X(ok, QString("Layout with name=%1 defines none-double value=%2").arg( atom->m_name ).arg( m_value ).toLocal8Bit());
			debugMsooXml << "applyValue: " << value;
            if (ok) {
                //applyLayout->m_factors.clear();
                //applyLayout->m_countFactors.clear();
                applyLayout->m_values[ m_type ] = value;
                applyLayout->setNeedsRelayout( true );
            }
        } else {
            //TODO proper handle the case where more then one layouts are referenced (means proper eval the constraints operator)
            LayoutNodeAtom* referencedLayout = referencedLayouts.isEmpty() ? atom : referencedLayouts.first().data();
            Q_ASSERT(referencedLayout);

            AbstractAlgorithm* r = referencedLayout->algorithmImpl();
            ASSERT_X(r, QString("No algorithm in referenced layout=%1 for constraint='%2'").arg( referencedLayout->m_name ).arg( dump() ).toLocal8Bit());

            const QMap<QString, qreal> values = referencedLayout->finalValues();
            const QString type = m_refType.isEmpty() ? m_type : m_refType;

            qreal value = -1.0;
            if( values.contains( type ) ) {
                value = values[ type ];
				debugMsooXml << "finalValue: " << value;
            } else {
                value = r ? r->defaultValue( type, values ) : -1.0;
                ASSERT_X(value >= 0.0, QString("algorithm=%1 value=%2 constraint='%3'").arg( r ? r->name() : "NULL" ).arg( value ).arg( dump() ).toLocal8Bit());
                if (value < 0.0) continue;
				debugMsooXml << "default Value: " << value;
            }
            applyLayout->m_values[ m_type ] = value;
            applyLayout->setNeedsRelayout( true );
            //applyLayout->m_factors.clear();
            //applyLayout->m_countFactors.clear();
        }
        if ( !m_fact.isEmpty() ) {
            bool ok;
            qreal v = m_fact.toDouble( &ok );
            ASSERT_X(ok, QString("Layout with name=%1 defines none-double factor=%2").arg( atom->m_name ).arg( m_fact ).toLocal8Bit());
			debugMsooXml << "fact: " << v;
            if (ok) {
                applyLayout->m_factors[ m_type ] += v;
                applyLayout->m_countFactors[ m_type ] += 1;
                //applyLayout->m_values[ m_type ] = applyLayout->m_values[ m_type ] * v;
                applyLayout->setNeedsRelayout( true );
            }
        }
    }
#endif
}

/****************************************************************************************************/

AdjustAtom* AdjustAtom::clone(Context*) {
    AdjustAtom* atom = new AdjustAtom;
    atom->m_index = m_index;
    atom->m_value = m_value;
    return atom;
}

void AdjustAtom::dump(Context*, int level) {
    DEBUG_DUMP << "index=" << m_index << "value=" << m_value;
}

void AdjustAtom::readAll(Context*, MsooXmlDiagramReader* reader) {
    const QXmlStreamAttributes attrs(reader->attributes());
    TRY_READ_ATTR_WITHOUT_NS(idx)
    m_index = idx.toInt();
    TRY_READ_ATTR_WITHOUT_NS(val)
    m_value = val.toDouble();
}

// http://social.msdn.microsoft.com/Forums/en-US/os_binaryfile/thread/74f86b76-37be-4087-b5b0-cf2fc68d5595/
void AdjustAtom::applyAdjustment(Context* /* context */, LayoutNodeAtom* /* atom */) {
    ASSERT_X(m_index >= 0 && m_index < context->m_shapeList.count(), QString("Index is out of bounds, index=%1 min=0 max=%2").arg(m_index).arg(context->m_shapeList.count()-1).toLocal8Bit());
    //TODO
    //ShapeAtom *shape = context->m_shapeList.at(m_index);
    //if (m_value > 90) m_value = 360 - (m_value - 90);
    //shape->parentLayout()->m_rotateAngle = m_value;
}

/****************************************************************************************************/

RuleAtom* RuleAtom::clone(Context*) {
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

void RuleAtom::dump(Context*, int level) {
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

void RuleAtom::readAll(Context*, MsooXmlDiagramReader* reader) {
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

ListAtom* ListAtom::clone(Context* context) {
    ListAtom* atom = new ListAtom(m_tagName);
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone(context));
    return atom;
}

void ListAtom::dump(Context* context, int level) {
    DEBUG_DUMP;
    AbstractAtom::dump(context, level);
}

void ListAtom::readElement(Context* context, MsooXmlDiagramReader* reader) {
    if (reader->isStartElement()) {
        QExplicitlySharedDataPointer<AbstractAtom> node;
        if (reader->qualifiedName() == QLatin1String("dgm:constr")) {
            node = QExplicitlySharedDataPointer<AbstractAtom>(new ConstraintAtom);
        } else if (reader->qualifiedName() == QLatin1String("dgm:adj")) {
            node = QExplicitlySharedDataPointer<AbstractAtom>(new AdjustAtom);
        } else if (reader->qualifiedName() == QLatin1String("dgm:rule")) {
            node = QExplicitlySharedDataPointer<AbstractAtom>(new RuleAtom);
        }
        if (node) {
            addChild(node);
            node->readAll(context, reader);
        }
    }
}

/****************************************************************************************************/

ShapeAtom* ShapeAtom::clone(Context* context) {
    ShapeAtom* atom = new ShapeAtom;
    atom->m_type = m_type;
    atom->m_blip = m_blip;
    atom->m_hideGeom = m_hideGeom;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone(context));
    return atom;
}

QList< QExplicitlySharedDataPointer<AdjustAtom> > ShapeAtom::adjustments() const {
    QList< QExplicitlySharedDataPointer<AdjustAtom> > result;
    foreach( QExplicitlySharedDataPointer<AbstractAtom> atom, m_children ) {
        AdjustAtom* adjustedAtom = dynamic_cast< AdjustAtom* >( atom.data() );
        if ( adjustedAtom ) {
            result.append(QExplicitlySharedDataPointer<AdjustAtom>(adjustedAtom));
        } else if (ListAtom *list = dynamic_cast< ListAtom* >( atom.data() ) ) {
            foreach( QExplicitlySharedDataPointer<AbstractAtom> val, list->children() ) {
                adjustedAtom = dynamic_cast< AdjustAtom* >( val.data() );
                if ( adjustedAtom )
                    result.append(QExplicitlySharedDataPointer<AdjustAtom>(adjustedAtom));
	    }
        }
    }
    return result;
}

void ShapeAtom::dump(Context* context, int level) {
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

void ShapeAtom::build(Context* context) {
    Q_ASSERT(!context->m_shapeList.contains(this));
    context->m_shapeList.append(this);
    AbstractAtom::build(context);
}

//TODO use filters/libmso/ODrawToOdf.h
void ShapeAtom::writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) {
    Q_ASSERT(context->m_parentLayout);
    if(m_type.isEmpty() || m_hideGeom) return;
    QMap<QString,QString> params = context->m_parentLayout->algorithmParams();
    QMap<QString, qreal> values = context->m_parentLayout->finalValues();
	debugMsooXml << values;
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
    /*
    QFile shapeListFile( "shapeList.txt" );
    QString cxt = QString::number( cx );
    QString cyt = QString::number( cy );
    QString wt = QString::number( w );
    QString ht = QString::number( h );
    QString lbrack = "(";
    QString rbrack = ")\n";
    QString space = " ";
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
    */

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
            warnMsooXml<<"Sibling spacing is bigger then width/height! Skipping sibSp-value! width="<<w<<"height="<<h<<"sibSp="<<sibSp;
            context->m_parentLayout->dump(context, 10);
        }
    }
#endif

    DEBUG_WRITE << "type=" << m_type << "blip=" << m_blip << "hideGeom=" << m_hideGeom << "geometry=" << x+cx << y+cy << w << h;
    //Q_ASSERT(x >= 0.0); Q_ASSERT(y >= 0.0); Q_ASSERT(cx >= 0.0); Q_ASSERT(cy >= 0.0); // they can be negative
    if (w < 0.0) w = -w;
    if (h < 0.0) h = -h;

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
    foreach(AbstractNode* n, context->m_parentLayout->axis( context )) {
        if(PointNode* pn = dynamic_cast<PointNode*>(n))
            if(!pn->m_text.isEmpty())
                textlist.prepend(pn);
    }

    if (!textlist.isEmpty()) {
        foreach(PointNode* pn, textlist) {
            bool bulletEnabled = QVariant(context->m_parentLayout->variable("bulletEnabled", false)).toBool();
            if (bulletEnabled) {
                int level = 0;
                for(AbstractNode* n = pn->parent(); n; n = n->parent(), ++level);
                if(level < 2) // seems only level2 has bullets while level1 has not even if bulletEnabled=1 (see me07_horizontal_bullet_list.xlsx).
                    bulletEnabled = false;
            }
            if (bulletEnabled) {
                xmlWriter->startElement("text:list");
                KoListStyle listStyle;
                KoListLevelProperties llp;
                llp.setLevel(1);
                llp.setBulletCharacter(QChar(0x2022));
                listStyle.setLevelProperties(llp);
                KoGenStyle style(KoGenStyle::ListAutoStyle);
                QByteArray array;
                QBuffer buffer(&array);
                KoXmlWriter tmpXmlWriter(&buffer);
                KoEmbeddedDocumentSaver embeddedSaver;
                KoShapeSavingContext context(tmpXmlWriter, *styles, embeddedSaver);
                listStyle.saveOdf(style, context);
                xmlWriter->addAttribute("text:style-name", styles->insert(style));
                xmlWriter->startElement("text:list-item");
            }
            xmlWriter->startElement("text:p");
            xmlWriter->addTextNode(pn->m_text);
            xmlWriter->endElement();
            if (bulletEnabled) {
                xmlWriter->endElement();
                xmlWriter->endElement();
            }
        }
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
        ASSERT_X(false, QString("TODO Handle shape of type=%1").arg(m_type).toUtf8());
    }
    
    xmlWriter->endElement(); // draw:custom-shape
}

/****************************************************************************************************/

PresentationOfAtom* PresentationOfAtom::clone(Context* context) {
    PresentationOfAtom* atom = new PresentationOfAtom;
    atom->m_axis = m_axis;
    atom->m_ptType = m_ptType;
    atom->m_start = m_start;
    atom->m_step = m_step;
    atom->m_count = m_count;
    atom->m_hideLastTrans = m_hideLastTrans;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone(context));
    return atom;
}

bool PresentationOfAtom::isEmpty() const {
    return m_axis.isEmpty() && m_ptType.isEmpty() && m_start.isEmpty() && m_step.isEmpty() && m_count.isEmpty() && m_hideLastTrans.isEmpty();
}

QString PresentationOfAtom::dump() const {
    QString s;
    if(!m_axis.isEmpty()) s += QString("axis=%1 ").arg(m_axis);
    if(!m_ptType.isEmpty()) s += QString("ptType=%1 ").arg(m_ptType);
    if(!m_start.isEmpty()) s += QString("start=%1 ").arg(m_start);
    if(!m_step.isEmpty()) s += QString("step=%1 ").arg(m_step);
    if(!m_count.isEmpty()) s += QString("count=%1 ").arg(m_count);
    if(!m_hideLastTrans.isEmpty()) s += QString("hideLastTrans=%1 ").arg(m_hideLastTrans);
    return s.trimmed();
}

void PresentationOfAtom::dump(Context* context, int level) {
    DEBUG_DUMP << context->m_parentLayout->m_name << dump();
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
    // first set the axis according to our layout
    Q_ASSERT(context->m_parentLayout->axis( context ).isEmpty());
    context->m_parentLayout->setAxis( context, this );

    // and then adjust the current node if 
    QList<AbstractNode*> nodes = context->m_parentLayout->axis( context );
    if ( nodes.isEmpty() ) {
        /*
        PointNode* ppp = dynamic_cast<PointNode*>(context->currentNode());
        Q_ASSERT(ppp);
        debugMsooXml<<QString("modelId=%2 type=%3").arg(ppp->m_modelId).arg(ppp->m_type);
        */
        ASSERT_X(isEmpty(), QString("Failed to proper apply the non-empty presOf %1").arg(dump()).toLocal8Bit());
    } else {
        //ASSERT_X(nodes.count() == 1, "Oha. The axis contains more then one note. It's not clear what to do in such cases...");
        if (nodes.count() >= 2) warnMsooXml << "TODO The axis contains more then one note. It's not clear what to do in such cases...";
        context->setCurrentNode( nodes.first() );
    }
}

/****************************************************************************************************/

IfAtom* IfAtom::clone(Context* context) {
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
        atom->addChild(a->clone(context));
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
	if ( m_name == "Name21" )
	{
		PointNode* node = dynamic_cast<PointNode* >( context->currentNode() );
		Q_ASSERT( node );
		debugMsooXml << "RULE21: " << m_axis.count() << " nodeId: " << node->m_modelId;
	}
    if(m_function == "cnt") { // Specifies a count.
        funcValue = QString::number(axis.count());
    } else if(m_function == "depth") { // Specifies the depth.
        //int depth = 0;
        //for(AbstractNode* n = context->currentNode(); n; n = n->parent(), ++depth);
        //funcValue = depth;
        //TODO
        warnMsooXml<<"TODO func=depth";
    } else if(m_function == "maxDepth") { // Defines the maximum depth.
        //int depth = 0;
        //for(AbstractNode* n = context->currentNode(); n; n = n->parent(), ++depth);
        //funcValue = depth;
        //TODO
        warnMsooXml<<"TODO func=maxDepth";
    } else if(m_function == "pos") { // Retrieves the position of the node in the specified set of nodes.
        const int position = axis.indexOf(context->currentNode()) + 1;
        funcValue = QString::number(position);
        //TODO 1-based? what index for not-found?
        warnMsooXml<<"TODO func=pos funcValue="<<funcValue;
    } else if(m_function == "posEven") { // Returns 1 if the specified node is at an even numbered position in the data model.
        //const int position = axis.indexOf(context->currentNode())+1;
        //funcValue = position>=1 && position % 2 == 0 ? 1 : 0;
        //TODO
        warnMsooXml<<"TODO func=posEven";
    } else if(m_function == "posOdd") { // Returns 1 if the specified node is in an odd position in the data model.
        //const int position = axis.indexOf(context->currentNode())+1;
        //funcValue = position>=1 && position % 2 != 0 = 1 : 0;
        //TODO
        warnMsooXml<<"TODO func=posOdd";
    } else if(m_function == "revPos") { // Reverse position function.
        const int position = axis.indexOf(context->currentNode()) + 1;
        funcValue = axis.count()-position;
        //TODO lastIndexOf? 1-based? what index for not-found?
        warnMsooXml<<"TODO func=revPos";
    } else if(m_function == "var") { // Used to reference a variable.
        funcValue = context->m_parentLayout->variable(m_argument, true /* check parents */);
        if(funcValue.isEmpty()) { // if not defined then use default variable-values
            if(m_argument == QLatin1String("dir")) { // Specifies the direction of the diagram.
                funcValue = "norm";
            } else {
                warnMsooXml<<"TODO figure out default for variable="<<m_argument;
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
                warnMsooXml<<"TODO figure out how non-integer comparison is expected to work";
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
                warnMsooXml<<"Unexpected operator="<<m_operator<<"name="<<m_name;
            }
        }
    }
    //debugMsooXml<<"name="<<m_name<<"value1="<<funcValue<<"value2="<<m_value<<"operator="<<m_operator<<"istrue="<<istrue;
    return istrue;
}

/****************************************************************************************************/

ChooseAtom* ChooseAtom::clone(Context* context) {
    ChooseAtom* atom = new ChooseAtom;
    atom->m_name = m_name;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> a, m_children)
        atom->addChild(a->clone(context));
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
    // build up list of IfAtom's that match the defined condition
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

#if 0
    // move the children of the selected IfAtom's to our parent
    int index = m_parent->indexOfChild(this);
    Q_ASSERT(index >= 0);
    typedef QVector< QExplicitlySharedDataPointer< AbstractAtom > > AtomPList;
    foreach( QExplicitlySharedDataPointer<AbstractAtom> atom, ifResult.isEmpty() ? elseResult : ifResult ) {
        AtomPList listResult = atom->children();
        // move the constraints to the parent's m_constraintsToBuild
        AtomPList::iterator it = std::stable_partition( listResult.begin(), listResult.end(), ConstraintPredicate() );    
        std::copy( it, listResult.end(), std::back_inserter( context->m_parentLayout->m_constraintsToBuild ) );
        listResult.erase( it, listResult.end() );
        // and move the remaining atom's to the parent
        foreach( QExplicitlySharedDataPointer<AbstractAtom> a, listResult ) {
            atom->removeChild( a );
            m_parent->insertChild( ++index, a );
            a->build( context );
        }
    }
    QExplicitlySharedDataPointer<AbstractAtom> ptr(this);
    m_parent->removeChild(ptr);
#else
    // move the children of the selected IfAtom's to our parent
    int index = m_parent->indexOfChild(this);
    Q_ASSERT(index >= 0);
    QVector< QExplicitlySharedDataPointer<AbstractAtom> > atoms;
    foreach( QExplicitlySharedDataPointer<AbstractAtom> atom, ifResult.isEmpty() ? elseResult : ifResult ) {
		IfAtom * ifAtom = dynamic_cast< IfAtom* >( atom.data() );
		if ( ifAtom )
			debugMsooXml << "atomNameChosen" << ifAtom->m_name;
        foreach( QExplicitlySharedDataPointer<AbstractAtom> a, atom->children() ) {
            atom->removeChild( a );
            m_parent->insertChild( ++index, a );
            atoms.append( a );
        }
    }

    // and finally detach ourself from our parent since we are done now
    QExplicitlySharedDataPointer<AbstractAtom> ptr(this);
    m_parent->removeChild(ptr);

    // and start building the moved children
    foreach( QExplicitlySharedDataPointer<AbstractAtom> atom, atoms ) {
        atom->build( context );
    }
#endif

}

/****************************************************************************************************/

ForEachAtom* ForEachAtom::clone(Context* context) {
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
        atom->addChild(a->clone(context));
    return atom;
}

QString ForEachAtom::dump() const {
    QString s;
    if(!m_name.isEmpty()) s += QString("name=%1 ").arg(m_name);
    if(!m_axis.isEmpty()) s += QString("axis=%1 ").arg(m_axis);
    if(!m_ptType.isEmpty()) s += QString("ptType=%1 ").arg(m_ptType);
    if(!m_reference.isEmpty()) s += QString("reference=%1 ").arg(m_reference);
    if(!m_start.isEmpty()) s += QString("start=%1 ").arg(m_start);
    if(!m_step.isEmpty()) s += QString("step=%1 ").arg(m_step);
    if(!m_count.isEmpty()) s += QString("count=%1 ").arg(m_count);
    if(!m_hideLastTrans.isEmpty()) s += QString("hideLastTrans=%1 ").arg(m_hideLastTrans);
    return s.trimmed();
}

void ForEachAtom::dump(Context* context, int level) {
    DEBUG_DUMP << dump();
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
    // determinate which children are selected
    QList<AbstractNode*> axis = fetchAxis(context, m_axis, m_ptType, m_start, m_count, m_step);
    typedef QPair<AbstractNode*, QList<QExplicitlySharedDataPointer<AbstractAtom> > > NodePair;
    QList<NodePair> newChildren;
    foreach(AbstractNode* node, axis) {
        QList<QExplicitlySharedDataPointer<AbstractAtom> > list;
        foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_children) {
            QExplicitlySharedDataPointer<AbstractAtom> atomCopy(atom->clone(context));
            /*
            if ( LayoutNodeAtom* layoutAtom = dynamic_cast< LayoutNodeAtom* >( atomCopy.data() ) ) {
                Q_ASSERT(layoutAtom->axis(context).isEmpty());
                layoutAtom->setAxis( context, QList< AbstractNode* >() << node );
            }
            */
            list.append(atomCopy);
        }
        newChildren.append(NodePair(node, list));
    }

    // move the selected children to our parent
    int index = m_parent->indexOfChild(this);
    Q_ASSERT(index >= 0);
    foreach(NodePair p, newChildren) {
        foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, p.second) {
            m_parent->insertChild(++index, atom);
        }
    }

    // detach ourself from our parent since we will evaluate the forEach once and forever and won't need it afterwards.
    QExplicitlySharedDataPointer<AbstractAtom> ptr(this);
    m_parent->removeChild(ptr);

    // and finally build the selected children which needs to be done here cause our own parent will deal
    // with a copy of it's children-list and will not know about it's new children during the build.
    AbstractNode* oldCurrentNode = context->currentNode();
    foreach(NodePair p, newChildren) {
        context->setCurrentNode(p.first); // move on to the next node        
        foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, p.second) {
            atom->build(context);
        }
    }
    context->setCurrentNode(oldCurrentNode);
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

QList<LayoutNodeAtom*> AbstractAlgorithm::childLayouts() const {
    QList<LayoutNodeAtom*> result;
    foreach(QExplicitlySharedDataPointer<AbstractAtom> atom, m_layout->children())
        if(LayoutNodeAtom* l = dynamic_cast<LayoutNodeAtom*>(atom.data()))
            result.append(l);
    return result;
}

void AbstractAlgorithm::setNodePosition(LayoutNodeAtom* l, qreal x, qreal y, qreal w, qreal h) {
//     QStringList removeList;
    l->m_values["l"] = parentLayout()->finalValues()["l"] + x;
    l->m_values["t"] = parentLayout()->finalValues()["t"] + y;
//     removeList << "l" << "t";
    if (w >= 0.0) {
        l->m_values["w"] = w;
//         removeList << "w";
    }
    if (h >= 0.0) {
        l->m_values["h"] = h;
//         removeList << "h";
    }
    //l->m_values["ctrX"] = 0.0;
    //l->m_values["ctrY"] = 0.0;
    //l->m_values["r"] = l->m_values["l"] + l->m_values["w"];
    //l->m_values.remove("ctrX");
    //l->m_values.remove("ctrY");
//     removeList << "ctrX" << "ctrY";
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
        } else if (type.startsWith(QLatin1String("user"))) { // userA, userB, userC, etc.
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
    debugMsooXml << "layout=" << layout()->m_name << "algorithm=" << __name;//name();

    // Specifies the aspect ratio (width to height) of the composite node to use when determining child constraints. A value of 0 specifies to
    // leave the width and height constraints unaltered. The algorithm may temporarily shrink one dimension to achieve the specified ratio.
    // For example, if a composite node has a width constraint of 20 and height constraint of 10, and if the value of ar is 1.5, the composite
    // algorithm uses a width value of 15 to calculate the composite node’s child constraints. However, the algorithm does not propagate this
    // value to other nodes.
    qreal aspectRatio = layout()->algorithmParam("ar", "0").toDouble();
    if (aspectRatio != 0.0)
        layout()->m_values["w"] = layout()->finalValues()["h"] * aspectRatio;

    //QVector< QExplicitlySharedDataPointer< LayoutNodeAtom > > allChilds = layout()->childrenLayouts();
    //foreach( QExplicitlySharedDataPointer< LayoutNodeAtom > curChild, allChilds )
    //    setNodePosition( curChild.data(), layout()->finalValues()[ "l" ], layout()->finalValues()[ "t" ], layout()->finalValues()[ "w" ], layout()->finalValues()[ "h" ] );        

    foreach( QExplicitlySharedDataPointer< ConstraintAtom > constr, layout()->constraints() )
        constr->applyConstraint( context(), layout() );

    foreach( QExplicitlySharedDataPointer< ShapeAtom > shape, layout()->shapes() ) 
        foreach( QExplicitlySharedDataPointer< AdjustAtom > adj, shape->adjustments() ){
            adj->applyAdjustment( context(), layout() );
    }

    //foreach( QExplicitlySharedDataPointer< LayoutNodeAtom > curChild, allChilds )
    //    setNodePosition( curChild.data(), layout()->finalValues()[ "l" ], layout()->finalValues()[ "t" ], layout()->finalValues()[ "w" ], layout()->finalValues()[ "h" ] );        
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

qreal ConnectorAlgorithm::connectorDistance() const {
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
    //if (!begPts.isEmpty() && !endPts.isEmpty()) debugMsooXml<<"begPts="<<begPts<<"endPts="<<endPts;

    QMap<QString, qreal> srcValues = srcAtom->finalValues();
    QMap<QString, qreal> dstValues = dstAtom->finalValues();
    qreal srcX = srcValues["l"];//+srcValues["ctrX"];
    qreal srcY = srcValues["t"];//+srcValues["ctrY"];
    qreal srcW = srcValues["w"];
    qreal srcH = srcValues["h"];
    qreal dstX = dstValues["l"];//+dstValues["ctrX"];
    qreal dstY = dstValues["t"];//+dstValues["ctrY"];
    qreal dstW = dstValues["w"];
    qreal dstH = dstValues["h"];
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
    ASSERT_X(!childs.isEmpty(), QString("Layout %1 does not have child-layouts").arg(layout()->m_name));
    if (childs.isEmpty()) return;

    // Specifies the angle at which the first shape is placed. Angles are in degrees, measured clockwise from a line pointing straight upward from the center of the cycle.
    int startAngel = layout()->algorithmParam("stAng", "0").toInt();
    // Specifies the angle the cycle spans. Final shapealign text is placed at stAng+spanAng, unless spanAng=360. In that case, the algorithm places the text so that shapes do not overlap.
    int spanAngel = layout()->algorithmParam("spanAng", "360").toInt();

    // Specifies where to place nodes in relation to the center circle.
    bool firstNodeInCenter = layout()->algorithmParam("ctrShpMap", "none") == "fNode";

    LayoutNodeAtom* nodeInCenter = firstNodeInCenter ? childs.takeFirst() : 0;
    const qreal childsCount = childs.count();

    QMap<QString, qreal> values = layout()->finalValues();
    const qreal w = values["w"];
    const qreal h = values["h"];
    const qreal rx = w / 2.0;
    const qreal ry = h / 2.0;
    qreal num = 360.0 / childsCount;
    const bool inverse = startAngel > spanAngel;
    if(inverse) num = -num;

    qreal spacing = values.value("sibSp");
    qreal dw = ( (2.0 * M_PI * rx - spacing) / childsCount );
    qreal dh = ( (2.0 * M_PI * ry - spacing) / childsCount );

    if(nodeInCenter) {
        //setNodePosition(nodeInCenter, rx, ry, -1, -1); //dw, dh);
        setNodePosition(nodeInCenter, rx, ry, dw, dh);
    }

    //for(qreal degree = startAngel; (!childs.isEmpty()) && (inverse ? degree > spanAngel : degree <= spanAngel); degree -= num) {
    for(qreal degree = startAngel; (!childs.isEmpty()) && (inverse ? degree > spanAngel : degree <= spanAngel); degree += num) {
        const qreal radian = (degree - 90.0) * (M_PI / 180.0);
        const qreal x = rx + cos(radian) * rx;
        const qreal y = ry + sin(radian) * ry;
        LayoutNodeAtom* l = childs.takeFirst();
        //setNodePosition(l, x, y, -1, -1);
        setNodePosition(l, x, y, dw, dh);
    }
}

/****************************************************************************************************/

qreal LinearAlgorithm::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) {
    Q_UNUSED(type);
    Q_UNUSED(values);
    qreal value = -1.0;
    /*
    if (type == "w" || type == "h") {
        value = 100;
    }
    */
    return value;
}

// http://msdn.microsoft.com/en-us/library/dd439457%28v=office.12%29.aspx
void LinearAlgorithm::virtualDoLayout() {
    AbstractAlgorithm::virtualDoLayout();
    // TODO handle infinite values somehow sensible
#if 1
    QMap< QString, qreal > values = layout()->finalValues();
    QString direction = layout()->algorithmParam( "linDir", "fromL" );
    qreal x = 0;
    qreal y = 0;
    if ( direction  == "fromR" )
    {
        x = values[ "w" ];
    }
    if ( direction == "fromB" )
    {
        y = values[ "h" ];
    }
    QList<LayoutNodeAtom*> childs = childLayouts();
    if ( childs.isEmpty() )
        return;
    LayoutNodeAtom *firstNSpaceNode = nullptr;
    LayoutNodeAtom *lastNSpaceNode = nullptr;
    debugMsooXml << values;
    for ( int i = 0; i < childs.count(); ++ i )
    {
        if ( direction  == "fromL" )
        {
            childs[ i ]->m_values[ "l" ] = x;        
            debugMsooXml << "XVAL: " << x;
            x = childs[ i ]->finalValues()[ "r" ];
        }
        else if ( direction == "fromR" )
        {
            childs[ i ]->m_values[ "r" ] = x;
            debugMsooXml << "XVAL: " << x;
            x = childs[ i ]->finalValues()[ "l" ];
        }
        else if ( direction == "fromT" )
        {
            debugMsooXml << "TVAL: " << childs[ i ]->finalValues()[ "t" ];
            debugMsooXml << "BVAL: " << childs[ i ]->finalValues()[ "b" ];
            childs[ i ]->m_values[ "t" ] = y;
            debugMsooXml << "YVAL: " << y;
            y = childs[ i ]->finalValues()[ "b" ];
        }
        else if ( direction == "fromB" )
        {
            childs[ i ]->m_values[ "b" ] = y;
            debugMsooXml << "YVAL: " << y;
            y = childs[ i ]->finalValues()[ "t" ];
        }
        if (childs[ i ]->algorithm()->m_type != AlgorithmAtom::SpaceAlg) {
            if ( !firstNSpaceNode )
                firstNSpaceNode = childs[ i ];
            lastNSpaceNode = childs[ i ];
        }
    }
    const qreal width = lastNSpaceNode->finalValues()[ "r" ] -  firstNSpaceNode->finalValues()[ "l" ];
    const qreal height = lastNSpaceNode->finalValues()[ "b" ] -  firstNSpaceNode->finalValues()[ "t" ];
    const qreal widthStretchFactor = values[ "w" ] / width;
    const qreal heightStretchFactor = values[ "h" ] / height;
    const qreal xOffset = firstNSpaceNode->finalValues()[ "l" ]  < 0 ? -firstNSpaceNode->finalValues()[ "l" ] : 0;
    const qreal yOffset = firstNSpaceNode->finalValues()[ "t" ]  < 0 ? -firstNSpaceNode->finalValues()[ "t" ] : 0;
    const qreal xOffsetRect = values[ "l" ];
    const qreal yOffsetRect = values[ "t" ];
    debugMsooXml << width;
    debugMsooXml << widthStretchFactor;
    debugMsooXml << xOffset;

    for ( int i = 0; i < childs.count(); ++i )
    {
        if ( direction == "fromL" || direction == "formR" )
        {
            const qreal aspectRatio = childs[ i ]->finalValues()[ "h" ] / childs[ i ]->finalValues()[ "w" ];
            const qreal heightRatio = widthStretchFactor * aspectRatio;
            qreal oldCenterX = childs[ i ]->finalValues()[ "ctrX" ];
            childs[ i ]->m_values[ "w" ] = childs[ i ]->finalValues()[ "w" ] * widthStretchFactor;
            childs[ i ]->m_values[ "h" ] = childs[ i ]->finalValues()[ "h" ] * heightRatio;
            oldCenterX *= widthStretchFactor;
            oldCenterX += xOffset * widthStretchFactor + xOffsetRect;
            childs[ i ]->m_values[ "ctrX" ] = oldCenterX;
            childs[ i ]->m_values[ "ctrY" ] = childs[ i ]->finalValues()[ "ctrY" ] + yOffsetRect;
        }
        else
        {
            const qreal aspectRatio = childs[ i ]->finalValues()[ "w" ] / childs[ i ]->finalValues()[ "h" ];
            const qreal widthRatio = heightStretchFactor * aspectRatio;
            qreal oldCenterY = childs[ i ]->finalValues()[ "ctrY" ];
            childs[ i ]->m_values[ "w" ] = childs[ i ]->finalValues()[ "w" ] * widthRatio;
            childs[ i ]->m_values[ "h" ] = childs[ i ]->finalValues()[ "h" ] * heightStretchFactor;
            oldCenterY *= heightStretchFactor;
            oldCenterY += yOffset * heightStretchFactor + yOffsetRect;
            childs[ i ]->m_values[ "ctrY" ] = oldCenterY;
            childs[ i ]->m_values[ "ctrX" ] = childs[ i ]->finalValues()[ "ctrX" ] + xOffsetRect;
        }

    }
#endif
#if 0
    QString direction = layout()->algorithmParam("linDir", "fromL");
    const qreal lMarg = layout()->finalValues()[ "lMarg" ];
    const qreal rMarg = layout()->finalValues()[ "rMarg" ];
    const qreal tMarg = layout()->finalValues()[ "tMarg" ];
    const qreal bMarg = layout()->finalValues()[ "bMarg" ];
    const qreal w = layout()->finalValues()["w"] - lMarg - rMarg;
    const qreal h = layout()->finalValues()["h"] - bMarg - tMarg;

    QList<LayoutNodeAtom*> childs = childLayouts();
    ASSERT_X(!childs.isEmpty(), QString("Layout %1 does not have child-layouts").arg(layout()->m_name));
    if (childs.isEmpty()) return;

    const qreal childsCount = childs.count();
	debugMsooXml << "REAL CHILD COUNTERRRRRRRRRRRRRR " << childsCount;
    const QSizeF usedSize = layout()->childrenUsedSize();
    const QSizeF totalSize = layout()->childrenTotalSize();

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

    // calculate weights
    qreal currentX = x;
    qreal currentY = y;
    qreal currentWidth = 0;
    qreal currentHeight = 0;
    const int xFactor = mx >= 0 ? 1 : -1;
    const int yFactor = my >= 0 ? 1 : -1;
    foreach(LayoutNodeAtom* l, childs) {		
        QMap< QString, qreal > values = l->finalValues();
        if ( l->algorithmType() != AlgorithmAtom::SpaceAlg ) {
			debugMsooXml << "NODETYPE: SPACE";
            currentWidth = l->finalValues()[ "w" ] / usedSize.width() * w;
            currentHeight = l->finalValues()[ "h" ] / usedSize.height() * h;
            setNodePosition(l, currentX, currentY, currentWidth, currentHeight);
            if ( direction == "fromR" || direction == "fromL" )
                currentX = currentX + xFactor * l->finalValues()[ "w" ];
            else
                currentY = currentY + yFactor * l->finalValues()[ "h" ];
        } else {
			debugMsooXml << "NODETYPE: ELSE";
            currentWidth = l->finalValues()[ "w" ] / totalSize.width() * w;
            currentHeight = l->finalValues()[ "h" ] / totalSize.height() * h;
            if ( direction == "fromR" || direction == "fromL" )
                currentX += currentWidth;
            else
                currentY += currentHeight;
        }
    }
#endif
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
    // Specifies the direction of the subsequent row or column. For example, if the algorithm initially places the nodes from left to right,
    // revDir places the nodes in the next row from right to left. However if the algorithm uses contDir, the nodes on the next row are
    // arranged from left to right.
    const bool inSameDirection = layout()->algorithmParam("contDir") != "revDir";
    // Specifies the offset.
    //const QString offset = layout()->algorithmParam("off");

    // Specifies the point at which the diagram starts to snake. The value bal specifies that snaking begin at an even number of rows and
    // columns. The value fixed specifies that snaking begin at a fixed point, for example, in a row that contains three nodes. The value
    // endCnv specifies that snaking begin when there is no more room for a shape in the row.
    //const QString breakpoint = layout()->algorithmParam("bkpt", "endCnv");
    // Specifies where the snake should break, if bkpt=fixed.
    //const int breakpointFixedValue = layout()->algorithmParam("bkPtFixedVal", "2").toInt();

    QList<LayoutNodeAtom*> childs = childLayouts();
    ASSERT_X(!childs.isEmpty(), QString("Layout %1 does not have child-layouts").arg(layout()->m_name));
    if (childs.isEmpty()) return;

    bool inRows = flowDirection != "column";
    const qreal w = layout()->finalValues()["w"];
    const qreal h = layout()->finalValues()["h"];
    qreal x = 0;
    qreal y = 0;

    if (growDirection == "tR") {
        x = w - childs.first()->finalValues()["w"];
    } else if (growDirection == "bL") {
        y = h - childs.first()->finalValues()["h"];
    } else if (growDirection == "bR") {
        x = w - childs.first()->finalValues()["w"];
        y = h - childs.first()->finalValues()["h"];
    }

    //TODO is hardcoding correct here? The specs say default is 100...
    qreal mx = 110;
    qreal my = 110;
    qreal dw = 100;
    qreal dh = 100;
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
    debugMsooXml<<"TODO Implement algorithm isRoot="<<m_isRoot;
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
    debugMsooXml<<"TODO Implement algorithm";
    AbstractAlgorithm::virtualDoLayout();
}

/****************************************************************************************************/

//NOTE I start to assume that the parent layout-algorithms are also responsible for setting defaults at children
//layout-algorithms. If that's the case then the question is how/where that happens. To bad the specs are
//missing the most basic information :-(
qreal SpaceAlg::virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) {
    Q_UNUSED(values);
    qreal value = -1.0;
    if (type == "w" || type == "h") {
        debugMsooXml<<"TODO type="<<type;
        value = 100; //TODO what default value is expected here?
    } else if (type == "sibSp") {
        debugMsooXml<<"TODO type="<<type;
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

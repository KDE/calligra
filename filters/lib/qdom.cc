#include "qdom.h"
#include "qxml.h"

#include <qtextstream.h>
#include <qiodevice.h>
#include <qmime.h>

#include <string.h>
#include <stdlib.h>

#include <koApplication.h>

/**************************************************************
 *
 * QDOMConsumer
 *
 **************************************************************/

class QDomConsumer : public QXMLConsumer
{
public:
  QDomConsumer( QDOM_DocumentPrivate* d );
  ~QDomConsumer();

  virtual bool tagStart( const QString& name );
  virtual bool tagEnd( const QString& name );
  virtual bool attrib( const QString& name, const QString& value );
  virtual bool text( const QString& text );
  virtual bool entityRef( const QString& name );
  virtual bool processingInstruction( const QString& name, const QString& value );
  virtual bool doctype( const QString& name );
  virtual bool doctypeExtern( const QString& publicId, const QString& systemId );
  // virtual bool parameterEntityRef( const QString& name );
  virtual bool element( const QString& data );
  virtual bool attlist( const QString& data );
  virtual bool parameterEntity( const QString& name, const QString& publicId, const QString& systemId );
  virtual bool parameterEntity( const QString& name, const QString& value );
  virtual bool entity( const QString& name, const QString& publicId, const QString& systemId, const QString& ndata );
  virtual bool entity( const QString& name, const QString& value );
  virtual bool notation( const QString& name, const QString& publicId, const QString& systemId );
  virtual void parseError( int pos );
  virtual bool finished();

private:
  QDOM_DocumentPrivate* doc;
  QDOM_NodePrivate* node;
  bool firstTag;
};

/**************************************************************
 *
 * QDomNamespace
 *
 **************************************************************/

/* QDomNamespace::Namespace()
{
}

QDomNamespace::~Namespace()
{
} */

/**************************************************************
 *
 * QDomException
 *
 **************************************************************/

/* QDomException::Exception()
{
}

QDomException::~Exception()
{
}

unsigned short QDomException::code()
{
  return c;
  } */

/*==============================================================*/
/*                Implementation                                */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_ImplementationPrivate
 *
 **************************************************************/

class QDOM_ImplementationPrivate : public QShared
{
public:
  QDOM_ImplementationPrivate();
  ~QDOM_ImplementationPrivate();

  QDOM_ImplementationPrivate* clone();

  bool hasFeature( const QString& feature, const QString& version );
};

QDOM_ImplementationPrivate::QDOM_ImplementationPrivate()
{
}

QDOM_ImplementationPrivate::~QDOM_ImplementationPrivate()
{
}

bool QDOM_ImplementationPrivate::hasFeature( const QString& feature, const QString& version )
{
  if ( feature == "XML" )
    if ( version.isEmpty() || version == "1.0" )
      return true;

  return false;
}

QDOM_ImplementationPrivate* QDOM_ImplementationPrivate::clone()
{
  return new QDOM_ImplementationPrivate;
}

/**************************************************************
 *
 * QDomImplementation
 *
 **************************************************************/

QDomImplementation::QDomImplementation()
{
  impl = 0;
}

QDomImplementation::QDomImplementation( const QDomImplementation& x )
{
  impl = x.impl;
  if ( impl ) impl->ref();
}

QDomImplementation::QDomImplementation( QDOM_ImplementationPrivate* p )
{
  // We want to be co-owners, so increase the reference count
  impl = p;
}

QDomImplementation& QDomImplementation::operator= ( const QDomImplementation& x )
{
  if ( impl && impl->deref() ) delete impl;
  impl = x.impl;
  if ( impl ) impl->ref();

  return *this;
}

bool QDomImplementation::operator==( const QDomImplementation& x ) const
{
  return ( impl == x.impl );
}

bool QDomImplementation::operator!=( const QDomImplementation& x ) const
{
  return ( impl != x.impl );
}

QDomImplementation::~QDomImplementation()
{
  if ( impl && impl->deref() ) delete impl;
}

bool QDomImplementation::hasFeature( const QString& feature, const QString& version )
{
  if ( !impl )
    return FALSE;

  return impl->hasFeature( feature, version );
}


/*==============================================================*/
/*                       NodeList                               */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_NodeListPrivate
 *
 **************************************************************/

class QDOM_NodeListPrivate : public QShared
{
public:
  QDOM_NodeListPrivate();
  virtual ~QDOM_NodeListPrivate();

  virtual QDOM_NodePrivate* item( int index ) = 0;
  virtual uint length() const = 0;
};

QDOM_NodeListPrivate::QDOM_NodeListPrivate()
{
}

QDOM_NodeListPrivate::~QDOM_NodeListPrivate()
{
}

/**************************************************************
 *
 * QDomNodeList
 *
 **************************************************************/

QDomNodeList::QDomNodeList()
{
  impl = 0;
}

QDomNodeList::QDomNodeList( QDOM_NodeListPrivate* p )
{
  impl = p;
  if ( impl ) impl->ref();
}

QDomNodeList::QDomNodeList( const QDomNodeList& n )
{
  impl = n.impl;
  if ( impl )
    impl->ref();
}

QDomNodeList& QDomNodeList::operator= ( const QDomNodeList& n )
{
  if ( impl && impl->deref() ) delete impl;
  impl = n.impl;
  if ( impl )
    impl->ref();

  return *this;
}

bool QDomNodeList::operator== ( const QDomNodeList& n ) const
{
  return ( impl == n.impl );
}

bool QDomNodeList::operator!= ( const QDomNodeList& n ) const
{
  return ( impl != n.impl );
}

QDomNodeList::~QDomNodeList()
{
  if ( impl && impl->deref() ) delete impl;
}

QDomNode QDomNodeList::item( int index ) const
{
  if ( !impl )
    return QDomNode();

  return QDomNode( impl->item( index ) );
}

uint QDomNodeList::length() const
{
  if ( !impl )
    return 0;
  return impl->length();
}

/*==============================================================*/
/*==============================================================*/

/**************************************************************
 *
 * QDOM_NodePrivate
 *
 **************************************************************/

class QDOM_NodePrivate : public QDOM_NodeListPrivate
{
public:
  QDOM_NodePrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent = 0 );
  QDOM_NodePrivate( QDOM_NodePrivate* n, bool deep );
  ~QDOM_NodePrivate();

  QString nodeName() const { return m_name; }
  QString nodeValue() const { return m_value; }
  void setNodeValue( const QString& v ) { m_value = v; }

  QDOM_NodePrivate*         parentNode();
  QDOM_NodeListPrivate*     childNodes();
  QDOM_NodePrivate*         firstChild();
  QDOM_NodePrivate*         lastChild();
  QDOM_NodePrivate*         previousSibling();
  QDOM_NodePrivate*         nextSibling();
  QDOM_DocumentPrivate*     ownerDocument();

  virtual QDOM_NamedNodeMapPrivate* attributes();
  virtual QDOM_NodePrivate* insertBefore( QDOM_NodePrivate* newChild, QDOM_NodePrivate* refChild );
  virtual QDOM_NodePrivate* insertAfter( QDOM_NodePrivate* newChild, QDOM_NodePrivate* refChild );
  virtual QDOM_NodePrivate* replaceChild( QDOM_NodePrivate* newChild, QDOM_NodePrivate* oldChild );
  virtual QDOM_NodePrivate* removeChild( QDOM_NodePrivate* oldChild );
  virtual QDOM_NodePrivate* appendChild( QDOM_NodePrivate* newChild );
  bool  hasChildNodes() const;

  QDOM_NodePrivate* namedItem( const QString& name );

  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual void clear();

  QDOM_NodePrivate* item( int index );
  uint length() const;

  void setParent( QDOM_NodePrivate* );

  // Dynamic cast
  virtual bool isAttr() { return false; }
  virtual bool isCDATASection() { return false; }
  virtual bool isDocumentFragment() { return false; }
  virtual bool isDocument() { return false; }
  virtual bool isDocumentType() { return false; }
  virtual bool isElement() { return false; }
  virtual bool isEntityReference() { return false; }
  virtual bool isText() { return false; }
  virtual bool isEntity() { return false; }
  virtual bool isNotation() { return false; }
  virtual bool isProcessingInstruction() { return false; }
  virtual bool isCharacterData() { return false; }

  virtual void save( QTextStream& ) const;

  // Variables
  QDOM_DocumentPrivate* m_doc;
  QDOM_NodePrivate* m_previousSibling;
  QDOM_NodePrivate* m_nextSibling;
  QDOM_NodePrivate* m_parentNode;
  QDOM_NodePrivate* m_firstChild;
  QDOM_NodePrivate* m_lastChild;

  QString m_name;
  QString m_value;
};

QDOM_NodePrivate::QDOM_NodePrivate( QDOM_DocumentPrivate* qd, QDOM_NodePrivate *par )
{
  m_parentNode = par;
  m_doc = qd;
  m_previousSibling = 0;
  m_nextSibling = 0;
  m_firstChild = 0;
  m_lastChild = 0;
}

QDOM_NodePrivate::QDOM_NodePrivate( QDOM_NodePrivate* n, bool deep )
{
  m_doc = n->ownerDocument();
  m_parentNode = 0;
  m_previousSibling = 0;
  m_nextSibling = 0;
  m_firstChild = 0;
  m_lastChild = 0;

  if ( !deep )
    return;

  uint len = n->length();

  for( uint i = 0; i < len; ++i )
  {
    QDOM_NodePrivate* x = n->item( i )->cloneNode( TRUE );
    x->setParent( this );
    appendChild( x );
  }
}

QDOM_NodePrivate::~QDOM_NodePrivate()
{
  QDOM_NodePrivate* p = m_firstChild;
  QDOM_NodePrivate* n;

  while( p )
  {
    n = p->nextSibling();
    if ( p->deref() )
      delete p;
    p = n;
  }

  m_firstChild = 0;
  m_lastChild = 0;
}

void QDOM_NodePrivate::clear()
{
  QDOM_NodePrivate* p = m_firstChild;
  QDOM_NodePrivate* n;

  while( p )
  {
    n = p->nextSibling();
    if ( p->deref() )
      delete p;
    p = n;
  }

  m_firstChild = 0;
  m_lastChild = 0;
}

QDOM_NodePrivate* QDOM_NodePrivate::namedItem( const QString& name )
{
  QDOM_NodePrivate* p = m_firstChild;
  while( p )
  {
    if ( p->nodeName() == name )
      return p;
    p = p->nextSibling();
  }

  return 0;
}

QDOM_NamedNodeMapPrivate* QDOM_NodePrivate::attributes()
{
  return 0;
}

QDOM_NodePrivate* QDOM_NodePrivate::insertBefore( QDOM_NodePrivate* newChild, QDOM_NodePrivate* refChild )
{
  // Error check
  if ( !newChild )
    return 0;

  // Error check
  if ( refChild && refChild->parentNode() != this )
    return 0;

  // No more errors can occure now, so we take
  // ownership of the node
  newChild->ref();

  if ( newChild->parentNode() )
    newChild->parentNode()->removeChild( newChild );

  newChild->m_parentNode = this;

  if ( !refChild )
  {
    if ( m_firstChild )
      m_firstChild->m_previousSibling = newChild;
    newChild->m_nextSibling = m_firstChild;
    if ( !m_lastChild )
      m_lastChild = newChild;
    m_firstChild = newChild;
    return newChild;
  }

  if ( refChild->previousSibling() == 0 )
  {
    if ( m_firstChild )
      m_firstChild->m_previousSibling = newChild;
    newChild->m_nextSibling = m_firstChild;
    if ( !m_lastChild )
      m_lastChild = newChild;
    m_firstChild = newChild;
    return newChild;
  }

  newChild->m_nextSibling = refChild;
  newChild->m_previousSibling = refChild->previousSibling();
  refChild->m_previousSibling->m_nextSibling = newChild;
  refChild->m_previousSibling = newChild;

  return newChild;
}

QDOM_NodePrivate* QDOM_NodePrivate::insertAfter( QDOM_NodePrivate* newChild, QDOM_NodePrivate* refChild )
{
  // Error check
  if ( !newChild )
    return 0;

  // Release new node from its current parent
  if ( newChild->parentNode() )
    newChild->parentNode()->removeChild( newChild );

  // No more errors can occure now, so we take
  // ownership of the node
  newChild->ref();

  newChild->m_parentNode = this;

  // Insert at the end
  if ( !refChild )
  {
    if ( m_lastChild )
      m_lastChild->m_nextSibling = newChild;
    newChild->m_previousSibling = m_lastChild;
    if ( !m_firstChild )
      m_firstChild = newChild;
    m_lastChild = newChild;
    return newChild;
  }

  // Error check
  if ( refChild->parentNode() != this )
    return 0;

  if ( refChild->nextSibling() == 0 )
  {
    if ( m_lastChild )
      m_lastChild->m_nextSibling = newChild;
    newChild->m_previousSibling = m_lastChild;
    if ( !m_firstChild )
      m_firstChild = newChild;
    m_lastChild = newChild;
    return newChild;
  }

  newChild->m_previousSibling = refChild;
  newChild->m_nextSibling = refChild->nextSibling();
  refChild->m_nextSibling->m_previousSibling = newChild;
  refChild->m_nextSibling = newChild;

  return newChild;
}

QDOM_NodePrivate* QDOM_NodePrivate::replaceChild( QDOM_NodePrivate* newChild, QDOM_NodePrivate* oldChild )
{
  // Error check
  if ( oldChild->parentNode() != this )
    return 0;

  // No more errors can occure now, so we take
  // ownership of the node
  newChild->ref();

  // Release new node from its current parent
  if ( newChild->parentNode() )
    newChild->parentNode()->removeChild( newChild );

  newChild->m_parentNode = this;

  if ( oldChild->nextSibling() )
    oldChild->nextSibling()->m_previousSibling = newChild;
  if ( oldChild->previousSibling() )
    oldChild->previousSibling()->m_nextSibling = newChild;

  newChild->m_nextSibling = oldChild->nextSibling();
  newChild->m_previousSibling = oldChild->previousSibling();

  if ( m_firstChild == oldChild )
    m_firstChild = newChild;
  if ( m_lastChild == oldChild )
    m_lastChild = newChild;

  oldChild->m_parentNode = 0;
  oldChild->m_nextSibling = 0;
  oldChild->m_previousSibling = 0;

  // We are no longer interested in the old node
  if ( oldChild ) oldChild->deref();

  return oldChild;
}

QDOM_NodePrivate* QDOM_NodePrivate::removeChild( QDOM_NodePrivate* oldChild )
{
  // Error check
  if ( oldChild->parentNode() != this )
    return 0;

  if ( oldChild->nextSibling() )
    oldChild->nextSibling()->m_previousSibling = oldChild->previousSibling();
  if ( oldChild->previousSibling() )
    oldChild->previousSibling()->m_nextSibling = oldChild->nextSibling();

  if ( m_lastChild == oldChild )
    m_lastChild = oldChild->previousSibling();
  if ( m_firstChild == oldChild )
    m_firstChild = oldChild->nextSibling();

  oldChild->m_parentNode = 0;
  oldChild->m_nextSibling = 0;
  oldChild->m_previousSibling = 0;

  // We are no longer interested in the old node
  if ( oldChild ) oldChild->deref();

  return oldChild;
}

QDOM_NodePrivate* QDOM_NodePrivate::appendChild( QDOM_NodePrivate* newChild )
{
  // No reference manipulation needed. Dont in insertAfter.
  return insertAfter( newChild, 0 );
}

QDOM_NodePrivate* QDOM_NodePrivate::item( int index )
{
  QDOM_NodePrivate* p = m_firstChild;
  int i;
  while( i < index && p )
  {
    p = p->nextSibling();
    ++i;
  }

  return p;
}

uint QDOM_NodePrivate::length() const
{
  uint i;
  QDOM_NodePrivate* p = m_firstChild;
  while( p )
  {
    p = p->nextSibling();
    ++i;
  }

  return i;
}

void QDOM_NodePrivate::setParent( QDOM_NodePrivate* n )
{
  // Dont take over ownership of our parent :-)
  m_parentNode = n;
}

QDOM_DocumentPrivate* QDOM_NodePrivate::ownerDocument()
{
  return m_doc;
}

QDOM_NodePrivate* QDOM_NodePrivate::parentNode()
{
  return m_parentNode;
}

QDOM_NodeListPrivate* QDOM_NodePrivate::childNodes()
{
  return this;
}

QDOM_NodePrivate* QDOM_NodePrivate::firstChild()
{
  return m_firstChild;
}

QDOM_NodePrivate* QDOM_NodePrivate::lastChild()
{
  return m_lastChild;
}

QDOM_NodePrivate* QDOM_NodePrivate::previousSibling()
{
  return m_previousSibling;
}

QDOM_NodePrivate* QDOM_NodePrivate::nextSibling()
{
  return m_nextSibling;
}

bool QDOM_NodePrivate::hasChildNodes() const
{
  return ( m_lastChild != 0 );
}

QDOM_NodePrivate* QDOM_NodePrivate::cloneNode( bool deep )
{
  QDOM_NodePrivate* p = new QDOM_NodePrivate( this, deep );
  // We are not interested in this node
  p->deref();
  return p;
}

void QDOM_NodePrivate::save( QTextStream& s ) const
{
  const QDOM_NodePrivate* n = m_firstChild;
  while( n )
  {
    n->save( s );
    n = n->m_nextSibling;
  }
}

/**************************************************************
 *
 * QDomNode
 *
 **************************************************************/

#define IMPL ((QDOM_NodePrivate*)impl)

QDomNode::QDomNode()
{
  impl = 0;
}

QDomNode::QDomNode( const QDomNode& n )
{
  impl = n.impl;
  if ( impl ) impl->ref();
}

QDomNode::QDomNode( QDOM_NodePrivate* n )
{
  impl = n;
  if ( impl ) impl->ref();
}

QDomNode& QDomNode::operator= ( const QDomNode& n )
{
  if ( impl && impl->deref() ) delete impl;
  impl = n.impl;
  if ( impl ) impl->ref();

  return *this;
}

bool QDomNode::operator== ( const QDomNode& n ) const
{
  return ( impl == n.impl );
}

bool QDomNode::operator!= ( const QDomNode& n ) const
{
  return ( impl != n.impl );
}

QDomNode::~QDomNode()
{
  if ( impl && impl->deref() ) delete impl;
}

QString QDomNode::nodeName() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_name;
}

QString QDomNode::nodeValue() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_value;
}

void QDomNode::setNodeValue( const QString& v )
{
  if ( !impl )
    return;
  IMPL->setNodeValue( v );
}

QDomNode::NodeType QDomNode::nodeType() const
{
  return QDomNode::BaseNode;
}

QDomNode QDomNode::parentNode() const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->parentNode() );
}

QDomNodeList QDomNode::childNodes() const
{
  if ( !impl )
    return QDomNodeList();
  return QDomNodeList( impl );
}

QDomNode QDomNode::firstChild() const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->firstChild() );
}

QDomNode QDomNode::lastChild() const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->lastChild() );
}

QDomNode QDomNode::previousSibling() const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->previousSibling() );
}

QDomNode QDomNode::nextSibling() const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->nextSibling() );
}

QDomNamedNodeMap QDomNode::attributes() const
{
  if ( !impl )
    return QDomNamedNodeMap();

  return QDomNamedNodeMap( impl->attributes() );
}

QDomDocument QDomNode::ownerDocument() const
{
  if ( !impl )
    return QDomDocument();
  return QDomDocument( IMPL->ownerDocument() );
}

QDomNode QDomNode::cloneNode( bool deep ) const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->cloneNode( deep ) );
}

QDomNode QDomNode::insertBefore( const QDomNode& newChild, const QDomNode& refChild )
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->insertBefore( newChild.impl, refChild.impl ) );
}

QDomNode QDomNode::insertAfter( const QDomNode& newChild, const QDomNode& refChild )
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->insertAfter( newChild.impl, refChild.impl ) );
}

QDomNode QDomNode::replaceChild( const QDomNode& newChild, const QDomNode& oldChild )
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->replaceChild( newChild.impl, oldChild.impl ) );
}

QDomNode QDomNode::removeChild( const QDomNode& oldChild )
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->removeChild( oldChild.impl ) );
}

QDomNode QDomNode::appendChild( const QDomNode& newChild )
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->appendChild( newChild.impl ) );
}

bool QDomNode::isNull() const
{
  return ( impl == 0 );
}

void QDomNode::clear()
{
  if ( impl && impl->deref() ) delete impl;
  impl = 0;
}

QDomNode QDomNode::namedItem( const QString& name ) const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( impl->namedItem( name ) );
}

void QDomNode::save( QTextStream& str ) const
{
  if ( impl )
    IMPL->save( str );
}

QTextStream& operator<<( QTextStream& str, const QDomNode& node )
{
  node.save( str );

  return str;
}

#undef IMPL

/*==============================================================*/
/*                      NamedNodeMap                            */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_NamedNodeMapPrivate
 *
 **************************************************************/

class QDOM_NamedNodeMapPrivate : public QShared
{
public:
  QDOM_NamedNodeMapPrivate( QDOM_NodePrivate* );
  ~QDOM_NamedNodeMapPrivate();

  QDOM_NodePrivate* namedItem( const QString& name ) const;
  QDOM_NodePrivate* setNamedItem( QDOM_NodePrivate* arg );
  QDOM_NodePrivate* removeNamedItem( const QString& name );
  QDOM_NodePrivate* item( int index ) const;
  uint length() const;
  bool contains( const QString& name ) const;

  /**
   * Deletes all children in this list.
   */
  void clear();
  bool isReadOnly() { return m_readonly; }
  void setReadOnly( bool r ) { m_readonly = r; }
  bool isAppendToParent() { return m_appendToParent; }
  void setAppendToParent( bool b ) { m_appendToParent = b; }

  /**
   * Creates a copy of the map. It is a deep copy
   * that means that all children are cloned.
   */
  QDOM_NamedNodeMapPrivate* clone();

  // Variables
  QDict<QDOM_NodePrivate> m_map;
  QDOM_NodePrivate* m_parent;
  bool m_readonly;
  bool m_appendToParent;
};

QDOM_NamedNodeMapPrivate::QDOM_NamedNodeMapPrivate( QDOM_NodePrivate* n )
{
  m_readonly = FALSE;
  m_parent = n;
  m_appendToParent = FALSE;
}

QDOM_NamedNodeMapPrivate::~QDOM_NamedNodeMapPrivate()
{
}

QDOM_NamedNodeMapPrivate* QDOM_NamedNodeMapPrivate::clone()
{
  QDOM_NamedNodeMapPrivate* m = new QDOM_NamedNodeMapPrivate( m_parent );

  QDictIterator<QDOM_NodePrivate> it ( m_map );
  for( ; it.current(); ++it )
    m->m_map.insert( it.current()->nodeName(), it.current()->cloneNode() );

  m->m_readonly = m_readonly;
  m->m_appendToParent = m_appendToParent;

  // we are no longer interested in ownership
  m->deref();
  return m;
}

void QDOM_NamedNodeMapPrivate::clear()
{
  // Dereference all of our children
  QDictIterator<QDOM_NodePrivate> it( m_map );
  for( ; it.current(); ++it )
    if ( it.current()->deref() )
      delete it.current();

  m_map.clear();
}

QDOM_NodePrivate* QDOM_NamedNodeMapPrivate::namedItem( const QString& name ) const
{
  QDOM_NodePrivate* p = m_map[ name ];
  return p;
}

QDOM_NodePrivate* QDOM_NamedNodeMapPrivate::setNamedItem( QDOM_NodePrivate* arg )
{
  if ( m_readonly || !arg )
    return 0;

  if ( m_appendToParent )
    return m_parent->appendChild( arg );
  else
  {
    // We take a reference
    arg->ref();
    m_map.insert( arg->nodeName(), arg );
    return arg;
  }
}

QDOM_NodePrivate* QDOM_NamedNodeMapPrivate::removeNamedItem( const QString& name )
{
  if ( m_readonly )
    return 0;

  QDOM_NodePrivate* p = namedItem( name );
  if ( p == 0 )
    return 0;

  return m_parent->removeChild( p );
}

QDOM_NodePrivate* QDOM_NamedNodeMapPrivate::item( int index ) const
{
  if ( (uint)index >= length() )
    return 0;

  QDictIterator<QDOM_NodePrivate> it( m_map );
  for( int i = 0; i < index; ++i ) { }

  return it.current();
}

uint QDOM_NamedNodeMapPrivate::length() const
{
  return m_map.count();
}

bool QDOM_NamedNodeMapPrivate::contains( const QString& name ) const
{
  return ( m_map[ name ] != 0 );
}

/**************************************************************
 *
 * QDomNamedNodeMap
 *
 **************************************************************/

#define IMPL ((QDOM_NamedNodeMapPrivate*)impl)

QDomNamedNodeMap::QDomNamedNodeMap()
{
  impl = 0;
}

QDomNamedNodeMap::QDomNamedNodeMap( const QDomNamedNodeMap& n )
{
  impl = n.impl;
  if ( impl ) impl->ref();
}

QDomNamedNodeMap::QDomNamedNodeMap( QDOM_NamedNodeMapPrivate* n )
{
  impl = n;
  if ( impl ) impl->ref();
}

QDomNamedNodeMap& QDomNamedNodeMap::operator= ( const QDomNamedNodeMap& n )
{
  if ( impl && impl->deref() ) delete impl;
  impl = n.impl;
  if ( impl ) impl->ref();

  return *this;
}

bool QDomNamedNodeMap::operator== ( const QDomNamedNodeMap& n ) const
{
  return ( impl == n.impl );
}

bool QDomNamedNodeMap::operator!= ( const QDomNamedNodeMap& n ) const
{
  return ( impl != n.impl );
}

QDomNamedNodeMap::~QDomNamedNodeMap()
{
  if ( impl && impl->deref() ) delete impl;
}

QDomNode QDomNamedNodeMap::namedItem( const QString& name ) const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->namedItem( name ) );
}

QDomNode QDomNamedNodeMap::setNamedItem( const QDomNode& arg )
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->setNamedItem( (QDOM_NodePrivate*)arg.impl ) );
}

QDomNode QDomNamedNodeMap::removeNamedItem( const QString& name )
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->removeNamedItem( name ) );
}

QDomNode QDomNamedNodeMap::item( int index ) const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( IMPL->item( index ) );
}

uint QDomNamedNodeMap::length() const
{
  if ( !impl )
    return 0;
  return IMPL->length();
}

bool QDomNamedNodeMap::contains( const QString& name ) const
{
  if ( !impl )
    return FALSE;
  return IMPL->contains( name );
}

#undef IMPL

/*==============================================================*/
/*==============================================================*/

/**************************************************************
 *
 * QDOM_DocumentTypePrivate
 *
 **************************************************************/

class QDOM_DocumentTypePrivate : public QDOM_NodePrivate
{
public:
  QDOM_DocumentTypePrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent = 0 );
  QDOM_DocumentTypePrivate( QDOM_DocumentTypePrivate* n, bool deep );
  ~QDOM_DocumentTypePrivate();

  virtual QDOM_NamedNodeMapPrivate* entities();
  virtual QDOM_NamedNodeMapPrivate* notations();

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual QDOM_NodePrivate* insertBefore( QDOM_NodePrivate* newChild, QDOM_NodePrivate* refChild );
  virtual QDOM_NodePrivate* insertAfter( QDOM_NodePrivate* newChild, QDOM_NodePrivate* refChild );
  virtual QDOM_NodePrivate* replaceChild( QDOM_NodePrivate* newChild, QDOM_NodePrivate* oldChild );
  virtual QDOM_NodePrivate* removeChild( QDOM_NodePrivate* oldChild );
  virtual QDOM_NodePrivate* appendChild( QDOM_NodePrivate* newChild );

  // Overloaded from QDOM_DocumentTypePrivate
  virtual bool isDocumentType() { return true; }
  virtual void save( QTextStream& s ) const;

  // Variables
  QDOM_NamedNodeMapPrivate* m_entities;
  QDOM_NamedNodeMapPrivate* m_notations;
};

QDOM_DocumentTypePrivate::QDOM_DocumentTypePrivate( QDOM_DocumentPrivate* doc, QDOM_NodePrivate* parent )
  : QDOM_NodePrivate( doc, parent )
{
  m_entities = new QDOM_NamedNodeMapPrivate( this );
  m_notations = new QDOM_NamedNodeMapPrivate( this );

  m_entities->setAppendToParent( TRUE );
  m_notations->setAppendToParent( TRUE );
}

QDOM_DocumentTypePrivate::QDOM_DocumentTypePrivate( QDOM_DocumentTypePrivate* n, bool deep )
  : QDOM_NodePrivate( n, deep )
{
  m_entities = new QDOM_NamedNodeMapPrivate( this );
  m_notations = new QDOM_NamedNodeMapPrivate( this );

  m_entities->setAppendToParent( TRUE );
  m_notations->setAppendToParent( TRUE );

  // Refill the maps with our new children
  QDOM_NodePrivate* p = firstChild();
  while( p )
  {
    if ( p->isEntity() )
      // Dont use normal insert method since we would create infinite recursion
      m_entities->m_map.insert( p->nodeName(), p );
    if ( p->isNotation() )
      // Dont use normal insert method since we would create infinite recursion
      m_notations->m_map.insert( p->nodeName(), p );
  }
}

QDOM_DocumentTypePrivate::~QDOM_DocumentTypePrivate()
{
  delete m_entities;
  delete m_notations;
}

QDOM_NamedNodeMapPrivate* QDOM_DocumentTypePrivate::entities()
{
  return m_entities;
}

QDOM_NamedNodeMapPrivate* QDOM_DocumentTypePrivate::notations()
{
  return m_notations;
}

QDOM_NodePrivate* QDOM_DocumentTypePrivate::cloneNode( bool deep)
{
  return new QDOM_DocumentTypePrivate( this, deep );
}

QDOM_NodePrivate* QDOM_DocumentTypePrivate::insertBefore( QDOM_NodePrivate* newChild, QDOM_NodePrivate* refChild )
{
  // Call the origianl implementation
  QDOM_NodePrivate* p = QDOM_NodePrivate::insertBefore( newChild, refChild );
  // Update the maps
  if ( p && p->isEntity() )
    m_entities->m_map.insert( p->nodeName(), p );
  else if ( p && p->isNotation() )
    m_notations->m_map.insert( p->nodeName(), p );

  return p;
}

QDOM_NodePrivate* QDOM_DocumentTypePrivate::insertAfter( QDOM_NodePrivate* newChild, QDOM_NodePrivate* refChild )
{
  // Call the origianl implementation
  QDOM_NodePrivate* p = QDOM_NodePrivate::insertAfter( newChild, refChild );
  // Update the maps
  if ( p && p->isEntity() )
    m_entities->m_map.insert( p->nodeName(), p );
  else if ( p && p->isNotation() )
    m_notations->m_map.insert( p->nodeName(), p );

  return p;
}

QDOM_NodePrivate* QDOM_DocumentTypePrivate::replaceChild( QDOM_NodePrivate* newChild, QDOM_NodePrivate* oldChild )
{
  // Call the origianl implementation
  QDOM_NodePrivate* p = QDOM_NodePrivate::replaceChild( newChild, oldChild );
  // Update the maps
  if ( p )
  {
    if ( oldChild && oldChild->isEntity() )
      m_entities->m_map.remove( oldChild->nodeName() );
    else if ( oldChild && oldChild->isNotation() )
      m_notations->m_map.remove( oldChild->nodeName() );

    if ( p->isEntity() )
      m_entities->m_map.insert( p->nodeName(), p );
    else if ( p->isNotation() )
      m_notations->m_map.insert( p->nodeName(), p );
  }

  return p;
}

QDOM_NodePrivate* QDOM_DocumentTypePrivate::removeChild( QDOM_NodePrivate* oldChild )
{
  // Call the origianl implementation
  QDOM_NodePrivate* p = QDOM_NodePrivate::removeChild(  oldChild );
  // Update the maps
  if ( p && p->isEntity() )
    m_entities->m_map.remove( p->nodeName() );
  else if ( p && p->isNotation() )
    m_notations->m_map.remove( p ->nodeName() );

  return p;
}

QDOM_NodePrivate* QDOM_DocumentTypePrivate::appendChild( QDOM_NodePrivate* newChild )
{
  // Call the origianl implementation
  QDOM_NodePrivate* p = QDOM_NodePrivate::appendChild( newChild );
  // Update the maps
  if ( p && p->isEntity() )
    m_entities->m_map.insert( p->nodeName(), p );
  else if ( p && p->isNotation() )
    m_notations->m_map.insert( p->nodeName(), p );

  return p;
}

void QDOM_DocumentTypePrivate::save( QTextStream& s ) const
{
  s << "<!DOCTYPE " << m_name << " ";

  if ( m_entities->length() > 0 || m_notations->length() > 0 )
  {
    s << "[ ";

    QDictIterator<QDOM_NodePrivate> it2( m_notations->m_map );
    for( ; it2.current(); ++it2 )
      it2.current()->save( s );

    QDictIterator<QDOM_NodePrivate> it( m_entities->m_map );
    for( ; it.current(); ++it )
      it.current()->save( s );

    s << " ]";
  }

  s << ">";
}

/**************************************************************
 *
 * QDomDocumentType
 *
 **************************************************************/

#define IMPL ((QDOM_DocumentTypePrivate*)impl)

QDomDocumentType::QDomDocumentType() : QDomNode()
{
}

QDomDocumentType::QDomDocumentType( const QDomDocumentType& n )
  : QDomNode( n )
{
}

QDomDocumentType::QDomDocumentType( QDOM_DocumentTypePrivate* n )
  : QDomNode( n )
{
}

QDomDocumentType& QDomDocumentType::operator= ( const QDomDocumentType& n )
{
  return (QDomDocumentType&) QDomNode::operator=( n );
}

QDomDocumentType::~QDomDocumentType()
{
}

QString QDomDocumentType::name() const
{
  if ( !impl )
    return QString::null;

  return IMPL->nodeName();
}

QDomNamedNodeMap QDomDocumentType::entities() const
{
  if ( !impl )
    return QDomNamedNodeMap();
  return QDomNamedNodeMap( IMPL->entities() );
}

QDomNamedNodeMap QDomDocumentType::notations() const
{
  if ( !impl )
    return QDomNamedNodeMap();
  return QDomNamedNodeMap( IMPL->notations() );
}

QDomNode::NodeType QDomDocumentType::nodeType() const
{
  return DocumentTypeNode;
}

bool QDomDocumentType::isDocumentType() const
{
  return true;
}

#undef IMPL

/*==============================================================*/
/*                     DocumentFragment                         */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_DocumentFragmentPrivate
 *
 **************************************************************/

class QDOM_DocumentFragmentPrivate : public QDOM_NodePrivate
{
public:
  QDOM_DocumentFragmentPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent = 0 );
  QDOM_DocumentFragmentPrivate( QDOM_NodePrivate* n, bool deep );
  ~QDOM_DocumentFragmentPrivate();

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isDocumentFragment() { return TRUE; }

  static QString* s_dfName;
};

QString* QDOM_DocumentFragmentPrivate::s_dfName = 0;

QDOM_DocumentFragmentPrivate::QDOM_DocumentFragmentPrivate( QDOM_DocumentPrivate* doc, QDOM_NodePrivate* parent )
  : QDOM_NodePrivate( doc, parent )
{
  if ( !s_dfName )
    s_dfName = new QString( "#document-fragment" );
  m_name = *s_dfName;
}

QDOM_DocumentFragmentPrivate::QDOM_DocumentFragmentPrivate( QDOM_NodePrivate* n, bool deep )
  : QDOM_NodePrivate( n, deep )
{
}

QDOM_DocumentFragmentPrivate::~QDOM_DocumentFragmentPrivate()
{
}

QDOM_NodePrivate* QDOM_DocumentFragmentPrivate::cloneNode( bool deep)
{
  return new QDOM_DocumentFragmentPrivate( this, deep );
}

/**************************************************************
 *
 * QDomDocumentFragment
 *
 **************************************************************/

#define IMPL ((QDOM_DocumentFragmentPrivate*)impl)

QDomDocumentFragment::QDomDocumentFragment()
{
}

QDomDocumentFragment::QDomDocumentFragment( QDOM_DocumentFragmentPrivate* n )
  : QDomNode( n )
{
}

QDomDocumentFragment::QDomDocumentFragment( const QDomDocumentFragment& x )
  : QDomNode( x )
{
}

QDomDocumentFragment& QDomDocumentFragment::operator= ( const QDomDocumentFragment& x )
{
  return (QDomDocumentFragment&) QDomNode::operator=( x );
}

QDomDocumentFragment::~QDomDocumentFragment()
{
}

QDomNode::NodeType QDomDocumentFragment::nodeType() const
{
  return QDomNode::DocumentFragmentNode;
}

/* QDomNode QDomDocumentFragment::cloneNode( bool deep) const
{
  return QDomNode( new QDOM_DocumentFragmentPrivate( IMPL, deep ) );
  } */

bool QDomDocumentFragment::isDocumentFragment() const
{
  return TRUE;
}

#undef IMPL

/*==============================================================*/
/*                     CharacterData                            */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_CharacterDataPrivate
 *
 **************************************************************/

class QDOM_CharacterDataPrivate : public QDOM_NodePrivate
{
public:
  QDOM_CharacterDataPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& data );
  QDOM_CharacterDataPrivate( QDOM_CharacterDataPrivate* n, bool deep );
  ~QDOM_CharacterDataPrivate();

  uint length() const;
  QString substringData( unsigned long offset, unsigned long count ) const;
  void    appendData( const QString& arg );
  void    insertData( unsigned long offset, const QString& arg );
  void    deleteData( unsigned long offset, unsigned long count );
  void    replaceData( unsigned long offset, unsigned long count, const QString& arg );

  // Overloaded from QDOM_NodePrivate
  virtual bool isCharacterData() { return TRUE; }
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );

  static QString* s_cdName;
};

QString* QDOM_CharacterDataPrivate::s_cdName = 0;

QDOM_CharacterDataPrivate::QDOM_CharacterDataPrivate( QDOM_DocumentPrivate* d, QDOM_NodePrivate* p,
						      const QString& data )
  : QDOM_NodePrivate( d, p )
{
  m_value = data;

  if ( !s_cdName )
    s_cdName = new QString( "#character-data" );
  m_name = *s_cdName;
}

QDOM_CharacterDataPrivate::QDOM_CharacterDataPrivate( QDOM_CharacterDataPrivate* n, bool deep )
  : QDOM_NodePrivate( n, deep )
{
}

QDOM_CharacterDataPrivate::~QDOM_CharacterDataPrivate()
{
}

QDOM_NodePrivate* QDOM_CharacterDataPrivate::cloneNode( bool deep )
{
  return new QDOM_CharacterDataPrivate( this, deep );
}

uint QDOM_CharacterDataPrivate::length() const
{
  return m_value.length();
}

QString QDOM_CharacterDataPrivate::substringData( unsigned long offset, unsigned long count ) const
{
  return m_value.mid( offset, count );
}

void QDOM_CharacterDataPrivate::insertData( unsigned long offset, const QString& arg )
{
  m_value.insert( offset, arg );
}

void QDOM_CharacterDataPrivate::deleteData( unsigned long offset, unsigned long count )
{
  m_value.remove( offset, count );
}

void QDOM_CharacterDataPrivate::replaceData( unsigned long offset, unsigned long count, const QString& arg )
{
  m_value.replace( offset, count, arg );
}

void QDOM_CharacterDataPrivate::appendData( const QString& arg )
{
  m_value += arg;
}

/**************************************************************
 *
 * QDomCharacterData
 *
 **************************************************************/

#define IMPL ((QDOM_CharacterDataPrivate*)impl)

QDomCharacterData::QDomCharacterData()
{
}

QDomCharacterData::QDomCharacterData( const QDomCharacterData& x )
  : QDomNode( x )
{
}

QDomCharacterData::QDomCharacterData( QDOM_CharacterDataPrivate* n )
  : QDomNode( n )
{
}

QDomCharacterData& QDomCharacterData::operator= ( const QDomCharacterData& x )
{
  return (QDomCharacterData&) QDomNode::operator=( x );
}

QDomCharacterData::~QDomCharacterData()
{
}

QString QDomCharacterData::data() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeValue();
}

void QDomCharacterData::setData( const QString& v )
{
  if ( impl )
    impl->setNodeValue( v );
}

uint QDomCharacterData::length() const
{
  if ( impl )
    return IMPL->length();
  return 0;
}

QString QDomCharacterData::substringData( unsigned long offset, unsigned long count )
{
  if ( !impl )
    return QString::null;
  return IMPL->substringData( offset, count );
}

void QDomCharacterData::appendData( const QString& arg )
{
  if ( impl )
    IMPL->appendData( arg );
}

void QDomCharacterData::insertData( unsigned long offset, const QString& arg )
{
  if ( impl )
    IMPL->insertData( offset, arg );
}

void QDomCharacterData::deleteData( unsigned long offset, unsigned long count )
{
  if ( impl )
    IMPL->deleteData( offset, count );
}

void QDomCharacterData::replaceData( unsigned long offset, unsigned long count, const QString& arg )
{
  if ( impl )
    IMPL->replaceData( offset, count, arg );
}

QDomNode::NodeType QDomCharacterData::nodeType() const
{
  return CharacterDataNode;
}

bool QDomCharacterData::isCharacterData() const
{
  return TRUE;
}

#undef IMPL

/**************************************************************
 *
 * QDOM_TextPrivate
 *
 **************************************************************/

class QDOM_TextPrivate : public QDOM_CharacterDataPrivate
{
public:
  QDOM_TextPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& value );
  QDOM_TextPrivate( QDOM_TextPrivate* n, bool deep );
  ~QDOM_TextPrivate();

  QDOM_TextPrivate* splitText( int offset );

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isText() { return TRUE; }
  virtual void save( QTextStream& s ) const;

  static QString* s_textName;
};

/*==============================================================*/
/*                        Attr                                  */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_AttrPrivate
 *
 **************************************************************/

class QDOM_AttrPrivate : public QDOM_NodePrivate
{
public:
  QDOM_AttrPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& name );
  QDOM_AttrPrivate( QDOM_AttrPrivate* n, bool deep );
  ~QDOM_AttrPrivate();

  bool specified() const;

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isAttr() { return TRUE; }
  virtual void save( QTextStream& s ) const;

  // Variables
  bool m_specified;
};

QDOM_AttrPrivate::QDOM_AttrPrivate( QDOM_DocumentPrivate* d, QDOM_NodePrivate* parent, const QString& name )
  : QDOM_NodePrivate( d, parent )
{
  m_name = name;
}

QDOM_AttrPrivate::QDOM_AttrPrivate( QDOM_AttrPrivate* n, bool deep )
  : QDOM_NodePrivate( n, deep )
{
  m_specified = n->specified();
}

QDOM_AttrPrivate::~QDOM_AttrPrivate()
{
}

QDOM_NodePrivate* QDOM_AttrPrivate::cloneNode( bool deep )
{
  return new QDOM_AttrPrivate( this, deep );
}

bool QDOM_AttrPrivate::specified() const
{
  return m_specified;
}

void QDOM_AttrPrivate::save( QTextStream& s ) const
{
  s << m_name << "=\"" << m_value.utf8() << "\"";
}

/**************************************************************
 *
 * QDomAttr
 *
 **************************************************************/

#define IMPL ((QDOM_AttrPrivate*)impl)

QDomAttr::QDomAttr()
{
}

QDomAttr::QDomAttr( const QDomAttr& x )
  : QDomNode( x )
{
}

QDomAttr::QDomAttr( QDOM_AttrPrivate* n )
  : QDomNode( n )
{
}

QDomAttr& QDomAttr::operator= ( const QDomAttr& x )
{
  return (QDomAttr&) QDomNode::operator=( x );
}

QDomAttr::~QDomAttr()
{
}

QString QDomAttr::name() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeName();
}

bool QDomAttr::specified() const
{
  if ( !impl )
    return false;
  return IMPL->specified();
}

QString QDomAttr::value() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeValue();
}

void QDomAttr::setValue( const QString& v )
{
  if ( !impl )
    return;
  impl->setNodeValue( v );
}

/* QDomNode QDomAttr::cloneNode( bool deep) const
{
  if ( !impl )
    return QDomNode();
  return QDomNode( new QDOM_AttrPrivate( IMPL, deep ) );
  } */

QDomNode::NodeType QDomAttr::nodeType() const
{
  return AttributeNode;
}

bool QDomAttr::isAttr() const
{
  return TRUE;
}

#undef IMPL

/*==============================================================*/
/*                        Element                               */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_ElementPrivate
 *
 **************************************************************/

static void qNormalizeElement( QDOM_NodePrivate* n )
{
  QDOM_NodePrivate* p = n->firstChild();
  QDOM_TextPrivate* t = 0;

  while( p )
  {
    if ( p->isText() )
    {
      if ( t )
      {
	QDOM_NodePrivate* tmp = p->nextSibling();
	t->appendData( p->nodeValue() );
	n->removeChild( p );
	p = tmp;
      }
      else
      {
	t = (QDOM_TextPrivate*)p;
	p = p->nextSibling();
      }
    }
    else
    {
      p = p->nextSibling();
      t = 0;
    }
  }
}

class QDOM_ElementPrivate : public QDOM_NodePrivate
{
public:
  QDOM_ElementPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& name );
  QDOM_ElementPrivate( QDOM_ElementPrivate* n, bool deep );
  ~QDOM_ElementPrivate();

  virtual QString   attribute( const QString& name ) const;
  virtual void      setAttribute( const QString& name, const QString& value );
  virtual void      removeAttribute( const QString& name );
  virtual QDOM_AttrPrivate* attributeNode( const QString& name);
  virtual QDOM_AttrPrivate* setAttributeNode( QDOM_AttrPrivate* newAttr );
  virtual QDOM_AttrPrivate* removeAttributeNode( QDOM_AttrPrivate* oldAttr );
  // TODO
  // virtual NodeList* elementsByTagName( const QString& name );
  virtual bool hasAttribute( const QString& name );
  virtual void normalize();

  QString text();

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NamedNodeMapPrivate* attributes() { return m_attr; }
  virtual bool isElement() { return TRUE; }
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual void save( QTextStream& s ) const;

  // Variables
  QDOM_NamedNodeMapPrivate* m_attr;
};

QDOM_ElementPrivate::QDOM_ElementPrivate( QDOM_DocumentPrivate* d, QDOM_NodePrivate* p,
					  const QString& tagname )
  : QDOM_NodePrivate( d, p )
{
  m_name = tagname;
  m_attr = new QDOM_NamedNodeMapPrivate( this );
}

QDOM_ElementPrivate::QDOM_ElementPrivate( QDOM_ElementPrivate* n, bool deep ) :
  QDOM_NodePrivate( n, deep )
{
  m_attr = n->m_attr->clone();
}

QDOM_ElementPrivate::~QDOM_ElementPrivate()
{
  delete m_attr;
}

QDOM_NodePrivate* QDOM_ElementPrivate::cloneNode( bool deep)
{
  return new QDOM_ElementPrivate( this, deep );
}

QString QDOM_ElementPrivate::attribute( const QString& name ) const
{
  QDOM_NodePrivate* n = m_attr->namedItem( name );
  if ( !n )
    return QString::null;

  return n->nodeValue();
}

void QDOM_ElementPrivate::setAttribute( const QString& name, const QString& value )
{
  QDOM_NodePrivate* n = m_attr->namedItem( name );
  if ( n )
      removeAttributeNode( (QDOM_AttrPrivate*)n );
  
  n = new QDOM_AttrPrivate( ownerDocument(), this, name );
  n->setNodeValue( value );

  m_attr->setNamedItem( n );
}

void QDOM_ElementPrivate::removeAttribute( const QString& name )
{
  m_attr->removeNamedItem( name );
}

QDOM_AttrPrivate* QDOM_ElementPrivate::attributeNode( const QString& name )
{
  return (QDOM_AttrPrivate*)m_attr->namedItem( name );
}

QDOM_AttrPrivate* QDOM_ElementPrivate::setAttributeNode( QDOM_AttrPrivate* newAttr )
{
  QDOM_NodePrivate* n = m_attr->namedItem( newAttr->nodeName() );
  m_attr->setNamedItem( newAttr );

  return (QDOM_AttrPrivate*)n;
}

QDOM_AttrPrivate* QDOM_ElementPrivate::removeAttributeNode( QDOM_AttrPrivate* oldAttr )
{
  return (QDOM_AttrPrivate*)m_attr->removeNamedItem( oldAttr->nodeName() );
}

bool QDOM_ElementPrivate::hasAttribute( const QString& name )
{
  return m_attr->contains( name );
}

// TODO
/*
QDomNodeList* QDOM_ElementPrivate::elementsByTagName( const QString& name )
{
  NodeList* l = new NodeList( FALSE );

  qElementsByTagName( this, name, l );

  return l;
}
*/
/* const QDomNode* QDOM_ElementPrivate::childByTagName( const QString& name ) const
{
  const QDomNodeList* l = childNodes();
  uint len = l->length();

  for( uint i = 0; i < len; ++i )
    if ( l->item(i)->nodeName() == name )
      return l->item(i);

  return 0;
  } */


void QDOM_ElementPrivate::normalize()
{
  qNormalizeElement( this );
}

QString QDOM_ElementPrivate::text()
{
  QString t( "" );

  QDOM_NodePrivate* p = m_firstChild;
  while( p )
  {
    if ( p->isText() )
      t += p->nodeValue();
    p = p->nextSibling();
  }

  return t;
}

void QDOM_ElementPrivate::save( QTextStream& s ) const
{
  s << "<" << m_name;

  uint len = length();
  if ( len )
  {
    s << " ";
    QDictIterator<QDOM_NodePrivate> it( m_attr->m_map );
    for( ; it.current(); ++it )
    {
      it.current()->save( s );
      s << " ";
    }
  }

  if ( hasChildNodes() )
  {
    s << ">" << endl;
    QDOM_NodePrivate::save( s );
    s << "</" << m_name << ">" << endl;
  }
  else
    s << "/>" << endl;
}

/**************************************************************
 *
 * QDomElement
 *
 **************************************************************/

#define IMPL ((QDOM_ElementPrivate*)impl)

QDomElement::QDomElement()
  : QDomNode()
{
}

QDomElement::QDomElement( const QDomElement& x )
  : QDomNode( x )
{
}

QDomElement::QDomElement( QDOM_ElementPrivate* n )
  : QDomNode( n )
{
}

QDomElement& QDomElement::operator= ( const QDomElement& x )
{
  return (QDomElement&) QDomNode::operator=( x );
}

QDomElement::~QDomElement()
{
}

/* QDomNode QDomElement::cloneNode( bool deep) const
{
  return QDomNode( new QDOM_ElementPrivate( this, deep ) );
  } */

QDomNode::NodeType QDomElement::nodeType() const
{
  return ElementNode;
}

void QDomElement::setTagName( const QString& name )
{
  if ( impl )
    impl->m_name = name;
}

QString QDomElement::tagName() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeName();
}

QString QDomElement::attribute( const QString& name ) const
{
  if ( !impl )
    return QString::null;
  return IMPL->attribute( name );
}

void QDomElement::setAttribute( const QString& name, const QString& value )
{
  if ( !impl )
    return;
  IMPL->setAttribute( name, value );
}

void QDomElement::setAttribute( const QString& name, int value )
{
  if ( !impl )
    return;
  QString x;
  x.setNum( value );
  IMPL->setAttribute( name, x );
}

void QDomElement::setAttribute( const QString& name, double value )
{
  if ( !impl )
    return;
  QString x;
  x.setNum( value );
  IMPL->setAttribute( name, x );
}

void QDomElement::removeAttribute( const QString& name )
{
  if ( !impl )
    return;
  IMPL->removeAttribute( name );
}

QDomAttr QDomElement::attributeNode( const QString& name)
{
  if ( !impl )
    return QDomAttr();
  return QDomAttr( IMPL->attributeNode( name ) );
}

QDomAttr QDomElement::setAttributeNode( const QDomAttr& newAttr )
{
  if ( !impl )
    return QDomAttr();
  return QDomAttr( IMPL->setAttributeNode( ((QDOM_AttrPrivate*)newAttr.impl) ) );
}

QDomAttr QDomElement::removeAttributeNode( const QDomAttr& oldAttr )
{
  if ( !impl )
    return QDomAttr();
  return QDomAttr( IMPL->removeAttributeNode( ((QDOM_AttrPrivate*)oldAttr.impl) ) );
}

// TODO
//  NodeList QDomElement::elementsByTagName( const QString& name );

void QDomElement::normalize()
{
  if ( !impl )
    return;
  IMPL->normalize();
}

bool QDomElement::isElement() const
{
  return TRUE;
}

QDomNamedNodeMap QDomElement::attributes() const
{
  if ( !impl )
    return QDomNamedNodeMap();
  return QDomNamedNodeMap( IMPL->attributes() );
}

bool QDomElement::hasAttribute( const QString& name ) const
{
  if ( !impl )
    return FALSE;
  return IMPL->hasAttribute( name );
}

QString QDomElement::text() const
{
  if ( !impl )
    return QString::null;
  return IMPL->text();
}

QRect QDomElement::toRect() const
{
  if ( !impl )
    return QRect();
  return QRect( attribute( "x" ).toInt(), attribute( "y" ).toInt(),
		attribute( "width" ).toInt(), attribute( "height" ).toInt() );
}

QSize QDomElement::toSize() const
{
  if ( !impl )
    return QSize();
  return QSize( attribute( "width" ).toInt(), attribute( "height" ).toInt() );
}

QPoint QDomElement::toPoint() const
{
  if ( !impl )
    return QPoint();
  return QPoint( attribute( "x" ).toInt(), attribute( "y" ).toInt() );
}

QPen QDomElement::toPen() const
{
  if ( !impl )
    return QPen();

  bool ok;
  QPen p;
  p.setStyle( (Qt::PenStyle)attribute("style").toInt( &ok ) );
  if ( !ok ) return QPen();

  p.setWidth( attribute("width").toInt( &ok ) );
  if ( !ok ) return QPen();

  p.setColor( QColor( attribute("color") ) );

  return p;
}

QFont QDomElement::toFont() const
{
  if ( !impl )
    return QFont();

  QFont f;
  f.setFamily( attribute( "family" ) );

  bool ok;
  f.setPointSize( attribute("size").toInt( &ok ) );
  if ( !ok ) return QFont();

  f.setWeight( attribute("weight").toInt( &ok ) );
  if ( !ok ) return QFont();

  if ( hasAttribute( "italic" ) )
    f.setItalic( TRUE );

  if ( hasAttribute( "bold" ) )
    f.setBold( TRUE );

  return f;
}

#undef IMPL

/*==============================================================*/
/*                          Text                                */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_TextPrivate
 *
 **************************************************************/

QString* QDOM_TextPrivate::s_textName = 0;

QDOM_TextPrivate::QDOM_TextPrivate( QDOM_DocumentPrivate* d, QDOM_NodePrivate* parent, const QString& value )
  : QDOM_CharacterDataPrivate( d, parent, value )
{
  if ( !s_textName )
    s_textName = new QString( "#text" );
  m_name = *s_textName;
}

QDOM_TextPrivate::QDOM_TextPrivate( QDOM_TextPrivate* n, bool deep )
  : QDOM_CharacterDataPrivate( n, deep )
{
}

QDOM_TextPrivate::~QDOM_TextPrivate()
{
}

QDOM_NodePrivate* QDOM_TextPrivate::cloneNode( bool deep)
{
  return new QDOM_TextPrivate( this, deep );
}

QDOM_TextPrivate* QDOM_TextPrivate::splitText( int offset )
{
  if ( !parentNode() )
  {
    qWarning( "QDomText::splitText  The node has no parent. So I can not split" );
    return 0;
  }

  QDOM_TextPrivate* t = new QDOM_TextPrivate( ownerDocument(), 0, m_value.mid( offset ) );
  m_value.truncate( offset );

  parentNode()->insertAfter( t, this );

  return t;
}

void QDOM_TextPrivate::save( QTextStream& s ) const
{
  s << m_value.utf8();
}

/**************************************************************
 *
 * QDomText
 *
 **************************************************************/

#define IMPL ((QDOM_TextPrivate*)impl)

QDomText::QDomText()
  : QDomCharacterData()
{
}

QDomText::QDomText( const QDomText& x )
  : QDomCharacterData( x )
{
}

QDomText::QDomText( QDOM_TextPrivate* n )
  : QDomCharacterData( n )
{
}

QDomText& QDomText::operator= ( const QDomText& x )
{
  return (QDomText&) QDomNode::operator=( x );
}

QDomText::~QDomText()
{
}

/* QDomNode QDomText::cloneNode( bool deep) const
{
  return QDomNode( new QDOM_TextPrivate( this, deep ) );
  } */

QDomNode::NodeType QDomText::nodeType() const
{
  return TextNode;
}

QDomText QDomText::splitText( int offset )
{
  if ( !impl )
    return QDomText();
  return QDomText( IMPL->splitText( offset ) );
}

bool QDomText::isText() const
{
  return TRUE;
}

#undef IMPL

/*==============================================================*/
/*                          Comment                             */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_CommentPrivate
 *
 **************************************************************/

class QDOM_CommentPrivate : public QDOM_CharacterDataPrivate
{
public:
  QDOM_CommentPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& value );
  QDOM_CommentPrivate( QDOM_CommentPrivate* n, bool deep );
  ~QDOM_CommentPrivate();

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isComment() { return TRUE; }
  virtual void save( QTextStream& s ) const;

  static QString* s_commentName;
};

QString* QDOM_CommentPrivate::s_commentName = 0;

QDOM_CommentPrivate::QDOM_CommentPrivate( QDOM_DocumentPrivate* d, QDOM_NodePrivate* parent, const QString& value )
  : QDOM_CharacterDataPrivate( d, parent, value )
{
  if ( !s_commentName )
    s_commentName = new QString( "#comment" );
  m_name = *s_commentName;
}

QDOM_CommentPrivate::QDOM_CommentPrivate( QDOM_CommentPrivate* n, bool deep )
  : QDOM_CharacterDataPrivate( n, deep )
{
}

QDOM_CommentPrivate::~QDOM_CommentPrivate()
{
}

QDOM_NodePrivate* QDOM_CommentPrivate::cloneNode( bool deep)
{
  return new QDOM_CommentPrivate( this, deep );
}

void QDOM_CommentPrivate::save( QTextStream& s ) const
{
  s << "<!--" << m_value << "-->";
}

/**************************************************************
 *
 * QDomComment
 *
 **************************************************************/

#define IMPL ((QDOM_CommentPrivate*)impl)

QDomComment::QDomComment()
  : QDomCharacterData()
{
}

QDomComment::QDomComment( const QDomComment& x )
  : QDomCharacterData( x )
{
}

QDomComment::QDomComment( QDOM_CommentPrivate* n )
  : QDomCharacterData( n )
{
}

QDomComment& QDomComment::operator= ( const QDomComment& x )
{
  return (QDomComment&) QDomNode::operator=( x );
}

QDomComment::~QDomComment()
{
}

/* QDomNode QDomComment::cloneNode( bool deep) const
{
  return QDomNode( new QDOM_CommentPrivate( this, deep ) );
  } */

QDomNode::NodeType QDomComment::nodeType() const
{
  return CommentNode;
}

bool QDomComment::isComment() const
{
  return TRUE;
}

#undef IMPL

/*==============================================================*/
/*                        CDATASection                          */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_CDATASectionPrivate
 *
 **************************************************************/

class QDOM_CDATASectionPrivate : public QDOM_TextPrivate
{
public:
  QDOM_CDATASectionPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& value );
  QDOM_CDATASectionPrivate( QDOM_CDATASectionPrivate* n, bool deep );
  ~QDOM_CDATASectionPrivate();

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isCDATASection() { return TRUE; }
  virtual void save( QTextStream& s ) const;

  static QString* s_cdataName;
};

QString* QDOM_CDATASectionPrivate::s_cdataName = 0;

QDOM_CDATASectionPrivate::QDOM_CDATASectionPrivate( QDOM_DocumentPrivate* d, QDOM_NodePrivate* parent,
						    const QString& value )
  : QDOM_TextPrivate( d, parent, value )
{
  if ( !s_cdataName )
    s_cdataName = new QString( "#cdata-section" );
  m_name = *s_cdataName;
}

QDOM_CDATASectionPrivate::QDOM_CDATASectionPrivate( QDOM_CDATASectionPrivate* n, bool deep )
  : QDOM_TextPrivate( n, deep )
{
}

QDOM_CDATASectionPrivate::~QDOM_CDATASectionPrivate()
{
}

QDOM_NodePrivate* QDOM_CDATASectionPrivate::cloneNode( bool deep)
{
  return new QDOM_CDATASectionPrivate( this, deep );
}

void QDOM_CDATASectionPrivate::save( QTextStream& s ) const
{
  s << "<[CDATA[" << m_value << "]]>";
}

/**************************************************************
 *
 * QDomCDATASection
 *
 **************************************************************/

#define IMPL ((QDOM_CDATASectionPrivate*)impl)

QDomCDATASection::QDomCDATASection()
  : QDomText()
{
}

QDomCDATASection::QDomCDATASection( const QDomCDATASection& x )
  : QDomText( x )
{
}

QDomCDATASection::QDomCDATASection( QDOM_CDATASectionPrivate* n )
  : QDomText( n )
{
}

QDomCDATASection& QDomCDATASection::operator= ( const QDomCDATASection& x )
{
  return (QDomCDATASection&) QDomNode::operator=( x );
}

QDomCDATASection::~QDomCDATASection()
{
}

/* QDomNode QDomCDATASection::cloneNode( bool deep) const
{
  return QDomNode( new QDOM_CDATASectionPrivate( this, deep ) );
  } */

QDomNode::NodeType QDomCDATASection::nodeType() const
{
  return CDATASectionNode;
}

bool QDomCDATASection::isCDATASection() const
{
  return TRUE;
}

#undef IMPL

/*==============================================================*/
/*                          Notation                            */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_NotationPrivate
 *
 **************************************************************/

class QDOM_NotationPrivate : public QDOM_NodePrivate
{
public:
  QDOM_NotationPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& name,
			const QString& pub, const QString& sys );
  QDOM_NotationPrivate( QDOM_NotationPrivate* n, bool deep );
  ~QDOM_NotationPrivate();

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isNotation() { return TRUE; }
  virtual void save( QTextStream& s ) const;

  // Variables
  QString m_sys;
  QString m_pub;
};

QDOM_NotationPrivate::QDOM_NotationPrivate( QDOM_DocumentPrivate* d, QDOM_NodePrivate* parent, const QString& name,
					    const QString& pub, const QString& sys )
  : QDOM_NodePrivate( d, parent )
{
  m_name = name;
  m_pub = pub;
  m_sys = sys;
}

QDOM_NotationPrivate::QDOM_NotationPrivate( QDOM_NotationPrivate* n, bool deep )
  : QDOM_NodePrivate( n, deep )
{
  m_sys = n->m_sys;
  m_pub = n->m_pub;
}

QDOM_NotationPrivate::~QDOM_NotationPrivate()
{
}

QDOM_NodePrivate* QDOM_NotationPrivate::cloneNode( bool deep)
{
  return new QDOM_NotationPrivate( this, deep );
}

void QDOM_NotationPrivate::save( QTextStream& s ) const
{
  s << "<!NOTATION " << m_name << " ";
  if ( !m_pub.isEmpty() )
  {
    s << "PUBLIC " << m_pub;
    if ( !m_sys.isEmpty() )
      s << " " << m_sys;
  }
  else
    s << "SYSTEM " << m_sys;

  s << ">";
}

/**************************************************************
 *
 * QDomNotation
 *
 **************************************************************/

#define IMPL ((QDOM_NotationPrivate*)impl)

QDomNotation::QDomNotation()
  : QDomNode()
{
}

QDomNotation::QDomNotation( const QDomNotation& x )
  : QDomNode( x )
{
}

QDomNotation::QDomNotation( QDOM_NotationPrivate* n )
  : QDomNode( n )
{
}

QDomNotation& QDomNotation::operator= ( const QDomNotation& x )
{
  return (QDomNotation&) QDomNode::operator=( x );
}

QDomNotation::~QDomNotation()
{
}

/* QDomNode QDomNotation::cloneNode( bool deep) const
{
  return QDomNode( new QDOM_NotationPrivate( this, deep ) );
  } */

QDomNode::NodeType QDomNotation::nodeType() const
{
  return NotationNode;
}

QString QDomNotation::publicId() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_pub;
}

QString QDomNotation::systemId() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_sys;
}

bool QDomNotation::isNotation() const
{
  return TRUE;
}

#undef IMPL


/*==============================================================*/
/*                          Entity                            */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_EntityPrivate
 *
 **************************************************************/

class QDOM_EntityPrivate : public QDOM_NodePrivate
{
public:
  QDOM_EntityPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& name,
		      const QString& pub, const QString& sys, const QString& notation );
  QDOM_EntityPrivate( QDOM_EntityPrivate* n, bool deep );
  ~QDOM_EntityPrivate();

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isEntity() { return TRUE; }
  virtual void save( QTextStream& s ) const;

  // Variables
  QString m_sys;
  QString m_pub;
  QString m_notationName;
};

QDOM_EntityPrivate::QDOM_EntityPrivate( QDOM_DocumentPrivate* d, QDOM_NodePrivate* parent, const QString& name,
					const QString& pub, const QString& sys, const QString& notation )
  : QDOM_NodePrivate( d, parent )
{
  m_name = name;
  m_pub = pub;
  m_sys = sys;
  m_notationName = notation;
}

QDOM_EntityPrivate::QDOM_EntityPrivate( QDOM_EntityPrivate* n, bool deep )
  : QDOM_NodePrivate( n, deep )
{
  m_sys = n->m_sys;
  m_pub = n->m_pub;
  m_notationName = n->m_notationName;
}

QDOM_EntityPrivate::~QDOM_EntityPrivate()
{
}

QDOM_NodePrivate* QDOM_EntityPrivate::cloneNode( bool deep)
{
  return new QDOM_EntityPrivate( this, deep );
}

void QDOM_EntityPrivate::save( QTextStream& s ) const
{
  if ( m_sys.isEmpty() && m_pub.isEmpty() )
  {
    s << "<!ENTITY " << m_name << " \"" << m_value << "\">";
  }
  else
  {
    s << "<!ENTITY " << m_name << " ";
    if ( m_pub.isEmpty() )
      s << "SYSTEM " << m_sys;
    else
      s << "PUBLIC " << m_pub << " " << m_sys;
    s << ">";
  }
}

/**************************************************************
 *
 * QDomEntity
 *
 **************************************************************/

#define IMPL ((QDOM_EntityPrivate*)impl)

QDomEntity::QDomEntity()
  : QDomNode()
{
}

QDomEntity::QDomEntity( const QDomEntity& x )
  : QDomNode( x )
{
}

QDomEntity::QDomEntity( QDOM_EntityPrivate* n )
  : QDomNode( n )
{
}

QDomEntity& QDomEntity::operator= ( const QDomEntity& x )
{
  return (QDomEntity&) QDomNode::operator=( x );
}

QDomEntity::~QDomEntity()
{
}

/* QDomNode QDomEntity::cloneNode( bool deep) const
{
  return QDomNode( new QDOM_EntityPrivate( this, deep ) );
  } */

QDomNode::NodeType QDomEntity::nodeType() const
{
  return EntityNode;
}

QString QDomEntity::publicId() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_pub;
}

QString QDomEntity::systemId() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_sys;
}

QString QDomEntity::notationName() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_notationName;
}

bool QDomEntity::isEntity() const
{
  return TRUE;
}

#undef IMPL


/*==============================================================*/
/*                      EntityReference                         */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_EntityReferencePrivate
 *
 **************************************************************/

class QDOM_EntityReferencePrivate : public QDOM_NodePrivate
{
public:
  QDOM_EntityReferencePrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& name );
  QDOM_EntityReferencePrivate( QDOM_NodePrivate* n, bool deep );
  ~QDOM_EntityReferencePrivate();

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isEntityReference() { return TRUE; }
  virtual void save( QTextStream& s ) const;
};

QDOM_EntityReferencePrivate::QDOM_EntityReferencePrivate( QDOM_DocumentPrivate* d, QDOM_NodePrivate* parent, const QString& name )
  : QDOM_NodePrivate( d, parent )
{
  m_name = name;
}

QDOM_EntityReferencePrivate::QDOM_EntityReferencePrivate( QDOM_NodePrivate* n, bool deep )
  : QDOM_NodePrivate( n, deep )
{
}

QDOM_EntityReferencePrivate::~QDOM_EntityReferencePrivate()
{
}

QDOM_NodePrivate* QDOM_EntityReferencePrivate::cloneNode( bool deep)
{
  return new QDOM_EntityReferencePrivate( this, deep );
}

void QDOM_EntityReferencePrivate::save( QTextStream& s ) const
{
  s << "&" << m_name << ";";
}

/**************************************************************
 *
 * QDomEntityReference
 *
 **************************************************************/

#define IMPL ((QDOM_EntityReferencePrivate*)impl)

QDomEntityReference::QDomEntityReference()
  : QDomNode()
{
}

QDomEntityReference::QDomEntityReference( const QDomEntityReference& x )
  : QDomNode( x )
{
}

QDomEntityReference::QDomEntityReference( QDOM_EntityReferencePrivate* n )
  : QDomNode( n )
{
}

QDomEntityReference& QDomEntityReference::operator= ( const QDomEntityReference& x )
{
  return (QDomEntityReference&) QDomNode::operator=( x );
}

QDomEntityReference::~QDomEntityReference()
{
}

/* QDomNode QDomEntityReference::cloneNode( bool deep) const
{
  return QDomNode( new QDOM_EntityReferencePrivate( this, deep ) );
  } */

QDomNode::NodeType QDomEntityReference::nodeType() const
{
  return EntityReferenceNode;
}

bool QDomEntityReference::isEntityReference() const
{
  return TRUE;
}

#undef IMPL


/*==============================================================*/
/*                      ProcessingInstruction                   */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_ProcessingInstructionPrivate
 *
 **************************************************************/

class QDOM_ProcessingInstructionPrivate : public QDOM_NodePrivate
{
public:
  QDOM_ProcessingInstructionPrivate( QDOM_DocumentPrivate*, QDOM_NodePrivate* parent, const QString& target,
				     const QString& data);
  QDOM_ProcessingInstructionPrivate( QDOM_ProcessingInstructionPrivate* n, bool deep );
  ~QDOM_ProcessingInstructionPrivate();

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isProcessingInstruction() { return TRUE; }
  virtual void save( QTextStream& s ) const;
};

QDOM_ProcessingInstructionPrivate::QDOM_ProcessingInstructionPrivate( QDOM_DocumentPrivate* d,
								      QDOM_NodePrivate* parent,
								      const QString& target,
								      const QString& data )
  : QDOM_NodePrivate( d, parent )
{
  m_name = target;
  m_value = data;
}

QDOM_ProcessingInstructionPrivate::QDOM_ProcessingInstructionPrivate( QDOM_ProcessingInstructionPrivate* n, bool deep )
  : QDOM_NodePrivate( n, deep )
{
}

QDOM_ProcessingInstructionPrivate::~QDOM_ProcessingInstructionPrivate()
{
}

QDOM_NodePrivate* QDOM_ProcessingInstructionPrivate::cloneNode( bool deep)
{
  return new QDOM_ProcessingInstructionPrivate( this, deep );
}

void QDOM_ProcessingInstructionPrivate::save( QTextStream& s ) const
{
  s << "<?" << m_name << " " << m_value << "?>";
}

/**************************************************************
 *
 * QDomProcessingInstruction
 *
 **************************************************************/

#define IMPL ((QDOM_ProcessingInstructionPrivate*)impl)

QDomProcessingInstruction::QDomProcessingInstruction()
  : QDomNode()
{
}

QDomProcessingInstruction::QDomProcessingInstruction( const QDomProcessingInstruction& x )
  : QDomNode( x )
{
}

QDomProcessingInstruction::QDomProcessingInstruction( QDOM_ProcessingInstructionPrivate* n )
  : QDomNode( n )
{
}

QDomProcessingInstruction& QDomProcessingInstruction::operator= ( const QDomProcessingInstruction& x )
{
  return (QDomProcessingInstruction&) QDomNode::operator=( x );
}

QDomProcessingInstruction::~QDomProcessingInstruction()
{
}

QDomNode::NodeType QDomProcessingInstruction::nodeType() const
{
  return ProcessingInstructionNode;
}

QString QDomProcessingInstruction::target() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeName();
}

QString QDomProcessingInstruction::data() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeValue();
}

void QDomProcessingInstruction::setData( const QString& d )
{
  if ( !impl )
    return;
  impl->setNodeValue( d );
}

bool QDomProcessingInstruction::isProcessingInstruction() const
{
  return TRUE;
}

#undef IMPL

/*==============================================================*/
/*                 Document                                     */
/*==============================================================*/

/**************************************************************
 *
 * QDOM_DocumentPrivate
 *
 **************************************************************/

class QDOM_DocumentPrivate : public QDOM_NodePrivate
{
public:
  QDOM_DocumentPrivate();
  QDOM_DocumentPrivate( const QString& name );
  QDOM_DocumentPrivate( QDOM_DocumentPrivate* n, bool deep );
  ~QDOM_DocumentPrivate();

  bool setContent( const QString& text );
  QMimeSourceFactory* mimeSourceFactory();
  void setMimeSourceFactory( QMimeSourceFactory* );

  // Attributes
  QDOM_DocumentTypePrivate* doctype() { return type; };
  QDOM_ImplementationPrivate* implementation() { return impl; };
  QDOM_ElementPrivate* documentElement();

  // Factories
  QDOM_ElementPrivate*               createElement( const QString& tagName );
  QDOM_DocumentFragmentPrivate*      createDocumentFragment();
  QDOM_TextPrivate*                  createTextNode( const QString& data );
  QDOM_CommentPrivate*               createComment( const QString& data );
  QDOM_CDATASectionPrivate*          createCDATASection( const QString& data );
  QDOM_ProcessingInstructionPrivate* createProcessingInstruction( const QString& target, const QString& data );
  QDOM_AttrPrivate*                  createAttribute( const QString& name );
  QDOM_EntityReferencePrivate*       createEntityReference( const QString& name );
  QDOM_NodeListPrivate*              elementsByTagName( const QString& tagname );

  // Overloaded from QDOM_NodePrivate
  virtual QDOM_NodePrivate* cloneNode( bool deep = TRUE );
  virtual bool isDocument() { return TRUE; }
  virtual void clear();
  virtual void save( QTextStream& ) const;

  // Variables
  QDOM_ImplementationPrivate* impl;
  QDOM_DocumentTypePrivate* type;
  QMimeSourceFactory* m_mimeSourceFactory;

  static QString* s_docName;
};

QString* QDOM_DocumentPrivate::s_docName = 0;

QDOM_DocumentPrivate::QDOM_DocumentPrivate()
  : QDOM_NodePrivate( 0 )
{
  m_mimeSourceFactory = 0;
  impl = new QDOM_ImplementationPrivate();
  type = new QDOM_DocumentTypePrivate( this, this );

  if ( !s_docName )
    s_docName = new QString( "#document" );
  m_name = *s_docName;
}

QDOM_DocumentPrivate::QDOM_DocumentPrivate( const QString& name )
  : QDOM_NodePrivate( 0 )
{
  m_mimeSourceFactory = 0;
  impl = new QDOM_ImplementationPrivate();
  type = new QDOM_DocumentTypePrivate( this, this );
  type->m_name = name;

  if ( !s_docName )
    s_docName = new QString( "#document" );
  m_name = *s_docName;
}

QDOM_DocumentPrivate::QDOM_DocumentPrivate( QDOM_DocumentPrivate* n, bool deep )
  : QDOM_NodePrivate::QDOM_NodePrivate( n, deep )
{
  m_mimeSourceFactory = n->m_mimeSourceFactory;
  impl = n->impl->clone();
  type = (QDOM_DocumentTypePrivate*)n->type->cloneNode();
}

QDOM_DocumentPrivate::~QDOM_DocumentPrivate()
{
  delete impl;
  delete type;
}

void QDOM_DocumentPrivate::clear()
{
  delete impl;
  delete type;
  impl = 0;
  type = 0;
  QDOM_NodePrivate::clear();
}

QMimeSourceFactory* QDOM_DocumentPrivate::mimeSourceFactory()
{
  if ( m_mimeSourceFactory )
    return m_mimeSourceFactory;

  return QMimeSourceFactory::defaultFactory();
}

void QDOM_DocumentPrivate::setMimeSourceFactory( QMimeSourceFactory* f )
{
  m_mimeSourceFactory = f;
}

bool QDOM_DocumentPrivate::setContent( const QString& text )
{
  clear();
  impl = new QDOM_ImplementationPrivate;
  type = new QDOM_DocumentTypePrivate( this, this );

  QXMLSimpleParser p;
  QDomConsumer c( this );
  int pos = p.parse( text, &c );
  if ( pos != -1 )
  {
    qWarning("Parsing error at %i\n",pos);
    return FALSE;
  }
  return TRUE;
}

QDOM_NodePrivate* QDOM_DocumentPrivate::cloneNode( bool deep)
{
  return new QDOM_DocumentPrivate( this, deep );
}

QDOM_ElementPrivate* QDOM_DocumentPrivate::documentElement()
{
  QDOM_NodePrivate* p = m_firstChild;
  while( p && !p->isElement() )
    p = p->nextSibling();

  return (QDOM_ElementPrivate*)p;
}

QDOM_ElementPrivate* QDOM_DocumentPrivate::createElement( const QString& tagName )
{
  return new QDOM_ElementPrivate( this, 0, tagName );
}

QDOM_DocumentFragmentPrivate* QDOM_DocumentPrivate::createDocumentFragment()
{
  return new QDOM_DocumentFragmentPrivate( this, 0 );
}

QDOM_TextPrivate* QDOM_DocumentPrivate::createTextNode( const QString& data )
{
  return new QDOM_TextPrivate( this, 0, data );
}

QDOM_CommentPrivate* QDOM_DocumentPrivate::createComment( const QString& data )
{
  return new QDOM_CommentPrivate( this, 0, data );
}

QDOM_CDATASectionPrivate* QDOM_DocumentPrivate::createCDATASection( const QString& data )
{
  return new QDOM_CDATASectionPrivate( this, 0, data );
}

QDOM_ProcessingInstructionPrivate* QDOM_DocumentPrivate::createProcessingInstruction( const QString& target, const QString& data )
{
  return new QDOM_ProcessingInstructionPrivate( this, 0, target, data );
}

QDOM_AttrPrivate* QDOM_DocumentPrivate::createAttribute( const QString& name )
{
  return new QDOM_AttrPrivate( this, 0, name );
}

QDOM_EntityReferencePrivate* QDOM_DocumentPrivate::createEntityReference( const QString& name )
{
  return new QDOM_EntityReferencePrivate( this, 0, name );
}

void QDOM_DocumentPrivate::save( QTextStream& s ) const
{
  bool doc = FALSE;

  QDOM_NodePrivate* n = m_firstChild;
  while( n )
  {
    if ( !doc && !n->isProcessingInstruction() )
    {
      type->save( s );
      doc = TRUE;
    }
    n->save( s );
    n = n->m_nextSibling;
  }
}

/*
static void qElementsByTagName( const QDomNode* node, const QString& tagname, QDomNodeList* l )
{
  const QDomNodeList* m = node->childNodes();
  uint len = m->length();
  for( uint i = 0; i < len; ++i )
  {
    const QDomNode* n = m->item( i );
    if ( n->nodeName() == tagname )
      l->appendChild( (QDomNode*)n );

    qElementsByTagName( n, tagname, l );
  }
}

QDomNodeList* QDOM_DocumentPrivate::elementsByTagName( const QString& tagname )
{
  NodeList* l = new NodeList( FALSE );

  qElementsByTagName( this, tagname, l );

  return l;
}
*/

/**************************************************************
 *
 * QDomDocument
 *
 **************************************************************/

#define IMPL ((QDOM_DocumentPrivate*)impl)

QDomDocument::QDomDocument()
{
}

QDomDocument::QDomDocument( const QString& name )
{
  // We take over ownership
  impl = new QDOM_DocumentPrivate( name );
}

QDomDocument::QDomDocument( const QDomDocument& x )
  : QDomNode( x )
{
}

QDomDocument::QDomDocument( QDOM_DocumentPrivate* x )
  : QDomNode( x )
{
}

QDomDocument::QDomDocument( QIODevice* dev )
{
  uint size = dev->size();
  char* buffer = new char[ size + 1 ];
  dev->readBlock( buffer, size );
  buffer[ size ] = 0;

  QString text = QString::fromUtf8( buffer, size );
  delete[] buffer;

  setContent( text );
}

QDomDocument& QDomDocument::operator= ( const QDomDocument& x )
{
  return (QDomDocument&) QDomNode::operator=( x );
}

QDomDocument::~QDomDocument()
{
}

bool QDomDocument::setContent( const QString& text )
{
  if ( !impl )
    impl = new QDOM_DocumentPrivate;
  return IMPL->setContent( text );
}

QDomDocumentType QDomDocument::doctype() const
{
  if ( !impl )
    return QDomDocumentType();
  return QDomDocumentType( IMPL->doctype() );
}

QDomImplementation QDomDocument::implementation() const
{
  if ( !impl )
    return QDomImplementation();
  return QDomImplementation( IMPL->implementation() );
}

QDomElement QDomDocument::documentElement() const
{
  if ( !impl )
    return QDomElement();
  return QDomElement( IMPL->documentElement() );
}

QDomElement QDomDocument::createElement( const QString& tagName )
{
  if ( !impl )
    return QDomElement();
  return QDomElement( IMPL->createElement( tagName ) );
}

QDomDocumentFragment QDomDocument::createDocumentFragment()
{
  if ( !impl )
    return QDomDocumentFragment();
  return QDomDocumentFragment( IMPL->createDocumentFragment() );
}

QDomText QDomDocument::createTextNode( const QString& value )
{
  if ( !impl )
    return QDomText();
  return QDomText( IMPL->createTextNode( value ) );
}

QDomComment QDomDocument::createComment( const QString& value )
{
  if ( !impl )
    return QDomComment();
  return QDomComment( IMPL->createComment( value ) );
}

QDomCDATASection QDomDocument::createCDATASection( const QString& value )
{
  if ( !impl )
    return QDomCDATASection();
  return QDomCDATASection( IMPL->createCDATASection( value ) );
}

QDomProcessingInstruction QDomDocument::createProcessingInstruction( const QString& target,
									 const QString& data )
{
  if ( !impl )
    return QDomProcessingInstruction();
  return QDomProcessingInstruction( IMPL->createProcessingInstruction( target, data ) );
}

QDomAttr QDomDocument::createAttribute( const QString& name )
{
  if ( !impl )
    return QDomAttr();
  return QDomAttr( IMPL->createAttribute( name ) );
}

QDomEntityReference QDomDocument::createEntityReference( const QString& name )
{
  if ( !impl )
    return QDomEntityReference();
  return QDomEntityReference( IMPL->createEntityReference( name ) );
}

QDomElement QDomDocument::createElement( const QString& tagname, const QRect& rect )
{
  if ( !impl )
    return QDomElement();
  QDomElement e( createElement( tagname ) );

  QString str;
  str.setNum( rect.x() );
  e.setAttribute( "x", str );
  str.setNum( rect.y() );
  e.setAttribute( "y", str );
  str.setNum( rect.width() );
  e.setAttribute( "width", str );
  str.setNum( rect.height() );
  e.setAttribute( "height", str );

  return e;
}

QDomElement QDomDocument::createElement( const QString& tagname, const QPoint& p )
{
  if ( !impl )
    return QDomElement();
  QDomElement e( createElement( tagname ) );

  QString str;
  str.setNum( p.x() );
  e.setAttribute( "x", str );
  str.setNum( p.y() );
  e.setAttribute( "y", str );

  return e;
}

QDomElement QDomDocument::createElement( const QString& tagname, const QSize& s )
{
  if ( !impl )
    return QDomElement();
  QDomElement e( createElement( tagname ) );

  QString str;
  str.setNum( s.width() );
  e.setAttribute( "width", str );
  str.setNum( s.height() );
  e.setAttribute( "height", str );

  return e;
}

QDomElement QDomDocument::createElement( const QString& tagname, const QPen& pen )
{
  if ( !impl )
    return QDomElement();
  QDomElement e( createElement( tagname ) );

  //e.setAttribute( "color", colorToName( pen.color() ) );
  e.setAttribute( "style", (int)pen.style() );
  e.setAttribute( "width", (int)pen.width() );

  return e;
}

QDomElement QDomDocument::createElement( const QString& tagname, const QFont& font )
{
  if ( !impl )
    return QDomElement();
  QDomElement e( createElement( tagname ) );

  e.setAttribute( "family", font.family() );
  e.setAttribute( "size", font.pointSize() );
  e.setAttribute( "weight", font.weight() );
  if ( font.bold() )
    e.setAttribute( "bold", "yes" );
  if ( font.italic() )
    e.setAttribute( "italic", "yes" );

  return e;
}

// TODO
// QDomNodeList QDomDocument::elementsByTagName( const QString& tagname )

QDomNode::NodeType QDomDocument::nodeType() const
{
  return DocumentNode;
}

bool QDomDocument::isDocument() const
{
  return TRUE;
}

/*!
  Returns the factory attached to this document. If no
  factory was attached until now to this document, then the default
  factory as delivered by QMimeSourceFactory::defaultFactory is returned.

  \sa setMimeSourceFactory()
*/
QMimeSourceFactory* QDomDocument::mimeSourceFactory()
{
  if ( !impl )
    return 0;
  return IMPL->mimeSourceFactory();
}

/*!
  Returns the factory attached to this document. If no
  factory was attached until now to this document, then the default
  factory as delivered by QMimeSourceFactory::defaultFactory is returned.

  \sa setMimeSourceFactory()
*/
const QMimeSourceFactory* QDomDocument::mimeSourceFactory() const
{
  if ( !impl )
    return 0;
  return IMPL->mimeSourceFactory();
}

/*!
  Changes the factory for this document. If it is set to zero, then
  the default factory as delivered by QMimeSourceFactory::defaultFactory is
  used.

  The factory is used to access external data such as pictures which reside
  on some external storage but are referenced in the document.

  \sa mimeSourceFactory()
*/
void QDomDocument::setMimeSourceFactory( QMimeSourceFactory* f )
{
  if ( !impl )
    return;
  IMPL->setMimeSourceFactory( f );
}

#undef IMPL

/*==============================================================*/
/*               Node casting functions                         */
/*==============================================================*/

QDomAttr QDomNode::toAttr()
{
  if ( impl && impl->isAttr() )
  {
    impl->ref();
    return QDomAttr( ((QDOM_AttrPrivate*)impl) );
  }
  return QDomAttr();
}

QDomCDATASection QDomNode::toCDATASection()
{
  if ( impl && impl->isCDATASection() )
  {
    impl->ref();
    return QDomCDATASection( ((QDOM_CDATASectionPrivate*)impl) );
  }
  return QDomCDATASection();
}

QDomDocumentFragment QDomNode::toDocumentFragment()
{
  if ( impl && impl->isDocumentFragment() )
  {
    impl->ref();
    return QDomDocumentFragment( ((QDOM_DocumentFragmentPrivate*)impl) );
  }
  return QDomDocumentFragment();
}

QDomDocument QDomNode::toDocument()
{
  if ( impl && impl->isDocument() )
  {
    impl->ref();
    return QDomDocument( ((QDOM_DocumentPrivate*)impl) );
  }
  return QDomDocument();
}

QDomDocumentType QDomNode::toDocumentType()
{
  if ( impl && impl->isDocumentType() )
  {
    impl->ref();
    return QDomDocumentType( ((QDOM_DocumentTypePrivate*)impl) );
  }
  return QDomDocumentType();
}

QDomElement QDomNode::toElement()
{
  if ( impl && impl->isElement() )
  {
    impl->ref();
    return QDomElement( ((QDOM_ElementPrivate*)impl) );
  }
  return QDomElement();
}

QDomEntityReference QDomNode::toEntityReference()
{
  if ( impl && impl->isEntityReference() )
  {
    impl->ref();
    return QDomEntityReference( ((QDOM_EntityReferencePrivate*)impl) );
  }
  return QDomEntityReference();
}

QDomText QDomNode::toText()
{
  if ( impl && impl->isText() )
  {
    impl->ref();
    return QDomText( ((QDOM_TextPrivate*)impl) );
  }
  return QDomText();
}

QDomEntity QDomNode::toEntity()
{
  if ( impl && impl->isEntity() )
  {
    impl->ref();
    return QDomEntity( ((QDOM_EntityPrivate*)impl) );
  }
  return QDomEntity();
}

QDomNotation QDomNode::toNotation()
{
  if ( impl && impl->isNotation() )
  {
    impl->ref();
    return QDomNotation( ((QDOM_NotationPrivate*)impl) );
  }
  return QDomNotation();
}

QDomProcessingInstruction QDomNode::toProcessingInstruction()
{
  if ( impl && impl->isProcessingInstruction() )
  {
    impl->ref();
    return QDomProcessingInstruction( ((QDOM_ProcessingInstructionPrivate*)impl) );
  }
  return QDomProcessingInstruction();
}

QDomCharacterData QDomNode::toCharacterData()
{
  if ( impl && impl->isCharacterData() )
  {
    impl->ref();
    return QDomCharacterData( ((QDOM_CharacterDataPrivate*)impl) );
  }
  return QDomCharacterData();
}

/*==============================================================*/
/*                      QDomConsumer                            */
/*==============================================================*/

/**************************************************
 *
 * QDomConsumer
 *
 **************************************************/

QDomConsumer::QDomConsumer( QDOM_DocumentPrivate* d )
{
  doc = d;
  node = doc;
  firstTag = FALSE;
}

QDomConsumer::~QDomConsumer()
{
}

bool QDomConsumer::tagStart( const QString& name )
{
  if ( node == doc )
  {
    // Document may only have one child
    if ( firstTag )
      return FALSE;
    firstTag = TRUE;
    // Has to be a special tag
    if ( name != doc->doctype()->nodeName() )
      return FALSE;
  }
  printf("Start=%s\n",name.ascii() );

  QDOM_NodePrivate* n = doc->createElement( name );
  node->appendChild( n );
  node = n;

  return TRUE;
}

bool QDomConsumer::tagEnd( const QString& name )
{
  qDebug("End=%s\n",name.ascii());
  if ( node == doc )
    return FALSE;

  if ( node->nodeName() != name )
  {
    qDebug("Tag %s does not close %s\n",name.ascii(),node->nodeName().ascii() );
    return FALSE;
  }

  node = node->parentNode();

  return TRUE;
}

bool QDomConsumer::attrib( const QString& name, const QString& value )
{
  if ( !node->isElement() )
    return FALSE;

  ((QDOM_ElementPrivate*)node)->setAttribute( name, value );

  return TRUE;
}

bool QDomConsumer::text( const QString& text )
{
  // No text as child of some document
  if ( node == doc )
    return FALSE;

  node->appendChild( doc->createTextNode( text ) );

  return TRUE;
}

bool QDomConsumer::entityRef( const QString& name )
{
  qDebug("ENTITYREF=%s\n",name.ascii());

  if ( node == doc )
    return FALSE;

  // TODO: Find corresponding entity
  QDOM_NamedNodeMapPrivate* m = doc->doctype()->entities();
  if ( !m )
    return FALSE;
  QDOM_NodePrivate* n = m->namedItem( name );
  if ( !n || !n->isEntity() )
  {
    qWarning( "Entity of name %s unsupported", name.latin1() );
    return FALSE;
  }

  node->appendChild( doc->createEntityReference( name ) );

  return TRUE;
}

bool QDomConsumer::processingInstruction( const QString& name, const QString& value )
{
  node->appendChild( doc->createProcessingInstruction( name, value ) );

  return TRUE;
}

bool QDomConsumer::doctype( const QString& name )
{
  printf("DOCTYPE %s\n", name.ascii());
  doc->doctype()->m_name = name;

  return TRUE;
}

bool QDomConsumer::doctypeExtern( const QString& publicId, const QString& systemId )
{
  printf("DOCTYPE EXTERN %s %s\n", publicId.ascii(),systemId.ascii());
  return TRUE;
}

bool QDomConsumer::element( const QString& data )
{
  printf("ELEMENT %s\n", data.ascii());
  return TRUE;
}

bool QDomConsumer::attlist( const QString& data )
{
  printf("ATTLIST %s\n", data.ascii());
  return TRUE;
}

bool QDomConsumer::parameterEntity( const QString& name, const QString& publicId, const QString& systemId )
{
  printf("ENTITY PARAM %s %s %s\n", name.ascii(),publicId.ascii(),systemId.ascii());
  return TRUE;
}

bool QDomConsumer::parameterEntity( const QString& name, const QString& value )
{
  printf("ENTITY PARAM %s %s\n", name.ascii(),value.ascii());
  return TRUE;
}

bool QDomConsumer::entity( const QString& name, const QString& publicId, const QString& systemId, const QString& ndata )
{
  printf("ENTITY %s %s %s %s\n", name.ascii(),publicId.ascii(),systemId.ascii(),ndata.ascii());
  QDOM_EntityPrivate* e = new QDOM_EntityPrivate( doc, 0, name, publicId, systemId, ndata );
  doc->doctype()->appendChild( e );
  return TRUE;
}

bool QDomConsumer::entity( const QString& name, const QString& value )
{
  printf("ENTITY %s %s\n", name.ascii(),value.ascii());

  QDOM_EntityPrivate* e = new QDOM_EntityPrivate( doc, 0, name, QString::null, QString::null, QString::null );
  e->m_value = value;
  doc->doctype()->appendChild( e );

  return TRUE;
}

bool QDomConsumer::notation( const QString& name, const QString& publicId, const QString& systemId )
{
  printf("NOTATION %s %s %s\n", name.ascii(),publicId.ascii(),systemId.ascii());
  QDOM_NotationPrivate* n = new QDOM_NotationPrivate( doc, 0, name, publicId, systemId );
  doc->doctype()->appendChild( n );
  return TRUE;
}

void QDomConsumer::parseError( int )
{
  // TODO
}

bool QDomConsumer::finished()
{
  if ( node != doc )
    return FALSE;
  return TRUE;
}

/**************************************************
 *
 * Convenience functions
 *
 **************************************************/

/*
QDomElement* QDomrectToElement( QDomDocument* doc, const QRect& r, const QString& name )
{
  Element* e = doc->createElement( name );

  QString str;
  str.setNum( r.x() );
  e->setAttribute( "x", str );
  str.setNum( r.y() );
  e->setAttribute( "y", str );
  str.setNum( r.width() );
  e->setAttribute( "width", str );
  str.setNum( r.height() );
  e->setAttribute( "height", str );

  return e;
}

QRect QDomelementToRect( const QDomElement* e )
{
  return QRect( e->attribute( "x" ).toInt(), e->attribute( "y" ).toInt(),
		e->attribute( "width" ).toInt(), e->attribute( "height" ).toInt() );
}
*/

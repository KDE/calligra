#include "qdom.h"
#include "qxml.h"

#include <qtextstream.h>

#include <string.h>
#include <stdlib.h>

/**************************************************************
 *
 * QDOMConsumer
 *
 **************************************************************/

class QDOMConsumer : public QXMLConsumer
{
public:
  QDOMConsumer( QDOM_DocumentPrivate* d );
  ~QDOMConsumer();

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
 * QDOM::Namespace
 *
 **************************************************************/

/* QDOM::Namespace::Namespace()
{
}

QDOM::Namespace::~Namespace()
{
} */

/**************************************************************
 *
 * QDOM::Exception
 *
 **************************************************************/

/* QDOM::Exception::Exception()
{
}

QDOM::Exception::~Exception()
{
}

unsigned short QDOM::Exception::code()
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
 * QDOM::Implementation
 *
 **************************************************************/

QDOM::Implementation::Implementation()
{
  impl = 0;
}

QDOM::Implementation::Implementation( const Implementation& x )
{
  impl = x.impl;
  if ( impl ) impl->ref();
}

QDOM::Implementation::Implementation( QDOM_ImplementationPrivate* p )
{
  // We want to be co-owners, so increase the reference count
  impl = p;
}

QDOM::Implementation& QDOM::Implementation::operator= ( const QDOM::Implementation& x )
{
  if ( impl && impl->deref() ) delete impl;
  impl = x.impl;
  if ( impl ) impl->ref();

  return *this;
}

bool QDOM::Implementation::operator==( const QDOM::Implementation& x ) const
{
  return ( impl == x.impl );
}

bool QDOM::Implementation::operator!=( const QDOM::Implementation& x ) const
{
  return ( impl != x.impl );
}

QDOM::Implementation::~Implementation()
{
  if ( impl && impl->deref() ) delete impl;
}

bool QDOM::Implementation::hasFeature( const QString& feature, const QString& version )
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
 * QDOM::NodeList
 *
 **************************************************************/

QDOM::NodeList::NodeList()
{
  impl = 0;
}

QDOM::NodeList::NodeList( QDOM_NodeListPrivate* p )
{
  impl = p;
  if ( impl ) impl->ref();
}

QDOM::NodeList::NodeList( const QDOM::NodeList& n )
{
  impl = n.impl;
  if ( impl )
    impl->ref();
}

QDOM::NodeList& QDOM::NodeList::operator= ( const QDOM::NodeList& n )
{
  if ( impl && impl->deref() ) delete impl;
  impl = n.impl;
  if ( impl )
    impl->ref();

  return *this;
}

bool QDOM::NodeList::operator== ( const QDOM::NodeList& n ) const
{
  return ( impl == n.impl );
}

bool QDOM::NodeList::operator!= ( const QDOM::NodeList& n ) const
{
  return ( impl != n.impl );
}

QDOM::NodeList::~NodeList()
{
  if ( impl && impl->deref() ) delete impl;
}

QDOM::Node QDOM::NodeList::item( int index ) const
{
  if ( !impl )
    return QDOM::Node();

  return QDOM::Node( impl->item( index ) );
}

uint QDOM::NodeList::length() const
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
 * QDOM::Node
 *
 **************************************************************/

#define IMPL ((QDOM_NodePrivate*)impl)
  
QDOM::Node::Node()
{
  impl = 0;
}

QDOM::Node::Node( const Node& n )
{
  impl = n.impl;
  if ( impl ) impl->ref();
}

QDOM::Node::Node( QDOM_NodePrivate* n )
{
  impl = n;
  if ( impl ) impl->ref();
}

QDOM::Node& QDOM::Node::operator= ( const QDOM::Node& n )
{
  if ( impl && impl->deref() ) delete impl;
  impl = n.impl;
  if ( impl ) impl->ref();

  return *this;
}

bool QDOM::Node::operator== ( const QDOM::Node& n ) const
{
  return ( impl == n.impl );
}

bool QDOM::Node::operator!= ( const QDOM::Node& n ) const
{
  return ( impl != n.impl );
}

QDOM::Node::~Node()
{
  if ( impl && impl->deref() ) delete impl;
}

QString QDOM::Node::nodeName() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_name;
}

QString QDOM::Node::nodeValue() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_value;
}

void QDOM::Node::setNodeValue( const QString& v )
{
  if ( !impl )
    return;
  IMPL->setNodeValue( v );
}

QDOM::Node::NodeType QDOM::Node::nodeType() const
{
  return QDOM::Node::BaseNode;
}

QDOM::Node QDOM::Node::parentNode() const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->parentNode() );
}

QDOM::NodeList QDOM::Node::childNodes() const
{
  if ( !impl )
    return QDOM::NodeList();
  return QDOM::NodeList( impl );
}

QDOM::Node QDOM::Node::firstChild() const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->firstChild() );
}

QDOM::Node QDOM::Node::lastChild() const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->lastChild() );
}

QDOM::Node QDOM::Node::previousSibling() const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->previousSibling() );
}

QDOM::Node QDOM::Node::nextSibling() const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->nextSibling() );
}

QDOM::NamedNodeMap QDOM::Node::attributes() const
{
  if ( !impl )
    return QDOM::NamedNodeMap();

  return QDOM::NamedNodeMap( impl->attributes() );
}

QDOM::Document QDOM::Node::ownerDocument() const
{
  if ( !impl )
    return QDOM::Document();
  return QDOM::Document( IMPL->ownerDocument() );
}

QDOM::Node QDOM::Node::cloneNode( bool deep ) const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->cloneNode( deep ) );
}

QDOM::Node QDOM::Node::insertBefore( const QDOM::Node& newChild, const QDOM::Node& refChild )
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->insertBefore( newChild.impl, refChild.impl ) );
}

QDOM::Node QDOM::Node::insertAfter( const QDOM::Node& newChild, const QDOM::Node& refChild )
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->insertAfter( newChild.impl, refChild.impl ) );
}

QDOM::Node QDOM::Node::replaceChild( const QDOM::Node& newChild, const QDOM::Node& oldChild )
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->replaceChild( newChild.impl, oldChild.impl ) );
}

QDOM::Node QDOM::Node::removeChild( const QDOM::Node& oldChild )
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->removeChild( oldChild.impl ) );
}

QDOM::Node QDOM::Node::appendChild( const QDOM::Node& newChild )
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->appendChild( newChild.impl ) );
}

bool QDOM::Node::isNull() const
{
  return ( impl == 0 );
}

void QDOM::Node::clear()
{
  if ( impl && impl->deref() ) delete impl;
  impl = 0;
}

QDOM::Node QDOM::Node::namedItem( const QString& name ) const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( impl->namedItem( name ) );
}

void QDOM::Node::save( QTextStream& str ) const
{
  if ( impl )
    IMPL->save( str );
}

QTextStream& operator<<( QTextStream& str, const QDOM::Node& node )
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
 * QDOM::NamedNodeMap
 *
 **************************************************************/

#define IMPL ((QDOM_NamedNodeMapPrivate*)impl)

QDOM::NamedNodeMap::NamedNodeMap()
{
  impl = 0;
}

QDOM::NamedNodeMap::NamedNodeMap( const QDOM::NamedNodeMap& n )
{
  impl = n.impl;
  if ( impl ) impl->ref();
}

QDOM::NamedNodeMap::NamedNodeMap( QDOM_NamedNodeMapPrivate* n )
{
  impl = n;
  if ( impl ) impl->ref();
}

QDOM::NamedNodeMap& QDOM::NamedNodeMap::operator= ( const QDOM::NamedNodeMap& n )
{
  if ( impl && impl->deref() ) delete impl;
  impl = n.impl;
  if ( impl ) impl->ref();

  return *this;
}

bool QDOM::NamedNodeMap::operator== ( const QDOM::NamedNodeMap& n ) const
{
  return ( impl == n.impl );
}

bool QDOM::NamedNodeMap::operator!= ( const QDOM::NamedNodeMap& n ) const
{
  return ( impl != n.impl );
}

QDOM::NamedNodeMap::~NamedNodeMap()
{
  if ( impl && impl->deref() ) delete impl;
}

QDOM::Node QDOM::NamedNodeMap::namedItem( const QString& name ) const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->namedItem( name ) );
}

QDOM::Node QDOM::NamedNodeMap::setNamedItem( const Node& arg )
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->setNamedItem( (QDOM_NodePrivate*)arg.impl ) );
}

QDOM::Node QDOM::NamedNodeMap::removeNamedItem( const QString& name )
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->removeNamedItem( name ) );
}

QDOM::Node QDOM::NamedNodeMap::item( int index ) const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( IMPL->item( index ) );
}

uint QDOM::NamedNodeMap::length() const
{
  if ( !impl )
    return 0;
  return IMPL->length();
}

bool QDOM::NamedNodeMap::contains( const QString& name ) const
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
 * QDOM::DocumentType
 *
 **************************************************************/

#define IMPL ((QDOM_DocumentTypePrivate*)impl)
  
QDOM::DocumentType::DocumentType() : QDOM::Node()
{
}

QDOM::DocumentType::DocumentType( const DocumentType& n )
  : QDOM::Node( n )
{
}

QDOM::DocumentType::DocumentType( QDOM_DocumentTypePrivate* n )
  : QDOM::Node( n )
{
}

QDOM::DocumentType& QDOM::DocumentType::operator= ( const QDOM::DocumentType& n )
{
  return (QDOM::DocumentType&) QDOM::Node::operator=( n );
}

QDOM::DocumentType::~DocumentType()
{
}

QString QDOM::DocumentType::name() const
{
  if ( !impl )
    return QString::null;

  return IMPL->nodeName();
}

QDOM::NamedNodeMap QDOM::DocumentType::entities() const
{
  if ( !impl )
    return QDOM::NamedNodeMap();
  return QDOM::NamedNodeMap( IMPL->entities() );
}

QDOM::NamedNodeMap QDOM::DocumentType::notations() const
{
  if ( !impl )
    return QDOM::NamedNodeMap();
  return QDOM::NamedNodeMap( IMPL->notations() );
}

QDOM::Node::NodeType QDOM::DocumentType::nodeType() const
{
  return DocumentTypeNode;
}

bool QDOM::DocumentType::isDocumentType() const
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
 * QDOM::DocumentFragment
 *
 **************************************************************/

#define IMPL ((QDOM_DocumentFragmentPrivate*)impl)

QDOM::DocumentFragment::DocumentFragment()
{
}

QDOM::DocumentFragment::DocumentFragment( QDOM_DocumentFragmentPrivate* n )
  : QDOM::Node( n )
{
}

QDOM::DocumentFragment::DocumentFragment( const QDOM::DocumentFragment& x )
  : QDOM::Node( x )
{
}

QDOM::DocumentFragment& QDOM::DocumentFragment::operator= ( const QDOM::DocumentFragment& x )
{
  return (QDOM::DocumentFragment&) QDOM::Node::operator=( x );
}

QDOM::DocumentFragment::~DocumentFragment()
{
}

QDOM::Node::NodeType QDOM::DocumentFragment::nodeType() const
{
  return QDOM::Node::DocumentFragmentNode;
}

/* QDOM::Node QDOM::DocumentFragment::cloneNode( bool deep) const
{
  return QDOM::Node( new QDOM_DocumentFragmentPrivate( IMPL, deep ) );
  } */

bool QDOM::DocumentFragment::isDocumentFragment() const
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
 * QDOM::CharacterData
 *
 **************************************************************/

#define IMPL ((QDOM_CharacterDataPrivate*)impl)

QDOM::CharacterData::CharacterData()
{
}

QDOM::CharacterData::CharacterData( const QDOM::CharacterData& x )
  : QDOM::Node( x )
{
}

QDOM::CharacterData::CharacterData( QDOM_CharacterDataPrivate* n )
  : QDOM::Node( n )
{
}

QDOM::CharacterData& QDOM::CharacterData::operator= ( const QDOM::CharacterData& x )
{
  return (QDOM::CharacterData&) QDOM::Node::operator=( x );
}

QDOM::CharacterData::~CharacterData()
{
}

QString QDOM::CharacterData::data() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeValue();
}

void QDOM::CharacterData::setData( const QString& v )
{
  if ( impl )
    impl->setNodeValue( v );
}

uint QDOM::CharacterData::length() const
{
  if ( impl )
    return IMPL->length();
  return 0;
}

QString QDOM::CharacterData::substringData( unsigned long offset, unsigned long count )
{
  if ( !impl )
    return QString::null;
  return IMPL->substringData( offset, count );
}

void QDOM::CharacterData::appendData( const QString& arg )
{
  if ( impl )
    IMPL->appendData( arg );
}

void QDOM::CharacterData::insertData( unsigned long offset, const QString& arg )
{
  if ( impl )
    IMPL->insertData( offset, arg );
}

void QDOM::CharacterData::deleteData( unsigned long offset, unsigned long count )
{
  if ( impl )
    IMPL->deleteData( offset, count );
}

void QDOM::CharacterData::replaceData( unsigned long offset, unsigned long count, const QString& arg )
{
  if ( impl )
    IMPL->replaceData( offset, count, arg );
}

QDOM::Node::NodeType QDOM::CharacterData::nodeType() const
{
  return CharacterDataNode;
}

bool QDOM::CharacterData::isCharacterData() const
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
 * QDOM::Attr
 *
 **************************************************************/

#define IMPL ((QDOM_AttrPrivate*)impl)

QDOM::Attr::Attr()
{
}

QDOM::Attr::Attr( const QDOM::Attr& x )
  : QDOM::Node( x )
{
}

QDOM::Attr::Attr( QDOM_AttrPrivate* n )
  : QDOM::Node( n )
{
}

QDOM::Attr& QDOM::Attr::operator= ( const QDOM::Attr& x )
{
  return (QDOM::Attr&) QDOM::Node::operator=( x );
}

QDOM::Attr::~Attr()
{
}

QString QDOM::Attr::name() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeName();
}

bool QDOM::Attr::specified() const
{
  if ( !impl )
    return false;
  return IMPL->specified();
}

QString QDOM::Attr::value() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeValue();
}

void QDOM::Attr::setValue( const QString& v )
{
  if ( !impl )
    return;
  impl->setNodeValue( v );
}

/* QDOM::Node QDOM::Attr::cloneNode( bool deep) const
{
  if ( !impl )
    return QDOM::Node();
  return QDOM::Node( new QDOM_AttrPrivate( IMPL, deep ) );
  } */

QDOM::Node::NodeType QDOM::Attr::nodeType() const
{
  return AttributeNode;
}

bool QDOM::Attr::isAttr() const
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
  if ( !n )
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
QDOM::NodeList* QDOM_ElementPrivate::elementsByTagName( const QString& name )
{
  NodeList* l = new NodeList( FALSE );

  qElementsByTagName( this, name, l );
   
  return l;
}
*/
/* const QDOM::Node* QDOM_ElementPrivate::childByTagName( const QString& name ) const
{
  const QDOM::NodeList* l = childNodes();
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
 * QDOM::Element
 *
 **************************************************************/

#define IMPL ((QDOM_ElementPrivate*)impl)

QDOM::Element::Element()
  : QDOM::Node()
{
}

QDOM::Element::Element( const QDOM::Element& x )
  : QDOM::Node( x )
{
}

QDOM::Element::Element( QDOM_ElementPrivate* n )
  : QDOM::Node( n )
{
}

QDOM::Element& QDOM::Element::operator= ( const QDOM::Element& x )
{
  return (QDOM::Element&) QDOM::Node::operator=( x );
}

QDOM::Element::~Element()
{
}

/* QDOM::Node QDOM::Element::cloneNode( bool deep) const
{
  return QDOM::Node( new QDOM_ElementPrivate( this, deep ) );
  } */

QDOM::Node::NodeType QDOM::Element::nodeType() const
{
  return ElementNode;
}

QString QDOM::Element::tagName() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeName();
}

QString QDOM::Element::attribute( const QString& name ) const
{
  if ( !impl )
    return QString::null;
  return IMPL->attribute( name );
}

void QDOM::Element::setAttribute( const QString& name, const QString& value )
{
  if ( !impl )
    return;
  IMPL->setAttribute( name, value );
}

void QDOM::Element::setAttribute( const QString& name, int value )
{
  if ( !impl )
    return;
  QString x;
  x.setNum( value );
  IMPL->setAttribute( name, x );
}

void QDOM::Element::setAttribute( const QString& name, double value )
{
  if ( !impl )
    return;
  QString x;
  x.setNum( value );
  IMPL->setAttribute( name, x );
}

void QDOM::Element::removeAttribute( const QString& name )
{
  if ( !impl )
    return;
  IMPL->removeAttribute( name );
}

QDOM::Attr QDOM::Element::attributeNode( const QString& name)
{
  if ( !impl )
    return QDOM::Attr();
  return QDOM::Attr( IMPL->attributeNode( name ) );
}

QDOM::Attr QDOM::Element::setAttributeNode( const Attr& newAttr )
{
  if ( !impl )
    return QDOM::Attr();
  return QDOM::Attr( IMPL->setAttributeNode( ((QDOM_AttrPrivate*)newAttr.impl) ) );
}

QDOM::Attr QDOM::Element::removeAttributeNode( const Attr& oldAttr )
{
  if ( !impl )
    return QDOM::Attr();
  return QDOM::Attr( IMPL->removeAttributeNode( ((QDOM_AttrPrivate*)oldAttr.impl) ) );
}

// TODO
//  NodeList QDOM::Element::elementsByTagName( const QString& name );

void QDOM::Element::normalize()
{
  if ( !impl )
    return;
  IMPL->normalize();
}

bool QDOM::Element::isElement() const
{
  return TRUE;
}

QDOM::NamedNodeMap QDOM::Element::attributes() const
{
  if ( !impl )
    return QDOM::NamedNodeMap();
  return QDOM::NamedNodeMap( IMPL->attributes() );
}

bool QDOM::Element::hasAttribute( const QString& name ) const
{
  if ( !impl )
    return FALSE;
  return IMPL->hasAttribute( name );
}

QString QDOM::Element::text() const
{
  if ( !impl )
    return QString::null;
  return IMPL->text();
}

QRect QDOM::Element::toRect() const
{
  if ( !impl )
    return QRect();
  return QRect( attribute( "x" ).toInt(), attribute( "y" ).toInt(),
		attribute( "width" ).toInt(), attribute( "height" ).toInt() );
}

QPen QDOM::Element::toPen() const
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

QFont QDOM::Element::toFont() const
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
    qWarning( "QDOM::Text::splitText  The node has no parent. So I can not split" );
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
 * QDOM::Text
 *
 **************************************************************/

#define IMPL ((QDOM_TextPrivate*)impl)

QDOM::Text::Text()
  : QDOM::CharacterData()
{
}

QDOM::Text::Text( const QDOM::Text& x )
  : QDOM::CharacterData( x )
{
}

QDOM::Text::Text( QDOM_TextPrivate* n )
  : QDOM::CharacterData( n )
{
}

QDOM::Text& QDOM::Text::operator= ( const QDOM::Text& x )
{
  return (QDOM::Text&) QDOM::Node::operator=( x );
}

QDOM::Text::~Text()
{
}

/* QDOM::Node QDOM::Text::cloneNode( bool deep) const
{
  return QDOM::Node( new QDOM_TextPrivate( this, deep ) );
  } */

QDOM::Node::NodeType QDOM::Text::nodeType() const
{
  return TextNode;
}

QDOM::Text QDOM::Text::splitText( int offset )
{
  if ( !impl )
    return QDOM::Text();
  return QDOM::Text( IMPL->splitText( offset ) );
}

bool QDOM::Text::isText() const
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
 * QDOM::Comment
 *
 **************************************************************/

#define IMPL ((QDOM_CommentPrivate*)impl)

QDOM::Comment::Comment()
  : QDOM::CharacterData()
{
}

QDOM::Comment::Comment( const QDOM::Comment& x )
  : QDOM::CharacterData( x )
{
}

QDOM::Comment::Comment( QDOM_CommentPrivate* n )
  : QDOM::CharacterData( n )
{
}

QDOM::Comment& QDOM::Comment::operator= ( const QDOM::Comment& x )
{
  return (QDOM::Comment&) QDOM::Node::operator=( x );
}

QDOM::Comment::~Comment()
{
}

/* QDOM::Node QDOM::Comment::cloneNode( bool deep) const
{
  return QDOM::Node( new QDOM_CommentPrivate( this, deep ) );
  } */

QDOM::Node::NodeType QDOM::Comment::nodeType() const
{
  return CommentNode;
}

bool QDOM::Comment::isComment() const
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
 * QDOM::CDATASection
 *
 **************************************************************/

#define IMPL ((QDOM_CDATASectionPrivate*)impl)

QDOM::CDATASection::CDATASection()
  : QDOM::Text()
{
}

QDOM::CDATASection::CDATASection( const QDOM::CDATASection& x )
  : QDOM::Text( x )
{
}

QDOM::CDATASection::CDATASection( QDOM_CDATASectionPrivate* n )
  : QDOM::Text( n )
{
}

QDOM::CDATASection& QDOM::CDATASection::operator= ( const QDOM::CDATASection& x )
{
  return (QDOM::CDATASection&) QDOM::Node::operator=( x );
}

QDOM::CDATASection::~CDATASection()
{
}

/* QDOM::Node QDOM::CDATASection::cloneNode( bool deep) const
{
  return QDOM::Node( new QDOM_CDATASectionPrivate( this, deep ) );
  } */

QDOM::Node::NodeType QDOM::CDATASection::nodeType() const
{
  return CDATASectionNode;
}

bool QDOM::CDATASection::isCDATASection() const
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
 * QDOM::Notation
 *
 **************************************************************/

#define IMPL ((QDOM_NotationPrivate*)impl)

QDOM::Notation::Notation()
  : QDOM::Node()
{
}

QDOM::Notation::Notation( const QDOM::Notation& x )
  : QDOM::Node( x )
{
}

QDOM::Notation::Notation( QDOM_NotationPrivate* n )
  : QDOM::Node( n )
{
}

QDOM::Notation& QDOM::Notation::operator= ( const QDOM::Notation& x )
{
  return (QDOM::Notation&) QDOM::Node::operator=( x );
}

QDOM::Notation::~Notation()
{
}

/* QDOM::Node QDOM::Notation::cloneNode( bool deep) const
{
  return QDOM::Node( new QDOM_NotationPrivate( this, deep ) );
  } */

QDOM::Node::NodeType QDOM::Notation::nodeType() const
{
  return NotationNode;
}

QString QDOM::Notation::publicId() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_pub;
}

QString QDOM::Notation::systemId() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_sys;
}

bool QDOM::Notation::isNotation() const
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
 * QDOM::Entity
 *
 **************************************************************/

#define IMPL ((QDOM_EntityPrivate*)impl)

QDOM::Entity::Entity()
  : QDOM::Node()
{
}

QDOM::Entity::Entity( const QDOM::Entity& x )
  : QDOM::Node( x )
{
}

QDOM::Entity::Entity( QDOM_EntityPrivate* n )
  : QDOM::Node( n )
{
}

QDOM::Entity& QDOM::Entity::operator= ( const QDOM::Entity& x )
{
  return (QDOM::Entity&) QDOM::Node::operator=( x );
}

QDOM::Entity::~Entity()
{
}

/* QDOM::Node QDOM::Entity::cloneNode( bool deep) const
{
  return QDOM::Node( new QDOM_EntityPrivate( this, deep ) );
  } */

QDOM::Node::NodeType QDOM::Entity::nodeType() const
{
  return EntityNode;
}

QString QDOM::Entity::publicId() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_pub;
}

QString QDOM::Entity::systemId() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_sys;
}

QString QDOM::Entity::notationName() const
{
  if ( !impl )
    return QString::null;
  return IMPL->m_notationName;
}

bool QDOM::Entity::isEntity() const
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
 * QDOM::EntityReference
 *
 **************************************************************/

#define IMPL ((QDOM_EntityReferencePrivate*)impl)

QDOM::EntityReference::EntityReference()
  : QDOM::Node()
{
}

QDOM::EntityReference::EntityReference( const QDOM::EntityReference& x )
  : QDOM::Node( x )
{
}

QDOM::EntityReference::EntityReference( QDOM_EntityReferencePrivate* n )
  : QDOM::Node( n )
{
}

QDOM::EntityReference& QDOM::EntityReference::operator= ( const QDOM::EntityReference& x )
{
  return (QDOM::EntityReference&) QDOM::Node::operator=( x );
}

QDOM::EntityReference::~EntityReference()
{
}

/* QDOM::Node QDOM::EntityReference::cloneNode( bool deep) const
{
  return QDOM::Node( new QDOM_EntityReferencePrivate( this, deep ) );
  } */

QDOM::Node::NodeType QDOM::EntityReference::nodeType() const
{
  return EntityReferenceNode;
}

bool QDOM::EntityReference::isEntityReference() const
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
 * QDOM::ProcessingInstruction
 *
 **************************************************************/

#define IMPL ((QDOM_ProcessingInstructionPrivate*)impl)

QDOM::ProcessingInstruction::ProcessingInstruction()
  : QDOM::Node()
{
}

QDOM::ProcessingInstruction::ProcessingInstruction( const QDOM::ProcessingInstruction& x )
  : QDOM::Node( x )
{
}

QDOM::ProcessingInstruction::ProcessingInstruction( QDOM_ProcessingInstructionPrivate* n )
  : QDOM::Node( n )
{
}

QDOM::ProcessingInstruction& QDOM::ProcessingInstruction::operator= ( const QDOM::ProcessingInstruction& x )
{
  return (QDOM::ProcessingInstruction&) QDOM::Node::operator=( x );
}

QDOM::ProcessingInstruction::~ProcessingInstruction()
{
}

QDOM::Node::NodeType QDOM::ProcessingInstruction::nodeType() const
{
  return ProcessingInstructionNode;
}

QString QDOM::ProcessingInstruction::target() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeName();
}

QString QDOM::ProcessingInstruction::data() const
{
  if ( !impl )
    return QString::null;
  return impl->nodeValue();
}

void QDOM::ProcessingInstruction::setData( const QString& d )
{
  if ( !impl )
    return;
  impl->setNodeValue( d );
}

bool QDOM::ProcessingInstruction::isProcessingInstruction() const
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

  static QString* s_docName;
};

QString* QDOM_DocumentPrivate::s_docName = 0;

QDOM_DocumentPrivate::QDOM_DocumentPrivate()
  : QDOM_NodePrivate( 0 )
{
  impl = new QDOM_ImplementationPrivate();
  type = new QDOM_DocumentTypePrivate( this, this );

  if ( !s_docName )
    s_docName = new QString( "#document" );
  m_name = *s_docName;
}

QDOM_DocumentPrivate::QDOM_DocumentPrivate( const QString& name )
  : QDOM_NodePrivate( 0 )
{
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

bool QDOM_DocumentPrivate::setContent( const QString& text )
{
  clear();
  impl = new QDOM_ImplementationPrivate;
  type = new QDOM_DocumentTypePrivate( this, this );

  QXMLSimpleParser p;
  QDOMConsumer c( this );
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
static void qElementsByTagName( const QDOM::Node* node, const QString& tagname, QDOM::NodeList* l )
{
  const QDOM::NodeList* m = node->childNodes();
  uint len = m->length();
  for( uint i = 0; i < len; ++i )
  {
    const QDOM::Node* n = m->item( i );
    if ( n->nodeName() == tagname )
      l->appendChild( (QDOM::Node*)n );

    qElementsByTagName( n, tagname, l );
  }
}

QDOM::NodeList* QDOM_DocumentPrivate::elementsByTagName( const QString& tagname )
{
  NodeList* l = new NodeList( FALSE );

  qElementsByTagName( this, tagname, l );
   
  return l;
}
*/

/**************************************************************
 *
 * QDOM::Document
 *
 **************************************************************/

#define IMPL ((QDOM_DocumentPrivate*)impl)

QDOM::Document::Document()
{
}

QDOM::Document::Document( const QString& name )
{
  // We take over ownership
  impl = new QDOM_DocumentPrivate( name );
}

QDOM::Document::Document( const QDOM::Document& x )
  : QDOM::Node( x )
{
}

QDOM::Document::Document( QDOM_DocumentPrivate* x )
  : QDOM::Node( x )
{
}

QDOM::Document& QDOM::Document::operator= ( const QDOM::Document& x )
{
  return (QDOM::Document&) QDOM::Node::operator=( x );
}

QDOM::Document::~Document()
{
}

bool QDOM::Document::setContent( const QString& text )
{
  if ( !impl )
    impl = new QDOM_DocumentPrivate;
  return IMPL->setContent( text );
}

QDOM::DocumentType QDOM::Document::doctype() const
{
  if ( !impl )
    return QDOM::DocumentType();
  return QDOM::DocumentType( IMPL->doctype() );
}

QDOM::Implementation QDOM::Document::implementation() const
{
  if ( !impl )
    return QDOM::Implementation();
  return QDOM::Implementation( IMPL->implementation() );
}

QDOM::Element QDOM::Document::documentElement() const
{
  if ( !impl )
    return QDOM::Element();
  return QDOM::Element( IMPL->documentElement() );
}

QDOM::Element QDOM::Document::createElement( const QString& tagName )
{
  if ( !impl )
    return QDOM::Element();
  return QDOM::Element( IMPL->createElement( tagName ) );
}

QDOM::DocumentFragment QDOM::Document::createDocumentFragment()
{
  if ( !impl )
    return QDOM::DocumentFragment();
  return QDOM::DocumentFragment( IMPL->createDocumentFragment() );
}

QDOM::Text QDOM::Document::createTextNode( const QString& value )
{
  if ( !impl )
    return QDOM::Text();
  return QDOM::Text( IMPL->createTextNode( value ) );
}

QDOM::Comment QDOM::Document::createComment( const QString& value )
{
  if ( !impl )
    return QDOM::Comment();
  return QDOM::Comment( IMPL->createComment( value ) );
}

QDOM::CDATASection QDOM::Document::createCDATASection( const QString& value )
{
  if ( !impl )
    return QDOM::CDATASection();
  return QDOM::CDATASection( IMPL->createCDATASection( value ) );
}

QDOM::ProcessingInstruction QDOM::Document::createProcessingInstruction( const QString& target,
									 const QString& data )
{
  if ( !impl )
    return QDOM::ProcessingInstruction();
  return QDOM::ProcessingInstruction( IMPL->createProcessingInstruction( target, data ) );
}

QDOM::Attr QDOM::Document::createAttribute( const QString& name )
{
  if ( !impl )
    return QDOM::Attr();
  return QDOM::Attr( IMPL->createAttribute( name ) );
}

QDOM::EntityReference QDOM::Document::createEntityReference( const QString& name )
{
  if ( !impl )
    return QDOM::EntityReference();
  return QDOM::EntityReference( IMPL->createEntityReference( name ) );
}

QDOM::Element QDOM::Document::createElement( const QString& tagname, const QRect& rect )
{
  if ( !impl )
    return QDOM::Element();
  QDOM::Element e( createElement( tagname ) );

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

QDOM::Element QDOM::Document::createElement( const QString& tagname, const QPen& pen )
{
  if ( !impl )
    return QDOM::Element();
  QDOM::Element e( createElement( tagname ) );

  e.setAttribute( "color", pen.color().name() );
  e.setAttribute( "style", (int)pen.style() );
  e.setAttribute( "width", (int)pen.width() );

  return e;
}

QDOM::Element QDOM::Document::createElement( const QString& tagname, const QFont& font )
{
  if ( !impl )
    return QDOM::Element();
  QDOM::Element e( createElement( tagname ) );

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
// QDOM::NodeList QDOM::Document::elementsByTagName( const QString& tagname )

QDOM::Node::NodeType QDOM::Document::nodeType() const
{
  return DocumentNode;
}

bool QDOM::Document::isDocument() const
{
  return TRUE;
}

#undef IMPL

/*==============================================================*/
/*               Node casting functions                         */
/*==============================================================*/

QDOM::Attr QDOM::Node::toAttr()
{
  if ( impl && impl->isAttr() )
  {
    impl->ref();
    return QDOM::Attr( ((QDOM_AttrPrivate*)impl) );
  }
  return QDOM::Attr();
}

QDOM::CDATASection QDOM::Node::toCDATASection()
{
  if ( impl && impl->isCDATASection() )
  {
    impl->ref();
    return QDOM::CDATASection( ((QDOM_CDATASectionPrivate*)impl) );
  }
  return QDOM::CDATASection();
}

QDOM::DocumentFragment QDOM::Node::toDocumentFragment()
{
  if ( impl && impl->isDocumentFragment() )
  {
    impl->ref();
    return QDOM::DocumentFragment( ((QDOM_DocumentFragmentPrivate*)impl) );
  }
  return QDOM::DocumentFragment();
}

QDOM::Document QDOM::Node::toDocument()
{
  if ( impl && impl->isDocument() )
  {
    impl->ref();
    return QDOM::Document( ((QDOM_DocumentPrivate*)impl) );
  }
  return QDOM::Document();
}

QDOM::DocumentType QDOM::Node::toDocumentType()
{
  if ( impl && impl->isDocumentType() )
  {
    impl->ref();
    return QDOM::DocumentType( ((QDOM_DocumentTypePrivate*)impl) );
  }
  return QDOM::DocumentType();
}

QDOM::Element QDOM::Node::toElement()
{
  if ( impl && impl->isElement() )
  {
    impl->ref();
    return QDOM::Element( ((QDOM_ElementPrivate*)impl) );
  }
  return QDOM::Element();
}

QDOM::EntityReference QDOM::Node::toEntityReference()
{
  if ( impl && impl->isEntityReference() )
  {
    impl->ref();
    return QDOM::EntityReference( ((QDOM_EntityReferencePrivate*)impl) );
  }
  return QDOM::EntityReference();
}

QDOM::Text QDOM::Node::toText()
{
  if ( impl && impl->isText() )
  {
    impl->ref();
    return QDOM::Text( ((QDOM_TextPrivate*)impl) );
  }
  return QDOM::Text();
}

QDOM::Entity QDOM::Node::toEntity()
{
  if ( impl && impl->isEntity() )
  {
    impl->ref();
    return QDOM::Entity( ((QDOM_EntityPrivate*)impl) );
  }
  return QDOM::Entity();
}

QDOM::Notation QDOM::Node::toNotation()
{
  if ( impl && impl->isNotation() )
  {
    impl->ref();
    return QDOM::Notation( ((QDOM_NotationPrivate*)impl) );
  }
  return QDOM::Notation();
}

QDOM::ProcessingInstruction QDOM::Node::toProcessingInstruction()
{
  if ( impl && impl->isProcessingInstruction() )
  {
    impl->ref();
    return QDOM::ProcessingInstruction( ((QDOM_ProcessingInstructionPrivate*)impl) );
  }
  return QDOM::ProcessingInstruction();
}

QDOM::CharacterData QDOM::Node::toCharacterData()
{
  if ( impl && impl->isCharacterData() )
  {
    impl->ref();
    return QDOM::CharacterData( ((QDOM_CharacterDataPrivate*)impl) );
  }
  return QDOM::CharacterData();
}

/*==============================================================*/
/*                      QDOMConsumer                            */
/*==============================================================*/

/**************************************************
 *
 * QDOMConsumer
 *
 **************************************************/

QDOMConsumer::QDOMConsumer( QDOM_DocumentPrivate* d )
{
  doc = d;
  node = doc;
  firstTag = FALSE;
}

QDOMConsumer::~QDOMConsumer()
{
}

bool QDOMConsumer::tagStart( const QString& name )
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

bool QDOMConsumer::tagEnd( const QString& name )
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

bool QDOMConsumer::attrib( const QString& name, const QString& value )
{
  if ( !node->isElement() )
    return FALSE;

  ((QDOM_ElementPrivate*)node)->setAttribute( name, value );

  return TRUE;
}

bool QDOMConsumer::text( const QString& text )
{
  // No text as child of some document
  if ( node == doc )
    return FALSE;

  node->appendChild( doc->createTextNode( text ) );

  return TRUE;
}

bool QDOMConsumer::entityRef( const QString& name )
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

bool QDOMConsumer::processingInstruction( const QString& name, const QString& value )
{
  node->appendChild( doc->createProcessingInstruction( name, value ) );

  return TRUE;
}

bool QDOMConsumer::doctype( const QString& name )
{
  printf("DOCTYPE %s\n", name.ascii());
  doc->doctype()->m_name = name;

  return TRUE;
}

bool QDOMConsumer::doctypeExtern( const QString& publicId, const QString& systemId )
{
  printf("DOCTYPE EXTERN %s %s\n", publicId.ascii(),systemId.ascii());
  return TRUE;
}

bool QDOMConsumer::element( const QString& data )
{
  printf("ELEMENT %s\n", data.ascii());
  return TRUE;
}

bool QDOMConsumer::attlist( const QString& data )
{
  printf("ATTLIST %s\n", data.ascii());
  return TRUE;
}

bool QDOMConsumer::parameterEntity( const QString& name, const QString& publicId, const QString& systemId )
{
  printf("ENTITY PARAM %s %s %s\n", name.ascii(),publicId.ascii(),systemId.ascii());
  return TRUE;
}

bool QDOMConsumer::parameterEntity( const QString& name, const QString& value )
{
  printf("ENTITY PARAM %s %s\n", name.ascii(),value.ascii());
  return TRUE;
}

bool QDOMConsumer::entity( const QString& name, const QString& publicId, const QString& systemId, const QString& ndata )
{
  printf("ENTITY %s %s %s %s\n", name.ascii(),publicId.ascii(),systemId.ascii(),ndata.ascii());
  QDOM_EntityPrivate* e = new QDOM_EntityPrivate( doc, 0, name, publicId, systemId, ndata );
  doc->doctype()->appendChild( e );
  return TRUE;
}

bool QDOMConsumer::entity( const QString& name, const QString& value )
{
  printf("ENTITY %s %s\n", name.ascii(),value.ascii());

  QDOM_EntityPrivate* e = new QDOM_EntityPrivate( doc, 0, name, QString::null, QString::null, QString::null );
  e->m_value = value;
  doc->doctype()->appendChild( e );

  return TRUE;
}

bool QDOMConsumer::notation( const QString& name, const QString& publicId, const QString& systemId )
{
  printf("NOTATION %s %s %s\n", name.ascii(),publicId.ascii(),systemId.ascii());
  QDOM_NotationPrivate* n = new QDOM_NotationPrivate( doc, 0, name, publicId, systemId );
  doc->doctype()->appendChild( n );
  return TRUE;
}

void QDOMConsumer::parseError( int )
{
  // TODO
}

bool QDOMConsumer::finished()
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
QDOM::Element* QDOM::rectToElement( QDOM::Document* doc, const QRect& r, const QString& name )
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

QRect QDOM::elementToRect( const QDOM::Element* e )
{
  return QRect( e->attribute( "x" ).toInt(), e->attribute( "y" ).toInt(),
		e->attribute( "width" ).toInt(), e->attribute( "height" ).toInt() );
}
*/

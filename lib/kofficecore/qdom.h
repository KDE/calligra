#ifndef QDOM_H
#define QDOM_H

#include <qstring.h>
#include <qdict.h>
#include <qrect.h>
#include <qfont.h>
#include <qpen.h>

class QDOM_DocumentPrivate;
class QDOM_DocumentTypePrivate;
class QDOM_DocumentFragmentPrivate;
class QDOM_NodePrivate;
class QDOM_NodeListPrivate;
class QDOM_ImplementationPrivate;
class QDOM_ElementPrivate;
class QDOM_NotationPrivate;
class QDOM_EntityPrivate;
class QDOM_EntityReferencePrivate;
class QDOM_ProcessingInstructionPrivate;
class QDOM_AttrPrivate;
class QDOM_CharacterDataPrivate;
class QDOM_TextPrivate;
class QDOM_CommentPrivate;
class QDOM_CDATASectionPrivate;
class QDOM_NamedNodeMapPrivate;
class QDOM_ImplementationPrivate;
class QTextStream;

struct QDOM
{
  class NodeList;
  class Element;
  class Text;
  class Comment;
  class CDATASection;
  class ProcessingInstruction;
  class Attr;
  class EntityReference;
  class Document;
  class NamedNodeMap;
  class Document;
  class DocumentFragment;
  class DocumentType;
  class Implementation;
  class Node;
  class Entity;
  class Notation;
  class CharacterData;

  /* class Namespace
  {
  public:
    Namespace();
    virtual ~Namespace();

    static const unsigned short INDEX_SIZE_ERR     = 1;
    static const unsigned short DOMSTRING_SIZE_ERR = 2;
    static const unsigned short HIERARCHY_REQUEST_ERR = 3;
    static const unsigned short WRONG_DOCUMENT_ERR = 4;
    static const unsigned short INVALID_CHARACTER_ERR = 5;
    static const unsigned short NO_DATA_ALLOWED_ERR = 6;
    static const unsigned short NO_MODIFICATION_ALLOWED_ERR = 7;
    static const unsigned short NOT_FOUND_ERR      = 8;
    static const unsigned short NOT_SUPPORTED_ERR  = 9;
    static const unsigned short INUSE_ATTRIBUTE_ERR = 10;
  };
  
  class Exception
  {
  public:
    Exception();
    ~Exception();

    virtual unsigned short code();

  private:
    Exception( const Exception& );
    Exception& operator= ( const Exception& );

    unsigned short c;
    }; */

  class Implementation
  {
  public:
    Implementation();
    Implementation( const Implementation& );
    virtual ~Implementation();
    Implementation& operator= ( const Implementation& );
    bool operator== ( const Implementation& ) const;
    bool operator!= ( const Implementation& ) const;

    virtual bool hasFeature( const QString& feature, const QString& version );

  private:
    Implementation( QDOM_ImplementationPrivate* );

    QDOM_ImplementationPrivate* impl;

    friend class Document;
  };

  class Node // Ok
  {
  public:
    enum NodeType {
      BaseNode                  = 0,
      ElementNode               = 1,
      AttributeNode             = 2,
      TextNode                  = 3,
      CDATASectionNode          = 4,
      EntityReferenceNode       = 5,
      EntityNode                = 6,
      ProcessingInstructionNode = 7,
      CommentNode               = 8,
      DocumentNode              = 9,
      DocumentTypeNode          = 10,
      DocumentFragmentNode      = 11,
      NotationNode              = 12,
      CharacterDataNode         = 13
    };

    Node();
    Node( const Node& );
    Node& operator= ( const Node& );
    bool operator== ( const Node& ) const;
    bool operator!= ( const Node& ) const;
    virtual ~Node();

    virtual QString nodeName() const;
    virtual QString nodeValue() const;
    virtual void setNodeValue( const QString& );
    virtual NodeType nodeType() const;

    virtual Node         parentNode() const;
    virtual NodeList     childNodes() const;
    virtual Node         firstChild() const;
    virtual Node         lastChild() const;
    virtual Node         previousSibling() const;
    virtual Node         nextSibling() const;
    virtual NamedNodeMap attributes() const;
    virtual Document     ownerDocument() const;

    virtual Node insertBefore( const Node& newChild, const Node& refChild );
    virtual Node insertAfter( const Node& newChild, const Node& refChild );
    virtual Node replaceChild( const Node& newChild, const Node& oldChild );
    virtual Node removeChild( const Node& oldChild );
    virtual Node appendChild( const Node& newChild );
    virtual Node cloneNode( bool deep) const;

    // Qt extension
    virtual bool isAttr() const { return false; }
    virtual bool isCDATASection() const { return false; }
    virtual bool isDocumentFragment() const { return false; }
    virtual bool isDocument() const { return false; }
    virtual bool isDocumentType() const { return false; }
    virtual bool isElement() const { return false; }
    virtual bool isEntityReference() const { return false; }
    virtual bool isText() const { return false; }
    virtual bool isEntity() const { return false; }
    virtual bool isNotation() const { return false; }
    virtual bool isProcessingInstruction() const { return false; }
    virtual bool isCharacterData() const { return false; }

    /**
     * Shortcut to avoid dealing with NodeList
     * all the time.
     */
    Node namedItem( const QString& name ) const;

    bool isNull() const;
    void clear();

    Attr toAttr();
    CDATASection toCDATASection();
    DocumentFragment toDocumentFragment();
    Document toDocument();
    DocumentType toDocumentType();
    Element toElement();
    EntityReference toEntityReference();
    Text toText();
    Entity toEntity();
    Notation toNotation();
    ProcessingInstruction toProcessingInstruction();
    CharacterData toCharacterData();

    void save( QTextStream& ) const;

  protected:
    QDOM_NodePrivate* impl;
    Node( QDOM_NodePrivate* );

  private:
    friend class Document;
    friend class DocumentType;
    friend class NodeList;
    friend class NamedNodeMap;
  };

  class NodeList // Ok
  {
  public:
    NodeList();
    NodeList( const NodeList& );
    NodeList& operator= ( const NodeList& );
    bool operator== ( const NodeList& ) const;
    bool operator!= ( const NodeList& ) const;
    virtual ~NodeList();

    virtual Node item( int index ) const;
    virtual uint length() const;

  protected:
    friend class Node;

    NodeList( QDOM_NodeListPrivate* );

    QDOM_NodeListPrivate* impl;
  };

  class DocumentType : public Node
  {
  public:
    DocumentType();
    DocumentType( const DocumentType& x );
    DocumentType& operator= ( const DocumentType& );
    ~DocumentType();

    virtual QString name() const;
    virtual NamedNodeMap entities() const;
    virtual NamedNodeMap notations() const;

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isDocumentType() const;

  private:
    DocumentType( QDOM_DocumentTypePrivate* );

    friend class Document;
    friend class Node;
  };

  class Document : public Node
  {
  public:
    Document();
    Document( const QString& name );
    Document( const Document& x );
    Document& operator= ( const Document& );
    ~Document();

    // Qt extensions
    bool setContent( const QString& text );

    // Attributes
    DocumentType doctype() const;
    Implementation implementation() const;
    Element documentElement() const;

    // Factories
    Element               createElement( const QString& tagName );
    DocumentFragment      createDocumentFragment();
    Text                  createTextNode( const QString& data );
    Comment               createComment( const QString& data );
    CDATASection          createCDATASection( const QString& data );
    ProcessingInstruction createProcessingInstruction( const QString& target, const QString& data );
    Attr                  createAttribute( const QString& name );
    EntityReference       createEntityReference( const QString& name );
    NodeList              elementsByTagName( const QString& tagname );
    // Qt extension factories
    Element createElement( const QString& tagname, const QRect& rect );
    Element createElement( const QString& tagname, const QPen& rect );
    Element createElement( const QString& tagname, const QFont& rect );

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isDocument() const;

  private:
    Document( QDOM_DocumentPrivate* );

    friend class Node;
  };

  class NamedNodeMap
  {
  public:
    NamedNodeMap();
    NamedNodeMap( const NamedNodeMap& );
    NamedNodeMap& operator= ( const NamedNodeMap& );
    bool operator== ( const NamedNodeMap& ) const;
    bool operator!= ( const NamedNodeMap& ) const;
    ~NamedNodeMap();

    Node namedItem( const QString& name ) const;
    Node setNamedItem( const Node& arg );
    Node removeNamedItem( const QString& name );
    Node item( int index ) const;
    uint length() const;
    bool contains( const QString& name ) const;

  private:
    friend class Node;
    friend class DocumentType;
    friend class Element;

    NamedNodeMap( QDOM_NamedNodeMapPrivate* );

    QDOM_NamedNodeMapPrivate* impl;
  };

  class DocumentFragment : public Node
  {
  public:
    DocumentFragment();
    DocumentFragment( const DocumentFragment& x );
    DocumentFragment& operator= ( const DocumentFragment& );
    ~DocumentFragment();

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isDocumentFragment() const;

  private:
    DocumentFragment( QDOM_DocumentFragmentPrivate* );

    friend class Document;
    friend class Node;
  };

  class CharacterData : public Node
  {
  public:
    CharacterData();
    CharacterData( const CharacterData& x );
    CharacterData& operator= ( const CharacterData& );
    ~CharacterData();

    virtual QString data() const;
    virtual void setData( const QString& );
    virtual uint length() const;

    virtual QString substringData( unsigned long offset, unsigned long count );
    virtual void    appendData( const QString& arg );
    virtual void    insertData( unsigned long offset, const QString& arg );
    virtual void    deleteData( unsigned long offset, unsigned long count );
    virtual void    replaceData( unsigned long offset, unsigned long count, const QString& arg );

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isCharacterData() const;

  private:
    CharacterData( QDOM_CharacterDataPrivate* );

    friend class Document;
    friend class Text;
    friend class Comment;
    friend class Node;
  };

  class Attr : public Node
  {
  public:
    Attr();
    Attr( const Attr& x );
    Attr& operator= ( const Attr& );
    ~Attr();

    virtual QString  name() const;
    virtual bool     specified() const;
    virtual QString  value() const;
    virtual void setValue( const QString& );

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isAttr() const;

  private:
    Attr( QDOM_AttrPrivate* );

    friend class Document;
    friend class Element;
    friend class Node;
  };

  class Element : public Node
  {
  public:
    Element();
    Element( const Element& x );
    Element& operator= ( const Element& );
    ~Element();

    QString  tagName() const;
    QString  attribute( const QString& name ) const;
    void     setAttribute( const QString& name, const QString& value );
    void     setAttribute( const QString& name, int value );
    void     setAttribute( const QString& name, double value );
    void     removeAttribute( const QString& name );
    Attr     attributeNode( const QString& name);
    Attr     setAttributeNode( const Attr& newAttr );
    Attr     removeAttributeNode( const Attr& oldAttr );
    bool     hasAttribute( const QString& name ) const;
    // TODO
    // virtual NodeList elementsByTagName( const QString& name );
    void     normalize();

    // Overloaded from Node
    virtual NamedNodeMap attributes() const;
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isElement() const;

    QRect toRect() const;
    QFont toFont() const;
    QPen toPen() const;

    QString text() const;

  private:
    Element( QDOM_ElementPrivate* );

    friend class Document;
    friend class Node;
  };

  class Text : public CharacterData
  {
  public:
    Text();
    Text( const Text& x );
    Text& operator= ( const Text& );
    ~Text();

    Text splitText( int offset );

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isText() const;

  private:
    Text( QDOM_TextPrivate* );

    friend class CDATASection;
    friend class Document;
    friend class Node;
  };

  class Comment : public CharacterData
  {
  public:
    Comment();
    Comment( const Comment& x );
    Comment& operator= ( const Comment& );
    ~Comment();

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isComment() const;

  private:    
    Comment( QDOM_CommentPrivate* );

    friend class Document;
    friend class Node;
  };

  class CDATASection : public Text
  {
  public:
    CDATASection();
    CDATASection( const CDATASection& x );
    CDATASection& operator= ( const CDATASection& );
    ~CDATASection();

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isCDATASection() const;

  private:
    CDATASection( QDOM_CDATASectionPrivate* );

    friend class Document;
    friend class Node;
  };

  class Notation : public Node
  {
  public:
    Notation();
    Notation( const Notation& x );
    Notation& operator= ( const Notation& );
    ~Notation();

    QString publicId() const;
    QString systemId() const;

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isNotation() const;

  private:
    Notation( QDOM_NotationPrivate* );

    friend class Document;
    friend class Node;
  };

  class Entity : public Node
  {
  public:
    Entity();
    Entity( const Entity& x );
    Entity& operator= ( const Entity& );
    ~Entity();

    virtual QString publicId() const;
    virtual QString systemId() const;
    virtual QString notationName() const;

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isEntity() const;

  private:
    Entity( QDOM_EntityPrivate* );

    friend class Node;
  };

  class EntityReference : public Node
  {
  public:
    EntityReference();
    EntityReference( const EntityReference& x );
    EntityReference& operator= ( const EntityReference& );
    ~EntityReference();

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isEntityReference() const;

  private:
    EntityReference( QDOM_EntityReferencePrivate* );

    friend class Document;
    friend class Node;
  };

  class ProcessingInstruction : public Node
  {
  public:
    ProcessingInstruction();
    ProcessingInstruction( const ProcessingInstruction& x );
    ProcessingInstruction& operator= ( const ProcessingInstruction& );
    ~ProcessingInstruction();

    virtual QString target() const;
    virtual QString data() const;
    virtual void setData( const QString& d );

    // Overloaded from Node
    virtual NodeType nodeType() const;
    // virtual Node cloneNode( bool deep) const;
    virtual bool isProcessingInstruction() const;

  private:
    ProcessingInstruction( QDOM_ProcessingInstructionPrivate* );

    friend class Document;
    friend class Node;
  };

  // static Element* rectToElement( Document* doc, const QRect& r, const QString& name );
  // static QRect elementToRect( const Element* );
};

QTextStream& operator<<( QTextStream&, const QDOM::Node& );

#endif

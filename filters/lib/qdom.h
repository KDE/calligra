#ifndef QDOM_H
#define QDOM_H

#include <qstring.h>
#include <qdict.h>
#include <qrect.h>
#include <qfont.h>
#include <qpen.h>
#include <qpoint.h>
#include <qsize.h>
#include <qvariant.h>

class QWidget;
class QLayout;
class QIODevice;
class QMimeSourceFactory;

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

class QDomNodeList;
class QDomElement;
class QDomText;
class QDomComment;
class QDomCDATASection;
class QDomProcessingInstruction;
class QDomAttr;
class QDomEntityReference;
class QDomDocument;
class QDomNamedNodeMap;
class QDomDocument;
class QDomDocumentFragment;
class QDomDocumentType;
class QDomImplementation;
class QDomNode;
class QDomEntity;
class QDomNotation;
class QDomCharacterData;


  class QDomImplementation
  {
  public:
    QDomImplementation();
    QDomImplementation( const QDomImplementation& );
    virtual ~QDomImplementation();
    QDomImplementation& operator= ( const QDomImplementation& );
    bool operator== ( const QDomImplementation& ) const;
    bool operator!= ( const QDomImplementation& ) const;

    virtual bool hasFeature( const QString& feature, const QString& version );

  private:
    QDomImplementation( QDOM_ImplementationPrivate* );

    QDOM_ImplementationPrivate* impl;

    friend class QDomDocument;
  };

  class QDomNode // Ok
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

    QDomNode();
    QDomNode( const QDomNode& );
    QDomNode& operator= ( const QDomNode& );
    bool operator== ( const QDomNode& ) const;
    bool operator!= ( const QDomNode& ) const;
    virtual ~QDomNode();

    virtual QString nodeName() const;
    virtual QString nodeValue() const;
    virtual void setNodeValue( const QString& );
    virtual NodeType nodeType() const;

    virtual QDomNode         parentNode() const;
    virtual QDomNodeList     childNodes() const;
    virtual QDomNode         firstChild() const;
    virtual QDomNode         lastChild() const;
    virtual QDomNode         previousSibling() const;
    virtual QDomNode         nextSibling() const;
    virtual QDomNamedNodeMap attributes() const;
    virtual QDomDocument     ownerDocument() const;

    virtual QDomNode insertBefore( const QDomNode& newChild, const QDomNode& refChild );
    virtual QDomNode insertAfter( const QDomNode& newChild, const QDomNode& refChild );
    virtual QDomNode replaceChild( const QDomNode& newChild, const QDomNode& oldChild );
    virtual QDomNode removeChild( const QDomNode& oldChild );
    virtual QDomNode appendChild( const QDomNode& newChild );
    virtual QDomNode cloneNode( bool deep) const;

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
     * Shortcut to avoid dealing with QDomNodeList
     * all the time.
     */
    QDomNode namedItem( const QString& name ) const;

    bool isNull() const;
    void clear();

    QDomAttr toAttr();
    QDomCDATASection toCDATASection();
    QDomDocumentFragment toDocumentFragment();
    QDomDocument toDocument();
    QDomDocumentType toDocumentType();
    QDomElement toElement();
    QDomEntityReference toEntityReference();
    QDomText toText();
    QDomEntity toEntity();
    QDomNotation toNotation();
    QDomProcessingInstruction toProcessingInstruction();
    QDomCharacterData toCharacterData();

    void save( QTextStream& ) const;

  protected:
    QDOM_NodePrivate* impl;
    QDomNode( QDOM_NodePrivate* );

  private:
    friend class QDomDocument;
    friend class QDomDocumentType;
    friend class QDomNodeList;
    friend class QDomNamedNodeMap;
  };

  class QDomNodeList // Ok
  {
  public:
    QDomNodeList();
    QDomNodeList( const QDomNodeList& );
    QDomNodeList& operator= ( const QDomNodeList& );
    bool operator== ( const QDomNodeList& ) const;
    bool operator!= ( const QDomNodeList& ) const;
    virtual ~QDomNodeList();

    virtual QDomNode item( int index ) const;
    virtual uint length() const;

  protected:
    friend class QDomNode;

    QDomNodeList( QDOM_NodeListPrivate* );

    QDOM_NodeListPrivate* impl;
  };

  class QDomDocumentType : public QDomNode
  {
  public:
    QDomDocumentType();
    QDomDocumentType( const QDomDocumentType& x );
    QDomDocumentType& operator= ( const QDomDocumentType& );
    ~QDomDocumentType();

    virtual QString name() const;
    virtual QDomNamedNodeMap entities() const;
    virtual QDomNamedNodeMap notations() const;

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isDocumentType() const;

  private:
    QDomDocumentType( QDOM_DocumentTypePrivate* );

    friend class QDomDocument;
    friend class QDomNode;
  };

  class QDomDocument : public QDomNode
  {
  public:
    QDomDocument();
    QDomDocument( const QString& name );
    QDomDocument( QIODevice* dev );
    QDomDocument( const QDomDocument& x );
    QDomDocument& operator= ( const QDomDocument& );
    ~QDomDocument();

    // Qt extensions
    bool setContent( const QString& text );
    QMimeSourceFactory* mimeSourceFactory();
    const QMimeSourceFactory* mimeSourceFactory() const;
    void setMimeSourceFactory( QMimeSourceFactory* );

    // QDomAttributes
    QDomDocumentType doctype() const;
    QDomImplementation implementation() const;
    QDomElement documentElement() const;

    // Factories
    QDomElement               createElement( const QString& tagName );
    QDomDocumentFragment      createDocumentFragment();
    QDomText                  createTextNode( const QString& data );
    QDomComment               createComment( const QString& data );
    QDomCDATASection          createCDATASection( const QString& data );
    QDomProcessingInstruction createProcessingInstruction( const QString& target, const QString& data );
    QDomAttr                  createAttribute( const QString& name );
    QDomEntityReference       createEntityReference( const QString& name );
    QDomNodeList              elementsByTagName( const QString& tagname );
    // Qt extension factories
    QDomElement createElement( const QString& tagname, const QRect& rect );
    QDomElement createElement( const QString& tagname, const QPoint& rect );
    QDomElement createElement( const QString& tagname, const QSize& rect );
    QDomElement createElement( const QString& tagname, const QPen& rect );
    QDomElement createElement( const QString& tagname, const QFont& rect );

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    virtual bool isDocument() const;

  private:
    QDomDocument( QDOM_DocumentPrivate* );

    friend class QDomNode;
  };

  class QDomNamedNodeMap
  {
  public:
    QDomNamedNodeMap();
    QDomNamedNodeMap( const QDomNamedNodeMap& );
    QDomNamedNodeMap& operator= ( const QDomNamedNodeMap& );
    bool operator== ( const QDomNamedNodeMap& ) const;
    bool operator!= ( const QDomNamedNodeMap& ) const;
    ~QDomNamedNodeMap();

    QDomNode namedItem( const QString& name ) const;
    QDomNode setNamedItem( const QDomNode& arg );
    QDomNode removeNamedItem( const QString& name );
    QDomNode item( int index ) const;
    uint length() const;
    bool contains( const QString& name ) const;

  private:
    friend class QDomNode;
    friend class QDomDocumentType;
    friend class QDomElement;

    QDomNamedNodeMap( QDOM_NamedNodeMapPrivate* );

    QDOM_NamedNodeMapPrivate* impl;
  };

  class QDomDocumentFragment : public QDomNode
  {
  public:
    QDomDocumentFragment();
    QDomDocumentFragment( const QDomDocumentFragment& x );
    QDomDocumentFragment& operator= ( const QDomDocumentFragment& );
    ~QDomDocumentFragment();

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isDocumentFragment() const;

  private:
    QDomDocumentFragment( QDOM_DocumentFragmentPrivate* );

    friend class QDomDocument;
    friend class QDomNode;
  };

  class QDomCharacterData : public QDomNode
  {
  public:
    QDomCharacterData();
    QDomCharacterData( const QDomCharacterData& x );
    QDomCharacterData& operator= ( const QDomCharacterData& );
    ~QDomCharacterData();

    virtual QString data() const;
    virtual void setData( const QString& );
    virtual uint length() const;

    virtual QString substringData( unsigned long offset, unsigned long count );
    virtual void    appendData( const QString& arg );
    virtual void    insertData( unsigned long offset, const QString& arg );
    virtual void    deleteData( unsigned long offset, unsigned long count );
    virtual void    replaceData( unsigned long offset, unsigned long count, const QString& arg );

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isCharacterData() const;

  private:
    QDomCharacterData( QDOM_CharacterDataPrivate* );

    friend class QDomDocument;
    friend class QDomText;
    friend class QDomComment;
    friend class QDomNode;
  };

  class QDomAttr : public QDomNode
  {
  public:
    QDomAttr();
    QDomAttr( const QDomAttr& x );
    QDomAttr& operator= ( const QDomAttr& );
    ~QDomAttr();

    virtual QString  name() const;
    virtual bool     specified() const;
    virtual QString  value() const;
    virtual void setValue( const QString& );

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isAttr() const;

  private:
    QDomAttr( QDOM_AttrPrivate* );

    friend class QDomDocument;
    friend class QDomElement;
    friend class QDomNode;
  };

  class QDomElement : public QDomNode
  {
  public:
    QDomElement();
    QDomElement( const QDomElement& x );
    QDomElement& operator= ( const QDomElement& );
    ~QDomElement();

    void setTagName( const QString& name );
    QString  tagName() const;
    QString  attribute( const QString& name ) const;
    void     setAttribute( const QString& name, const QString& value );
    void     setAttribute( const QString& name, int value );
    void     setAttribute( const QString& name, double value );
    void     removeAttribute( const QString& name );
    QDomAttr     attributeNode( const QString& name);
    QDomAttr     setAttributeNode( const QDomAttr& newAttr );
    QDomAttr     removeAttributeNode( const QDomAttr& oldAttr );
    bool     hasAttribute( const QString& name ) const;
    // TODO
    // virtual QDomNodeList elementsByTagName( const QString& name );
    void     normalize();

    // Overloaded from QDomNode
    virtual QDomNamedNodeMap attributes() const;
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isElement() const;

    QRect toRect() const;
    QFont toFont() const;
    QPen toPen() const;
    QSize toSize() const;
    QPoint toPoint() const;
//     QWidget* toWidget( QWidget* parent = 0 ) const;
//     QLayout* toLayout( QWidget* parent ) const;
//     QLayout* toLayout( QLayout* parent, QWidget* mainwidget = 0 ) const;

//     QVariant property( const QString& name, QVariant::Type ) const;
//     void setProperty( const QString& name, const QVariant& prop );

    QString text() const;

  private:
    QDomElement( QDOM_ElementPrivate* );

    friend class QDomDocument;
    friend class QDomNode;
  };

  class QDomText : public QDomCharacterData
  {
  public:
    QDomText();
    QDomText( const QDomText& x );
    QDomText& operator= ( const QDomText& );
    ~QDomText();

    QDomText splitText( int offset );

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isText() const;

  private:
    QDomText( QDOM_TextPrivate* );

    friend class QDomCDATASection;
    friend class QDomDocument;
    friend class QDomNode;
  };

  class QDomComment : public QDomCharacterData
  {
  public:
    QDomComment();
    QDomComment( const QDomComment& x );
    QDomComment& operator= ( const QDomComment& );
    ~QDomComment();

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isComment() const;

  private:
    QDomComment( QDOM_CommentPrivate* );

    friend class QDomDocument;
    friend class QDomNode;
  };

  class QDomCDATASection : public QDomText
  {
  public:
    QDomCDATASection();
    QDomCDATASection( const QDomCDATASection& x );
    QDomCDATASection& operator= ( const QDomCDATASection& );
    ~QDomCDATASection();

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isCDATASection() const;

  private:
    QDomCDATASection( QDOM_CDATASectionPrivate* );

    friend class QDomDocument;
    friend class QDomNode;
  };

  class QDomNotation : public QDomNode
  {
  public:
    QDomNotation();
    QDomNotation( const QDomNotation& x );
    QDomNotation& operator= ( const QDomNotation& );
    ~QDomNotation();

    QString publicId() const;
    QString systemId() const;

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isNotation() const;

  private:
    QDomNotation( QDOM_NotationPrivate* );

    friend class QDomDocument;
    friend class QDomNode;
  };

  class QDomEntity : public QDomNode
  {
  public:
    QDomEntity();
    QDomEntity( const QDomEntity& x );
    QDomEntity& operator= ( const QDomEntity& );
    ~QDomEntity();

    virtual QString publicId() const;
    virtual QString systemId() const;
    virtual QString notationName() const;

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isEntity() const;

  private:
    QDomEntity( QDOM_EntityPrivate* );

    friend class QDomNode;
  };

  class QDomEntityReference : public QDomNode
  {
  public:
    QDomEntityReference();
    QDomEntityReference( const QDomEntityReference& x );
    QDomEntityReference& operator= ( const QDomEntityReference& );
    ~QDomEntityReference();

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isEntityReference() const;

  private:
    QDomEntityReference( QDOM_EntityReferencePrivate* );

    friend class QDomDocument;
    friend class QDomNode;
  };

  class QDomProcessingInstruction : public QDomNode
  {
  public:
    QDomProcessingInstruction();
    QDomProcessingInstruction( const QDomProcessingInstruction& x );
    QDomProcessingInstruction& operator= ( const QDomProcessingInstruction& );
    ~QDomProcessingInstruction();

    virtual QString target() const;
    virtual QString data() const;
    virtual void setData( const QString& d );

    // Overloaded from QDomNode
    virtual NodeType nodeType() const;
    // virtual QDomNode cloneNode( bool deep) const;
    virtual bool isProcessingInstruction() const;

  private:
    QDomProcessingInstruction( QDOM_ProcessingInstructionPrivate* );

    friend class QDomDocument;
    friend class QDomNode;
  };

QTextStream& operator<<( QTextStream&, const QDomNode& );

#endif

#include <qxml.h>
#include <qptrstack.h>

class KWordParagraph;
class KWordDocument;
class KWordFrameset;

/**
 * Type of element
 *
 * Note: we do not care of the tags: \<FRAMESETS\>
 */
enum StackItemElementType
{
    ElementTypeUnknown  = 0,
    ElementTypeBottom,      ///< Bottom of the stack
    ElementTypeIgnore,      ///< Element is known but ignored
    ElementTypeEmpty,       ///< Element is empty
    ElementTypeDocument,    ///< Element is the document ( \<DOC\> )
    ElementTypePaper,       ///< \<PAPER\>
    ElementTypeFrameset,    ///< \<FRAMESET\>
    ElementTypeUnknownFrameset, ///< a \<FRAMESET\> which is not supported
    ElementTypeParagraph,   ///< \<PARAGRAPH\>
    ElementTypeText        ///< \<TEXT\>
};

class StackItem
{
public:
    StackItem();
    ~StackItem();
public:
    QString itemName;   ///< Name of the tag (only for error purposes)
    StackItemElementType elementType;
    KWordFrameset* m_currentFrameset;
    KWordParagraph* m_paragraph;
};

class StackItemStack : public QPtrStack<StackItem>
{
public:
        StackItemStack(void) { }
        ~StackItemStack(void) { }
};

class KWordParser : public QXmlDefaultHandler
{
public:
    KWordParser( KWordDocument* kwordDocument );
    virtual ~KWordParser( void );
public:
    /// Process opening tag
    virtual bool startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes);
    /// Process closing tag
    virtual bool endElement( const QString&, const QString& , const QString& qName);
    /// Process element's characters (between opening and closing tags)
    virtual bool characters ( const QString & ch );
protected:
    /// Process \<PARAGRAPH\>
    bool startElementParagraph( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem );
    /// Process \<FRAME\>
    bool startElementFrame( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem );
    /// Process \<FRAMESET\>
    bool startElementFrameset( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem );
    /// Process opening tag of some elements that are children of \<DOC\> and which only define document properties
    bool startElementDocumentAttributes( const QString& name, const QXmlAttributes& attributes,
        StackItem *stackItem, const StackItemElementType& allowedParentType, const StackItemElementType& newType );
protected:
    QString indent; //DEBUG
    StackItemStack parserStack;
    KWordDocument* m_kwordDocument;
};

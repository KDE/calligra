#include <qxml.h>
#include <qptrstack.h>

class KWordDocument;
class KWordFrameset;

/**
 * Type of element
 *
 * Note: we do not care of: \<FRAMESETS\>
 */
enum StackItemElementType
{
    ElementTypeUnknown  = 0,
    ElementTypeBottom,      ///< Bottom of the stack
    ElementTypeIgnore,      ///< Element is known but ignored
    ElementTypeEmpty,       ///< Element is empty
    ElementTypeDocument,    ///< Element is the document ( \<DOC\> )
    ElementTypePaper,       ///< \<PAPER\>
    ElementTypeFrameset    ///< \<FRAMESET\>
};

class StackItem
{
public:
    StackItem();
    ~StackItem();
public:
    QString itemName;   ///< Name of the tag (only for error purposes)
    StackItemElementType elementType;
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
protected:
    /// Process opening tag of some elements that are children of \<DOC\> and which only define document properties
    bool startElementDocumentAttributes( const QString& name, const QXmlAttributes& attributes,
        StackItem *stackItem, const StackItemElementType& allowedParentType, const StackItemElementType& newType );
    bool startElementFrameset( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem );
protected:
    QString indent; //DEBUG
    StackItemStack parserStack;
    KWordDocument* m_kwordDocument;
    KWordFrameset* m_currentFrameset;
};

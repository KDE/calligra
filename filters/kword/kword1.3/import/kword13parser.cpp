// 

#include "kwordparser.h"
#include "kwordframeset.h"
#include "kworddocument.h"

StackItem::StackItem() : elementType( ElementTypeUnknown )
{
}

StackItem::~StackItem()
{
}

KWordParser::KWordParser( KWordDocument* kwordDocument ) : m_kwordDocument(kwordDocument), m_currentFrameset(0)
{
    parserStack.setAutoDelete( true );
    StackItem* bottom = new StackItem;
    bottom->elementType = ElementTypeBottom;
    parserStack.push( bottom ); //Security item (not to empty the stack)
}

KWordParser::~KWordParser( void )
{
    parserStack.clear();
}

bool KWordParser::startElementFrameset( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem )
{
    const QString frameTypeStr( attributes.value( "frameType" ) );
    const QString frameInfoStr( attributes.value( "frameInfo" ) );
    
    if ( frameTypeStr.isEmpty() || frameInfoStr.isEmpty() )
    {
        // kdError(30520) << "<FRAMESET> without frameType or frameInfo attribute!" << endl;
        qDebug("<FRAMESET> without frameType or frameInfo attribute!");
        return false;
    }
    
    const int frameType = frameTypeStr.toInt();
    const int frameInfo = frameInfoStr.toInt();
    
    if ( frameType == 1 && frameInfo == 0 )
    {
        // Normal text frame (in or outside a table)
        if ( attributes.value( "grpMgr" ).isEmpty() )
        {
            stackItem->elementType = ElementTypeFrameset;
            
            KWordNormalTextFrameset* frameset = new KWordNormalTextFrameset( frameType, frameInfo, attributes.value( "name" ) );
            m_kwordDocument->m_normalTextFramesetList.append( frameset );
            m_currentFrameset = m_kwordDocument->m_normalTextFramesetList.current();
        }
        else
        {
            qDebug("Tables are not supported yet!");
        }
    }
    else
    {
        // Frame of unknown/unsupport type
        //kdWarning(30520) << "Unknown/unsupported <FRAMESET> type! Type: " << frameTypeStr << " Info: " << frameInfoStr << emdl;
        qDebug("Unknown <FRAMESET> type! Type: %i Info: %i", frameType, frameInfo);
        stackItem->elementType = ElementTypeUnknown;
    }
    return true;
}


bool KWordParser::startElementDocumentAttributes( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem,
     const StackItemElementType& allowedParentType, const StackItemElementType& newType )
{
    if ( parserStack.current()->elementType == allowedParentType )
    {
        stackItem->elementType = newType;
        for (int i = 0; i < attributes.count(); ++i )
        {
            QString attrName ( name );
            attrName += ':';
            attrName += attributes.qName( i );
            m_kwordDocument->m_documentProperties[ attrName ] = attributes.value( i );
            qDebug("DocAttr: %s = %s", attrName.latin1(), attributes.value( i ).latin1() );
        }
        return true;
    }
    else
    {
        qDebug("Wrong parent!");
        return false;
    }
}

bool KWordParser::startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes )
{
    qDebug("%s<%s>", indent.latin1(), name.latin1() );
    indent += "*"; //DEBUG
    if (parserStack.isEmpty())
    {
        //kdError(30520) << "Stack is empty!! Aborting! (in KWordParser::startElement)" << endl;
        qDebug("Stack is empty!! Aborting! (in KWordParser::startElement)");
        return false;
    }
    
    // Create a new stack element copying the top of the stack.
    StackItem *stackItem=new StackItem(*parserStack.current());

    if (!stackItem)
    {
        //kdError(30506) << "Could not create Stack Item! Aborting! (in StructureParser::startElement)" << endl;
        qDebug("Could not create Stack Item! Aborting! (in StructureParser::startElement)");
        return false;
    }

    stackItem->itemName=name;

    bool success=false;

    if ( name == "FRAMESET" )
    {
        success = startElementFrameset( name, attributes, stackItem );
    }
    else if ( name == "DOC" )
    {
        success = startElementDocumentAttributes( name, attributes, stackItem, ElementTypeBottom, ElementTypeDocument );
    }
    else if  ( name == "PAPER") 
    {
        success = startElementDocumentAttributes( name, attributes, stackItem, ElementTypeDocument, ElementTypePaper );
    }
    else if ( name == "PAPERBORDERS" )
    {
        success = startElementDocumentAttributes( name, attributes, stackItem, ElementTypePaper, ElementTypeEmpty );
    }
    else if ( ( name == "ATTRIBUTES" ) || ( name == "VARIABLESETTINGS" )
         || ( name == "FOOTNOTESETTINGS" ) || ( name == "ENDNOTESETTINGS" ) )
    {
        success = startElementDocumentAttributes( name, attributes, stackItem, ElementTypeDocument, ElementTypeEmpty );
    }
    else
    {
        stackItem->elementType = ElementTypeUnknown;
        success = true;
    }

    if ( success )
    {
        parserStack.push( stackItem );
    }
    else
    {   // We have a problem so destroy our resources.
        delete stackItem;
    }
    
    return success;
}

bool KWordParser :: endElement( const QString&, const QString& , const QString& name)
{
    indent.remove( 0, 1 ); // DEBUG
    //qDebug("%s</%s>", indent.latin1(), name.latin1() );
    if (parserStack.isEmpty())
    {
        //kdError(30506) << "Stack is empty!! Aborting! (in StructureParser::endElement)" << endl;
        qDebug("Stack is empty!! Aborting! (in StructureParser::endElement)");
        return false;
    }

    bool success=false;
    
    if ( name == "DOC" )
    {
        success = true;
    }
    else
    {
        success = true; // No problem, so authorisation to continue parsing
    }

    StackItem *stackItem=parserStack.pop();
    
    if (!success)
    {
        // If we have no success, then it was surely a tag mismatch. Help debugging!
        //kdError(30506) << "Found tag name: " << name << " expected: " << stackItem->itemName << endl;
        qDebug("Found tag name: %s expected: %s", name.latin1(), stackItem->itemName.latin1() );
    }
    
    delete stackItem;
    
    return success;
}

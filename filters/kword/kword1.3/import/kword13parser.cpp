// 

/* This file is part of the KDE project
   Copyright 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include "kword13layout.h"
#include "kword13frameset.h"
#include "kword13document.h"
#include "kword13parser.h"

StackItem::StackItem() : elementType( ElementTypeUnknown ), m_currentFrameset( 0 )
{
}

StackItem::~StackItem()
{
}

KWord13Parser::KWord13Parser( KWord13Document* kwordDocument ) 
    : m_kwordDocument(kwordDocument), m_currentParagraph( 0 ), 
    m_currentLayout( 0 ), m_currentFormat( 0 )
{
    parserStack.setAutoDelete( true );
    StackItem* bottom = new StackItem;
    bottom->elementType = ElementTypeBottom;
    parserStack.push( bottom ); //Security item (not to empty the stack)
}

KWord13Parser::~KWord13Parser( void )
{
    parserStack.clear();
    delete m_currentParagraph;
    delete m_currentLayout;
    delete m_currentFormat;
}

bool KWord13Parser::startElementFormatOneProperty( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem)
{
    // ### TODO: check status
    if ( m_currentFormat )
    {
        for (int i = 0; i < attributes.count(); ++i )
        {
            QString attrName ( name );
            attrName += ':';
            attrName += attributes.qName( i );
            m_currentFormat->m_properties[ attrName ] = attributes.value( i );
            kdDebug(30520) << "Format Property: " << attrName << " = " << attributes.value( i ) << endl;
        }
        stackItem->elementType = ElementTypeEmpty;
        return true;
    }
    else
    {
        kdError(30520) << "No current layout for storing property: " << name << endl;
        return false;
    }
}

bool KWord13Parser::startElementLayoutProperty( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem)
{
    // ### TODO: check status
    if ( m_currentLayout )
    {
        for (int i = 0; i < attributes.count(); ++i )
        {
            QString attrName ( name );
            attrName += ':';
            attrName += attributes.qName( i );
            m_currentLayout->m_layoutProperties[ attrName ] = attributes.value( i );
            kdDebug(30520) << "Layout Property: " << attrName << " = " << attributes.value( i ) << endl;
        }
        stackItem->elementType = ElementTypeEmpty;
        return true;
    }
    else
    {
        kdError(30520) << "No current layout for storing property: " << name << endl;
        return false;
    }
}

bool KWord13Parser::startElementName( const QString&, const QXmlAttributes& attributes, StackItem *stackItem )
{
    if ( stackItem->elementType != ElementTypeLayout )
    {
        // We have something else than a LAYOU/STYLE, so ignore for now.
        stackItem->elementType = ElementTypeIgnore;
        return true;
    }
    
    stackItem->elementType = ElementTypeEmpty;
    
    if ( m_currentLayout )
    {
        m_currentLayout->m_name = attributes.value( "value" );
    }
    return  true;
}

bool KWord13Parser::startElementLayout( const QString&, const QXmlAttributes& attributes, StackItem *stackItem )
{
    // ##TODO: check parent?
    if ( stackItem->elementType == ElementTypeIgnore )
    {
        return true;
    }
    
    stackItem->elementType = ElementTypeLayout;
    
    if ( m_currentLayout )
    {
        // Delete an eventually already existing paragraph (should not happen)
        kdWarning(30520) << "Current layout already defined!" << endl;
        delete m_currentLayout;
    }
    
    m_currentLayout = new KWord13Layout;
    m_currentLayout->m_outline = ( attributes.value( "outline" ) == "true" );
    
    return true;    
}

bool KWord13Parser::startElementParagraph( const QString&, const QXmlAttributes&, StackItem *stackItem )
{
    if ( stackItem->elementType == ElementTypeUnknownFrameset )
    {
        stackItem->elementType = ElementTypeIgnore;
        return true;
    }

    stackItem->elementType = ElementTypeParagraph;
    
    if ( m_currentParagraph )
    {
        // Delete an eventually already existing paragraph (should not happen)
        kdWarning(30520) << "Current paragraph already defined!" << endl;
        delete m_currentParagraph;
    }
        
    m_currentParagraph = new KWordParagraph;
    
    return true;
}

bool KWord13Parser::startElementFrame( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem )
{
    if ( stackItem->elementType == ElementTypeFrameset )
    {
        stackItem->elementType = ElementTypeEmpty;
        if ( stackItem->m_currentFrameset )
        {
            const int num = ++stackItem->m_currentFrameset->m_numFrames;
            for (int i = 0; i < attributes.count(); ++i )
            {
                QString attrName ( name );
                attrName += ':';
                attrName += QString::number( num );
                attrName += ':';
                attrName += attributes.qName( i );
                stackItem->m_currentFrameset->m_frameData[ attrName ] = attributes.value( i );
                kdDebug(30520) << "FrameData: " << attrName << " = " << attributes.value( i ) << endl;
            }
            
        }
        else
        {
            kdError(30520) << "Data of <FRAMESET> not found" << endl;
            return false;
        }
    }
    else if ( stackItem->elementType != ElementTypeUnknownFrameset )
    {
        kdError(30520) << "<FRAME> not child of <FRAMESET>" << endl;
        return false;
    }
    return true;
}

bool KWord13Parser::startElementFrameset( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem )
{
    const QString frameTypeStr( attributes.value( "frameType" ) );
    const QString frameInfoStr( attributes.value( "frameInfo" ) );
    
    if ( frameTypeStr.isEmpty() || frameInfoStr.isEmpty() )
    {
        kdError(30520) << "<FRAMESET> without frameType or frameInfo attribute!" << endl;
        return false;
    }
    
    const int frameType = frameTypeStr.toInt();
    const int frameInfo = frameInfoStr.toInt();
    
    if ( frameType == 1 )
    {
        stackItem->elementType = ElementTypeFrameset;
        KWordTextFrameset* frameset = new KWordTextFrameset( frameType, frameInfo, attributes.value( "name" ) );
        
        // Normal text frame (in or outside a table)
        if ( ( !frameInfo ) && attributes.value( "grpMgr" ).isEmpty() )
        {
            m_kwordDocument->m_normalTextFramesetList.append( frameset );
            stackItem->m_currentFrameset = m_kwordDocument->m_normalTextFramesetList.current();
        }
        else if ( !frameInfo )
        {
            // We just store the frameset in the frameset table list
            // Grouping the framesets by table will be done after the parsing, not now.
            m_kwordDocument->m_tableFramesetList.append( frameset );
            stackItem->m_currentFrameset = m_kwordDocument->m_tableFramesetList.current();
        }
        else if ( frameInfo >= 1 && frameInfo <= 6 )
        {
            m_kwordDocument->m_headerFooterFramesetList.append( frameset );
            stackItem->m_currentFrameset = m_kwordDocument->m_headerFooterFramesetList.current();
        }
        else if ( frameInfo == 7 )
        {
            m_kwordDocument->m_footEndNoteFramesetList.append( frameset );
            stackItem->m_currentFrameset = m_kwordDocument->m_footEndNoteFramesetList.current();
        }
        // ### frameType == 2 or 5 : image/picture or clipart
        // ### frameType == 6 : horizontal line (however KWord did not save it correctly)
        // ### frameType == 4 : formula
        // ### frametype == 3 : embedded (but only in <SETTINGS>)
        else
        {
            kdError(30520) << "Unknown text frameset!" << endl;
            m_kwordDocument->m_otherFramesetList.append( frameset );
            stackItem->m_currentFrameset = m_kwordDocument->m_otherFramesetList.current();
        }
    }
    else
    {
        // Frame of unknown/unsupported type
        kdWarning(30520) << "Unknown/unsupported <FRAMESET> type! Type: " << frameTypeStr << " Info: " << frameInfoStr << endl;
        stackItem->elementType = ElementTypeUnknownFrameset;
        KWordFrameset* frameset = new KWordFrameset( frameType, frameInfo, attributes.value( "name" ) );
        m_kwordDocument->m_otherFramesetList.append( frameset );
        stackItem->m_currentFrameset = m_kwordDocument->m_otherFramesetList.current();
    }
    return true;
}


bool KWord13Parser::startElementDocumentAttributes( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem,
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
            kdDebug(30520) << "DocAttr: " <<  attrName << " = " << attributes.value( i ) << endl;
        }
        return true;
    }
    else
    {
        kdError(30520) << "Wrong parent!" << endl;
        return false;
    }
}

bool KWord13Parser::startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes )
{
    kdDebug(30520) << indent << "<" << name << ">" << endl; // DEBUG
    indent += "*"; //DEBUG
    if (parserStack.isEmpty())
    {
        kdError(30520) << "Stack is empty!! Aborting! (in KWordParser::startElement)" << endl;
        return false;
    }
    
    // Create a new stack element copying the top of the stack.
    StackItem *stackItem=new StackItem(*parserStack.current());

    if (!stackItem)
    {
        kdError(30520) << "Could not create Stack Item! Aborting! (in StructureParser::startElement)" << endl;
        return false;
    }

    stackItem->itemName=name;

    bool success=false;

    // Order of element names: probability in a document
    if ( name == "COLOR" || name == "FONT" || name =="SIZE" 
        || name == "WEIGHT" || name == "ITALIC" || name == "UNDERLINE" 
        || name == "STRIKEOUT" || name == "VERTALIGN" || name == "SHADOW"
        || name == "FONTATTRIBUTE" || name == "LANGUAGE"
        || name == "TEXTBACKGROUNDCOLOR" || name == "OFFSETFROMBASELINE" )
    {
        success = startElementFormatOneProperty( name, attributes, stackItem );
    }
    else if ( name == "FLOW" || name == "INDENTS" || name == "OFFSETS"
        || name == "LINESPACING" || name == "PAGEBREAKING" 
        || name == "LEFTBORDER" || name == "RIGHTBORDER" || name == "FOLLOWING"
        || name == "TOPBORDER" || name == "BOTTOMBORDER" || name == "COUNTER" )
    {
        success = startElementLayoutProperty( name, attributes, stackItem );
    }
    else if ( name == "TEXT" )
    {
        if ( stackItem->elementType == ElementTypeParagraph && m_currentParagraph )
        {
            stackItem->elementType = ElementTypeText;
            m_currentParagraph->setText( QString::null );
        }
        else
        {
            stackItem->elementType = ElementTypeIgnore;
        }
        success = true;
    }
    else if ( name == "NAME" )
    {
        success = startElementName( name, attributes, stackItem );
    }
    else if ( name == "PARAGRAPH" )
    {
        success = startElementParagraph( name, attributes, stackItem );
    }
    else if (name == "LAYOUT" )
    {
        success = startElementLayout( name, attributes, stackItem );
    }
    else if ( name == "FRAME" )
    {
        success = startElementFrame( name, attributes, stackItem );
    }
    else if ( name == "FRAMESET" )
    {
        success = startElementFrameset( name, attributes, stackItem );
    }
    else if (name == "STYLE" )
    {
        success = startElementLayout( name, attributes, stackItem );
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

bool KWord13Parser :: endElement( const QString&, const QString& , const QString& name)
{
    indent.remove( 0, 1 ); // DEBUG
    //kdDebug(30520) << indent << "</" << name << ">" << endl; // DEBUG
    if (parserStack.isEmpty())
    {
        kdError(30520) << "Stack is empty!! Aborting! (in StructureParser::endElement)" << endl;
        return false;
    }

    bool success=false;
    
    StackItem *stackItem=parserStack.pop();
        
    if ( name == "PARAGRAPH" )
    {
        if ( stackItem->m_currentFrameset && m_currentParagraph )
        {
            if ( stackItem->m_currentFrameset->addParagraph( *m_currentParagraph ) )
            {
                success = true;
            }
        }
        else if ( stackItem->elementType == ElementTypeIgnore )
        {
            success = true;
        }
        delete m_currentParagraph;
        m_currentParagraph = 0;
    }
    else if ( name == "LAYOUT" )
    {
        if ( m_currentLayout && m_currentParagraph )
        {
            m_currentParagraph->m_layout = *m_currentLayout;
        }
        delete m_currentLayout;
        m_currentLayout = 0;
        success = true;
    }
    else if ( name == "STYLE" )
    {
        if ( m_kwordDocument && m_currentLayout )
        {
            if ( m_currentLayout->m_name.isEmpty() )
            {
                // ### TODO: what should be really done with anonymous styles (should not happen but it would have consequences)
                kdError(30520) << "Anonymous style found! Aborting" << endl;
                return false; // Assume a parsing error!
            }
            m_kwordDocument->m_styles.append( *m_currentLayout );
            success = true;
        }
        delete m_currentLayout;
        m_currentLayout = 0;
    }
    else if ( name == "DOC" )
    {
        success = true;
    }
    else
    {
        success = true; // No problem, so authorisation to continue parsing
    }
    
    if (!success)
    {
        // If we have no success, then it was surely a tag mismatch. Help debugging!
        kdError(30506) << "Found closing tag name: " << name << " expected: " << stackItem->itemName << endl;
    }
    
    delete stackItem;
    
    return success;
}

bool KWord13Parser :: characters ( const QString & ch )
{
#if 0
    // DEBUG start
    if (ch=="\n")
    {
        kdDebug(30520) << indent << " (LINEFEED)" << endl;
    }
    else if (ch.length()> 40)
    {   // 40 characters are enough (especially for image data)
        kdDebug(30520) << indent << " :" << ch.left(40) << "..." << endl;
    }
    else
    {
        kdDebug(30520) << indent << " :" << ch << ":" << endl;
    }
    // DEBUG end
#endif

    if (parserStack.isEmpty())
    {
        kdError(30520) << "Stack is empty!! Aborting! (in StructureParser::characters)" << endl;
        return false;
    }

    bool success=false;

    StackItem *stackItem = parserStack.current();

    if ( stackItem->elementType == ElementTypeText )
    { 
        // <TEXT>
        if ( m_currentParagraph )
        {
            // ### TODO: this is perhaps a good point to check for non-XML characters
            m_currentParagraph->appendText( ch );
            success = true;
        }
        else
        {
            kdError(30520) << "No current paragraph defined! Tag mismatch?" << endl;
            success = false;
        }
    }
    else if (stackItem->elementType==ElementTypeEmpty)
    {
        success=ch.stripWhiteSpace().isEmpty();
        if (!success)
        {
            // We have a parsing error, so abort!
            kdError(30520) << "Empty element "<< stackItem->itemName <<" is not empty! Aborting! (in KWordParser::characters)" << endl;
        }
    }
    else
    {
        success=true;
    }

    return success;
}

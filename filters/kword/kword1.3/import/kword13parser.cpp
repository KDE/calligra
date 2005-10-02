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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kdebug.h>

#include "kword13formatone.h"
#include "kword13formatother.h"
#include "kword13layout.h"
#include "kword13frameset.h"
#include "kword13picture.h"
#include "kword13document.h"
#include "kword13parser.h"

KWord13StackItem::KWord13StackItem() : elementType( KWord13TypeUnknown ), m_currentFrameset( 0 )
{
}

KWord13StackItem::~KWord13StackItem()
{
}

KWord13Parser::KWord13Parser( KWord13Document* kwordDocument ) 
    : m_kwordDocument(kwordDocument), m_currentParagraph( 0 ), 
    m_currentLayout( 0 ), m_currentFormat( 0 )
{
    parserStack.setAutoDelete( true );
    KWord13StackItem* bottom = new KWord13StackItem;
    bottom->elementType = KWord13TypeBottom;
    parserStack.push( bottom ); //Security item (not to empty the stack)
}

KWord13Parser::~KWord13Parser( void )
{
    parserStack.clear();
    delete m_currentParagraph;
    delete m_currentLayout;
    delete m_currentFormat;
}

bool KWord13Parser::startElementFormatOneProperty( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem)
{
    // ### TODO: check status
    if ( stackItem->elementType == KWord13TypeLayoutFormatOne )
    {
        if ( ! m_currentLayout )
        {
             kdError(30520) << "No current LAYOUT for storing FORMAT property: " << name << endl;
             return false;
        }
        for (int i = 0; i < attributes.count(); ++i )
        {
            QString attrName ( name );
            attrName += ':';
            attrName += attributes.qName( i );
            m_currentLayout->m_format.m_properties[ attrName ] = attributes.value( i );
            kdDebug(30520) << "Format Property (for LAYOUT): " << attrName << " = " << attributes.value( i ) << endl;
        }
        stackItem->elementType = KWord13TypeEmpty;
        return true;
    }
    else if ( stackItem->elementType == KWord13TypeFormat )
    {
        if ( ! m_currentFormat )
        {
             kdError(30520) << "No current FORMAT for storing FORMAT property: " << name << endl;
             return false;
        }
        KWord13FormatOneData* data = m_currentFormat->getFormatOneData();
        
        if ( ! data )
        {
             kdError(30520) << "Current FORMAT cannot store FORMAT text property: " << name << endl;
             return false;
        }
        
        for (int i = 0; i < attributes.count(); ++i )
        {
            QString attrName ( name );
            attrName += ':';
            attrName += attributes.qName( i );
            data->m_properties[ attrName ] = attributes.value( i );
            kdDebug(30520) << "Format Property (for FORMATS): " << attrName << " = " << attributes.value( i ) << endl;
        }
        stackItem->elementType = KWord13TypeEmpty;
        return true;
    }
    else if ( stackItem->elementType == KWord13TypeIgnore )
    {
        return true;
    }
    else
    {
        kdError(30520) << "Wrong parents for FORMAT property: " << name << endl;
        return false;
    }
}

bool KWord13Parser::startElementLayoutProperty( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem)
{
    // ### TODO: check status
    if ( stackItem->elementType == KWord13TypeIgnore )
    {
        return true;
    }
    else if ( m_currentLayout )
    {
        for (int i = 0; i < attributes.count(); ++i )
        {
            QString attrName ( name );
            attrName += ':';
            attrName += attributes.qName( i );
            m_currentLayout->m_layoutProperties[ attrName ] = attributes.value( i );
            kdDebug(30520) << "Layout Property: " << attrName << " = " << attributes.value( i ) << endl;
        }
        stackItem->elementType = KWord13TypeEmpty;
        return true;
    }
    else
    {
        kdError(30520) << "No current layout for storing property: " << name << endl;
        return false;
    }
}

bool KWord13Parser::startElementName( const QString&, const QXmlAttributes& attributes, KWord13StackItem *stackItem )
{
    if ( stackItem->elementType != KWord13TypeLayout )
    {
        // We have something else than a LAYOU/STYLE, so ignore for now.
        stackItem->elementType = KWord13TypeIgnore;
        return true;
    }
    
    stackItem->elementType = KWord13TypeEmpty;
    
    if ( m_currentLayout )
    {
        m_currentLayout->m_name = attributes.value( "value" );
    }
    return  true;
}

bool KWord13Parser::startElementFormat( const QString&, const QXmlAttributes& attributes, KWord13StackItem *stackItem )
{
    // ### TODO: check parent?
    if ( stackItem->elementType == KWord13TypeIgnore )
    {
        return true;
    }
    else if ( stackItem->elementType == KWord13TypeLayout )
    {
        stackItem->elementType = KWord13TypeLayoutFormatOne;
        return true; // Everything is done directly on the layout
    }
    else if ( stackItem->elementType != KWord13TypeFormatsPlural )
    {
        kdError(30520) << "<FORMAT> is child neither of <FORMATS> nor of <LAYOUT> nor of <STYLE>! Aborting!" << endl;
        return false; // Assume parsing error!
    }
        
    stackItem->elementType = KWord13TypeFormat;
    
    if ( m_currentFormat )
    {
        kdWarning(30520) << "Current format already defined!" << endl;
        delete m_currentFormat;
        m_currentFormat = 0;
    }
    
    bool ok = false;
    const int id = attributes.value( "id" ).toInt( &ok );
    
    if ( id == 1 && ok ) // Normal text
    {
        KWord13FormatOne* one = new KWord13FormatOne;
        const int len = attributes.value( "len" ).toInt( &ok );
        if ( ok )
            one->m_length = len;
        m_currentFormat = one;
    }
    else if ( id == 4 && ok ) // Variable
    {
        stackItem->elementType = KWord13TypeVariable;
        m_currentFormat = new KWord13FormatFour;
    }
    else if ( id == 6 && ok ) // Anchor
    {
        stackItem->elementType = KWord13TypeAnchor;
        m_currentFormat = new KWord13FormatSix;
    }
    else
    {
        // ### TODO: provisory
        stackItem->elementType = KWord13TypeIgnore;
        m_currentFormat = new KWord13Format;
        if ( ok )
            m_currentFormat->m_id = id;
    }
    const int pos = attributes.value( "pos" ).toInt( &ok );
    if ( ok )
    {
        m_currentFormat->m_pos = pos;
    }
    else
    {
        kdWarning(30520) << "Cannot set position of <FORMAT>: " << attributes.value( "pos" ) << endl;
        return false; // Assume parse error!
    }
    
    kdDebug(30520) << "<FORMAT id=\"" << id << "\" pos=\"" << pos << "\" len=\"" << attributes.value( "len" ) << "\">" << endl;
        
    return true;    
}

bool KWord13Parser::startElementLayout( const QString&, const QXmlAttributes& attributes, KWord13StackItem *stackItem )
{
    // ### TODO: check parent?
    if ( stackItem->elementType == KWord13TypeIgnore )
    {
        return true;
    }
    
    stackItem->elementType = KWord13TypeLayout;
    
    if ( m_currentFormat )
    {
        kdWarning(30520) << "Current format defined! (Layout)" << endl;
        delete m_currentFormat;
        m_currentFormat = 0;
    }
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

bool KWord13Parser::startElementParagraph( const QString&, const QXmlAttributes&, KWord13StackItem *stackItem )
{
    if ( stackItem->elementType == KWord13TypeUnknownFrameset )
    {
        stackItem->elementType = KWord13TypeIgnore;
        return true;
    }

    stackItem->elementType = KWord13TypeParagraph;
    
    if ( m_currentParagraph )
    {
        // Delete an eventually already existing paragraph (should not happen)
        kdWarning(30520) << "Current paragraph already defined!" << endl;
        delete m_currentParagraph;
    }
        
    m_currentParagraph = new KWord13Paragraph;
    
    return true;
}

bool KWord13Parser::startElementFrame( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem )
{
    if ( stackItem->elementType == KWord13TypeFrameset || stackItem->elementType == KWord13TypePictureFrameset )
    {
        stackItem->elementType = KWord13TypeEmpty;
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
    else if ( stackItem->elementType != KWord13TypeUnknownFrameset )
    {
        kdError(30520) << "<FRAME> not child of <FRAMESET>" << endl;
        return false;
    }
    return true;
}

bool KWord13Parser::startElementFrameset( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem )
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
        stackItem->elementType = KWord13TypeFrameset;
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
        else
        {
            kdError(30520) << "Unknown text frameset!" << endl;
            m_kwordDocument->m_otherFramesetList.append( frameset );
            stackItem->m_currentFrameset = m_kwordDocument->m_otherFramesetList.current();
        }
    }
    else if ( ( frameType == 2 ) // picture or image
        || ( frameType == 5 ) ) // ciipart
    {
        if ( !frameInfo )
        {
            kdWarning(30520) << "Unknown FrameInfo for pictures: " << frameInfo << endl;
        }
        stackItem->elementType = KWord13TypePictureFrameset;
        KWord13PictureFrameset* frameset = new KWord13PictureFrameset( frameType, frameInfo, attributes.value( "name" ) );
        m_kwordDocument->m_otherFramesetList.append( frameset );
        stackItem->m_currentFrameset = m_kwordDocument->m_otherFramesetList.current();
    }
    // ### frameType == 6 : horizontal line (however KWord did not save it correctly)
    // ### frameType == 4 : formula
    // ### frametype == 3 : embedded (but only in <SETTINGS>)
    else
    {
        // Frame of unknown/unsupported type
        kdWarning(30520) << "Unknown/unsupported <FRAMESET> type! Type: " << frameTypeStr << " Info: " << frameInfoStr << endl;
        stackItem->elementType = KWord13TypeUnknownFrameset;
        KWord13Frameset* frameset = new KWord13Frameset( frameType, frameInfo, attributes.value( "name" ) );
        m_kwordDocument->m_otherFramesetList.append( frameset );
        stackItem->m_currentFrameset = m_kwordDocument->m_otherFramesetList.current();
    }
    return true;
}


bool KWord13Parser::startElementDocumentAttributes( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem,
     const KWord13StackItemType& allowedParentType, const KWord13StackItemType& newType )
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

bool KWord13Parser::startElementKey( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem )
{
    const QString key( calculatePictureKey( attributes.value( "filename" ),
            attributes.value( "year" ), attributes.value( "month" ),  attributes.value( "day" ),
            attributes.value( "hour" ), attributes.value( "minute" ), attributes.value( "second" ),
            attributes.value( "msec" ) ) );
    kdDebug(30520) << "Picture key: " << key << endl;
            
    if ( stackItem->elementType == KWord13TypePicturesPlural )
    {
        KWord13Picture* pic = new KWord13Picture;
        pic->m_storeName = attributes.value( "name" );
        if ( pic->m_storeName.isEmpty() )
        {
            kdError(30520) << "Picture defined without store name! Aborting!" << endl;
            return false; // Assume parse error
        }
        // ### TODO: catch duplicate keys (should not happen but who knows?)
        m_kwordDocument->m_pictureDict.insert( key, pic );
    }
    else if ( stackItem->elementType == KWord13TypePicture )
    {
        // ### TODO: error messages?
        if ( stackItem->m_currentFrameset )
        {
            stackItem->m_currentFrameset->setKey( key );
        }
    }
    else
    {
        // Neither child of <PICTURES>, <PIXMAPS>, <CLIPARTS>
        // nor of <PICTURE>, <IMAGE>, <CLIPART>
        // ### TODO: parse error?
    }
    return true;
}

bool KWord13Parser::startElementAnchor( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem )
{
    if ( stackItem->elementType == KWord13TypeAnchor )
    {
        const QString anchorType ( attributes.value( "type" ) );
	if ( anchorType == "grpMgr" )
	    kdWarning(30520) << "Anchor of type grpMgr! Not tested!" << endl; // ### TODO
        else if ( anchorType != "frameset" )
	{
	    kdError(30520) << "Unsupported anchor type: " << anchorType << endl;
	    return false;
	} 
	const QString frameset ( attributes.value( "instance" ) );
	if ( frameset.isEmpty() )
	{
	    kdError(30520) << "Anchor to an empty frameset name! Aborting!" << endl;
	    return false;
	}
	if ( m_currentFormat )
	{
	    KWord13FormatSix* six = (KWord13FormatSix*) m_currentFormat;
	    six->m_anchorName = frameset;
	}
	// add frameset name to the list of anchored framesets
	if ( m_kwordDocument->m_anchoredFramesetNames.find( frameset ) == m_kwordDocument->m_anchoredFramesetNames.end() )
	{
	    m_kwordDocument->m_anchoredFramesetNames.append( frameset );
	}
    }
    else
    {
        kdError(30520) << "Anchor not child of <FORMAT id=\"6\"> Aborting!" << endl;
	return false;
    }
    return true;
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
    KWord13StackItem *stackItem=new KWord13StackItem(*parserStack.current());

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
        if ( stackItem->elementType == KWord13TypeParagraph && m_currentParagraph )
        {
            stackItem->elementType = KWord13TypeText;
            m_currentParagraph->setText( QString::null );
        }
        else
        {
            stackItem->elementType = KWord13TypeIgnore;
        }
        success = true;
    }
    else if ( name == "NAME" )
    {
        success = startElementName( name, attributes, stackItem );
    }
    else if ( name == "FORMATS" )
    {
        if ( stackItem->elementType == KWord13TypeParagraph && m_currentParagraph )
        {
            stackItem->elementType = KWord13TypeFormatsPlural;
        }
        else
        {
            stackItem->elementType = KWord13TypeIgnore;
        }
        success = true;
    }
    else if ( name == "PARAGRAPH" )
    {
        success = startElementParagraph( name, attributes, stackItem );
    }
    else if ( name == "FORMAT" )
    {
        success = startElementFormat( name, attributes, stackItem );
    }
    else if (name == "LAYOUT" )
    {
        success = startElementLayout( name, attributes, stackItem );
    }
    else if ( name == "TYPE" )
    {
        // ### TEMPORARY
        if ( m_currentFormat && ( stackItem->elementType == KWord13TypeVariable ) )
        {
            ( (KWord13FormatFour*) m_currentFormat ) -> m_text =  attributes.value( "text" );
        }
        success = true;
    }
    else if ( name == "KEY" )
    {
        success = startElementKey( name, attributes, stackItem );
    }
    else if ( name == "ANCHOR" )
    {
        success = startElementAnchor( name, attributes, stackItem );
    }
    else if ( name == "PICTURE" || name == "IMAGE" || name == "CLIPART" )
    {
        // ### TODO: keepAspectRatio (but how to transform it to OASIS)
        if ( stackItem->elementType == KWord13TypePictureFrameset )
        {
            stackItem->elementType = KWord13TypePicture;
        }
        success = true;
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
        success = startElementDocumentAttributes( name, attributes, stackItem, KWord13TypeBottom, KWord13TypeDocument );
    }
    else if  ( name == "PAPER") 
    {
        success = startElementDocumentAttributes( name, attributes, stackItem, KWord13TypeDocument, KWord13TypePaper );
    }
    else if ( name == "PAPERBORDERS" )
    {
        success = startElementDocumentAttributes( name, attributes, stackItem, KWord13TypePaper, KWord13TypeEmpty );
    }
    else if ( ( name == "ATTRIBUTES" ) || ( name == "VARIABLESETTINGS" )
         || ( name == "FOOTNOTESETTINGS" ) || ( name == "ENDNOTESETTINGS" ) )
    {
        success = startElementDocumentAttributes( name, attributes, stackItem, KWord13TypeDocument, KWord13TypeEmpty );
    }
    else if ( name == "FRAMESTYLE" )
    {
        // ### TODO, but some of the <STYLE> children are also children of <FRAMESTYLE>, so we have to set it to "ignore"
        stackItem->elementType = KWord13TypeIgnore;
        success = true;
    }
    else if ( name == "PICTURES" || name == "PIXMAPS" || name == "CLIPARTS" )
    {
        // We just need a separate "type" for the <KEY> children
        stackItem->elementType = KWord13TypePicturesPlural;
        success = true;
    }
    else
    {
        stackItem->elementType = KWord13TypeUnknown;
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
    
    KWord13StackItem *stackItem=parserStack.pop();
        
    if ( name == "PARAGRAPH" )
    {
        if ( stackItem->m_currentFrameset && m_currentParagraph )
        {
            if ( stackItem->m_currentFrameset->addParagraph( *m_currentParagraph ) )
            {
                success = true;
            }
            // ### HACK: do not delete the data of <FORMATS>
            m_currentParagraph->m_formats.setAutoDelete( false );
        }
        else if ( stackItem->elementType == KWord13TypeIgnore )
        {
            success = true;
        }
        delete m_currentParagraph;
        m_currentParagraph = 0;
    }
    else if ( name == "FORMAT" )
    {
        if ( stackItem->elementType == KWord13TypeFormat )
        {
            if ( m_currentParagraph )
            {
                m_currentParagraph->m_formats.append( m_currentFormat );
                kdDebug(30520) << "Adding to <FORMATS>: " << ((void*) m_currentFormat) << endl;
                m_currentFormat = 0;
            }
            else
            {
                kdError(30520) << "No paragraph to store <FORMAT>! Aborting!" << endl;
                delete m_currentFormat;
                m_currentFormat = 0;
                return false; // Assume parsing error!
            }

        }
        else if ( stackItem->elementType == KWord13TypeLayoutFormatOne )
        {
            // Nothing to do!
        }
        success = true;
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

    KWord13StackItem *stackItem = parserStack.current();

    if ( stackItem->elementType == KWord13TypeText )
    { 
        // <TEXT>
        if ( m_currentParagraph )
        {
            bool found = false; // Some unexpected control character?
            // ### TODO: this is perhaps a good point to check for non-XML characters
            const uint length = ch.length();
            for ( uint i = 0; i < length; ++i )
            {
                const ushort uni = ch.at(i).unicode();
                if ( uni >= 32 )
                    continue; // Normal character
                else if ( uni == 9 || uni == 10 || uni == 13)
                    continue; // Tabulator, Line Feed, Carriage Return
                else if ( uni == 1 )
                {
                    // Old KWord documents have a QChar(1) as anchor character
                    // So replace it with the anchor character of recent KWord versions
                    ch[i]='#';
                }
                else
                {
                    ch[i]='?';
                    found = true;
                }
            }
            if ( found )
                kdWarning(30520) << "Unexcepted control characters found in text!" << endl;
            m_currentParagraph->appendText( ch );
            success = true;
        }
        else
        {
            kdError(30520) << "No current paragraph defined! Tag mismatch?" << endl;
            success = false;
        }
    }
    else if (stackItem->elementType==KWord13TypeEmpty)
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

bool KWord13Parser::warning(const QXmlParseException& exception)
{
    kdWarning(30520) << "XML parsing warning: line " << exception.lineNumber()
        << " col " << exception.columnNumber() << " message: " << exception.message() << endl;
    return true;
}

bool KWord13Parser::error(const QXmlParseException& exception)
{
    // A XML error is recoverable, so it is only a KDE warning
    kdWarning(30520) << "XML parsing error: line " << exception.lineNumber()
        << " col " << exception.columnNumber() << " message: " << exception.message() << endl;
    return true;
}

bool KWord13Parser::fatalError (const QXmlParseException& exception)
{
    kdError(30520) << "XML parsing fatal error: line " << exception.lineNumber()
        << " col " << exception.columnNumber() << " message: " << exception.message() << endl;
    // ### TODO: user message box
    return false; // Stop parsing now, we do not need further errors.
}

QString KWord13Parser::calculatePictureKey( const QString& filename,
     const QString& year,  const QString& month,  const QString& day,
     const QString& hour,  const QString& minute,  const QString& second,
     const QString& microsecond ) const
{
    bool ok;
    bool globalOk = true;
    
    ok = false;
    const int iYear = year.toInt( & ok );
    globalOk = globalOk && ok; 

    ok = false;
    const int iMonth = month.toInt( & ok );
    globalOk = globalOk && ok; 

    ok = false;
    const int iDay = day.toInt( & ok );
    globalOk = globalOk && ok; 

    ok = false;
    const int iHour = hour.toInt( & ok );
    globalOk = globalOk && ok; 

    ok = false;
    const int iMinute = minute.toInt( & ok );
    globalOk = globalOk && ok; 

    ok = false;
    const int iSecond = second.toInt( & ok );
    globalOk = globalOk && ok; 

    ok = false;
    const int iMicrosecond = microsecond.toInt( & ok );
    globalOk = globalOk && ok; 
    
    if ( globalOk )
    {
        // No error until then, so check if the date/time is valid at all
        globalOk = globalOk && QDate::isValid( iYear, iMonth, iDay );
        globalOk = globalOk && QTime::isValid( iHour, iMinute, iSecond, iMicrosecond ); 
    }

    QDateTime dt;
    if ( globalOk )
    {
        // The date/time seems correct
        dt = QDateTime( QDate( iYear, iMonth, iDay ), QTime( iHour, iMinute, iSecond, iMicrosecond ) );
    }
    else
    {
        // *NIX epoch (We do not really care if it is UTC or local time)
        dt = QDateTime( QDate( 1970, 1, 1 ), QTime( 0, 0, 0, 0 ) );
    }
    
    // We put the date/time first, as if the date is useful, it will have faster a difference than a path
    // where the common pth might be very long.
    
    // Output the date/time as compact as possible
    QString result ( dt.toString( "yyyyMMddhhmmsszzz" ) );
    result += '@'; // A separator
    result += filename;
    return result;
}

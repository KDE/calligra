//

/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef KWORD_1_3_PARSER_H
#define KWORD_1_3_PARSER_H

#include <qxml.h>
#include <qptrstack.h>

class KWord13Paragraph;
class KWord13Document;
class KWord13Frameset;
class KWord13Layout;
class KWord13FormatOne;

// ### TODO: stackItem and friends should be renamed to become unique (see AbiWord import filter), as Doxygen cannot handle multiple classes with the same name.

/**
 * Type of element
 *
 * Note: we do not care of the tags: \<FRAMESETS\>, \<STYLES\>
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
    ElementTypeText,        ///< \<TEXT\>
    ElementTypeLayout,      ///< \<STYLE\> and \<LAYOUT\>
    ElementTypeFormatOne,    ///< \<FORMATS id="1"\>, not child of \<LAYOUT\>
    ElementTypeLayoutFormatOne    ///< \<FORMATS id="1"\> as child of \<LAYOUT\>
};

class StackItem
{
public:
    StackItem();
    ~StackItem();
public:
    QString itemName;   ///< Name of the tag (only for error purposes)
    StackItemElementType elementType;
    KWord13Frameset* m_currentFrameset;
};

class StackItemStack : public QPtrStack<StackItem>
{
public:
        StackItemStack(void) { }
        ~StackItemStack(void) { }
};

class KWord13Parser : public QXmlDefaultHandler
{
public:
    KWord13Parser( KWord13Document* kwordDocument );
    virtual ~KWord13Parser( void );
public:
    /// Process opening tag
    virtual bool startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes);
    /// Process closing tag
    virtual bool endElement( const QString&, const QString& , const QString& qName);
    /// Process element's characters (between opening and closing tags)
    virtual bool characters ( const QString & ch );
protected:
    /// Process children of \<FORMAT id="1"\>
    bool startElementFormatOneProperty( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem);
    /// Process children of \<LAYOUT\> (with exceptions)
    bool startElementLayoutProperty( const QString& name, const QXmlAttributes& attributes, StackItem *stackItem);
    /// Process \<NAME\>
    bool startElementName( const QString&, const QXmlAttributes& attributes, StackItem *stackItem );
    /// Process \<FORMAT\>
    bool startElementFormat( const QString&, const QXmlAttributes& attributes, StackItem *stackItem );
    /// Process \<LAYOUT\> and \<STYLE\>
    bool startElementLayout( const QString&, const QXmlAttributes&, StackItem *stackItem );
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
    KWord13Document* m_kwordDocument;
    KWord13Paragraph* m_currentParagraph; ///< Current paragraph
    KWord13Layout* m_currentLayout; ///< Current layout (or style)
    KWord13FormatOne* m_currentFormat; ///< Current character format 
};

#endif // KWORD_1_3_PARSER_H

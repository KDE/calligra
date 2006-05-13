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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWORD_1_3_PARSER_H
#define KWORD_1_3_PARSER_H

#include <QtXml>
#include <q3ptrstack.h>

class KWord13Paragraph;
class KWord13Document;
class KWord13Frameset;
class KWord13Layout;
class KWord13Format;

/**
 * Type of element
 *
 * Note: we do not care of the tags: \<FRAMESETS\>, \<STYLES\>
 */
enum KWord13StackItemType
{
    KWord13TypeUnknown  = 0,
    KWord13TypeBottom,      ///< Bottom of the stack
    KWord13TypeIgnore,      ///< Element is known but ignored
    KWord13TypeEmpty,       ///< Element is empty
    KWord13TypeDocument,    ///< Element is the document ( \<DOC\> )
    KWord13TypePaper,       ///< \<PAPER\>
    KWord13TypeFrameset,    ///< \<FRAMESET\> (with exceptions)
    KWord13TypeUnknownFrameset, ///< a \<FRAMESET\> which is not supported
    KWord13TypeParagraph,   ///< \<PARAGRAPH\>
    KWord13TypeText,        ///< \<TEXT\>
    KWord13TypeLayout,      ///< \<STYLE\> and \<LAYOUT\>
    KWord13TypeFormat,      ///< \<FORMAT\>, child of \<FORMATS\>
    KWord13TypeLayoutFormatOne,///< \<FORMAT id="1"\> as child of \<LAYOUT\>
    KWord13TypeFormatsPlural,///< \<FORMATS\>, as child of \<PARAGRAPH\>
    KWord13TypeVariable,        ///< \<FORMAT id="4"\> or \<VARIABLE\>
    KWord13TypePicturesPlural,  ///< \<PICTURES\>, \<PIXMAPS\> or \<CLIPARTS\>
    KWord13TypePictureFrameset, ///< \<FRAMESET typeInfo="2"\> (picture, image, clipart)
    KWord13TypePicture,         ///<  \<PICTURE\>, \<IMAGE\> or \<CLIPART\>
    KWord13TypeAnchor           ///< \<FORMAT id="6"\>
};

class KWord13StackItem
{
public:
    KWord13StackItem();
    ~KWord13StackItem();
public:
    QString itemName;   ///< Name of the tag (only for error purposes)
    KWord13StackItemType elementType;
    KWord13Frameset* m_currentFrameset;
};

class KWord13StackItemStack : public Q3PtrStack<KWord13StackItem>
{
public:
        KWord13StackItemStack(void) { }
        ~KWord13StackItemStack(void) { }
};

class KWord13Parser : public QXmlDefaultHandler
{
public:
    KWord13Parser( KWord13Document* kwordDocument );
    virtual ~KWord13Parser( void );
protected: //QXml
    /// Process opening tag
    virtual bool startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes);
    /// Process closing tag
    virtual bool endElement( const QString&, const QString& , const QString& qName);
    /// Process element's characters (between opening and closing tags)
    virtual bool characters ( const QString & ch );
    virtual bool warning(const QXmlParseException& exception);
    virtual bool error(const QXmlParseException& exception);
    virtual bool fatalError(const QXmlParseException& exception);
protected:
    /// Process children of \<FORMAT id="1"\>
    bool startElementFormatOneProperty( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem);
    /// Process children of \<LAYOUT\> (with exceptions)
    bool startElementLayoutProperty( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem);
    /// Process \<NAME\>
    bool startElementName( const QString&, const QXmlAttributes& attributes, KWord13StackItem *stackItem );
    /// Process \<FORMAT\>
    bool startElementFormat( const QString&, const QXmlAttributes& attributes, KWord13StackItem *stackItem );
    /// Process \<LAYOUT\> and \<STYLE\>
    bool startElementLayout( const QString&, const QXmlAttributes&, KWord13StackItem *stackItem );
    /// Process \<PARAGRAPH\>
    bool startElementParagraph( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem );
    /// Process \<FRAME\>
    bool startElementFrame( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem );
    /// Process \<FRAMESET\>
    bool startElementFrameset( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem );
    /// Process opening tag of some elements that are children of \<DOC\> and which only define document properties
    bool startElementDocumentAttributes( const QString& name, const QXmlAttributes& attributes,
        KWord13StackItem *stackItem, const KWord13StackItemType& allowedParentType, const KWord13StackItemType& newType );
    /// Process \<KEY\>
    bool startElementKey( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem );
    /// Process \<ANCHOR\>
    bool startElementAnchor( const QString& name, const QXmlAttributes& attributes, KWord13StackItem *stackItem );
    /**
     * Get a picture key out of the individual \<KEY\> attributes
     *
     * \note The generated key is different that one of the class KoPictureKey
     */
    QString calculatePictureKey( const QString& filename,
     const QString& year,  const QString& month,  const QString& day,
     const QString& hour,  const QString& minute,  const QString& second,
     const QString& microsecond ) const;
protected:
    QString indent; //DEBUG
    KWord13StackItemStack parserStack;
    KWord13Document* m_kwordDocument;
    KWord13Paragraph* m_currentParagraph; ///< Current paragraph
    KWord13Layout* m_currentLayout; ///< Current layout (or style)
    KWord13Format* m_currentFormat; ///< Current format 
};

#endif // KWORD_1_3_PARSER_H

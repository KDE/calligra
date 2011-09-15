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

#ifndef WORDS_1_3_PARSER_H
#define WORDS_1_3_PARSER_H

#include <QtXml>
#include <q3ptrstack.h>

class Words13Paragraph;
class Words13Document;
class Words13Frameset;
class Words13Layout;
class Words13Format;

/**
 * Type of element
 *
 * Note: we do not care of the tags: \<FRAMESETS\>, \<STYLES\>
 */
enum Words13StackItemType {
    Words13TypeUnknown  = 0,
    Words13TypeBottom,      ///< Bottom of the stack
    Words13TypeIgnore,      ///< Element is known but ignored
    Words13TypeEmpty,       ///< Element is empty
    Words13TypeDocument,    ///< Element is the document ( \<DOC\> )
    Words13TypePaper,       ///< \<PAPER\>
    Words13TypeFrameset,    ///< \<FRAMESET\> (with exceptions)
    Words13TypeUnknownFrameset, ///< a \<FRAMESET\> which is not supported
    Words13TypeParagraph,   ///< \<PARAGRAPH\>
    Words13TypeText,        ///< \<TEXT\>
    Words13TypeLayout,      ///< \<STYLE\> and \<LAYOUT\>
    Words13TypeFormat,      ///< \<FORMAT\>, child of \<FORMATS\>
    Words13TypeLayoutFormatOne,///< \<FORMAT id="1"\> as child of \<LAYOUT\>
    Words13TypeFormatsPlural,///< \<FORMATS\>, as child of \<PARAGRAPH\>
    Words13TypeVariable,        ///< \<FORMAT id="4"\> or \<VARIABLE\>
    Words13TypePicturesPlural,  ///< \<PICTURES\>, \<PIXMAPS\> or \<CLIPARTS\>
    Words13TypePictureFrameset, ///< \<FRAMESET typeInfo="2"\> (picture, image, clipart)
    Words13TypePicture,         ///<  \<PICTURE\>, \<IMAGE\> or \<CLIPART\>
    Words13TypeAnchor           ///< \<FORMAT id="6"\>
};

class Words13StackItem
{
public:
    Words13StackItem();
    ~Words13StackItem();
public:
    QString itemName;   ///< Name of the tag (only for error purposes)
    Words13StackItemType elementType;
    Words13Frameset* m_currentFrameset;
};

class Words13StackItemStack : public Q3PtrStack<Words13StackItem>
{
public:
    Words13StackItemStack(void) { }
    ~Words13StackItemStack(void) { }
};

class Words13Parser : public QXmlDefaultHandler
{
public:
    explicit Words13Parser(Words13Document* kwordDocument);
    virtual ~Words13Parser(void);
protected: //QXml
    /// Process opening tag
    virtual bool startElement(const QString&, const QString&, const QString& name, const QXmlAttributes& attributes);
    /// Process closing tag
    virtual bool endElement(const QString&, const QString& , const QString& qName);
    /// Process element's characters (between opening and closing tags)
    virtual bool characters(const QString & ch);
    virtual bool warning(const QXmlParseException& exception);
    virtual bool error(const QXmlParseException& exception);
    virtual bool fatalError(const QXmlParseException& exception);
protected:
    /// Process children of \<FORMAT id="1"\>
    bool startElementFormatOneProperty(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem);
    /// Process children of \<LAYOUT\> (with exceptions)
    bool startElementLayoutProperty(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem);
    /// Process \<NAME\>
    bool startElementName(const QString&, const QXmlAttributes& attributes, Words13StackItem *stackItem);
    /// Process \<FORMAT\>
    bool startElementFormat(const QString&, const QXmlAttributes& attributes, Words13StackItem *stackItem);
    /// Process \<LAYOUT\> and \<STYLE\>
    bool startElementLayout(const QString&, const QXmlAttributes&, Words13StackItem *stackItem);
    /// Process \<PARAGRAPH\>
    bool startElementParagraph(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem);
    /// Process \<FRAME\>
    bool startElementFrame(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem);
    /// Process \<FRAMESET\>
    bool startElementFrameset(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem);
    /// Process opening tag of some elements that are children of \<DOC\> and which only define document properties
    bool startElementDocumentAttributes(const QString& name, const QXmlAttributes& attributes,
                                        Words13StackItem *stackItem, const Words13StackItemType& allowedParentType, const Words13StackItemType& newType);
    /// Process \<KEY\>
    bool startElementKey(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem);
    /// Process \<ANCHOR\>
    bool startElementAnchor(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem);
    /**
     * Get a picture key out of the individual \<KEY\> attributes
     *
     * \note The generated key is different that one of the class PictureKey
     */
    QString calculatePictureKey(const QString& filename,
                                const QString& year,  const QString& month,  const QString& day,
                                const QString& hour,  const QString& minute,  const QString& second,
                                const QString& microsecond) const;
protected:
    QString indent; //DEBUG
    Words13StackItemStack parserStack;
    Words13Document* m_kwordDocument;
    Words13Paragraph* m_currentParagraph; ///< Current paragraph
    Words13Layout* m_currentLayout; ///< Current layout (or style)
    Words13Format* m_currentFormat; ///< Current format
};

#endif // WORDS_1_3_PARSER_H

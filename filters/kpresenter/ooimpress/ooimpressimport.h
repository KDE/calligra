/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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

#ifndef OoImpress_IMPORT_H__
#define OoImpress_IMPORT_H__

#include <koFilter.h>

#include <qdom.h>
#include <qdict.h>
#include <qptrlist.h>

/**
 *  This class implements a stack for the different styles of an object.
 *
 *  There can be several styles that are valid for one object. For example
 *  a textobject on a page has styles 'pr3' and 'P7' and a paragraph in
 *  that textobject has styles 'P1' and 'T3'. And some styles even have
 *  parent-styles...
 *
 *  If you want to know if there is, for example,  the attribute 'fo:font-family'
 *  for this paragraph, you have to look into style 'T3', 'P1', 'P7' and 'pr3'.
 *  When you find this attribute in one style you have to stop processing the list
 *  and take the found attribute for this object.
 *
 *  This is what this class does. You can push styles on the stack while walking
 *  through the xml-tree to your object and then ask the stack if any of the styles
 *  provides a certain attribute. The stack will search from top to bottom, i.e.
 *  in our example from 'T3' to 'pr3' and return the first occurrence of the wanted
 *  attribute.
 *
 *  So this is some sort of inheritance where the styles on top of the stack overwrite
 *  the same attribute of a lower style on the stack.
 */
class StyleStack
{
public:
    StyleStack();
    virtual ~StyleStack();

    /**
     * Clears the complete stack.
     */
    void clear();

    /**
     * Keep only the page styles on the stack.
     */
    void clearPageMark();

    /**
     * Set the mark (stores the index of the object on top of the stack).
     */
    void setPageMark();

    /**
     * Keep page and object styles on the stack.
     */
    void clearObjectMark();

    /**
     * Set the mark (stores the index of the object on top of the stack).
     */
    void setObjectMark();

    /**
     * Removes the style on top of the stack.
     */
    void pop();

    /**
     * Pushs the new style onto the stack.
     */
    void push( const QDomElement* style );

    /**
     * Check if any of the styles on the stack has an attribute called 'name'.
     */
    bool hasAttribute( const QString& name );

    /**
     * Search for the attribute called 'name', starting on top of the stack,
     * and return it.
     */
    QString attribute( const QString& name );

private:
    uint m_pageMark, m_objectMark;

    // We use QPtrList instead of QPtrStack because we need access to all styles
    // not only the top one.
    QPtrList<QDomElement> m_stack;
};

class OoImpressImport : public KoFilter
{
    Q_OBJECT
public:
    OoImpressImport( KoFilter * parent, const char * name, const QStringList & );
    virtual ~OoImpressImport();

    virtual KoFilter::ConversionStatus convert( QCString const & from, QCString const & to );

private:
    void createDocumentInfo( QDomDocument &docinfo );
    void createDocumentContent( QDomDocument &doccontent );
    void createStyleMap( QDomDocument &docstyles );
    void insertDraws( const QDomElement& styles );
    void insertStyles( const QDomElement& styles );
    void fillStyleStack( const QDomElement& object );
    void addStyles( const QDomElement* style );
    void storeObjectStyles( const QDomElement& object );
    void append2DGeometry( QDomDocument& doc, QDomElement& e, const QDomElement& object, int offset );
    void appendLineGeometry( QDomDocument& doc, QDomElement& e, const QDomElement& object, int offset );
    void appendPie( QDomDocument& doc, QDomElement& e, const QDomElement& object );
    void appendImage( QDomDocument& doc, QDomElement& e, QDomElement& p, const QDomElement& object );
    void appendBackgroundImage( QDomDocument& doc, QDomElement& e, QDomElement& p, const QDomElement& object );
    void appendBackgroundGradient( QDomDocument& doc, QDomElement& e, const QDomElement& object );
    void appendRounding( QDomDocument& doc, QDomElement& e, const QDomElement& object );
    void appendPen( QDomDocument& doc, QDomElement& e );
    void appendBrush( QDomDocument& doc, QDomElement& e );
    void appendShadow( QDomDocument& doc, QDomElement& e );
    void appendLineEnds( QDomDocument& doc, QDomElement& e );
    void appendTextObjectMargin( QDomDocument& doc, QDomElement& e );

    double toPoint( QString value );

    QString storeImage( const QDomElement& object );
    QDomElement parseTextBox( QDomDocument& doc, const QDomElement& textBox );
    QDomElement parseList( QDomDocument& doc, const QDomElement& paragraph );
    QDomElement parseParagraph( QDomDocument& doc, const QDomElement& list );
    KoFilter::ConversionStatus openFile();

    int m_numPicture;
    QDomDocument    m_content;
    QDomDocument    m_meta;
    QDomDocument    m_settings;
    QDict<QDomElement> m_styles, m_draws;
    StyleStack m_styleStack;
};

#endif


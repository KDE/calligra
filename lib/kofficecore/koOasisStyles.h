/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KOOASISSTYLES_H
#define KOOASISSTYLES_H

#include <qdom.h>
#include <qdict.h>
#include <qvaluevector.h>
#include <qmap.h>

/**
 * Repository of styles used during loading of OASIS/OOo file
 */
class KoOasisStyles
{
public:
    KoOasisStyles();
    ~KoOasisStyles();

    /// Look into @p doc for styles and remember them
    void createStyleMap( const QDomDocument& doc );

    /// @return all styles (with tag "style:style", "style:page-layout" or "style:presentation-page-layout")
    /// hashed by name
    const QDict<QDomElement>& styles() const { return m_styles; }

    /// @return the style:styles that are "user styles", i.e. those from office:styles
    /// styles() is used for lookup. userStyles() is used to load all user styles upfront.
    QValueVector<QDomElement> userStyles() const;

    /// @return the default style
    const QDomElement& defaultStyle() const { return m_defaultStyle; }

    /// @return the office:style element
    const QDomElement& officeStyle() const { return m_officeStyle; }

    /// @return all list styles ("text:list-style" elements), hashed by name
    const QDict<QDomElement>& listStyles() const { return m_listStyles; }

    /// @return master pages ("style:master-page" elements), hashed by name
    const QDict<QDomElement>& masterPages() const { return m_masterPages; }

    /// @return draw styles ("draw:name" element), hashed by name
    const QDict<QDomElement>& drawStyles() const { return m_drawStyles; }


    typedef QMap<QString, QString> DataFormatsMap;
    /// Value (date/time/number...) formats found while parsing styles. Used e.g. for fields.
    /// Key: format name. Value:
    const DataFormatsMap& dataFormats() const { return m_dataFormats; }

protected:
    /// Add styles to styles map
    void insertStyles( const QDomElement& styles );

private:
    void insertOfficeStyles( const QDomElement& styles );
    void insertStyle( const QDomElement& style );
    void importDataStyle( const QDomElement& parent );

    KoOasisStyles( const KoOasisStyles & ); // forbidden
    KoOasisStyles& operator=( const KoOasisStyles & ); // forbidden

private:
    QDict<QDomElement>   m_styles;
    QDomElement m_defaultStyle;
    QDomElement m_officeStyle;

    QDict<QDomElement>   m_masterPages;
    QDict<QDomElement>   m_listStyles;

    QDict<QDomElement>   m_drawStyles;
    DataFormatsMap m_dataFormats; // maybe generalize to include number formats.

    class Private;
    Private *d;
};

#endif /* KOOASISSTYLES_H */

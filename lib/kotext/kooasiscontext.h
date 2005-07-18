/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

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

#ifndef KOOASISCONTEXT_H
#define KOOASISCONTEXT_H

class KoVariableSettings;
class KoXmlWriter;
class KoTextParag;
class KoParagStyle;
class KoGenStyles;
class QDomElement;
class KoDocument;
class KoOasisStyles;
class KoPictureCollection;
class KoStore;
class KoVariableCollection;

#include <koStyleStack.h>
#include "koliststylestack.h"
#include <qmap.h>
#include <koffice_export.h>
#include <qstringlist.h>

/**
 * Used during loading of Oasis format (and discarded at the end of the loading).
 * In addition to keeping a reference to the KoOasisStyles instance, this class
 * also has 'state' information - a stack with the currently used styles
 * (with its ancestors in the stack), another one with the list styles
 * currently applicable (one item in the stack per list level).
 *
 * @author David Faure <faure@kde.org>
 */
class KOTEXT_EXPORT KoOasisContext
{
public:
    /**
     * Stores reference to the KoOasisStyles parsed by KoDocument.
     * Make sure that the KoOasisStyles instance outlives this KoOasisContext instance.
     * (This is the case during loaiding, when using the KoOasisStyles given by KoDocument)
     * @param doc KoDocument, needed by some field variables
     * @param varColl reference to the collection that creates and stores variables (fields)
     * @param styles reference to the KoOasisStyles parsed by KoDocument
     * @param store pointer to store, if available, for e.g. loading images.
     */
    KoOasisContext( KoDocument* doc, KoVariableCollection& varColl,
                    KoOasisStyles& styles, KoStore* store );
    ~KoOasisContext();

    KoDocument* koDocument() { return m_doc; }
    KoVariableCollection& variableCollection() { return m_varColl; }
    KoStore* store() { return m_store; }

    KoOasisStyles& oasisStyles() { return m_styles; }
    KoStyleStack& styleStack() { return m_styleStack; }

    const QDomDocument& manifestDocument() const { return m_manifestDoc; }

    void fillStyleStack( const QDomElement& object, const char* nsURI, const char* attrName );
    void addStyles( const QDomElement* style );

    ///// List handling

    KoListStyleStack& listStyleStack() { return m_listStyleStack; }
    QString currentListStyleName() const { return m_currentListStyleName; }
    void setCurrentListStyleName( const QString& s ) { m_currentListStyleName = s; }

    /// Used for lists (numbered paragraphs)
    /// @return true on success (a list style was found and pushed)
    bool pushListLevelStyle( const QString& listStyleName, int level );
    /// Used for outline levels
    bool pushOutlineListLevelStyle( int level );

    /// Set cursor position (set by KoTextParag upon finding the processing instruction)
    void setCursorPosition( KoTextParag* cursorTextParagraph,
                            int cursorTextIndex );

    KoTextParag* cursorTextParagraph() const { return m_cursorTextParagraph; }
    int cursorTextIndex() const { return m_cursorTextIndex; }

private:
    /// @return true on success (a list style was found and pushed)
    bool pushListLevelStyle( const QString& listStyleName, const QDomElement& fullListStyle, int level );

private:
    KoDocument* m_doc;
    KoStore* m_store;
    KoVariableCollection& m_varColl;
    KoOasisStyles& m_styles;
    KoStyleStack m_styleStack;

    KoListStyleStack m_listStyleStack;
    QString m_currentListStyleName;

    KoTextParag* m_cursorTextParagraph;
    int m_cursorTextIndex;

    QDomDocument m_manifestDoc;

    class Private;
    Private *d;
};

/**
 * Used during saving to Oasis format (and discarded at the end of the saving).
 *
 * Among other things, this class acts as a repository of fonts used by a
 * document during saving, in order to create the office:font-face-decls element.
 *
 * @author David Faure <faure@kde.org>
 */
class KOTEXT_EXPORT KoSavingContext
{
public:
    enum SavingMode { Store, Flat };

    /// Constructor
    /// @param mainStyles
    /// @param settings optional, used for saving the page-number in the first paragraph
    /// @param hasColumns optional, used by KoParagLayout for the type of page breaks
    /// @param savingMode either Store (a KoStore will be used) or Flat (all data must be inline in the XML)
    KoSavingContext( KoGenStyles& mainStyles, KoVariableSettings* settings = 0, bool hasColumns = false, SavingMode savingMode = Store );

    ~KoSavingContext();


    KoGenStyles& mainStyles() { return m_mainStyles; }

    /// @return the saving mode: Store (a KoStore will be used) or Flat (all data must be inline in the XML)
    SavingMode savingMode() const { return m_savingMode; }

    typedef QMap<KoParagStyle*, QString> StyleNameMap;

    /// Called after saving the user styles.
    /// Associates every KoParagStyle with its automatic name (style:name attribute)
    void setStyleNameMap( const StyleNameMap& map ) { m_styleNameMap = map; }

    /// @return the automatic name for a KoParagStyle
    QString styleAutoName( KoParagStyle* style ) const {
        StyleNameMap::const_iterator it = m_styleNameMap.find( style );
        if ( it != m_styleNameMap.end() )
            return *it;
        return QString::null;
    }

    /// Set cursor position (so that KoTextParag can insert a processing instruction)
    void setCursorPosition( KoTextParag* cursorTextParagraph,
                            int cursorTextIndex );

    KoTextParag* cursorTextParagraph() const { return m_cursorTextParagraph; }
    int cursorTextIndex() const { return m_cursorTextIndex; }

    void addFontFace( const QString& fontName );
    typedef QMap<QString, bool> FontFaces;
    void writeFontFaces( KoXmlWriter& writer );

    // See KoParagLayout::saveOasis
    bool hasColumns() const { return m_hasColumns; }

    // See KoTextParag::saveOasis
    KoVariableSettings* variableSettings() const { return m_variableSettings; }

private:
    KoGenStyles& m_mainStyles;
    StyleNameMap m_styleNameMap;
    SavingMode m_savingMode;

    KoTextParag* m_cursorTextParagraph;
    int m_cursorTextIndex;
    FontFaces m_fontFaces;
    KoVariableSettings* m_variableSettings;
    bool m_hasColumns;

    class Private;
    Private *d;
};

#endif /* KOOASISCONTEXT_H */

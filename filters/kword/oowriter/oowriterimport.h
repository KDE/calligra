/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2003 David Faure <faure@kde.org>

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

#ifndef OoWriter_IMPORT_H__
#define OoWriter_IMPORT_H__

#include <koFilter.h>

#include <qdict.h>
#include <qdom.h>
#include <stylestack.h>
#include <liststylestack.h>

class KZip;

class OoWriterImport : public KoFilter
{
    Q_OBJECT
public:
    OoWriterImport( KoFilter * parent, const char * name, const QStringList & );
    virtual ~OoWriterImport();

    virtual KoFilter::ConversionStatus convert( QCString const & from, QCString const & to );

private:
    void prepareDocument( QDomDocument& mainDocument, QDomElement& framesetsElem );
    void writePageLayout( QDomDocument& mainDocument, const QString& masterPageName );
    void parseList( QDomDocument& doc, const QDomElement& list, QDomElement& currentFramesetElement );
    bool pushListLevelStyle( const QString& listStyleName, int level );
    bool pushListLevelStyle( const QString& listStyleName, QDomElement& fullListStyle, int level );
    void applyListStyle( QDomDocument& doc, QDomElement& layoutElement, const QDomElement& paragraph );
    QDomElement parseParagraph( QDomDocument& doc, const QDomElement& paragraph );
    void parseSpanOrSimilar( QDomDocument& doc, const QDomElement& parent, QDomElement& kwordParagraph, QDomElement& kwordFormats, QString& paragraphText, uint& pos);
    // Reads from m_styleStack, writes the text properties to parentElement
    void writeFormat( QDomDocument& doc, QDomElement& parentElement, int id, int pos, int length );
    // Reads from m_styleStack, writes the paragraph properties to layoutElement
    void writeLayout( QDomDocument& doc, QDomElement& layoutElement );
    void createInitialFrame( QDomElement& parentFramesetElem, int top, int bottom, bool headerFooter );
    void createStyles( QDomDocument &doc );
    void createDocumentInfo( QDomDocument &docinfo );
    void createDocumentContent( QDomDocument &doccontent, QDomElement& mainFramesetElement );
    void parseBodyOrSimilar( QDomDocument &doc, const QDomElement& parent, QDomElement& currentFramesetElement );
    KoFilter::ConversionStatus loadAndParse(const QString& filename, QDomDocument& doc);
    KoFilter::ConversionStatus openFile();
    bool createStyleMap( const QDomDocument & styles );
    void insertStyles( const QDomElement& element );
    void importDateTimeStyle( const QDomElement& parent );
    void fillStyleStack( const QDomElement& object, const QString& attrName );
    void addStyles( const QDomElement* style );
    QString appendPicture(QDomDocument& doc, const QDomElement& object);
    QString appendTextBox(QDomDocument& doc, const QDomElement& object);
    void importFrame( QDomElement& frameElementOut, const QDomElement& object, bool isText );
    void anchorFrameset( QDomDocument& doc, QDomElement& formats, uint pos, const QString& frameName );
    void appendField(QDomDocument& doc, QDomElement& outputFormats, QDomElement& object, uint pos);
    void appendKWordVariable(QDomDocument& doc, QDomElement& formats, const QDomElement& object, uint pos,
        const QString& key, int type, QDomElement& child);
    void appendBookmark( QDomDocument& doc, int paragId, int pos, const QString& name );
    void appendBookmark( QDomDocument& doc, int paragId, int pos, int endParagId, int endPos, const QString& name );
    void parseTable( QDomDocument &doc, const QDomElement& parent, QDomElement& currentFramesetElement );
    void parseInsideOfTable( QDomDocument &doc, const QDomElement& parent, QDomElement& currentFramesetElement,
        const QString& tableName, const QMemArray<double> & columnLefts, uint& row, uint& column );

    QDomDocument    m_content;
    QDomDocument    m_meta;
    QDomDocument    m_settings;
    QDomDocument    m_stylesDoc;

    QDict<QDomElement>   m_styles;
    QDict<QDomElement>   m_masterPages;
    QDict<QDomElement>   m_listStyles;

    StyleStack m_styleStack;
    QDomElement m_defaultStyle;
    ListStyleStack m_listStyleStack;
    QDomElement m_outlineStyle;
    bool m_insideOrderedList;
    bool m_nextItemIsListItem; // only the first elem inside list-item is numbered
    int m_restartNumbering;
    QString m_currentMasterPage;
    QDomElement m_currentFrameset; // set by parseBodyOrSimilar

    struct BookmarkStart {
        BookmarkStart() {} // for stupid QValueList
        BookmarkStart( const QString&s, int par, int ind )
            : frameSetName( s ), paragId( par ), pos( ind ) {}
        QString frameSetName;
        int paragId;
        int pos;
    };
    typedef QMap<QString, BookmarkStart> BookmarkStartsMap;
    BookmarkStartsMap m_bookmarkStarts;

    typedef QMap<QString, QString> DataFormatsMap;
    DataFormatsMap m_dateTimeFormats; // maybe generalize to include number formats.

    uint m_pictureNumber; // Number of the picture (increment *before* use)
    KZip* m_zip; // Input KZip file
};

#endif


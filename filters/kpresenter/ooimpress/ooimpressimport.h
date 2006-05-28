/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>

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

#ifndef OoImpress_IMPORT_H__
#define OoImpress_IMPORT_H__

#include <KoFilter.h>
#include <KoStore.h>

#include <qdom.h>
#include <q3dict.h>
#include <QColor>
//Added by qt3to4:
#include <KoStyleStack.h>
#include <liststylestack.h>

class KZip;

struct animationList
{
    QDomElement *element;
    int order;
};

class OoImpressImport : public KoFilter
{
    Q_OBJECT
public:
    OoImpressImport( QObject * parent, const QStringList & );
    virtual ~OoImpressImport();

    virtual KoFilter::ConversionStatus convert( QByteArray const & from, QByteArray const & to );

private:
    void createDocumentInfo( QDomDocument &docinfo );
    void createDocumentContent( QDomDocument &doccontent );
    void createStyleMap( QDomDocument &docstyles );
    void insertDraws( const QDomElement& styles );
    void insertStyles( const QDomElement& styles );
    void insertStylesPresentation( const QDomElement& styles );

    void fillStyleStack( const QDomElement& object,bool sticky = false );
    void addStyles( const QDomElement* style );
    void appendName(QDomDocument& doc, QDomElement& e, const QDomElement& object);
    void append2DGeometry( QDomDocument& doc, QDomElement& e, const QDomElement& object, int offset );
    bool appendLineGeometry( QDomDocument& doc, QDomElement& e, const QDomElement& object, int offset );
    void appendPoints(QDomDocument& doc, QDomElement& e, const QDomElement& object);
    void appendPie( QDomDocument& doc, QDomElement& e, const QDomElement& object );
    void appendImage( QDomDocument& doc, QDomElement& e, QDomElement& p, const QDomElement& object );
    void appendBackgroundImage( QDomDocument& doc, QDomElement& e, QDomElement& p, const QDomElement& object );
    void appendBackgroundGradient( QDomDocument& doc, QDomElement& e, const QDomElement& object );
    void appendRounding( QDomDocument& doc, QDomElement& e, const QDomElement& object );
    void appendPen( QDomDocument& doc, QDomElement& e );
    void appendBrush( QDomDocument& doc, QDomElement& e );
    void appendShadow( QDomDocument& doc, QDomElement& e );
    void appendLineEnds( QDomDocument& doc, QDomElement& e, bool _orderEndStartLine = true );
    void appendTextObjectMargin( QDomDocument& doc, QDomElement& e );
    void appendField(QDomDocument& doc, QDomElement& e, const QDomElement& object, uint pos);
    void createPresentationAnimation(const QDomElement& element);
    QDomElement findAnimationByObjectID(const QString & id,  int & order);

    void appendObjectEffect(QDomDocument& doc, QDomElement& e, const QDomElement& object, QDomElement& sound);
    void appendBackgroundPage( QDomDocument &doc, QDomElement &e,QDomElement & pictureElement,  QDomElement &soundElement );

    QDomElement saveHelper(const QString &tmpText, QDomDocument &doc);
    void appendObject(QDomNode & drawPage,  QDomDocument & doc,  QDomElement & soundElement, QDomElement & pictureElement, QDomElement & pageNoteElement, QDomElement &objectElement,double offset, bool sticky = false);

    QString storeImage( const QDomElement& object );
    QString storeSound(const QDomElement & object, QDomElement & p, QDomDocument & doc);
    QDomElement parseTextBox( QDomDocument& doc, const QDomElement& textBox );
    bool pushListLevelStyle( const QString& listStyleName, int level );
    bool pushListLevelStyle( const QString& listStyleName, QDomElement& fullListStyle, int level );
    void applyListStyle( QDomElement& paragraph );
    void parseList( QDomDocument& doc, QDomElement& textObjectElement, const QDomElement& list );
    void parseParagraphs( QDomDocument& doc, QDomElement& textObjectElement, const QDomElement& textBox );
    QDomElement parseParagraph( QDomDocument& doc, const QDomElement& paragraph );
    void parseSpanOrSimilar( QDomDocument& doc, const QDomElement& parent,
                             QDomElement& outputParagraph, uint& pos);
    bool parseSettings( QDomDocument &doc, QDomElement &helpLineElement, QDomElement &attributeElement );
    void parseHelpLine( QDomDocument &doc,QDomElement &helpLineElement, const QString &text );

    KoFilter::ConversionStatus openFile();
    KoFilter::ConversionStatus loadAndParse(const QString& filename, QDomDocument& doc);

    int m_numPicture;
    int m_numSound;
    QDomDocument    m_content;
    QDomDocument    m_meta;
    QDomDocument    m_settings;
    Q3Dict<QDomElement> m_styles, m_draws, m_stylesPresentation;
    Q3Dict<QDomElement>   m_listStyles;
    Q3Dict<animationList> m_animations;

    bool m_insideOrderedList;
    bool m_nextItemIsListItem; // only the first elem inside list-item is numbered
    int m_restartNumbering;
    QString m_currentListStyleName;

    KZip * m_zip;
    KoStyleStack m_styleStack;
    ListStyleStack m_listStyleStack;
};

#endif

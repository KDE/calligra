// -*- Mode: c++-mode; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef OoImpress_IMPORT_H__
#define OoImpress_IMPORT_H__

#include <koFilter.h>
#include <koStore.h>

#include <qdom.h>
#include <qdict.h>
#include <qcolor.h>
#include <stylestack.h>

class KZip;

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
	QDomNode findAnimationByObjectID(const QString & id);

    void appendObjectEffect(QDomDocument& doc, QDomElement& e, const QDomElement& object, QDomElement& sound);
	void appendBackgroundPage( QDomDocument &doc, QDomElement &e,QDomElement & pictureElement,  QDomElement &soundElement );

    QDomElement saveHelper(const QString &tmpText, QDomDocument &doc);
	void appendObject(QDomNode & drawPage,  QDomDocument & doc,  QDomElement & soundElement, QDomElement & pictureElement, QDomElement & pageNoteElement, QDomElement &objectElement,double offset, bool sticky = false);

    QString storeImage( const QDomElement& object );
    QString storeSound(const QDomElement & object, QDomElement & p, QDomDocument & doc);
    QDomElement parseTextBox( QDomDocument& doc, const QDomElement& textBox );
    QDomElement parseList( QDomDocument& doc, const QDomElement& paragraph );
    QDomElement parseParagraph( QDomDocument& doc, const QDomElement& list );
    void parseSpanOrSimilar( QDomDocument& doc, const QDomElement& parent,
                             QDomElement& outputParagraph, uint& pos);
    KoFilter::ConversionStatus openFile();
    KoFilter::ConversionStatus loadAndParse(const QString& filename, QDomDocument& doc);

    int m_numPicture;
    int m_numSound;
    QDomDocument    m_content;
    QDomDocument    m_meta;
    QDomDocument    m_settings;
    QDict<QDomElement> m_styles, m_draws;
    QDict<QDomElement> m_animations;
    KZip * m_zip;
    StyleStack m_styleStack;
};

#endif

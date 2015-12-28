/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>
                 2015 Yue Liu <yue.liu@mail.com>

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
   Boston, MA 02110-1301, USA.
*/

#include "KoFormulaShape.h"

#include <QUrl>
#include <QPainter>
#include <QBuffer>

#include <kmessagebox.h>
#include <kguiitem.h>

#include <KoStore.h>
#include <KoDocumentEntry.h>
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoDocumentResourceManager.h>

#include <QMathView.hh>
#include "FormulaDocument.h"
#include "FormulaDebug.h"

KoFormulaShape::KoFormulaShape(KoDocumentResourceManager *documentResourceManager)
  : KoFrameShape( KoXmlNS::draw, "object" )
  , m_isInline(false)
{
    m_document = new FormulaDocument( this );
    m_qmathview = new QMathView(m_document->font(), FORMULA_LOG());
    m_resourceManager = documentResourceManager;
}

KoFormulaShape::~KoFormulaShape()
{
    delete m_qmathview;
    delete m_document;
}

void KoFormulaShape::paint( QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &)
{
    painter.save();
    applyConversion( painter, converter );   // apply zooming and coordinate translation
    m_qmathview->update(&painter);  // paint the formula
    painter.restore();
}

QMathView* KoFormulaShape::mathview() const
{
    return m_qmathview;
}

bool KoFormulaShape::loadOdf( const KoXmlElement& element, KoShapeLoadingContext &context )
{
    debugFormula <<"Loading ODF in Formula";
    loadOdfAttributes(element, context, OdfAllAttributes);
    return loadOdfFrame(element, context);
}

bool KoFormulaShape::loadOdfFrameElement(const KoXmlElement &element,
                                         KoShapeLoadingContext &context)
{
    bool done;
    // If this formula is embedded and not inline, then load the embedded document.
    if ( element.tagName() == "object" && element.hasAttributeNS( KoXmlNS::xlink, "href" )) {
        m_isInline = false;
        done = m_document->loadEmbeddedDocument( context.odfLoadingContext().store(),
                                                 element, context.odfLoadingContext() );
    } else {
        // It's not a frame:object, so it must be inline.
        const KoXmlElement& topLevelElement = KoXml::namedItemNS(element, KoXmlNS::math, "math");
        if (topLevelElement.isNull()) {
            warnFormula << "no math element as first child";
            return false;
        }

        m_isInline = true;
        done = m_document->loadOdfEmbedded(topLevelElement, context);
    }

    if (done) {
        m_qmathview->loadBuffer(m_document->content().toUtf8().constData());
    }

    return done;
}

void KoFormulaShape::saveOdf( KoShapeSavingContext& context ) const
{
    // FIXME: Add saving of embedded document if m_isInline is false;
    debugFormula << "Saving ODF in Formula";
    KoXmlWriter& writer = context.xmlWriter();
    writer.startElement("draw:frame");
    saveOdfAttributes(context, OdfAllAttributes);
    writer.startElement( "draw:object" );
    writer.addCompleteElement(m_document->content().toUtf8().constData());
    writer.endElement(); // draw:object
    writer.endElement(); // draw:frame
}

KoDocumentResourceManager *KoFormulaShape::resourceManager() const
{
    return m_resourceManager;
}

QString KoFormulaShape::content() const {
    return m_document->content();
}

void KoFormulaShape::setContent(QString mathML) {
    m_document->setContent(mathML);
    m_qmathview->loadBuffer(m_document->content().toUtf8().constData());
}

QFont KoFormulaShape::font() const {
    return m_document->font();
}

void KoFormulaShape::setFont(QFont font) {
    m_document->setFont(font);
    m_qmathview->setFont(m_document->font());
}

QColor KoFormulaShape::backgroundColor() const {
    return m_document->backgroundColor();
}

void KoFormulaShape::setBackgroundColor(QColor color) {
    m_document->setBackgroundColor(color);
    m_qmathview->setBackgroundColor(m_document->backgroundColor());
}

QColor KoFormulaShape::foregroundColor() const {
    return m_document->foregroundColor();
}

void KoFormulaShape::setForegroundColor(QColor color) {
    m_document->setForegroundColor(color);
    m_qmathview->setForegroundColor(m_document->foregroundColor());
}
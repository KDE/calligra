/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CommentShape.h"
#include "Globals.h"
#include "InitialsCommentShape.h"

#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <KoApplication.h>
#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoShapeApplicationData.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoShapeStroke.h>
#include <KoTextShapeData.h>

#include <KLocalizedString>
#include <KMessageBox>

#include <QBrush>
#include <QGradient>
#include <QPainter>
#include <QRectF>

#define TextShapeId "TextShapeID"

CommentShape::CommentShape(KoDocumentResourceManager *resourceManager)
    : KoShapeContainer()
    , m_active(false)
    , m_comment(nullptr)
{
    KoShapeContainer::setSize(initialsBoxSize);

    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(TextShapeId);
    if (factory) {
        m_comment = factory->createDefaultShape(resourceManager);
    }
    if (!m_comment) {
        //         m_comment = new KoShape;
        KMessageBox::error(0, i18n("The plugin needed for displaying comments is not present."), i18n("Plugin Missing"));
    }
    if (dynamic_cast<KoTextShapeData *>(m_comment->userData()) == 0) {
        KMessageBox::error(0,
                           i18n("The plugin needed for displaying the comment is not compatible with the current version of the comment shape."),
                           i18n("Plugin Incompatible"));
        m_comment->setUserData(new KoTextShapeData);
    }

    m_comment->setSize(commentBoxSize);
    m_comment->setPosition(commentBoxPoint);
    m_comment->setVisible(false);

    QLinearGradient *gradient = new QLinearGradient(commentBoxPoint, QPointF(commentBoxPoint.x(), commentBoxPoint.y() + commentBoxSize.height()));
    gradient->setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient->setColorAt(0.0, Qt::yellow);
    gradient->setColorAt(1.0, QColor(254, 201, 7));
    m_comment->setBackground(new KoGradientBackground(gradient));

    KoShapeStroke *stroke = new KoShapeStroke;
    stroke->setLineBrush(QBrush(Qt::black));
    stroke->setLineWidth(0.5);
    m_comment->setStroke(stroke);

    addShape(m_comment);

    m_initials = new InitialsCommentShape();
    m_initials->setSize(QSizeF(20, 20));
    m_initials->setSelectable(false);
    addShape(m_initials);
}

CommentShape::~CommentShape()
{
    delete m_comment;
    delete m_initials;
}

bool CommentShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    loadOdfAttributes(element, context, OdfPosition);

    KoXmlElement child;
    forEachElement(child, element)
    {
        if (child.namespaceURI() == KoXmlNS::dc) {
            if (child.localName() == "creator") {
                m_creator = child.text();
                QStringList creatorNames = m_creator.split(' ');
                QString initials;
                if (KoApplication::isLeftToRight()) {
                    foreach (const QString &name, creatorNames) {
                        initials += name.left(1);
                    }
                } else {
                    foreach (const QString &name, creatorNames) {
                        initials += name.right(1);
                    }
                }
                m_initials->setInitials(initials);
            } else if (child.localName() == "date") {
                m_date = QDate::fromString(child.text(), Qt::ISODate);
            }
        } else if (child.namespaceURI() == KoXmlNS::text && child.localName() == "p") {
            commentData()->document()->setHtml(child.text().replace('\n', "<br>"));
        }
    }

    return true;
}

void CommentShape::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter &writer = context.xmlWriter();

    writer.startElement("officeooo:annotation"); // TODO replace with standardized element name
    saveOdfAttributes(context, OdfPosition);

    writer.startElement("dc:creator");
    writer.addTextSpan(m_creator);
    writer.endElement(); // dc:creator

    writer.startElement("dc:date");
    writer.addTextSpan(m_date.toString(Qt::ISODate));
    writer.endElement(); // dc:date

    writer.startElement("text:p");
    writer.addTextSpan(commentData()->document()->toPlainText());
    writer.endElement(); // text:p

    writer.endElement(); // officeooo:annotation
}

void CommentShape::paintComponent(QPainter & /*painter*/, const KoViewConverter & /*converter*/, KoShapePaintingContext &)
{
}

void CommentShape::setSize(const QSizeF & /*size*/)
{
    KoShapeContainer::setSize(initialsBoxSize);
}

void CommentShape::toogleActive()
{
    setActive(!m_active);
}

bool CommentShape::isActive() const
{
    return m_active;
}

void CommentShape::setActive(bool active)
{
    m_active = active;
    if (!m_active) {
        KoShapeContainer::setSize(initialsBoxSize);
    } else {
        KoShapeContainer::setSize(wholeSize);
    }
    m_initials->setActive(m_active);
    m_comment->setVisible(m_active);
    update();
}

KoTextShapeData *CommentShape::commentData() const
{
    return qobject_cast<KoTextShapeData *>(m_comment->userData());
}

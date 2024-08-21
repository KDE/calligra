/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoAnnotation.h"

#include <KoShape.h>
#include <KoShapeSavingContext.h>
#include <KoTextInlineRdf.h>
#include <KoTextRangeManager.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include "TextDebug.h"
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

// Include Q_UNSUSED classes, for building on Windows
#include <KoShapeLoadingContext.h>

class Q_DECL_HIDDEN KoAnnotation::Private
{
public:
    Private(const QTextDocument *doc)
        : document(doc)
        , posInDocument(0)
    {
    }
    const QTextDocument *document;
    int posInDocument;

    // Name of this annotation. It is used to tie together the annotation and annotation-end tags
    QString name;

    KoShape *shape;
};

KoAnnotation::KoAnnotation(const QTextCursor &cursor)
    : KoTextRange(cursor)
    , d(new Private(cursor.block().document()))
{
}

KoAnnotation::KoAnnotation(QTextDocument *document, int position)
    : KoTextRange(document, position)
    , d(new Private(document))
{
}

KoAnnotation::~KoAnnotation()
{
    delete d;
}

void KoAnnotation::setName(const QString &name)
{
    d->name = name;
}

QString KoAnnotation::name() const
{
    return d->name;
}

void KoAnnotation::setAnnotationShape(KoShape *shape)
{
    d->shape = shape;
}

KoShape *KoAnnotation::annotationShape() const
{
    return d->shape;
}

bool KoAnnotation::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(context);

    if (element.localName() != "annotation") {
        return false;
    }

    // debugText << "****** Start Load odf ******";
    QString annotationName = element.attribute("name");

    if (manager()) {
        // For cut and paste, make sure that the name is unique.
        d->name = createUniqueAnnotationName(manager()->annotationManager(), annotationName, false);

        // When loading an annotation we must assume that it is for a point rather than
        // a range. If we encounter an <annotation-end> tag later, we will change that.
        setPositionOnlyMode(true);

        // Add inline Rdf to the annotation.
        if (element.hasAttributeNS(KoXmlNS::xhtml, "property") || element.hasAttribute("id")) {
            KoTextInlineRdf *inlineRdf = new KoTextInlineRdf(const_cast<QTextDocument *>(d->document), this);
            if (inlineRdf->loadOdf(element)) {
                setInlineRdf(inlineRdf);
            } else {
                delete inlineRdf;
                inlineRdf = nullptr;
            }
        }
        // debugText << "****** End Load ******";

        return true;
    }

    return false;
}

void KoAnnotation::saveOdf(KoShapeSavingContext &context, int position, TagType tagType) const
{
    KoXmlWriter *writer = &context.xmlWriter();

    if (!hasRange()) {
        if (tagType == StartTag) {
            writer->startElement("office:annotation", false);
            writer->addAttribute("text:name", d->name.toUtf8());
            if (inlineRdf()) {
                inlineRdf()->saveOdf(context, writer);
            }

            d->shape->saveOdf(context);

            writer->endElement(); // office:annotation
        }

    } else if ((tagType == StartTag) && (position == rangeStart())) {
        writer->startElement("office:annotation", false);
        writer->addAttribute("text:name", d->name.toUtf8());
        if (inlineRdf()) {
            inlineRdf()->saveOdf(context, writer);
        }

        d->shape->saveOdf(context);

        writer->endElement(); // office:annotation
    } else if ((tagType == EndTag) && (position == rangeEnd())) {
        writer->startElement("office:annotation-end", false);
        writer->addAttribute("text:name", d->name.toUtf8());
        writer->endElement();
    }
    // else nothing
}

QString KoAnnotation::createUniqueAnnotationName(const KoAnnotationManager *kam, const QString &annotationName, bool isEndMarker)
{
    QString ret = annotationName;
    int uniqID = 0;

    while (true) {
        if (kam->annotation(ret)) {
            ret = QString("%1_%2").arg(annotationName).arg(++uniqID);
        } else {
            if (isEndMarker) {
                --uniqID;
                if (!uniqID)
                    ret = annotationName;
                else
                    ret = QString("%1_%2").arg(annotationName).arg(uniqID);
            }
            break;
        }
    }
    return ret;
}

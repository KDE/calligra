/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFormShape.h"
#include "KoFormShapeRenderer.h"

#include <KoDocument.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeLoadingContext.h>
#include <KoShapePaintingContext.h>
#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <KoImageData.h>
#include <KoOdfLoadingContext.h>
#include <QPainter>

using namespace Qt::StringLiterals;

KoFormShape::KoFormShape()
{
    setShapeId(u"FormShape"_s);
    setSize(QSizeF(80, 24));
}

void KoFormShape::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &)
{
    paintFormControl(painter, converter, size(), m_controlKind, formControl(), m_image);
}

bool KoFormShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    loadOdfAttributes(element, context, OdfAllAttributes);
    m_controlId = element.attributeNS(KoXmlNS::draw, u"control"_s);
    m_controlKind.clear();
    m_formControl.reset();
    m_image = {};
    m_document.clear();
    if (auto *resources = context.documentResourceManager()) {
        if (auto *document = dynamic_cast<KoDocument *>(resources->odfDocument())) {
            m_document = document;
            m_controlKind = document->form().controlKind(m_controlId);
            m_formControl = document->form().controlById(m_controlId);
        }
    }
    if (m_formControl) {
        const QString imagePath = m_formControl->formAttribute(u"image-data"_s);
        setPrintable(m_formControl->printable());
        if (!imagePath.isEmpty() && context.odfLoadingContext().store()) {
            KoImageData image;
            image.setImage(imagePath, context.odfLoadingContext().store());
            m_image = image.image();
        }
    }
    return !m_controlId.isEmpty();
}

void KoFormShape::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter &writer = context.xmlWriter();
    writer.startElement("draw:control");
    writer.addAttribute("draw:control", m_controlId);
    saveOdfAttributes(context, OdfAllAttributes);
    saveOdfCommonChildElements(context);
    writer.endElement();
}

QString KoFormShape::controlId() const
{
    return m_controlId;
}

QString KoFormShape::controlKind() const
{
    return m_controlKind;
}

void KoFormShape::setControlId(const QString &id)
{
    if (m_controlId == id)
        return;
    m_controlId = id;
    m_controlKind.clear();
    m_formControl.reset();
    m_image = {};
    notifyChanged();
    update();
}

const KoOdfForm::Control *KoFormShape::formControl() const
{
    return m_formControl.get();
}

void KoFormShape::setControlProperties(const KoOdfForm::Control &properties)
{
    if (!m_formControl || !m_document || !m_document->form().setControlProperties(m_controlId, properties))
        return;
    *m_formControl = properties;
    m_formControl->setId(m_controlId);
    setPrintable(m_formControl->printable());
    notifyChanged();
    update();
}

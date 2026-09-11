/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFormShape.h"
#include "KoFormShapeRenderer.h"

#include <KoDocument.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeBasedDocumentBase.h>
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
#include <QSet>

using namespace Qt::StringLiterals;

namespace
{
QSet<KoFormShape *> s_formShapes;
}

namespace
{
std::unique_ptr<KoOdfForm::Control> createControl(KoOdfForm::ControlKind kind)
{
    switch (kind) {
    case KoOdfForm::ControlKind::Text:
        return std::make_unique<KoOdfForm::Text>();
    case KoOdfForm::ControlKind::Textarea:
        return std::make_unique<KoOdfForm::Textarea>();
    case KoOdfForm::ControlKind::FormattedText:
        return std::make_unique<KoOdfForm::FormattedText>();
    case KoOdfForm::ControlKind::Number:
        return std::make_unique<KoOdfForm::Number>();
    case KoOdfForm::ControlKind::Date:
        return std::make_unique<KoOdfForm::Date>();
    case KoOdfForm::ControlKind::Time:
        return std::make_unique<KoOdfForm::Time>();
    case KoOdfForm::ControlKind::Button:
        return std::make_unique<KoOdfForm::Button>();
    case KoOdfForm::ControlKind::Checkbox:
        return std::make_unique<KoOdfForm::Checkbox>();
    case KoOdfForm::ControlKind::Radio:
        return std::make_unique<KoOdfForm::Radio>();
    case KoOdfForm::ControlKind::Combobox:
        return std::make_unique<KoOdfForm::Combobox>();
    case KoOdfForm::ControlKind::Listbox:
        return std::make_unique<KoOdfForm::Listbox>();
    case KoOdfForm::ControlKind::Password:
        return std::make_unique<KoOdfForm::Password>();
    case KoOdfForm::ControlKind::Hidden:
        return std::make_unique<KoOdfForm::Hidden>();
    case KoOdfForm::ControlKind::File:
        return std::make_unique<KoOdfForm::File>();
    case KoOdfForm::ControlKind::FixedText:
        return std::make_unique<KoOdfForm::FixedText>();
    case KoOdfForm::ControlKind::ValueRange:
        return std::make_unique<KoOdfForm::ValueRange>();
    case KoOdfForm::ControlKind::Image:
        return std::make_unique<KoOdfForm::Image>();
    case KoOdfForm::ControlKind::ImageFrame:
        return std::make_unique<KoOdfForm::ImageFrame>();
    case KoOdfForm::ControlKind::Frame:
        return std::make_unique<KoOdfForm::Frame>();
    case KoOdfForm::ControlKind::Grid:
        return std::make_unique<KoOdfForm::Grid>();
    case KoOdfForm::ControlKind::Unknown:
    case KoOdfForm::ControlKind::GenericControl:
        return std::make_unique<KoOdfForm::GenericControl>();
    }
    return nullptr;
}
}

QList<FormPdfField> collectFormPdfFields(const QList<KoShape *> &shapes, int pageNumber)
{
    QList<FormPdfField> fields;
    for (KoShape *shape : shapes) {
        auto *formShape = dynamic_cast<KoFormShape *>(shape);
        if (!formShape || !formShape->formControl() || !formShape->formControl()->printable()) {
            continue;
        }
        const auto *control = formShape->formControl();
        FormPdfField field;
        field.id = formShape->controlId();
        field.name = control->name();
        field.label = control->label();
        field.kind = formShape->controlKind();
        field.rect = formShape->boundingRect();
        field.page = pageNumber;
        field.value = control->value();
        field.currentValue = control->currentValue();
        field.dataField = control->dataField();
        field.linkedCell = control->linkedCell();
        field.xformsBind = control->xformsBind();
        field.target = control->formAttribute(u"for"_s);
        field.entries = control->entries();
        field.readOnly = control->readOnly();
        field.required = control->inputRequired();
        field.printable = control->printable();
        field.tabIndex = control->tabIndex();
        fields.append(std::move(field));
    }
    return fields;
}

KoFormShape::KoFormShape()
{
    s_formShapes.insert(this);
    setShapeId(u"FormShape"_s);
    setSize(QSizeF(80, 24));
}

KoFormShape::~KoFormShape()
{
    s_formShapes.remove(this);
}

KoDocument *KoFormShape::document() const
{
    return m_document;
}

QList<KoShape *> registeredFormShapes()
{
    QList<KoShape *> shapes;
    shapes.reserve(s_formShapes.size());
    for (KoFormShape *shape : s_formShapes) {
        shapes.append(shape);
    }
    return shapes;
}

void KoFormShape::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &)
{
    if (m_formControl && !m_formControl->printable()) {
        return;
    }
    paintFormControl(painter, converter, size(), m_controlKind, formControl(), m_image);
}

bool KoFormShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    loadOdfAttributes(element, context, OdfAllAttributes);
    m_controlId = element.attributeNS(KoXmlNS::draw, u"control"_s);
    m_controlKind = KoOdfForm::ControlKind::Unknown;
    m_formControl.reset();
    m_image = {};
    m_document.clear();
    if (auto *resources = context.documentResourceManager()) {
        if (auto *document = dynamic_cast<KoDocument *>(resources->odfDocument())) {
            m_document = document;
            m_controlKind = document->form().controlKindEnum(m_controlId);
            m_formControl = document->form().controlById(m_controlId);
            if (m_formControl && m_formControl->label().isEmpty()) {
                m_formControl->setLabel(document->form().labelForControl(m_controlId));
            }
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

KoOdfForm::ControlKind KoFormShape::controlKind() const
{
    return m_controlKind;
}

QString KoFormShape::controlKindName() const
{
    return KoOdfForm::controlKindName(m_controlKind);
}

void KoFormShape::setControlKind(KoOdfForm::ControlKind kind)
{
    m_controlKind = kind;
    m_formControl = createControl(kind);
    m_formControl->setLabel(KoOdfForm::controlKindName(kind));
    notifyChanged();
    update();
}

void KoFormShape::initializeControl(KoOdfForm::ControlKind kind, KoDocument *document)
{
    if (!document) {
        setControlKind(kind);
        return;
    }
    m_document = document;
    m_controlKind = kind;
    m_controlId = document->form().addControl(KoOdfForm::controlKindName(kind));
    m_formControl = document->form().controlById(m_controlId);
    notifyChanged();
    update();
}

void KoFormShape::setControlId(const QString &id)
{
    if (m_controlId == id)
        return;
    m_controlId = id;
    m_controlKind = KoOdfForm::ControlKind::Unknown;
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
    if (!m_formControl)
        return;
    if (m_document) {
        if (!m_document->form().setControlProperties(m_controlId, properties)) {
            return;
        }
    }
    *m_formControl = properties;
    m_formControl->setId(m_controlId);
    setPrintable(m_formControl->printable());
    notifyChanged();
    update();
}

QMap<QString, QString> KoFormShape::formEventHandlers() const
{
    return m_document ? m_document->form().eventHandlers() : QMap<QString, QString>();
}

void KoFormShape::setFormEventHandlers(const QMap<QString, QString> &handlers)
{
    if (!m_document) {
        return;
    }
    m_document->form().setEventHandlers(handlers);
    notifyChanged();
    update();
}

void KoFormShape::shapeAddedToDocument(KoShapeBasedDocumentBase *document)
{
    auto *odfDocument = dynamic_cast<KoDocument *>(document);
    if (!odfDocument || m_controlId.isEmpty() || m_controlKind == KoOdfForm::ControlKind::Unknown) {
        return;
    }
    if (odfDocument->form().controlKindEnum(m_controlId) == KoOdfForm::ControlKind::Unknown) {
        odfDocument->form().addControl(m_controlKind, m_controlId);
        if (m_formControl) {
            odfDocument->form().setControlProperties(m_controlId, *m_formControl);
        }
    }
    m_document = odfDocument;
}

void KoFormShape::shapeRemovedFromDocument(KoShapeBasedDocumentBase *document)
{
    auto *odfDocument = dynamic_cast<KoDocument *>(document);
    if (!odfDocument || m_controlId.isEmpty()) {
        return;
    }
    odfDocument->form().removeControl(m_controlId);
}

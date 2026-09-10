/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <KoDocument.h>
#include <KoOdfForm.h>
#include <KoShape.h>
#include <QPointer>

#include <QImage>
#include <QString>

class KoFormShape final : public KoShape
{
public:
    KoFormShape();

    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) const override;

    QString controlId() const;
    KoOdfForm::ControlKind controlKind() const;
    QString controlKindName() const;
    void setControlKind(KoOdfForm::ControlKind kind);
    void initializeControl(KoOdfForm::ControlKind kind, KoDocument *document);
    void setControlId(const QString &id);
    const KoOdfForm::Control *formControl() const;
    void setControlProperties(const KoOdfForm::Control &properties);
    void shapeAddedToDocument(KoShapeBasedDocumentBase *document) override;
    void shapeRemovedFromDocument(KoShapeBasedDocumentBase *document) override;

private:
    QPointer<KoDocument> m_document;
    QString m_controlId;
    KoOdfForm::ControlKind m_controlKind = KoOdfForm::ControlKind::Unknown;
    QImage m_image;
    std::unique_ptr<KoOdfForm::Control> m_formControl;
};

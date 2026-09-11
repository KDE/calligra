/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#pragma once

#include <KoOdfForm.h>
#include <QList>
#include <QRectF>

class QPainter;
class QImage;
class QSizeF;
class KoViewConverter;
class KoShape;

struct FormPdfField {
    QString id;
    QString name;
    QString label;
    KoOdfForm::ControlKind kind = KoOdfForm::ControlKind::Unknown;
    QRectF rect;
    QPointF pageOffset;
    int page = 0;
    QString value;
    QString currentValue;
    QString dataField;
    QString linkedCell;
    QString xformsBind;
    QString target;
    QVector<KoOdfForm::Control::Entry> entries;
    bool readOnly = false;
    bool required = false;
    bool printable = true;
    int tabIndex = 0;
};

QList<FormPdfField> collectFormPdfFields(const QList<KoShape *> &shapes, int pageNumber = 0);

void paintFormControl(QPainter &painter,
                      const KoViewConverter &converter,
                      const QSizeF &size,
                      KoOdfForm::ControlKind kind,
                      const KoOdfForm::Control *control,
                      const QImage &image);

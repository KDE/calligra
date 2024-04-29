/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_SPREADSHEETIMPL_H
#define CALLIGRA_COMPONENTS_SPREADSHEETIMPL_H

#include "DocumentImpl.h"

namespace Calligra
{
namespace Components
{

class SpreadsheetImpl : public DocumentImpl
{
    Q_OBJECT
public:
    explicit SpreadsheetImpl(QObject *parent = nullptr);
    ~SpreadsheetImpl() override;

    bool load(const QUrl &url) override;
    int currentIndex() override;
    void setCurrentIndex(int newValue) override;
    int indexCount() const override;
    QUrl urlAtPoint(QPoint point) override;
    QObject *part() const override;

private Q_SLOTS:
    void updateDocumentSize(const QSize &size);

private:
    class Private;
    Private *const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_SPREADSHEETIMPL_H

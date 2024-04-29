/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_PRESENTATIONIMPL_H
#define CALLIGRA_COMPONENTS_PRESENTATIONIMPL_H

#include "DocumentImpl.h"

namespace Calligra
{
namespace Components
{

class PresentationImpl : public DocumentImpl
{
    Q_OBJECT
public:
    explicit PresentationImpl(QObject *parent = nullptr);
    ~PresentationImpl() override;

    bool load(const QUrl &url) override;
    int currentIndex() override;
    void setCurrentIndex(int newValue) override;
    int indexCount() const override;
    QUrl urlAtPoint(QPoint point) override;
    QObject *part() const override;

private:
    class Private;
    Private *const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_PRESENTATIONIMPL_H

/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef CALLIGRA_COMPONENTS_TEXTDOCUMENTIMPL_H
#define CALLIGRA_COMPONENTS_TEXTDOCUMENTIMPL_H

#include "DocumentImpl.h"

namespace Calligra {
namespace Components {

class TextDocumentImpl : public DocumentImpl
{
    Q_OBJECT
public:
    explicit TextDocumentImpl(QObject* parent = 0);
    ~TextDocumentImpl() override;

    bool load(const QUrl& url) override;
    int currentIndex() override;
    void setCurrentIndex(int newValue) override;
    int indexCount() const override;
    QUrl urlAtPoint(QPoint point) override;
    QObject* part() const override;

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_TEXTDOCUMENTIMPL_H

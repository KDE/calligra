/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CALLIGRA_COMPONENTS_DOCUMENTIMPL_H
#define CALLIGRA_COMPONENTS_DOCUMENTIMPL_H

#include <QtCore/QObject>
#include "Global.h"

class KoFindBase;

namespace Calligra {
namespace Components {

/**
 * \brief Defines an interface for handling specific documents in Document.
 *
 */

class DocumentImpl : public QObject
{
    Q_OBJECT
public:
    explicit DocumentImpl(QObject* parent = 0);
    virtual ~DocumentImpl();

    virtual Global::DocumentType documentType() const = 0;

    virtual bool load(const QUrl& url) = 0;
    virtual KoFindBase* finder() const = 0;

};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_DOCUMENTIMPL_H

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

#ifndef CALLIGRA_COMPONENTS_SPREADSHEETIMPL_H
#define CALLIGRA_COMPONENTS_SPREADSHEETIMPL_H

#include "DocumentImpl.h"

namespace Calligra {
namespace Components {

class SpreadsheetImpl : public DocumentImpl
{
    Q_OBJECT
public:
    SpreadsheetImpl(QObject* parent = 0);
    ~SpreadsheetImpl();

    virtual Global::DocumentType documentType() const;
    virtual bool load(const QUrl& url);
    virtual KoFindBase* finder() const;

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_SPREADSHEETIMPL_H

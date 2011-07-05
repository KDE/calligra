/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
 *
 * Contact: Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef FOEXTERNAL_EDITOR_IFACE
#define FOEXTERNAL_EDITOR_IFACE

#include "koabstraction_export.h"

#include <QString>
#include <QWidget>

namespace Calligra { namespace Tables
{
    class CellToolBase;
    class CellEditorBase;
} }

/**
 * Abstraction of external editor for spreadsheet's cell.
 */
class KOABSTRACTION_EXPORT KoExternalEditorInterface
{
public:
    KoExternalEditorInterface(Calligra::Tables::CellToolBase*) {}
    virtual ~KoExternalEditorInterface() {}

    //! Inserts operator @a operatorCharacter in the editor
    virtual void insertOperator(const QString& operatorCharacter) = 0;

    //! Clears contents of the editor
    virtual void clear() = 0;

    //! Sets the text of the editor to @a text, may be rich text
    virtual void setText(const QString& text) = 0;

    //! Sets the plain text @a text for the editor
    virtual void setPlainText(const QString& text) = 0;

    //! Sets the cursor position within the editor to @a position
    virtual void setCursorPosition(int position) = 0;

    //! @return plain text representation of the editor's contents
    virtual QString toPlainText() const = 0;

    //! @return this object casted to QWidget* if it is derived from QWidget, otherwise 0.
    QWidget* thisWidget() { return dynamic_cast<QWidget*>(this); }
};

#endif

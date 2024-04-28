/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_ENUMS_H
#define CALLIGRA_COMPONENTS_ENUMS_H

#include <QObject>

namespace Calligra
{
namespace Components
{

class DocumentType : public QObject
{
    Q_OBJECT

public:
    enum Type {
        Unknown, ///< An unknown document.
        TextDocument, ///< A text document, for example an OpenDocument Text document.
        Spreadsheet, ///< A spreadsheet, for example an Excel document.
        Presentation, ///< A presentation, for example a Keynote document.
        StaticTextDocument, ///< An unmodifiable text document, for example a PDF document.
    };

    Q_ENUM(Type)
};

class DocumentStatus : public QObject
{
    Q_OBJECT

public:
    enum Status {
        Unloaded,
        Loading,
        Loaded,
        Failed,
    };

    Q_ENUM(Status);
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_ENUMS_H

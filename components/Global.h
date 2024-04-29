/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_GLOBAL_H
#define CALLIGRA_COMPONENTS_GLOBAL_H

#include <QObject>

#include "Enums.h"

namespace Calligra
{
namespace Components
{

/**
 * \brief Provides a singleton wrapper for global Calligra functionality.
 *
 */

class Global : public QObject
{
    Q_OBJECT
public:
    explicit Global(QObject *parent = nullptr);

    static Q_INVOKABLE int documentType(const QUrl &document);
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_GLOBAL_H

/*
 * SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTOOLBOXFACTORY_H
#define KOTOOLBOXFACTORY_H

#include "kowidgets_export.h"
#include <KoDockFactoryBase.h>

#include <QDockWidget>
#include <QString>

/**
 * Factory class to create a new KoToolBox that contains the buttons
 * to activate tools.
 */
class KOWIDGETS_EXPORT KoToolBoxFactory : public KoDockFactoryBase
{
public:
    explicit KoToolBoxFactory() = default;
    ~KoToolBoxFactory() override = default;

    QString id() const override;
    KoDockFactoryBase::DockPosition defaultDockPosition() const override;
    QDockWidget *createDockWidget() override;
    bool isCollapsable() const override
    {
        return false;
    }
};

#endif

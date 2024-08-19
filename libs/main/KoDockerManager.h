/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "komain_export.h"

#include <QObject>
#include <QPointer>

class KoMainWindow;

/**
   The docker manager makes sure that tool option widgets are shown at the right time.
 */
class KOMAIN_EXPORT KoDockerManager : public QObject
{
    Q_OBJECT
public:
    explicit KoDockerManager(KoMainWindow *mainWindow);
    ~KoDockerManager() override;

    void resetToolDockerWidgets();

    void removeToolOptionsDocker();

public Q_SLOTS:
    /**
     * Update the option widgets to the argument ones, removing the currently set widgets.
     */
    void newOptionWidgets(const QList<QPointer<QWidget>> &optionWidgetList);

private:
    Q_PRIVATE_SLOT(d, void restoringDone())
    class Private;
    std::unique_ptr<Private> const d;
};

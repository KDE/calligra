/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Emmet O 'Neill <emmetoneill.pdx@gmail.com>
 * SPDX-FileCopyrightText: 2018 Eoin O 'Neill <eoinoneill1991@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISKINECTICSCROLLER_H
#define KISKINECTICSCROLLER_H
#include <QScroller>
#include <kowidgetutils_export.h>

class QAbstractScrollArea;

/* This is a convenience namespace for setting up global kinetic scrolling
 * with consistent settings across various UI elements within Krita. */

namespace KoKineticScroller
{
KOWIDGETUTILS_EXPORT QScroller *createPreconfiguredScroller(QAbstractScrollArea *target);

KOWIDGETUTILS_EXPORT QScroller::ScrollerGestureType getConfiguredGestureType();

KOWIDGETUTILS_EXPORT void updateCursor(QWidget *source, QScroller::State state);
}

#endif // KISKINECTICSCROLLER_H

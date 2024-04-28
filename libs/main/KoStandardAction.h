/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/
#ifndef KOSTANDARDACTION_H
#define KOSTANDARDACTION_H

#include "komain_export.h"

class QObject;
class QAction;
class KToggleAction;

/**
 * Convenience methods to access all standard Calligra actions.
 * See KStandardAction for usage details.
 */
namespace KoStandardAction
{
/**
 * The standard actions.
 */
enum StandardAction {
    ActionNone,

    ShowGuides
};

/**
 * Creates an action corresponding to the
 * KoStandardAction::StandardAction enum.
 */
KOMAIN_EXPORT QAction *create(StandardAction id, const QObject *receiver, const char *slot, QObject *parent);

/**
 * This will return the internal name of a given standard action.
 */
KOMAIN_EXPORT const char *name(StandardAction id);

/**
 * Show or hide guide lines
 */
KOMAIN_EXPORT KToggleAction *showGuides(const QObject *receiver, const char *slot, QObject *parent);
}

#endif // KSTDACTION_H

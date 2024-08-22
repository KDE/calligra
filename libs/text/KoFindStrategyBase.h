/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOFINDSTRATEGYBASE_H
#define KOFINDSTRATEGYBASE_H

class KFindDialog;
class QTextCursor;
class FindDirection;

/**
 * Abstract base class for the different strategies
 * find and replace in KoFind.
 */
class KoFindStrategyBase
{
public:
    KoFindStrategyBase() = default;
    virtual ~KoFindStrategyBase() = default;

    /**
     * Get the find dialog
     */
    virtual KFindDialog *dialog() const = 0;

    /**
     * Reset internal status
     *
     * E.g. set number of matches found to 0
     */
    virtual void reset() = 0;

    /**
     * Displays the final dialog
     */
    virtual void displayFinalDialog() = 0;

    /**
     * This get called when a match was found
     *
     * @param cursor The cursor
     * @param findDirection The find direction helper that can be used for highlighting
     */
    virtual bool foundMatch(QTextCursor &cursor, FindDirection *findDirection) = 0;
};

#endif /* KOFINDSTRATEGYBASE_H */

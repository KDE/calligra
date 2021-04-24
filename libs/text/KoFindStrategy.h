/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOFINDSTRATEGY_H
#define KOFINDSTRATEGY_H

#include "KoFindStrategyBase.h"

class QWidget;

/**
 * Strategy used for implementing find
 */
class KoFindStrategy : public KoFindStrategyBase
{
public:
    explicit KoFindStrategy(QWidget *parent);
    ~KoFindStrategy() override;

    /// reimplemented
    KFindDialog *dialog() const override;

    /// reimplemented
    void reset() override;

    /// reimplemented
    void displayFinalDialog() override;

    /// reimplemented
    bool foundMatch(QTextCursor &cursor, FindDirection *findDirection) override;

private:
    KFindDialog *m_dialog;
    int m_matches;
};

#endif /* KOFINDSTRATEGY_H */

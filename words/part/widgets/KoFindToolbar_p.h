/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KoFindToolbar_p_h
#define KoFindToolbar_p_h

#include "KoFindToolbar.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QMenu>
#include <QTimer>
#include <QToolButton>

#include <KLocalizedString>
#include <khistorycombobox.h>

#include <KoIcon.h>

#include "KoFindBase.h"
#include "KoFindOption.h"
#include "KoFindOptionSet.h"

class KSqueezedTextLabel;
class QLabel;

class Q_DECL_HIDDEN KoFindToolbar::Private
{
public:
    Private(KoFindToolbar *qq)
        : q(qq)
    {
    }

    void matchFound();
    void noMatchFound();
    void searchWrapped(bool direction);
    void addToHistory();
    void find(const QString &pattern);
    void optionChanged();
    void replace();
    void replaceAll();
    void inputTimeout();
    void returnPressed();

    KoFindToolbar *q;

    KoFindBase *finder;

    QToolButton *closeButton;
    KHistoryComboBox *searchLine;
    KHistoryComboBox *replaceLine;
    QToolButton *previousButton;
    QToolButton *nextButton;
    QToolButton *optionsButton;
    QToolButton *replaceButton;
    QToolButton *replaceAllButton;
    QLabel *replaceLabel;
    KSqueezedTextLabel *information;
    QLabel *matchCounter;
    QTimer *textTimeout;

    static QStringList searchCompletionItems;
    static QStringList replaceCompletionItems;
};

#endif

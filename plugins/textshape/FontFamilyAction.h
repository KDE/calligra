/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
              SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
              SPDX-FileCopyrightText: 2000 Nicolas Hadacek <haadcek@kde.org>
              SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
              SPDX-FileCopyrightText: 2000 Michael Koch <koch@kde.org>
              SPDX-FileCopyrightText: 2001 Holger Freyther <freyther@kde.org>
              SPDX-FileCopyrightText: 2002 Ellis Whitehead <ellis@kde.org>
              SPDX-FileCopyrightText: 2003 Andras Mantia <amantia@kde.org>
              SPDX-FileCopyrightText: 2005-2006 Hamish Rodda <rodda@kde.org>
              SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen

    SPDX-License-Identifier: LGPL-2.0-only
*/

// This is a minorly modified version of the KFontAction class. It exists
// entirely because there's a hang bug on windows at the moment.

#ifndef KOFONTACTION_H
#define KOFONTACTION_H

#include <kselectaction.h>

class QIcon;

/**
 * An action to select a font family.
 * On a toolbar this will show a combobox with all the fonts on the system.
 */
class KoFontFamilyAction : public KSelectAction
{
    Q_OBJECT
    Q_PROPERTY(QString font READ font WRITE setFont)

public:
    KoFontFamilyAction(uint fontListCriteria, QObject *parent);
    explicit KoFontFamilyAction(QObject *parent);
    KoFontFamilyAction(const QString &text, QObject *parent);
    KoFontFamilyAction(const QIcon &icon, const QString &text, QObject *parent);
    ~KoFontFamilyAction() override;

    QString font() const;

    void setFont(const QString &family);

    QWidget *createWidget(QWidget *parent) override;

private:
    class KoFontFamilyActionPrivate;
    KoFontFamilyActionPrivate *const d;

    Q_PRIVATE_SLOT(d, void _ko_slotFontChanged(const QFont &))
};

#endif

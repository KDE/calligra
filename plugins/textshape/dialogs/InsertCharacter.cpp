/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "InsertCharacter.h"

#include <KLocalizedString>
#include <kcharselect.h>

#include <QGridLayout>
#include <QMainWindow>
#include <QPushButton>

InsertCharacter::InsertCharacter(QWidget *parent)
    : QDockWidget(i18n("Special Characters"))
{
    QWidget *specialCharacterWidget = new QWidget();
    QGridLayout *lay = new QGridLayout(specialCharacterWidget);
    lay->setContentsMargins(6, 6, 6, 6);
    m_charSelector =
        new KCharSelect(specialCharacterWidget,
                        nullptr,
                        KCharSelect::SearchLine | KCharSelect::FontCombo | KCharSelect::BlockCombos | KCharSelect::CharacterTable | KCharSelect::DetailBrowser);
    lay->addWidget(m_charSelector, 0, 0, 1, 3);
    QPushButton *insert = new QPushButton(i18n("Insert"), specialCharacterWidget);
    lay->addWidget(insert, 1, 1);
    QPushButton *close = new QPushButton(i18nc("Close dialog", "Close"), specialCharacterWidget);
    lay->addWidget(close, 1, 2);
    lay->setColumnStretch(0, 9);

    setObjectName("insertSpecialCharacter");
    setWidget(specialCharacterWidget);
    while (parent->parentWidget())
        parent = parent->parentWidget();
    QMainWindow *mw = dynamic_cast<QMainWindow *>(parent);
    if (mw)
        mw->addDockWidget(Qt::TopDockWidgetArea, this);
    setFloating(true);

    connect(close, &QAbstractButton::released, this, &QWidget::hide);
    connect(insert, &QPushButton::released, this, QOverload<>::of(&InsertCharacter::insertCharacter));
    connect(m_charSelector, &KCharSelect::charSelected, this, QOverload<>::of(&InsertCharacter::insertCharacter));
}

void InsertCharacter::insertCharacter()
{
    Q_EMIT insertCharacter(QString(m_charSelector->currentChar()));
}

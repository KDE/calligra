/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CharacterSelectDialog.h"

#include <QGridLayout>

#include <KLocalizedString>
#include <kcharselect.h>

using namespace Calligra::Sheets;

class CharacterSelectDialog::Private
{
public:
    Private()
        : charSelect(nullptr)
    {
    }
    KCharSelect *charSelect;
};

/******************************************************************/
/* class CharacterSelectDialog                                           */
/******************************************************************/

CharacterSelectDialog::CharacterSelectDialog(QWidget *parent, const QString &_font)
    : ActionDialog(parent)
    , d(new Private())
{
    setCaption(i18n("Select Character"));
    setObjectName("SpecialCharDialog");

    QWidget *page = mainWidget() /*plainPage()*/;

    QGridLayout *grid = new QGridLayout(page);
    grid->setContentsMargins({});

    d->charSelect = new KCharSelect(page, nullptr);
    d->charSelect->setCurrentChar(QChar(' '));
    d->charSelect->setCurrentFont(QFont(_font));
    connect(d->charSelect, &KCharSelect::charSelected, this, &CharacterSelectDialog::slotDoubleClicked);
    d->charSelect->resize(d->charSelect->sizeHint());
    //     d->charSelect->enableFontCombo( true );
    grid->addWidget(d->charSelect, 0, 0);

    grid->addItem(new QSpacerItem(d->charSelect->width(), 0), 0, 0);
    grid->addItem(new QSpacerItem(0, d->charSelect->height()), 0, 0);
    grid->setRowStretch(0, 0);

    setButtonText(Apply, i18n("&Insert"));
    setButtonToolTip(Apply, i18n("Insert the selected character in the text"));
}

CharacterSelectDialog::~CharacterSelectDialog()
{
    delete d;
}

QChar CharacterSelectDialog::chr() const
{
    return d->charSelect->currentChar();
}

QString CharacterSelectDialog::font() const
{
    return d->charSelect->font().family();
}

void CharacterSelectDialog::onApply()
{
    Q_EMIT insertChar(chr(), font());
}

QWidget *CharacterSelectDialog::defaultWidget()
{
    return d->charSelect;
}

void CharacterSelectDialog::slotDoubleClicked()
{
    onApply();
}

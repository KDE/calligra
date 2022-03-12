/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CharacterSelectDialog.h"

// #include <QLayout>
// #include <QGridLayout>
// #include <QPointer>

// #include <KLocalizedString>
// #include <kcharselect.h>
// #include <kstandardguiitem.h>

using namespace Calligra::Sheets;

class CharacterSelectDialog::Private
{
public:
    Private() : charSelect(0) {}
    KCharSelect *charSelect;
};

/******************************************************************/
/* class CharacterSelectDialog                                           */
/******************************************************************/

CharacterSelectDialog::CharacterSelectDialog(QWidget *parent, const QString &name, const QChar &_chr, const QString &_font, bool _modal)
        : KoDialog(parent),
        d(new Private())
{
    setCaption(i18n("Select Character"));
    setModal(_modal);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    setObjectName(name);

    initDialog(_chr, _font);

    KGuiItem okItem = KStandardGuiItem::ok(); // start from std item to keep the OK icon...
    okItem.setText(i18n("&Insert"));
    okItem.setWhatsThis(i18n("Insert the selected character in the text"));
    setButtonGuiItem(KoDialog::Ok, okItem);
}

CharacterSelectDialog::CharacterSelectDialog(QWidget *parent, const QString &name, const QString &_font, const QChar &_chr, bool _modal)
        : KoDialog(parent),
        d(new Private())
{
    setCaption(i18n("Select Character"));
    setModal(_modal);
    setButtons(User1 | Close);
    setDefaultButton(User1);
    setObjectName(name);

    initDialog(_chr, _font);

    setButtonText(User1, i18n("&Insert"));
    setButtonToolTip(User1, i18n("Insert the selected character in the text"));
    connect(this, &KoDialog::user1Clicked, this, &CharacterSelectDialog::slotUser1);
}

void CharacterSelectDialog::initDialog(const QChar &_chr, const QString &_font)
{
    QWidget *page = mainWidget()/*plainPage()*/;

    QGridLayout *grid = new QGridLayout(page);
    grid->setMargin(0);

    d->charSelect = new KCharSelect(page, 0);
    d->charSelect->setCurrentChar(_chr);
    d->charSelect->setCurrentFont(QFont(_font));
    connect(d->charSelect, &KCharSelect::charSelected, this, &CharacterSelectDialog::slotDoubleClicked);
    d->charSelect->resize(d->charSelect->sizeHint());
//     d->charSelect->enableFontCombo( true );
    grid->addWidget(d->charSelect, 0, 0);

    grid->addItem(new QSpacerItem(d->charSelect->width(), 0), 0, 0);
    grid->addItem(new QSpacerItem(0, d->charSelect->height()), 0, 0);
    grid->setRowStretch(0, 0);
    d->charSelect->setFocus();
}

CharacterSelectDialog::~CharacterSelectDialog()
{
    delete d;
}

void CharacterSelectDialog::closeDialog()
{
    KoDialog::close();
}

// static
bool CharacterSelectDialog::selectChar(QString &_font, QChar &_chr, QWidget* parent, const char* name)
{
    bool res = false;

    QPointer<CharacterSelectDialog> dlg = new CharacterSelectDialog(parent, name, _chr, _font);
    dlg->setFocus();
    if (dlg->exec() == Accepted) {
        _font = dlg->font();
        _chr = dlg->chr();
        res = true;
    }

    delete dlg;

    return res;
}

QChar CharacterSelectDialog::chr() const
{
    return d->charSelect->currentChar();
}

QString CharacterSelectDialog::font() const
{
    return d->charSelect->font().family();
}

void CharacterSelectDialog::slotUser1()
{
    emit insertChar(chr(), font());
}

void CharacterSelectDialog::slotDoubleClicked()
{
    emit insertChar(chr(), font());
}

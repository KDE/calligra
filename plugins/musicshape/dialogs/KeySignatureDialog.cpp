/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KeySignatureDialog.h"

#include "../core/KeySignature.h"

using namespace MusicCore;

KeySignatureDialog::KeySignatureDialog(QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18n("Set key signature"));
    QWidget *w = new QWidget(this);
    widget.setupUi(w);
    setMainWidget(w);
    m_ks = new KeySignature(widget.preview->staff(), 0, 0);
    widget.preview->setStaffElement(m_ks);
    connect(widget.accidentals, &QAbstractSlider::valueChanged, this, &KeySignatureDialog::accidentalsChanged);
}

void KeySignatureDialog::setBar(int bar)
{
    widget.startBar1->setValue(bar + 1);
    widget.startBar2->setValue(bar + 1);
    widget.startBar3->setValue(bar + 1);
    widget.endBar->setValue(bar + 1);
}

int KeySignatureDialog::accidentals()
{
    return -widget.accidentals->value();
}

void KeySignatureDialog::setAccidentals(int accidentals)
{
    widget.accidentals->setValue(accidentals);
    accidentalsChanged(-accidentals);
}

void KeySignatureDialog::accidentalsChanged(int accidentals)
{
    m_ks->setAccidentals(-accidentals);
    widget.preview->update();
}

void KeySignatureDialog::setMusicStyle(MusicStyle *style)
{
    widget.preview->setMusicStyle(style);
}

bool KeySignatureDialog::updateAllStaves()
{
    return widget.allStaves->isChecked();
}

bool KeySignatureDialog::updateToNextChange()
{
    return widget.toNextChange->isChecked();
}

bool KeySignatureDialog::updateTillEndOfPiece()
{
    return widget.throughEndOfPiece->isChecked();
}

int KeySignatureDialog::startBar()
{
    if (updateToNextChange()) {
        return widget.startBar3->value() - 1;
    } else if (updateTillEndOfPiece()) {
        return widget.startBar2->value() - 1;
    } else {
        return widget.startBar1->value() - 1;
    }
}

int KeySignatureDialog::endBar()
{
    return widget.endBar->value() - 1;
}

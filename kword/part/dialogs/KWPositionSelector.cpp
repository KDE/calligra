/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWPositionSelector.h"

#include <QRadioButton>
#include <QGridLayout>
#include <QButtonGroup>

#include <KDebug>

class KWPositionSelector::Private {
public:
    Private()
        : position(KWPositionSelector::TopLeft)
    {
        topLeft = createButton();
        topRight = createButton();
        center = createButton();
        bottomRight = createButton();
        bottomLeft = createButton();
    }

    QRadioButton *createButton() {
        QRadioButton *b = new QRadioButton();
        b->setContentsMargins(0, 0, -10, -10);
        buttonGroup.addButton(b);
        return b;
    }

    QRadioButton *topLeft, *topRight, *center, *bottomRight, *bottomLeft;
    QButtonGroup buttonGroup;
    KWPositionSelector::Position position;
};

KWPositionSelector::KWPositionSelector(QWidget *parent)
    : QWidget(parent),
    d(new Private())
{
    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(d->topLeft, 0, 0);
    grid->addWidget(d->topRight, 0, 2);
    grid->addWidget(d->center, 1, 1);
    grid->addWidget(d->bottomRight, 2, 2);
    grid->addWidget(d->bottomLeft, 2, 0);
    grid->setMargin(0);
    grid->setSpacing(0);
d->topLeft->setContentsMargins(0, 0, -10, -10);
    setLayout(grid);

    connect(d->topLeft, SIGNAL(clicked()), this, SLOT(positionChanged()));
    connect(d->topRight, SIGNAL(clicked()), this, SLOT(positionChanged()));
    connect(d->center, SIGNAL(clicked()), this, SLOT(positionChanged()));
    connect(d->bottomRight, SIGNAL(clicked()), this, SLOT(positionChanged()));
    connect(d->bottomLeft, SIGNAL(clicked()), this, SLOT(positionChanged()));
}

KWPositionSelector::~KWPositionSelector() {
    delete d;
}

KWPositionSelector::Position KWPositionSelector::position() const {
    return d->position;
}

void KWPositionSelector::setPosition(KWPositionSelector::Position position) {
    d->position = position;
    switch(d->position) {
        case KWPositionSelector::TopLeft:
            d->topLeft->setChecked(true);
            break;
        case KWPositionSelector::TopRight:
            d->topRight->setChecked(true);
            break;
        case KWPositionSelector::Center:
            d->center->setChecked(true);
            break;
        case KWPositionSelector::BottomLeft:
            d->bottomLeft->setChecked(true);
            break;
        case KWPositionSelector::BottomRight:
            d->bottomRight->setChecked(true);
            break;
    }
}

void KWPositionSelector::positionChanged() {
    // TODO
    //emit positionSelected(KWPositionSelector::Position position);
}


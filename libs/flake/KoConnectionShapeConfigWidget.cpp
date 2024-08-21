/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoConnectionShapeConfigWidget.h"
#include "commands/KoConnectionShapeTypeCommand.h"
#include <KLocalizedString>
#include <KoIcon.h>

KoConnectionShapeConfigWidget::KoConnectionShapeConfigWidget()
{
    widget.setupUi(this);

    widget.connectionType->clear();
    widget.connectionType->addItem(koIcon("standard-connector"), i18n("Standard"));
    widget.connectionType->addItem(koIcon("lines-connector"), i18n("Lines"));
    widget.connectionType->addItem(koIcon("straight-connector"), i18n("Straight"));
    widget.connectionType->addItem(koIcon("curve-connector"), i18n("Curve"));

    connect(widget.connectionType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KoConnectionShapeConfigWidget::propertyChanged);
    connect(widget.connectionType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KoConnectionShapeConfigWidget::connectionTypeChanged);
}

void KoConnectionShapeConfigWidget::setConnectionType(int type)
{
    widget.connectionType->blockSignals(true);
    widget.connectionType->setCurrentIndex(type);
    widget.connectionType->blockSignals(false);
}

void KoConnectionShapeConfigWidget::open(KoShape *shape)
{
    m_connection = dynamic_cast<KoConnectionShape *>(shape);
    if (!m_connection)
        return;

    widget.connectionType->blockSignals(true);
    widget.connectionType->setCurrentIndex(m_connection->type());
    widget.connectionType->blockSignals(false);
}

void KoConnectionShapeConfigWidget::save()
{
    if (!m_connection) {
        return;
    }
    m_connection->setType(static_cast<KoConnectionShape::Type>(widget.connectionType->currentIndex()));
}

KUndo2Command *KoConnectionShapeConfigWidget::createCommand()
{
    if (!m_connection) {
        return nullptr;
    } else {
        KoConnectionShape::Type type = static_cast<KoConnectionShape::Type>(widget.connectionType->currentIndex());
        return new KoConnectionShapeTypeCommand(m_connection, type);
    }
}

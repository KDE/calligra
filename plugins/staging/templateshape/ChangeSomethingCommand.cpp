/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "ChangeSomethingCommand.h"

// KF5
#include <KLocalizedString>
#include <kdebug.h>

// Calligra
#include <KoSomethingData.h>

#include "TemplateShape.h"

ChangeSomethingCommand::ChangeSomethingCommand(TemplateShape *shape, KoSomethingData *newSomethingData, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
    , m_oldSomethingData(0)
    , m_newSomethingData(newSomethingData)
{
    Q_ASSERT(shape);
    KoSomethingData *oldSomethingData = qobject_cast<KoSomethingData *>(m_shape->userData());

    // We need new here as setUserData deletes the old data.
    m_oldSomethingData = oldSomethingData ? new KoSomethingData(*oldSomethingData) : 0;
    setText(kundo2_i18n("Change something"));

    m_oldSize = shape->size();
    m_newSize = newSomethingData->somethingSize();
    qreal oldarea = m_oldSize.width() * m_oldSize.height();
    qreal newarea = m_newSize.width() * m_newSize.height();
    m_newSize *= sqrt(oldarea / newarea);
}

ChangeSomethingCommand::~ChangeSomethingCommand()
{
    delete m_oldSomethingData;
    delete m_newSomethingData;
}

void ChangeSomethingCommand::redo()
{
    m_shape->update();

    // We need new here as setUserData deletes the old data
    m_shape->setUserData(m_newSomethingData ? new KoSomethingData(*m_newSomethingData) : 0);
    m_shape->setSize(m_newSize);
    m_shape->update();
}

void ChangeSomethingCommand::undo()
{
    // We need new here as setUserData deletes the old data
    m_shape->update();
    m_shape->setUserData(m_oldSomethingData ? new KoSomethingData(*m_oldSomethingData) : 0);
    m_shape->setSize(m_oldSize);
    m_shape->update();
}

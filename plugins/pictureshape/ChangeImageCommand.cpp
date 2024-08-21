/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "ChangeImageCommand.h"

#include <cmath>

#include <KLocalizedString>

#include <KoImageData.h>

#include "PictureDebug.h"

ChangeImageCommand::ChangeImageCommand(PictureShape *shape, KoImageData *newImageData, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_imageChanged(true)
    , m_shape(shape)
    , m_newImageData(newImageData)
    , m_oldCroppingRect(shape->cropRect())
    , m_newCroppingRect(0, 0, 1, 1)
    , m_oldColorMode(shape->colorMode())
    , m_newColorMode(shape->colorMode())
{
    setText(kundo2_i18n("Change image"));

    // we need new here as setUserData deletes the old data
    m_oldImageData = m_shape->imageData() ? new KoImageData(*m_shape->imageData()) : nullptr;
}

ChangeImageCommand::ChangeImageCommand(PictureShape *shape, const QRectF &croppingRect, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_imageChanged(false)
    , m_shape(shape)
    , m_oldImageData(nullptr)
    , m_newImageData(nullptr)
    , m_oldCroppingRect(shape->cropRect())
    , m_newCroppingRect(croppingRect)
    , m_oldColorMode(shape->colorMode())
    , m_newColorMode(shape->colorMode())
{
    setText(kundo2_i18n("Crop image"));
}

ChangeImageCommand::ChangeImageCommand(PictureShape *shape, PictureShape::ColorMode colorMode, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_imageChanged(false)
    , m_shape(shape)
    , m_oldImageData(nullptr)
    , m_newImageData(nullptr)
    , m_oldCroppingRect(shape->cropRect())
    , m_newCroppingRect(shape->cropRect())
    , m_oldColorMode(shape->colorMode())
    , m_newColorMode(colorMode)
{
    setText(kundo2_i18n("Change image color mode"));
}

ChangeImageCommand::~ChangeImageCommand()
{
    delete m_oldImageData;
    delete m_newImageData;
}

void ChangeImageCommand::redo()
{
    if (m_imageChanged) {
        // we need new here as setUserData deletes the old data
        m_shape->setUserData(m_newImageData ? new KoImageData(*m_newImageData) : nullptr);
    }

    m_shape->setColorMode(m_newColorMode);
    m_shape->setCropRect(m_newCroppingRect);
    Q_EMIT sigExecuted();
}

void ChangeImageCommand::undo()
{
    if (m_imageChanged) {
        // we need new here as setUserData deletes the old data
        m_shape->setUserData(m_oldImageData ? new KoImageData(*m_oldImageData) : nullptr);
    }

    m_shape->setColorMode(m_oldColorMode);
    m_shape->setCropRect(m_oldCroppingRect);
    Q_EMIT sigExecuted();
}

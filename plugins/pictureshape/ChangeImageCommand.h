/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef CHANGEIMAGECOMMAND_H
#define CHANGEIMAGECOMMAND_H

#include "PictureShape.h"
#include <QObject>
#include <kundo2command.h>

class ChangeImageCommand : public QObject, public KUndo2Command
{
    Q_OBJECT

public:
    ChangeImageCommand(PictureShape *shape, KoImageData *newImageData, KUndo2Command *parent = nullptr);
    ChangeImageCommand(PictureShape *shape, const QRectF &croppingRect, KUndo2Command *parent = nullptr);
    ChangeImageCommand(PictureShape *shape, PictureShape::ColorMode colorMode, KUndo2Command *parent = nullptr);
    ~ChangeImageCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

Q_SIGNALS:
    void sigExecuted();

private:
    bool m_imageChanged;
    PictureShape *m_shape;
    KoImageData *m_oldImageData;
    KoImageData *m_newImageData;
    QRectF m_oldCroppingRect;
    QRectF m_newCroppingRect;
    PictureShape::ColorMode m_oldColorMode;
    PictureShape::ColorMode m_newColorMode;
};

#endif /* CHANGEIMAGECOMMAND_H */

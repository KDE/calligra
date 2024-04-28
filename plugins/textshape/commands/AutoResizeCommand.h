// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Sebastian Sauer <sebsauer@kdab.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef AUTORESIZECOMMAND_H
#define AUTORESIZECOMMAND_H

#include <KoTextDocumentLayout.h>
#include <QPointer>
#include <TextTool.h>
#include <kundo2command.h>

class TextShape;

class AutoResizeCommand : public KUndo2Command
{
public:
    AutoResizeCommand(KoTextShapeData *shapeData, KoTextShapeData::ResizeMethod resizeMethod, bool enable);

    void undo() override;
    void redo() override;

private:
    KoTextShapeData *m_shapeData;
    KoTextShapeData::ResizeMethod m_resizeMethod;
    bool m_enabled;
    bool m_first;
    KoTextShapeData::ResizeMethod m_prevResizeMethod;
};

#endif // TEXTCUTCOMMAND_H

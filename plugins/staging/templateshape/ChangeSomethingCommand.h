/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef CHANGESOMETHINGCOMMAND_H
#define CHANGESOMETHINGCOMMAND_H

#include <kundo2command.h>

#include <QSizeF>

class KoSomethingData;
class TemplateShape;

class ChangeSomethingCommand : public KUndo2Command
{
public:
    ChangeSomethingCommand(TemplateShape *shape, KoSomethingData *newSomethingData, KUndo2Command *parent = nullptr);
    virtual ~ChangeSomethingCommand();

    /// redo the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

private:
    TemplateShape *m_shape;

    KoSomethingData *m_oldSomethingData;
    KoSomethingData *m_newSomethingData;

    QSizeF m_oldSize;
    QSizeF m_newSize;
};

#endif /* CHANGESOMETHINGCOMMAND_H */

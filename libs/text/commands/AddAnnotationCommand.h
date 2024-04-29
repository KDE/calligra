// This file is part of the KDE project
// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef ADDANNOTATIONCOMMAND_H
#define ADDANNOTATIONCOMMAND_H

#include "AddTextRangeCommand.h"

class KoAnnotation;
class KoShape;

class AddAnnotationCommand : public AddTextRangeCommand
{
public:
    explicit AddAnnotationCommand(KoAnnotation *range, KUndo2Command *parent = nullptr);
    ~AddAnnotationCommand() override;

    void undo() override;
    void redo() override;

private:
    KoAnnotation *m_annotation;
    KoShape *m_shape;
};

#endif // ADDANNOTATIONCOMMAND_H

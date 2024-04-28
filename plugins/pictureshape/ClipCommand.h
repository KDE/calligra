/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef CLIPCOMMAND_H
#define CLIPCOMMAND_H

#include "PictureShape.h"
#include <kundo2command.h>

class ClipCommand : public KUndo2Command
{
public:
    ClipCommand(PictureShape *shape, bool clip);
    ~ClipCommand() override;

    void redo() override;

    void undo() override;

private:
    PictureShape *m_pictureShape;
    bool m_clip;
};

#endif /* CLIPCOMMAND_H */

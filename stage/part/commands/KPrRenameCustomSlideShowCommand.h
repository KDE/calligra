/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRRENAMECUSTOMSLIDESHOWCOMMAND_H
#define KPRRENAMECUSTOMSLIDESHOWCOMMAND_H

#include <kundo2command.h>

class KPrDocument;
class KPrCustomSlideShowsModel;

#include "stage_export.h"

class STAGE_TEST_EXPORT KPrRenameCustomSlideShowCommand : public KUndo2Command
{
public:
    KPrRenameCustomSlideShowCommand(KPrDocument *doc,
                                    KPrCustomSlideShowsModel *model,
                                    const QString &oldName,
                                    const QString &newName,
                                    KUndo2Command *parent = nullptr);

    ~KPrRenameCustomSlideShowCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KPrDocument *m_doc;
    KPrCustomSlideShowsModel *m_model;
    QString m_oldName;
    QString m_newName;
};

#endif // KPRRENAMECUSTOMSLIDESHOWCOMMAND_H

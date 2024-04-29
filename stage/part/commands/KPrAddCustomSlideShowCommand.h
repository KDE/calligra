/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRADDCUSTOMSLIDESHOWCOMMAND_H
#define KPRADDCUSTOMSLIDESHOWCOMMAND_H

#include <kundo2command.h>

class KPrDocument;
class KPrCustomSlideShowsModel;

#include "stage_export.h"

class STAGE_TEST_EXPORT KPrAddCustomSlideShowCommand : public KUndo2Command
{
public:
    explicit KPrAddCustomSlideShowCommand(KPrDocument *doc, KPrCustomSlideShowsModel *model, const QString &name, KUndo2Command *parent = nullptr);

    ~KPrAddCustomSlideShowCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KPrDocument *m_doc;
    KPrCustomSlideShowsModel *m_model;
    QString m_name;
};

#endif // KPRADDCUSTOMSLIDESHOWCOMMAND_H

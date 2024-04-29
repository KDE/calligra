/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRDELCUSTOMSLIDESHOWCOMMAND_H
#define KPRDELCUSTOMSLIDESHOWCOMMAND_H

#include <kundo2command.h>

class KPrDocument;
class KoPAPageBase;
class KPrCustomSlideShowsModel;

#include "stage_export.h"

class STAGE_TEST_EXPORT KPrDelCustomSlideShowCommand : public KUndo2Command
{
public:
    KPrDelCustomSlideShowCommand(KPrDocument *doc, KPrCustomSlideShowsModel *model, const QString &name, KUndo2Command *parent = nullptr);

    ~KPrDelCustomSlideShowCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KPrDocument *m_doc;
    KPrCustomSlideShowsModel *m_model;
    QString m_name;
    QList<KoPAPageBase *> m_oldCustomShow;
};

#endif // KPRDELCUSTOMSLIDESHOWCOMMAND_H

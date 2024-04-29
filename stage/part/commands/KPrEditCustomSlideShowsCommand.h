/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPREDITCUSTOMSLIDESHOWSCOMMAND_H
#define KPREDITCUSTOMSLIDESHOWSCOMMAND_H

#include <kundo2command.h>
class KPrDocument;
class KoPAPageBase;

#include <stage_export.h>

class STAGE_TEST_EXPORT KPrEditCustomSlideShowsCommand : public KUndo2Command
{
public:
    KPrEditCustomSlideShowsCommand(KPrDocument *doc, const QString &name, QList<KoPAPageBase *> newCustomShow, KUndo2Command *parent = nullptr);

    ~KPrEditCustomSlideShowsCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KPrDocument *m_doc;
    QString m_name;
    QList<KoPAPageBase *> m_newCustomShow;
    QList<KoPAPageBase *> m_oldCustomShow;
};

#endif // KPREDITCUSTOMSLIDESHOWSCOMMAND_H

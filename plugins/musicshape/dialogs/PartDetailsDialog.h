/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef PARTDETAILSDIALOG_H
#define PARTDETAILSDIALOG_H

#include <ui_PartDetailsDialog.h>

#include <KoDialog.h>

class MusicTool;
namespace MusicCore
{
class Part;
}

class PartDetailsDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit PartDetailsDialog(MusicCore::Part *part, QWidget *parent = nullptr);

    static void showDialog(MusicTool *tool, MusicCore::Part *part, QWidget *parent = nullptr);

private:
    Ui::PartDetailsDialog widget;
};

#endif // PARTDETAILSDIALOG_H

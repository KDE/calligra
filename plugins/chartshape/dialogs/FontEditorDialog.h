/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_FONT_EDITOR_DIALOG_H
#define KCHART_FONT_EDITOR_DIALOG_H

#include <KoDialog.h>

class KFontChooser;

namespace KoChart
{

class FontEditorDialog : public KoDialog
{
public:
    explicit FontEditorDialog(QWidget *parent = nullptr);
    ~FontEditorDialog();
    KFontChooser *fontChooser;
};

} // Namespace KoChart

#endif // KCHART_FONT_EDITOR_DIALOG_H

/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "FontEditorDialog.h"

#include <KLocalizedString>
#include <kfontchooser.h>

using namespace KoChart;


FontEditorDialog::FontEditorDialog(QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18n("Select Font"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);

    QStringList list;
    KFontChooser::getFontList(list, KFontChooser::SmoothScalableFonts);
    fontChooser = new KFontChooser(this, KFontChooser::NoDisplayFlags, list, 7);

    setMainWidget(fontChooser);
}

FontEditorDialog::~FontEditorDialog()
{
}

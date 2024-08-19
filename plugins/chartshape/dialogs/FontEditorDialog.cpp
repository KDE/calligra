/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FontEditorDialog.h"

#include <KFontChooser>
#include <KLocalizedString>

using namespace KoChart;

FontEditorDialog::FontEditorDialog(QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18n("Select Font"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);

    const QStringList list = KFontChooser::createFontList(KFontChooser::SmoothScalableFonts);
    fontChooser = new KFontChooser(KFontChooser::NoDisplayFlags, this);
    fontChooser->setFontListItems(list);

    setMainWidget(fontChooser);
}

FontEditorDialog::~FontEditorDialog() = default;

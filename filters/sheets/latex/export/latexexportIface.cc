/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Robert JACOLIN <rjacolin@ifrance.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
   This file use code from koTemplateOpenDia for the method chooseSlot.
*/

#include <latexexportIface.h>
#include "kspreadlatexexportdiaImpl.h"

/*
 *  Constructs a WordsLatexExportDia which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
LatexExportIface::LatexExportIface(KSpreadLatexExportDiaImpl* dia)
        : DCOPObject("FilterConfigDia")
{
    _dialog = dia;
}

LatexExportIface::~LatexExportIface()
{
}

void LatexExportIface::useDefaultConfig()
{
    _dialog->accept();
}


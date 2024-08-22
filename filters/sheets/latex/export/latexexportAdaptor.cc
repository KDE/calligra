/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Robert JACOLIN <rjacolin@ifrance.com>
   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
   This file use code from koTemplateOpenDia for the method chooseSlot.
*/

#include <latexexportAdaptor.h>
#include "latexexportdialog.h"

LatexExportAdaptor::LatexExportAdaptor(LatexExportDialog* dia)
        : QDBusAbstractAdaptor(dia)
{
    // constructor
    setAutoRelaySignals(true);
    _dialog = dia;
}

LatexExportAdaptor::~LatexExportAdaptor()
= default;

void LatexExportAdaptor::useDefaultConfig()
{
    _dialog->accept();
}

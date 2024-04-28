/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Robert JACOLIN <rjacolin@ifrance.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __LATEXEXPORTIFACE_H__
#define __LATEXEXPORTIFACE_H__

#include <dcopobject.h>

class KSpreadLatexExportDiaImpl;

class LatexExportIface : public DCOPObject
{
    K_DCOP

public:
    explicit LatexExportIface(KSpreadLatexExportDiaImpl *dia);

    ~LatexExportIface();

    k_dcop : void useDefaultConfig();

private:
    KSpreadLatexExportDiaImpl *_dialog;
};

#endif /* __LATEXEXPORTIFACE_H__ */

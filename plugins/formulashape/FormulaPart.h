/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FORMULAPART_H
#define FORMULAPART_H

#include <KoPart.h>

#include "koformula_export.h"

class KoView;

class FormulaPart : public KoPart
{
    Q_OBJECT

public:
    explicit FormulaPart(QObject *parent);

    ~FormulaPart() override;

    /// reimplemented
    KoView *createViewInstance(KoDocument *document, QWidget *parent) override;

    /// reimplemented
    KoMainWindow *createMainWindow() override;
};

#endif

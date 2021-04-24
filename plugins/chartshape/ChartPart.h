/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CHARTPART_H
#define CHARTPART_H

#include <KoPart.h>

class KoView;

class ChartPart : public KoPart
{
    Q_OBJECT

public:
    explicit ChartPart(QObject *parent);

    virtual ~ChartPart();

    /// reimplemented
    KoView *createViewInstance(KoDocument *document, QWidget *parent) override;

    /// reimplemented
    KoMainWindow *createMainWindow() override;
};

#endif

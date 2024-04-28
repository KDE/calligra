/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2013 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPABACKGROUNDFILLWIDGET_H
#define KOPABACKGROUNDFILLWIDGET_H

#include <KoFillConfigWidget.h>

#include "kopageapp_export.h"

class KoShape;
class KoPAViewBase;

class KOPAGEAPP_EXPORT KoPABackgroundFillWidget : public KoFillConfigWidget
{
public:
    explicit KoPABackgroundFillWidget(QWidget *parent);

    void setView(KoPAViewBase *view);

    QList<KoShape *> currentShapes() override;

    KoShape *currentShape() override;
};

#endif /* KOPABACKGROUNDFILLWIDGET_H */

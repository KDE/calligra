/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ChartPart.h"
#include "ChartDocument.h"

#include <KoComponentData.h>

#include <calligra-version.h>

#include <KAboutData>

ChartPart::ChartPart(QObject *parent)
    : KoPart(KoComponentData(KAboutData(QStringLiteral("kochart"), QStringLiteral("KoChart"), QStringLiteral(CALLIGRA_VERSION_STRING))), parent)
{
}

ChartPart::~ChartPart() = default;

KoView *ChartPart::createViewInstance(KoDocument *document, QWidget *parent)
{
    Q_UNUSED(document);
    Q_UNUSED(parent);

    return nullptr;
}

KoMainWindow *ChartPart::createMainWindow()
{
    return new KoMainWindow(CHART_MIME_TYPE, componentData());
}

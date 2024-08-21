/*
 *  SPDX-FileCopyrightText: 2007-2010 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "zoomcontroller_test.h"

#include <QCoreApplication>
#include <QTest>

#include <KActionCollection>
#include <WidgetsDebug.h>

#include "KoCanvasControllerWidget.h"
#include "KoZoomController.h"
#include "KoZoomHandler.h"

void zoomcontroller_test::testApi()
{
    KoZoomHandler zoomHandler;
    KoZoomController zoomController(new KoCanvasControllerWidget(nullptr), &zoomHandler, new KActionCollection(this), KoZoomAction::AspectMode);
    Q_UNUSED(zoomController);
}

QTEST_MAIN(zoomcontroller_test)

/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2013 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPABackgroundFillWidget.h"

#include <KoCanvasBase.h>
#include <KoDocumentResourceManager.h>
#include <KoPAViewBase.h>
#include <KoPageApp.h>
#include <KoShape.h>

#include <KLocalizedString>

KoPABackgroundFillWidget::KoPABackgroundFillWidget(QWidget *parent)
    : KoFillConfigWidget(parent)
{
    setWindowTitle(i18n("Background"));
}

void KoPABackgroundFillWidget::setView(KoPAViewBase *view)
{
    Q_ASSERT(view);
    connect(view->proxyObject, &KoPAViewProxyObject::activePageChanged, this, &KoPABackgroundFillWidget::shapeChanged);
}

KoShape *KoPABackgroundFillWidget::currentShape()
{
    KoShape *slide = canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage);
    return slide;
}

QList<KoShape *> KoPABackgroundFillWidget::currentShapes()
{
    KoShape *slide = canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage);
    QList<KoShape *> list;
    list.append(slide);
    return list;
}

/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPABackgroundTool.h"

// Qt includes
#include <QList>

// KF5
#include <KLocalizedString>

// Calligra includes
#include <KoCanvasResourceManager.h>
#include <KoPACanvasBase.h>
#include <KoPAPageBase.h> // this is needed to make setResource work correctly
#include <KoPAViewBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include "KoPABackgroundToolWidget.h"
#include "KoPAMasterPageDocker.h"
#include <KoPADocument.h>

KoPABackgroundTool::KoPABackgroundTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
{
    m_view = static_cast<KoPACanvasBase *>(canvas)->koPAView();
}

KoPABackgroundTool::~KoPABackgroundTool() = default;

void KoPABackgroundTool::paint(QPainter & /*painter*/, const KoViewConverter & /*converter*/)
{
}

void KoPABackgroundTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &)
{
    Q_UNUSED(toolActivation);

    canvas()->shapeManager()->selection()->deselectAll();
    canvas()->resourceManager()->setResource(KoPageApp::CurrentPage, m_view->activePage());

    connect(m_view->proxyObject, &KoPAViewProxyObject::activePageChanged, this, &KoPABackgroundTool::slotActivePageChanged);
}

void KoPABackgroundTool::deactivate()
{
    disconnect(m_view->proxyObject, &KoPAViewProxyObject::activePageChanged, this, &KoPABackgroundTool::slotActivePageChanged);
    canvas()->resourceManager()->clearResource(KoPageApp::CurrentPage);
}

void KoPABackgroundTool::mousePressEvent(KoPointerEvent *event)
{
    event->ignore();
}

void KoPABackgroundTool::mouseMoveEvent(KoPointerEvent *event)
{
    event->ignore();
}

void KoPABackgroundTool::mouseReleaseEvent(KoPointerEvent *event)
{
    event->ignore();
}

void KoPABackgroundTool::slotActivePageChanged()
{
    canvas()->resourceManager()->setResource(KoPageApp::CurrentPage, m_view->activePage());
}

KoPAViewBase *KoPABackgroundTool::view() const
{
    return m_view;
}

QList<QPointer<QWidget>> KoPABackgroundTool::createOptionWidgets()
{
    KoPABackgroundToolWidget *widget = new KoPABackgroundToolWidget(this);
    QList<QPointer<QWidget>> widgets;
    const QString title = (m_view->kopaDocument()->pageType() == KoPageApp::Page) ? i18n("Page Background") : i18n("Background");
    widget->setWindowTitle(title);
    widgets.append(widget);
    widgets.append(m_addOnWidgets);
#if 0
    KoPAMasterPageDocker *masterPageDocker = new KoPAMasterPageDocker();
    masterPageDocker->setView( static_cast<KoPACanvas *>(m_canvas)->koPAView() );
    widgets.insert( i18n("Master Page"), masterPageDocker );
#endif
    return widgets;
}

void KoPABackgroundTool::addOptionWidget(QWidget *widget)
{
    m_addOnWidgets.append(widget);
}

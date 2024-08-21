/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrClickActionDocker.h"

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QLabel>
#include <QVBoxLayout>

#include <KLocalizedString>

#include <KPrDocument.h>
#include <KoCanvasBase.h>
#include <KoEventAction.h>
#include <KoEventActionFactoryBase.h>
#include <KoEventActionRegistry.h>
#include <KoPACanvas.h>
#include <KoSelection.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>

#include "KPrEventActionData.h"
#include "KPrPage.h"
#include "KPrSoundCollection.h"
#include "KPrSoundData.h"
#include "KPrView.h"
#include "StageDebug.h"
#include <KPrEventActionWidget.h>
#include <KPresenter.h>

KPrClickActionDocker::KPrClickActionDocker(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , m_view(nullptr)
    , m_soundCollection(nullptr)
    , m_canvas(nullptr)
{
    setObjectName("KPrClickActionDocker");
    // setup widget layout
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins({});

    const QList<KoEventActionFactoryBase *> factories = KoEventActionRegistry::instance()->presentationEventActions();
    for (KoEventActionFactoryBase *factory : factories) {
        QWidget *optionWidget = factory->createOptionWidget();
        layout->addWidget(optionWidget);
        m_eventActionWidgets.insert(factory->id(), optionWidget);
        connect(optionWidget, SIGNAL(addCommand(KUndo2Command *)), this, SLOT(addCommand(KUndo2Command *)));
    }
    // The following widget activates a special feature in the
    // ToolOptionsDocker that makes the components of the widget align
    // to the top if there is extra space.
    QWidget *specialSpacer = new QWidget(this);
    specialSpacer->setObjectName("SpecialSpacer");
    layout->addWidget(specialSpacer);
}

void KPrClickActionDocker::selectionChanged()
{
    if (!m_canvas)
        return;
    KoSelection *selection = m_canvas->shapeManager()->selection();
    KoShape *shape = selection->firstSelectedShape();

    if (shape) {
        QSet<KoEventAction *> eventActions = shape->eventActions();
        QMap<QString, KoEventAction *> eventActionMap;
        foreach (KoEventAction *eventAction, eventActions) {
            eventActionMap.insert(eventAction->id(), eventAction);
        }

        QMap<QString, QWidget *>::const_iterator it(m_eventActionWidgets.constBegin());

        for (; it != m_eventActionWidgets.constEnd(); ++it) {
            KPrEventActionWidget *actionWidget = dynamic_cast<KPrEventActionWidget *>(it.value());
            if (actionWidget) {
                // if it is not in the map a default value 0 pointer will be returned
                KPrEventActionData data(shape, eventActionMap.value(it.key()), m_soundCollection);
                actionWidget->setData(&data);
            }
        }
    } else {
        foreach (QWidget *widget, m_eventActionWidgets) {
            KPrEventActionWidget *actionWidget = dynamic_cast<KPrEventActionWidget *>(widget);
            if (actionWidget) {
                KPrEventActionData data(nullptr, nullptr, m_soundCollection);
                actionWidget->setData(&data);
            }
        }
    }
}

void KPrClickActionDocker::setCanvas(KoCanvasBase *canvas)
{
    if (m_canvas) {
        m_canvas->disconnectCanvasObserver(this);
    }

    m_canvas = canvas;

    if (m_canvas) {
        connect(m_canvas->shapeManager(), &KoShapeManager::selectionChanged, this, &KPrClickActionDocker::selectionChanged);
    }

    selectionChanged();
}

void KPrClickActionDocker::setView(KoPAViewBase *view)
{
    m_view = view;
    if (m_view->kopaDocument()->resourceManager()->hasResource(KPresenter::SoundCollection)) {
        QVariant variant = m_view->kopaDocument()->resourceManager()->resource(KPresenter::SoundCollection);
        m_soundCollection = variant.value<KPrSoundCollection *>();
    }

    setCanvas(view->kopaCanvas());
}

void KPrClickActionDocker::addCommand(KUndo2Command *command)
{
    if (m_view) {
        m_view->kopaCanvas()->addCommand(command);
    }
}

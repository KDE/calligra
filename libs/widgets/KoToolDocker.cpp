/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2005-2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoToolDocker.h"

#include <KoDockWidgetTitleBar.h>
#include <KoDockWidgetTitleBarButton.h>
#include <KoShapePropertyWidget.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QPointer>
#include <QScrollArea>
#include <QScrollBar>
#include <QSet>
#include <QStackedWidget>
#include <QStyleOptionFrame>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include <WidgetsDebug.h>

class Q_DECL_HIDDEN KoToolDocker::Private
{
public:
    Private(KoToolDocker *dock)
        : q(dock)
    {
    }

    QList<QPointer<QWidget>> currentWidgetList;
    QSet<QWidget *> currentAuxWidgets;
    QScrollArea *scrollArea;
    QWidget *hiderWidget; // non current widgets are hidden by being children of this
    QWidget *housekeeperWidget;
    QVBoxLayout *housekeeperLayout;
    KoToolDocker *const q;

    void resetWidgets()
    {
        currentWidgetList.clear();
        qDeleteAll(currentAuxWidgets);
        currentAuxWidgets.clear();
    }

    void recreateLayout(const QList<QPointer<QWidget>> &optionWidgetList)
    {
        for (QPointer<QWidget> widget : std::as_const(currentWidgetList)) {
            if (!widget.isNull() && widget && hiderWidget) {
                widget->setParent(hiderWidget);
            }
        }
        qDeleteAll(currentAuxWidgets);
        currentAuxWidgets.clear();

        QList<QLayoutItem *> spacerItems;
        for (int i = 0, count = housekeeperLayout->count(); i < count; i++) {
            const auto item = housekeeperLayout->itemAt(i);
            const auto spacer = item->spacerItem();
            if (spacer) {
                spacerItems << spacer;
            }
        }
        for (const auto spacer : std::as_const(spacerItems)) {
            housekeeperLayout->removeItem(spacer);
            delete spacer;
        }

        currentWidgetList = optionWidgetList;

        housekeeperLayout->setSpacing(2);
        for (QPointer<QWidget> widget : currentWidgetList) {
            if (widget.isNull() || widget->objectName().isEmpty()) {
                Q_ASSERT(!(widget->objectName().isEmpty()));
                continue; // skip this docker in release build when assert don't crash
            }
            if (!widget->windowTitle().isEmpty()) {
                auto label = new QLabel(widget->windowTitle());
                auto palette = label->palette();
                auto foregroundColor = palette.color(QPalette::WindowText);
                foregroundColor.setAlphaF(0.8);
                palette.setColor(QPalette::WindowText, foregroundColor);
                label->setPalette(palette);
                housekeeperLayout->addWidget(label);
                currentAuxWidgets.insert(label);

                auto separator = new QFrame;
                separator->setFrameShape(QFrame::HLine);
                separator->setFixedHeight(1);
                separator->setPalette(palette);
                currentAuxWidgets.insert(separator);
                housekeeperLayout->addWidget(separator);
                housekeeperLayout->addSpacing(q->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing) / 2);
            }
            housekeeperLayout->addWidget(widget);
            widget->show();
        }
        housekeeperLayout->addSpacerItem(new QSpacerItem(QSizePolicy::Minimum, QSizePolicy::Expanding));
        housekeeperLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        housekeeperLayout->invalidate();
    }
};

KoToolDocker::KoToolDocker(QWidget *parent)
    : QDockWidget(i18n("Tool Options"), parent)
    , d(new Private(this))
{
    KConfigGroup cfg = KSharedConfig::openConfig()->group("DockWidget sharedtooldocker");

    toggleViewAction()->setVisible(false); // should always be visible, so hide option in menu
    setFeatures(DockWidgetMovable | DockWidgetFloatable);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setTitleBarWidget(new KoDockWidgetTitleBar(this));

    d->housekeeperWidget = new QWidget;
    d->housekeeperLayout = new QVBoxLayout(d->housekeeperWidget);
    d->housekeeperLayout->setContentsMargins(4, 4, 4, 0);
    d->housekeeperLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    d->housekeeperWidget->setLayout(d->housekeeperLayout);

    d->hiderWidget = new QWidget(d->housekeeperWidget);
    d->hiderWidget->setVisible(false);

    d->scrollArea = new QScrollArea;
    d->scrollArea->setWidget(d->housekeeperWidget);
    d->scrollArea->setFrameShape(QFrame::NoFrame);
    d->scrollArea->setWidgetResizable(true);
    d->scrollArea->setFocusPolicy(Qt::NoFocus);

    setWidget(d->scrollArea);
}

KoToolDocker::~KoToolDocker() = default;

bool KoToolDocker::hasOptionWidget()
{
    return !d->currentWidgetList.isEmpty();
}

void KoToolDocker::setOptionWidgets(const QList<QPointer<QWidget>> &optionWidgetList)
{
    d->recreateLayout(optionWidgetList);
}

void KoToolDocker::resetWidgets()
{
    d->resetWidgets();
}

void KoToolDocker::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != nullptr);
}

void KoToolDocker::unsetCanvas()
{
    setEnabled(false);
}

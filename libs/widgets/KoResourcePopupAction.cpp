/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoResourcePopupAction.h"

#include "KoCheckerBoardPainter.h"
#include "KoResource.h"
#include "KoResourceItemDelegate.h"
#include "KoResourceItemView.h"
#include "KoResourceModel.h"
#include "KoResourceServerAdapter.h"
#include "KoShapeBackground.h"
#include <KoAbstractGradient.h>
#include <KoGradientBackground.h>
#include <KoImageCollection.h>
#include <KoPattern.h>
#include <KoPatternBackground.h>

#include <QGradient>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QPainter>
#include <QRect>
#include <QToolButton>
#include <QWidgetAction>

#include <utility>

class KoResourcePopupAction::Private
{
public:
    Private()
        : resourceList(nullptr)
        , background(nullptr)
        , checkerPainter(4)
    {
    }
    QMenu *menu;
    KoResourceItemView *resourceList;
    QSharedPointer<KoShapeBackground> background;
    KoCheckerBoardPainter checkerPainter;
};

KoResourcePopupAction::KoResourcePopupAction(QSharedPointer<KoAbstractResourceServerAdapter> resourceAdapter, QObject *parent)
    : QAction(parent)
    , d(new Private())
{
    Q_ASSERT(resourceAdapter);

    d->menu = new QMenu();
    QWidget *widget = new QWidget();
    QWidgetAction *wdgAction = new QWidgetAction(this);

    d->resourceList = new KoResourceItemView(widget);
    d->resourceList->setModel(new KoResourceModel(resourceAdapter, widget));
    d->resourceList->setItemDelegate(new KoResourceItemDelegate(widget));
    KoResourceModel *resourceModel = qobject_cast<KoResourceModel *>(d->resourceList->model());
    if (resourceModel) {
        resourceModel->setColumnCount(1);
    }

    KoResource *resource = nullptr;
    if (resourceAdapter->resources().count() > 0) {
        resource = resourceAdapter->resources().at(0);
    }

    KoAbstractGradient *gradient = dynamic_cast<KoAbstractGradient *>(resource);
    KoPattern *pattern = dynamic_cast<KoPattern *>(resource);
    if (gradient) {
        QGradient *qg = gradient->toQGradient();
        qg->setCoordinateMode(QGradient::ObjectBoundingMode);
        d->background = QSharedPointer<KoShapeBackground>(new KoGradientBackground(qg));
    } else if (pattern) {
        KoImageCollection *collection = new KoImageCollection();
        d->background = QSharedPointer<KoShapeBackground>(new KoPatternBackground(collection));
        static_cast<KoPatternBackground *>(d->background.data())->setPattern(pattern->pattern());
    }

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(d->resourceList);
    widget->setLayout(layout);

    wdgAction->setDefaultWidget(widget);
    d->menu->addAction(wdgAction);
    setMenu(d->menu);
    new QHBoxLayout(d->menu);
    d->menu->layout()->addWidget(widget);
    d->menu->layout()->setContentsMargins({});

    connect(d->resourceList, &QAbstractItemView::clicked, this, &KoResourcePopupAction::indexChanged);

    updateIcon();
}

KoResourcePopupAction::~KoResourcePopupAction()
{
    /* Removing the actions here make them be deleted together with their default widget.
     * This happens only if the actions are QWidgetAction, and we know they are since
     * the only ones added are in KoResourcePopupAction constructor. */
    int i = 0;
    while (!d->menu->actions().empty()) {
        d->menu->removeAction(d->menu->actions().at(i));
        ++i;
    }

    delete d->menu;

    delete d;
}

QSharedPointer<KoShapeBackground> KoResourcePopupAction::currentBackground() const
{
    return d->background;
}

void KoResourcePopupAction::setCurrentBackground(QSharedPointer<KoShapeBackground> background)
{
    d->background = std::move(background);

    updateIcon();
}

void KoResourcePopupAction::indexChanged(const QModelIndex &modelIndex)
{
    if (!modelIndex.isValid()) {
        return;
    }

    d->menu->hide();

    KoResource *resource = static_cast<KoResource *>(modelIndex.internalPointer());
    if (resource) {
        KoAbstractGradient *gradient = dynamic_cast<KoAbstractGradient *>(resource);
        KoPattern *pattern = dynamic_cast<KoPattern *>(resource);
        if (gradient) {
            QGradient *qg = gradient->toQGradient();
            qg->setCoordinateMode(QGradient::ObjectBoundingMode);
            d->background = QSharedPointer<KoShapeBackground>(new KoGradientBackground(qg));
        } else if (pattern) {
            KoImageCollection *collection = new KoImageCollection();
            d->background = QSharedPointer<KoShapeBackground>(new KoPatternBackground(collection));
            qSharedPointerDynamicCast<KoPatternBackground>(d->background)->setPattern(pattern->pattern());
        }

        Q_EMIT resourceSelected(d->background);

        updateIcon();
    }
}

void KoResourcePopupAction::updateIcon()
{
    QSize iconSize;
    QToolButton *toolButton = dynamic_cast<QToolButton *>(parentWidget());
    if (toolButton) {
        iconSize = QSize(toolButton->iconSize());
    } else {
        iconSize = QSize(16, 16);
    }

    // This must be a QImage, as drawing to a QPixmap outside the
    // UI thread will cause sporadic crashes.
    QImage pm = QImage(iconSize, QImage::Format_ARGB32_Premultiplied);

    pm.fill(Qt::transparent);

    QPainter p(&pm);
    QSharedPointer<KoGradientBackground> gradientBackground = qSharedPointerDynamicCast<KoGradientBackground>(d->background);
    QSharedPointer<KoPatternBackground> patternBackground = qSharedPointerDynamicCast<KoPatternBackground>(d->background);

    if (gradientBackground) {
        QRect innerRect(0, 0, iconSize.width(), iconSize.height());
        QLinearGradient paintGradient;
        paintGradient.setStops(gradientBackground->gradient()->stops());
        paintGradient.setStart(innerRect.topLeft());
        paintGradient.setFinalStop(innerRect.topRight());

        d->checkerPainter.paint(p, innerRect);
        p.fillRect(innerRect, QBrush(paintGradient));
    } else if (patternBackground) {
        d->checkerPainter.paint(p, QRect(QPoint(), iconSize));
        p.fillRect(0, 0, iconSize.width(), iconSize.height(), patternBackground->pattern());
    }

    p.end();

    setIcon(QIcon(QPixmap::fromImage(pm)));
}

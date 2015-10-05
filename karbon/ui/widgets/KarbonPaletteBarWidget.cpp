/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonPaletteBarWidget.h"
#include "KarbonPaletteWidget.h"
#include <KoResourceServerProvider.h>

#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoShape.h>
#include <KoShapeBackground.h>
#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoShapeStroke.h>

#include <KSharedConfig>
#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>

const int FixedWidgetSize = 20;
const int ScrollUpdateIntervall = 25;
const QString DocumentColorsName("Document Colors");

KarbonPaletteBarWidget::KarbonPaletteBarWidget(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
    , m_prevButton(0), m_nextButton(0), m_choosePalette(0), m_colorBar(0)
    , m_palettes(KoResourceServerProvider::instance()->paletteServer())
{
    m_palettes.connectToResourceServer();

    m_prevButton = new QToolButton(this);
    m_prevButton->setAutoRepeat(true);
    m_prevButton->setAutoRepeatInterval(ScrollUpdateIntervall);
    m_nextButton = new QToolButton(this);
    m_nextButton->setAutoRepeat(true);
    m_nextButton->setAutoRepeatInterval(ScrollUpdateIntervall);

    m_choosePalette = new QToolButton(this);
    m_choosePalette->setToolTip(i18n("Select palette"));
    m_choosePalette->setArrowType(Qt::DownArrow);

    m_colorBar = new KarbonPaletteWidget(this);
    m_colorBar->setOrientation(orientation);
    connect(m_prevButton, SIGNAL(clicked()), m_colorBar, SLOT(scrollBackward()));
    connect(m_nextButton, SIGNAL(clicked()), m_colorBar, SLOT(scrollForward()));
    connect(m_colorBar, SIGNAL(colorSelected(KoColor)), this, SIGNAL(colorSelected(KoColor)));
    connect(m_colorBar, SIGNAL(scrollOffsetChanged()), this, SLOT(updateButtons()));
    connect(m_choosePalette, SIGNAL(clicked()), this, SLOT(selectPalette()));

    setMinimumSize(FixedWidgetSize, FixedWidgetSize);
    m_colorBar->setMinimumSize(FixedWidgetSize, FixedWidgetSize);

    createLayout();

    m_documentColors.setName(DocumentColorsName);

    QList<KoResource*> resources = m_palettes.resources();
    if (resources.count()) {
        KConfigGroup paletteGroup = KSharedConfig::openConfig()->group("PaletteBar");
        QString lastPalette = paletteGroup.readEntry("LastPalette", "SVG Colors");
        KoResource *r = resources.first();
        if (lastPalette == DocumentColorsName) {
            r = &m_documentColors;
        } else {
            foreach(KoResource *res, resources) {
                if (res->name() == lastPalette) {
                    r = res;
                    break;
                }
            }
        }
        m_colorBar->setPalette(dynamic_cast<KoColorSet*>(r));
        updateDocumentColors();
    }
}

KarbonPaletteBarWidget::~KarbonPaletteBarWidget()
{
}

void KarbonPaletteBarWidget::setOrientation(Qt::Orientation orientation)
{
    if (m_colorBar->orientation() == orientation)
        return;

    m_colorBar->setOrientation(orientation);
    createLayout();
}

void KarbonPaletteBarWidget::createLayout()
{
    if (m_colorBar->orientation() == Qt::Horizontal) {
        m_prevButton->setArrowType(Qt::LeftArrow);
        m_nextButton->setArrowType(Qt::RightArrow);
        QHBoxLayout *h = new QHBoxLayout();
        h->addWidget(m_choosePalette);
        h->addWidget(m_colorBar, 1, Qt::AlignVCenter);
        h->addWidget(m_prevButton);
        h->addWidget(m_nextButton);
        setLayout(h);
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        m_colorBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    } else {
        m_prevButton->setArrowType(Qt::UpArrow);
        m_nextButton->setArrowType(Qt::DownArrow);
        QVBoxLayout *v = new QVBoxLayout();
        v->addWidget(m_choosePalette);
        v->addWidget(m_colorBar, 1, Qt::AlignHCenter);
        v->addWidget(m_prevButton);
        v->addWidget(m_nextButton);
        setLayout(v);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
        m_colorBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    }
    layout()->setMargin(0);
    layout()->setSpacing(2);
}

void KarbonPaletteBarWidget::updateButtons()
{
    m_prevButton->setEnabled(m_colorBar->currentScrollOffset() > 0);
    m_nextButton->setEnabled(m_colorBar->currentScrollOffset() < m_colorBar->maximalScrollOffset());
}

void KarbonPaletteBarWidget::selectPalette()
{
    QList<KoResource*> resources = m_palettes.resources();
    if (!resources.count())
        return;

    int index = 0;

    QMenu palletteMenu;
    QAction *action = palletteMenu.addAction(DocumentColorsName);
    action->setData(QVariant(index++));
    if (m_colorBar->palette() == &m_documentColors) {
        action->setCheckable(true);
        action->setChecked(true);
    }

    foreach(KoResource* r, resources) {
        QAction *a = palletteMenu.addAction(r->name());
        if (r == m_colorBar->palette()) {
            a->setCheckable(true);
            a->setChecked(true);
        }
        a->setData(QVariant(index++));
    }

    QAction *selectedAction = palletteMenu.exec(m_choosePalette->mapToGlobal(QPoint(0,0)));
    if (selectedAction) {
        int selectedIndex = selectedAction->data().toInt();
        KoColorSet *selectedColorSet = 0;
        if (selectedIndex) {
            selectedColorSet = dynamic_cast<KoColorSet*>(resources.at(selectedIndex-1));
        } else {
            selectedColorSet = &m_documentColors;
        }
        if (selectedColorSet) {
            m_colorBar->setPalette(selectedColorSet);
            KConfigGroup paletteGroup = KSharedConfig::openConfig()->group("PaletteBar");
            paletteGroup.writeEntry("LastPalette", selectedColorSet->name());
            updateDocumentColors();
        }
    }
}

uint qHash(const QColor &key)
{
    int h = (key.hue() + 360) % 360;
    return ((h << 17) | (key.saturation() << 8)) | key.value();
}

void KarbonPaletteBarWidget::updateDocumentColors()
{
    KoCanvasBase* canvas = KoToolManager::instance()->activeCanvasController()->canvas();
    if (!canvas)
        return;

    if (m_colorBar->palette() != &m_documentColors)
        return;

    QMap<uint, QColor> colors;

    foreach(KoShape *shape, canvas->shapeManager()->shapes()) {
        QSharedPointer<KoShapeBackground> fill = shape->background();
        if (fill) {
            QSharedPointer<KoColorBackground> cbg = qSharedPointerDynamicCast<KoColorBackground>(shape->background());
            if (cbg) {
                //colors.insert(cbg->color());
                colors.insert(qHash(cbg->color()), cbg->color());
            }
            QSharedPointer<KoGradientBackground> gbg = qSharedPointerDynamicCast<KoGradientBackground>(shape->background());
            if (gbg) {
                foreach(const QGradientStop &stop, gbg->gradient()->stops()) {
                     colors.insert(qHash(stop.second), stop.second);
                }
            }
        }
        KoShapeStrokeModel *stroke = shape->stroke();
        if (stroke) {
            KoShapeStroke *lb = dynamic_cast<KoShapeStroke*>(shape->stroke());
            if (lb) {
                if (lb->lineStyle() == Qt::SolidLine) {
                    colors.insert(qHash(lb->color()), lb->color());
                } else if (lb->lineBrush().gradient()) {
                    foreach(const QGradientStop &stop, lb->lineBrush().gradient()->stops()) {
                         colors.insert(qHash(stop.second), stop.second);
                    }
                }
            }
        }
    }

    int colorCount = m_documentColors.nColors();
    for (int i = 0; i < colorCount; ++i) {
        m_documentColors.remove(m_documentColors.getColor(0));
    }
    foreach(const QColor &c, colors) {
        KoColorSetEntry e;
        e.color.fromQColor(c);
        m_documentColors.add(e);

    }
    m_colorBar->update();
}


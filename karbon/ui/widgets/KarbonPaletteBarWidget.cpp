/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonPaletteBarWidget.h"
#include "KarbonPaletteWidget.h"
#include <KoResourceServerProvider.h>

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoShape.h>
#include <KoShapeBackground.h>
#include <KoShapeManager.h>
#include <KoShapeStroke.h>
#include <KoToolManager.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QHBoxLayout>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>

const int FixedWidgetSize = 20;
const int ScrollUpdateIntervall = 25;
const QString DocumentColorsName("Document Colors");

KarbonPaletteBarWidget::KarbonPaletteBarWidget(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
    , m_prevButton(nullptr)
    , m_nextButton(nullptr)
    , m_choosePalette(nullptr)
    , m_colorBar(nullptr)
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
    connect(m_prevButton, &QAbstractButton::clicked, m_colorBar, &KarbonPaletteWidget::scrollBackward);
    connect(m_nextButton, &QAbstractButton::clicked, m_colorBar, &KarbonPaletteWidget::scrollForward);
    connect(m_colorBar, &KarbonPaletteWidget::colorSelected, this, &KarbonPaletteBarWidget::colorSelected);
    connect(m_colorBar, &KarbonPaletteWidget::scrollOffsetChanged, this, &KarbonPaletteBarWidget::updateButtons);
    connect(m_choosePalette, &QAbstractButton::clicked, this, &KarbonPaletteBarWidget::selectPalette);

    setMinimumSize(FixedWidgetSize, FixedWidgetSize);
    m_colorBar->setMinimumSize(FixedWidgetSize, FixedWidgetSize);

    createLayout();

    m_documentColors.setName(DocumentColorsName);

    QList<KoResource *> resources = m_palettes.resources();
    if (resources.count()) {
        KConfigGroup paletteGroup = KSharedConfig::openConfig()->group("PaletteBar");
        QString lastPalette = paletteGroup.readEntry("LastPalette", "SVG Colors");
        KoResource *r = resources.first();
        if (lastPalette == DocumentColorsName) {
            r = &m_documentColors;
        } else {
            foreach (KoResource *res, resources) {
                if (res->name() == lastPalette) {
                    r = res;
                    break;
                }
            }
        }
        m_colorBar->setPalette(dynamic_cast<KoColorSet *>(r));
        updateDocumentColors();
    }
}

KarbonPaletteBarWidget::~KarbonPaletteBarWidget() = default;

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
    layout()->setContentsMargins({});
    layout()->setSpacing(2);
}

void KarbonPaletteBarWidget::updateButtons()
{
    m_prevButton->setEnabled(m_colorBar->currentScrollOffset() > 0);
    m_nextButton->setEnabled(m_colorBar->currentScrollOffset() < m_colorBar->maximalScrollOffset());
}

void KarbonPaletteBarWidget::selectPalette()
{
    QList<KoResource *> resources = m_palettes.resources();
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

    foreach (KoResource *r, resources) {
        QAction *a = palletteMenu.addAction(r->name());
        if (r == m_colorBar->palette()) {
            a->setCheckable(true);
            a->setChecked(true);
        }
        a->setData(QVariant(index++));
    }

    QAction *selectedAction = palletteMenu.exec(m_choosePalette->mapToGlobal(QPoint(0, 0)));
    if (selectedAction) {
        int selectedIndex = selectedAction->data().toInt();
        KoColorSet *selectedColorSet = nullptr;
        if (selectedIndex) {
            selectedColorSet = dynamic_cast<KoColorSet *>(resources.at(selectedIndex - 1));
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
    KoCanvasBase *canvas = KoToolManager::instance()->activeCanvasController()->canvas();
    if (!canvas)
        return;

    if (m_colorBar->palette() != &m_documentColors)
        return;

    QMap<uint, QColor> colors;

    foreach (KoShape *shape, canvas->shapeManager()->shapes()) {
        QSharedPointer<KoShapeBackground> fill = shape->background();
        if (fill) {
            QSharedPointer<KoColorBackground> cbg = qSharedPointerDynamicCast<KoColorBackground>(shape->background());
            if (cbg) {
                // colors.insert(cbg->color());
                colors.insert(qHash(cbg->color()), cbg->color());
            }
            QSharedPointer<KoGradientBackground> gbg = qSharedPointerDynamicCast<KoGradientBackground>(shape->background());
            if (gbg) {
                foreach (const QGradientStop &stop, gbg->gradient()->stops()) {
                    colors.insert(qHash(stop.second), stop.second);
                }
            }
        }
        KoShapeStrokeModel *stroke = shape->stroke();
        if (stroke) {
            KoShapeStroke *lb = dynamic_cast<KoShapeStroke *>(shape->stroke());
            if (lb) {
                if (lb->lineStyle() == Qt::SolidLine) {
                    colors.insert(qHash(lb->color()), lb->color());
                } else if (lb->lineBrush().gradient()) {
                    foreach (const QGradientStop &stop, lb->lineBrush().gradient()->stops()) {
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
    foreach (const QColor &c, colors) {
        KoColorSetEntry e;
        e.color.fromQColor(c);
        m_documentColors.add(e);
    }
    m_colorBar->update();
}

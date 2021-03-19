/*
 * Copyright (c) 2005-2009 Thomas Zander <zander@kde.org>
 * Copyright (c) 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * Copyright (c) 2010 Cyrille Berger <cberger@cberger.net>
 * Copyright (c) 2011 C. Boemann <cbo@boemann.dk>
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

#include <algorithm>

#include "KoModeBox_p.h"

#include <KoCanvasControllerWidget.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoShapeLayer.h>

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <kselectaction.h>
#include <ksharedconfig.h>

#include <QIcon>

#include <QMap>
#include <QList>
#include <QHash>
#include <QSet>
#include <QRect>
#include <QLabel>
#include <QFrame>
#include <QGridLayout>
#include <QApplication>
#include <QTabBar>
#include <QStackedWidget>
#include <QPainter>
#include <QTextLayout>
#include <QMenu>
#include <QScrollBar>
#include <QFontDatabase>

#include <WidgetsDebug.h>

class KoModeBox::Private
{
public:
    Private(KoCanvasController *c)
        : canvas(c->canvas())
        , activeId(-1)
        , iconTextFitted(true)
        , fittingIterations(0)
        , iconMode(IconAndText)
        , verticalTabsSide(TopSide)
        , horizontalTabsSide(LeftSide)
        , horizontalMode(false)
    {
    }

    KoCanvasBase *canvas;
    QGridLayout *layout;
    QList<KoToolAction*> toolActions; // toolActions maintained by toolmanager
    QList<KoToolAction*> addedToolActions; //tools in the order added to QToolBox
    QMap<int, QWidget *> addedWidgets;
    QSet<QWidget *> currentAuxWidgets;
    int activeId;
    QTabBar *tabBar;
    QStackedWidget *stack;
    bool iconTextFitted;
    int fittingIterations;
    IconMode iconMode;
    VerticalTabsSide verticalTabsSide;
    HorizontalTabsSide horizontalTabsSide;
    bool horizontalMode;
};

QString KoModeBox::applicationName;

static bool compareToolActions(const KoToolAction *b1, const KoToolAction *b2)
{
    int b1Level;
    int b2Level;
    if (b1->section().contains(KoModeBox::applicationName)) {
        b1Level = 0;
    } else if (b1->section().contains("main")) {
        b1Level = 1;
    } else {
        b1Level = 2;
    }

    if (b2->section().contains(KoModeBox::applicationName)) {
        b2Level = 0;
    } else if (b2->section().contains("main")) {
        b2Level = 1;
    } else {
        b2Level = 2;
    }

    if (b1Level == b2Level) {
        return b1->priority() < b2->priority();
    } else {
        return b1Level < b2Level;
    }
}

KoModeBox::KoModeBox(KoCanvasControllerWidget *canvas, const QString &appName)
    : QQuickWidget()
    , d(new Private(canvas))
{
    applicationName = appName;

    KConfigGroup cfg =  KSharedConfig::openConfig()->group("calligra");
    d->iconMode = (IconMode)cfg.readEntry("ModeBoxIconMode", (int)IconAndText);
    d->verticalTabsSide = (VerticalTabsSide)cfg.readEntry("ModeBoxVerticalTabsSide", (int)TopSide);
    d->horizontalTabsSide = (HorizontalTabsSide)cfg.readEntry("ModeBoxHorizontalTabsSide", (int)LeftSide);

    Q_INIT_RESOURCE(KoModeBox);
    setSource(QUrl("qrc:/KoModeBox.qml"));
    show();

    /*
    d->layout = new QGridLayout();
    d->stack = new QStackedWidget();
    d->tabBar = new QTabBar();
    setIconSize();
    d->tabBar->setExpanding(d->horizontalMode);
    if (d->horizontalMode) {
        switchTabsSide(d->verticalTabsSide);
    } else {
        switchTabsSide(d->horizontalTabsSide);
    }
    d->tabBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    d->layout->addWidget(d->stack, 0, 1);

    d->layout->setContentsMargins(0,0,0,0);
    setLayout(d->layout);
    */

    foreach(KoToolAction *toolAction, KoToolManager::instance()->toolActionList()) {
        addToolAction(toolAction);
    }

    std::sort(d->toolActions.begin(), d->toolActions.end(), compareToolActions);

    // Update visibility of toolActions
    updateShownTools(QList<QString>());

    //d->tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    //connect(d->tabBar, &QTabBar::currentChanged, this, &KoModeBox::toolSelected);
    //connect(d->tabBar, &QWidget::customContextMenuRequested, this, &KoModeBox::slotContextMenuRequested);

    connect(KoToolManager::instance(), &KoToolManager::changedTool,
            this, &KoModeBox::setActiveTool);
    connect(KoToolManager::instance(), &KoToolManager::currentLayerChanged,
            this, &KoModeBox::setCurrentLayer);
    connect(KoToolManager::instance(), &KoToolManager::toolCodesSelected, this, &KoModeBox::updateShownTools);
    connect(KoToolManager::instance(),
            &KoToolManager::addedTool,
            this, &KoModeBox::toolAdded);

    connect(canvas, &KoCanvasControllerWidget::toolOptionWidgetsChanged,
         this, &KoModeBox::setOptionWidgets);
}

KoModeBox::~KoModeBox()
{
}

void KoModeBox::addToolAction(KoToolAction *toolAction)
{
    d->toolActions.append(toolAction);
}

void KoModeBox::locationChanged(Qt::DockWidgetArea area)
{
    /*
    resize(0,0);
    switch(area) {
        case Qt::TopDockWidgetArea:
        case Qt::BottomDockWidgetArea:
            d->horizontalMode = true;
            d->layout->removeWidget(d->stack);
            d->layout->addWidget(d->stack, 1, 0);
            d->layout->setColumnStretch(1, 0);
            d->layout->setRowStretch(1, 100);
            break;
        case Qt::LeftDockWidgetArea:
        case Qt::RightDockWidgetArea:
            d->horizontalMode = false;
            d->layout->removeWidget(d->stack);
            d->layout->addWidget(d->stack, 0, 1);
            d->layout->setColumnStretch(1, 100);
            d->layout->setRowStretch(1, 0);
            break;
        default:
            break;
    }
    d->layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    d->layout->invalidate();

    setIconSize();
    d->tabBar->setExpanding(d->horizontalMode);
    if (d->horizontalMode) {
        switchTabsSide(d->verticalTabsSide);
    } else {
        switchTabsSide(d->horizontalTabsSide);
    }
    */
}

void KoModeBox::setActiveTool(KoCanvasController *canvas, int id)
{
    /*
    if (canvas->canvas() == d->canvas) {
        // Clear the minimumSize instigated by the previous tool
        // The new minimumSize will be set in updateShownTools()
        if (d->addedWidgets.contains(d->activeId)) {
            ScrollArea *sa = qobject_cast<ScrollArea *>(d->addedWidgets[d->activeId]->parentWidget()->parentWidget());
            sa->setMinimumWidth(0);
            sa->setMinimumHeight(0);
        }

        d->activeId = id;
        d->tabBar->blockSignals(true);
        int i = 0;
        foreach (KoToolAction *toolAction, d->addedToolActions) {
            if (toolAction->buttonGroupId() == d->activeId) {
                d->tabBar->setCurrentIndex(i);
                d->stack->setCurrentIndex(i);
                break;
            }
            ++i;
        }
        d->tabBar->blockSignals(false);
        return;
    }*/
}

QIcon KoModeBox::createTextIcon(KoToolAction *toolAction) const
{
    Q_ASSERT(!d->horizontalMode);

    QSize iconSize = d->tabBar->iconSize();
    QFont smallFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    qreal pointSize = QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont).pointSizeF();
    smallFont.setPointSizeF(pointSize);
    // This must be a QImage, as drawing to a QPixmap outside the
    // UI thread will cause sporadic crashes.
    QImage pm(iconSize, QImage::Format_ARGB32_Premultiplied);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    if (d->horizontalTabsSide == LeftSide ) {
        p.rotate(90);
        p.translate(0,-iconSize.width());
    } else {
        p.rotate(-90);
        p.translate(-iconSize.height(),0);
    }

    QIcon::fromTheme(toolAction->iconName()).paint(&p, 0, 0, iconSize.height(), 22);

    QTextLayout textLayout(toolAction->iconText(), smallFont, p.device());
    QTextOption option = QTextOption(Qt::AlignTop | Qt::AlignHCenter);
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    textLayout.setTextOption(option);
    textLayout.beginLayout();
    qreal height = 0;
    while (1) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(iconSize.height());
        line.setPosition(QPointF(0, height));
        height += line.height();
    }
    textLayout.endLayout();

    if (textLayout.lineCount() > 2) {
        iconSize.setHeight(iconSize.height() + 8);
        d->tabBar->setIconSize(iconSize);
        d->iconTextFitted = false;
    } else if (height > iconSize.width() - 22) {
        iconSize.setWidth(22 + height);
        d->tabBar->setIconSize(iconSize);
        d->iconTextFitted = false;
    }

    p.setFont(smallFont);
    p.setPen(palette().text().color());
    textLayout.draw(&p, QPoint(0, 22));
    p.end();

    return QIcon(QPixmap::fromImage(pm));
}

QIcon KoModeBox::createSimpleIcon(KoToolAction *toolAction) const
{
    QSize iconSize = d->tabBar->iconSize();

    // This must be a QImage, as drawing to a QPixmap outside the
    // UI thread will cause sporadic crashes.
    QImage pm(iconSize, QImage::Format_ARGB32_Premultiplied);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    if (!d->horizontalMode) {
        if (d->horizontalTabsSide == LeftSide ) {
            p.rotate(90);
            p.translate(0,-iconSize.width());
        } else {
            p.rotate(-90);
            p.translate(-iconSize.height(),0);
        }
    }

    QIcon::fromTheme(toolAction->iconName()).paint(&p, 0, 0, iconSize.height(), iconSize.width());

    return QIcon(QPixmap::fromImage(pm));
}

void KoModeBox::addItem(KoToolAction *toolAction)
{
    /*
    QWidget *oldwidget = d->addedWidgets[toolAction->buttonGroupId()];
    QWidget *widget;

    // We need to create a new widget in all cases as QToolBox seems to crash if we reuse
    // a widget (even though the item had been removed)
    QLayout *layout;
    if (!oldwidget) {
        layout = new QGridLayout();
    } else {
        layout = oldwidget->layout();
    }
    widget = new QWidget();
    widget->setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    d->addedWidgets[toolAction->buttonGroupId()] = widget;

    // Create a rotated icon with text
    if (d->iconMode == IconAndText) {
        if (d->horizontalMode) {
            d->tabBar->addTab(QIcon::fromTheme(toolAction->iconName()), toolAction->iconText());
        } else {
            d->tabBar->addTab(createTextIcon(toolAction), QString());
        }
    } else {
        int index = d->tabBar->addTab(createSimpleIcon(toolAction), QString());
        d->tabBar->setTabToolTip(index, toolAction->toolTip());
    }
    d->tabBar->blockSignals(false);
    ScrollArea *sa = new ScrollArea();
    if (d->horizontalMode) {
        sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        sa->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        sa->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    sa->setWidgetResizable(true);
    sa->setContentsMargins(0,0,0,0);
    sa->setWidget(widget);
    sa->setFrameShape(QFrame::NoFrame);
    sa->setFocusPolicy(Qt::NoFocus);
    d->stack->addWidget(sa);
    d->addedToolActions.append(toolAction);
    */
}

void KoModeBox::updateShownTools(const QList<QString> &codes)
{
    /*
    if (d->iconTextFitted) {
        d->fittingIterations = 0;
    }
    d->iconTextFitted = true;

    d->tabBar->blockSignals(true);

    while (d->tabBar->count()) {
        d->tabBar->removeTab(0);
        d->stack->removeWidget(d->stack->widget(0));
    }

    d->addedToolActions.clear();

    int newIndex = -1;
    foreach (KoToolAction *toolAction, d->toolActions) {
        const QString toolCodes = toolAction->visibilityCode();
        if (toolAction->buttonGroupId() == d->activeId) {
            newIndex = d->addedToolActions.length();
        }
        if (toolAction->section().contains(applicationName)) {
            addItem(toolAction);
            continue;
        } else if (!toolAction->section().contains("dynamic")
            && !toolAction->section().contains("main")) {
            continue;
        }
        if (toolCodes.startsWith(QLatin1String("flake/"))) {
            addItem(toolAction);
            continue;
        }

        if (toolCodes.endsWith( QLatin1String( "/always"))) {
            addItem(toolAction);
            continue;
        } else if (toolCodes.isEmpty() && codes.count() != 0) {
            addItem(toolAction);
            continue;
        } else {
           foreach (const QString &shapeCode, codes) {
                if (toolCodes.contains(shapeCode)) {
                    addItem(toolAction);
                    break;
                }
           }
        }
    }
    if (newIndex != -1) {
        d->tabBar->setCurrentIndex(newIndex);
        d->stack->setCurrentIndex(newIndex);
    }
    d->tabBar->blockSignals(false);

    if (!d->iconTextFitted &&  d->fittingIterations++ < 8) {
        updateShownTools(codes);
    }
    d->iconTextFitted = true;
    */
}

void KoModeBox::setOptionWidgets(const QList<QPointer<QWidget> > &optionWidgetList)
{
    /*
    if (! d->addedWidgets.contains(d->activeId)) return;

    // For some reason we need to set some attr on our placeholder widget here
    // eventhough these settings should be default
    // Otherwise Sheets' celltool's optionwidget looks ugly
    d->addedWidgets[d->activeId]->setAutoFillBackground(false);
    d->addedWidgets[d->activeId]->setBackgroundRole(QPalette::NoRole);

    qDeleteAll(d->currentAuxWidgets);
    d->currentAuxWidgets.clear();

    int cnt = 0;
    QGridLayout *layout = (QGridLayout *)d->addedWidgets[d->activeId]->layout();
    // need to unstretch row/column that have previously been stretched
    layout->setRowStretch(layout->rowCount()-1, 0);
    layout->setRowStretch(layout->columnCount()-1, 0);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 0);
    layout->setRowStretch(0, 0);
    layout->setRowStretch(1, 0);
    layout->setRowStretch(2, 0);

    if (d->horizontalMode) {
        layout->setRowStretch(0, 1);
        layout->setRowStretch(1, 2);
        layout->setRowStretch(2, 1);
        layout->setHorizontalSpacing(2);
        layout->setVerticalSpacing(0);
        foreach(QWidget *widget, optionWidgetList) {
            if (!widget->windowTitle().isEmpty()) {
                QLabel *l;
                layout->addWidget(l = new QLabel(widget->windowTitle()), 0, cnt, 1, 1, Qt::AlignLeft);
                d->currentAuxWidgets.insert(l);
            }
            layout->addWidget(widget, 1, cnt++, 2, 1);
            widget->show();
            if (widget != optionWidgetList.last()) {
                QFrame *s;
                layout->addWidget(s = new QFrame(), 1, cnt, 1, 1, Qt::AlignHCenter);
                layout->setColumnMinimumWidth(cnt++, 16);
                s->setFrameStyle(QFrame::VLine | QFrame::Sunken);
                d->currentAuxWidgets.insert(s);
                ++cnt;
            }
            layout->setColumnStretch(cnt, 100);
        }
    } else {
        layout->setColumnStretch(0, 1);
        layout->setColumnStretch(1, 2);
        layout->setColumnStretch(2, 1);
        layout->setHorizontalSpacing(0);
        layout->setVerticalSpacing(2);
        int specialCount = 0;
        foreach(QWidget *widget, optionWidgetList) {
            if (!widget->windowTitle().isEmpty()) {
                QLabel *l;
                layout->addWidget(l = new QLabel(widget->windowTitle()), cnt++, 0, 1, 3, Qt::AlignHCenter);
                d->currentAuxWidgets.insert(l);
            }
            layout->addWidget(widget, cnt++, 0, 1, 3);
            QLayout *subLayout = widget->layout();
            if (subLayout) {
                for (int i = 0; i < subLayout->count(); ++i) {
                    QWidget *spacerWidget = subLayout->itemAt(i)->widget();
                    if (spacerWidget && spacerWidget->objectName().contains("SpecialSpacer")) {
                        specialCount++;
                    }
                }
            }
            widget->show();
            if (widget != optionWidgetList.last()) {
                QFrame *s;
                layout->addWidget(s = new QFrame(), cnt, 1, 1, 1);
                layout->setRowMinimumHeight(cnt++, 16);
                s->setFrameStyle(QFrame::HLine | QFrame::Sunken);
                d->currentAuxWidgets.insert(s);
            }
        }
        if (specialCount == optionWidgetList.count()) {
            layout->setRowStretch(cnt, 100);
        }
    }
*/
}

void ScrollArea::showEvent(QShowEvent *e)
{
    /*
    QScrollArea::showEvent(e);
    if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {
        setMinimumWidth(widget()->minimumSizeHint().width() + (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0));
    } else {
        setMinimumHeight(widget()->minimumSizeHint().height() + (horizontalScrollBar()->isVisible() ? horizontalScrollBar()->height() : 0));
    }*/
}

void KoModeBox::setCurrentLayer(const KoCanvasController *canvas, const KoShapeLayer *layer)
{
    Q_UNUSED(canvas);
    Q_UNUSED(layer);
    //Since targeted application don't use this we won't bother implementing
}

void KoModeBox::setCanvas(KoCanvasBase *canvas)
{
    KoCanvasControllerWidget *ccwidget;

    if (d->canvas) {
        ccwidget = dynamic_cast<KoCanvasControllerWidget *>(d->canvas->canvasController());
        disconnect(ccwidget, &KoCanvasControllerWidget::toolOptionWidgetsChanged,
                    this, &KoModeBox::setOptionWidgets);
    }

    d->canvas = canvas;

    ccwidget = dynamic_cast<KoCanvasControllerWidget *>(d->canvas->canvasController());
    connect(
        ccwidget, &KoCanvasControllerWidget::toolOptionWidgetsChanged,
         this, &KoModeBox::setOptionWidgets);
}

void KoModeBox::unsetCanvas()
{
    d->canvas = 0;
}

void KoModeBox::toolAdded(KoToolAction *toolAction, KoCanvasController *canvas)
{
    if (canvas->canvas() == d->canvas) {
        addToolAction(toolAction);

        std::stable_sort(d->toolActions.begin(), d->toolActions.end(), compareToolActions);

        updateShownTools(QList<QString>());
    }
}

void KoModeBox::toolSelected(int index)
{
    if (index != -1) {
        d->addedToolActions.at(index)->trigger();
    }
}

void KoModeBox::slotContextMenuRequested(const QPoint &pos)
{
    /*
    QMenu menu;
    KSelectAction* textAction = new KSelectAction(i18n("Text"), &menu);
    connect(textAction, SIGNAL(triggered(int)), SLOT(switchIconMode(int)));
    menu.addAction(textAction);
    textAction->addAction(i18n("Icon and Text"));
    textAction->addAction(i18n("Icon only"));
    textAction->setCurrentItem(d->iconMode);

    KSelectAction* buttonPositionAction = new KSelectAction(i18n("Tabs side"), &menu);
    connect(buttonPositionAction, SIGNAL(triggered(int)), SLOT(switchTabsSide(int)));
    menu.addAction(buttonPositionAction);
    if (d->horizontalMode) {
        buttonPositionAction->addAction(i18n("Top side"));
        buttonPositionAction->addAction(i18n("Bottom side"));
        buttonPositionAction->setCurrentItem(d->verticalTabsSide);
    } else {
        buttonPositionAction->addAction(i18n("Left side"));
        buttonPositionAction->addAction(i18n("Right side"));
        buttonPositionAction->setCurrentItem(d->horizontalTabsSide);
    }

    menu.exec(d->tabBar->mapToGlobal(pos));
    */
}

void KoModeBox::switchIconMode(int mode)
{
    /*
    d->iconMode = static_cast<IconMode>(mode);
    setIconSize();
    updateShownTools(QList<QString>());

    KConfigGroup cfg =  KSharedConfig::openConfig()->group("calligra");
    cfg.writeEntry("ModeBoxIconMode", (int)d->iconMode);
    */
}

void KoModeBox::switchTabsSide(int side)
{
    /*
    if (d->horizontalMode) {
        d->verticalTabsSide = static_cast<VerticalTabsSide>(side);
        if (d->verticalTabsSide == TopSide) {
            d->layout->removeWidget(d->tabBar);
            d->tabBar->setShape(QTabBar::RoundedNorth);
            d->layout->addWidget(d->tabBar, 0, 0);
        } else {
            d->layout->removeWidget(d->tabBar);
            d->tabBar->setShape(QTabBar::RoundedSouth);
            d->layout->addWidget(d->tabBar, 2, 0);
        }

        KConfigGroup cfg =  KSharedConfig::openConfig()->group("calligra");
        cfg.writeEntry("ModeBoxVerticalTabsSide", (int)d->verticalTabsSide);
    } else {
        d->horizontalTabsSide = static_cast<HorizontalTabsSide>(side);
        if (d->horizontalTabsSide == LeftSide) {
            d->layout->removeWidget(d->tabBar);
            d->tabBar->setShape(QTabBar::RoundedWest);
            d->layout->addWidget(d->tabBar, 0, 0);
        } else {
            d->layout->removeWidget(d->tabBar);
            d->tabBar->setShape(QTabBar::RoundedEast);
            d->layout->addWidget(d->tabBar, 0, 2);
        }

        KConfigGroup cfg =  KSharedConfig::openConfig()->group("calligra");
        cfg.writeEntry("ModeBoxHorizontalTabsSide", (int)d->horizontalTabsSide);
    }
    updateShownTools(QList<QString>());
    */
}

void KoModeBox::setIconSize() const {
    /*
    if (!d->horizontalMode && d->iconMode == IconAndText) {
        d->tabBar->setIconSize(QSize(32,64));
    } else {
        d->tabBar->setIconSize(QSize(22,22));
    }
    */
}

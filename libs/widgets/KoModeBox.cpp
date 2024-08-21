/*
 * SPDX-FileCopyrightText: 2005-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <algorithm>

#include "KoModeBox_p.h"
#include "KoTabBarProxyStyle.h"

#include <KoCanvasBase.h>
#include <KoCanvasControllerWidget.h>
#include <KoShapeLayer.h>
#include <KoShapePropertyWidget.h>
#include <KoToolManager.h>
#include <WidgetsDebug.h>

#include <KColorUtils>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSelectAction>
#include <KSharedConfig>
#include <KStatefulBrush>
#include <KTitleWidget>

#include <QApplication>
#include <QFontDatabase>
#include <QFrame>
#include <QGridLayout>
#include <QHash>
#include <QIcon>
#include <QLabel>
#include <QList>
#include <QMap>
#include <QMenu>
#include <QPainter>
#include <QProxyStyle>
#include <QRect>
#include <QScrollBar>
#include <QSet>
#include <QStackedWidget>
#include <QStyle>
#include <QStyleOptionTabWidgetFrame>
#include <QTabBar>
#include <QTextLayout>

class KoModeBox::Private
{
public:
    Private(KoCanvasController *c)
        : canvas(c->canvas())
        , activeId(-1)
        , iconTextFitted(true)
        , fittingIterations(0)
        , iconMode(IconAndText)
        , horizontalTabsSide(LeftSide)
    {
    }

    KoCanvasBase *canvas;
    QHBoxLayout *layout;
    QList<KoToolAction *> toolActions; // toolActions maintained by toolmanager
    QList<KoToolAction *> addedToolActions; // tools in the order added to QToolBox
    QMap<int, QWidget *> addedWidgets;
    QSet<QWidget *> currentAuxWidgets;
    int activeId;
    QTabBar *tabBar;
    QStackedWidget *stack;
    bool iconTextFitted;
    int fittingIterations;
    IconMode iconMode;
    HorizontalTabsSide horizontalTabsSide;
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
    : QWidget()
    , d(std::make_unique<Private>(canvas))
{
    applicationName = appName;

    KConfigGroup cfg = KSharedConfig::openConfig()->group("calligra");
    d->iconMode = (IconMode)cfg.readEntry("ModeBoxIconMode", (int)IconOnly);
    d->horizontalTabsSide = (HorizontalTabsSide)cfg.readEntry("ModeBoxHorizontalTabsSide", (int)LeftSide);

    setContentsMargins({});

    d->layout = new QHBoxLayout(this);
    d->layout->setObjectName("KoModeBoxLayout");
    d->layout->setContentsMargins({});
    d->layout->setSpacing({});
    d->stack = new QStackedWidget;

    d->tabBar = new QTabBar;
    d->tabBar->setDocumentMode(true);
    d->tabBar->setShape(QTabBar::RoundedWest);
    auto style = new KoTabBarProxyStyle;
    d->tabBar->setStyle(style);
    d->tabBar->setAutoFillBackground(true);
    QPalette palette = d->tabBar->palette();
    palette.setColor(QPalette::Window, palette.color(QPalette::Base));
    d->tabBar->setPalette(palette);
    d->layout->addWidget(d->tabBar);

    auto separator = new QFrame;
    separator->setFrameStyle(QFrame::VLine);
    separator->setFixedWidth(1);
    d->layout->addWidget(separator);

    setIconSize();
    d->tabBar->setExpanding(false);
    d->tabBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    d->layout->addWidget(d->stack);

    const auto toolActions = KoToolManager::instance()->toolActionList();
    for (KoToolAction *toolAction : toolActions) {
        addToolAction(toolAction);
    }

    std::sort(d->toolActions.begin(), d->toolActions.end(), compareToolActions);

    // Update visibility of toolActions
    updateShownTools(QList<QString>());

    d->tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(d->tabBar, &QTabBar::currentChanged, this, &KoModeBox::toolSelected);
    connect(d->tabBar, &QWidget::customContextMenuRequested, this, &KoModeBox::slotContextMenuRequested);

    connect(KoToolManager::instance(), &KoToolManager::changedTool, this, &KoModeBox::setActiveTool);
    connect(KoToolManager::instance(), &KoToolManager::currentLayerChanged, this, &KoModeBox::setCurrentLayer);
    connect(KoToolManager::instance(), &KoToolManager::toolCodesSelected, this, &KoModeBox::updateShownTools);
    connect(KoToolManager::instance(), &KoToolManager::addedTool, this, &KoModeBox::toolAdded);

    connect(canvas, &KoCanvasControllerWidget::toolOptionWidgetsChanged, this, &KoModeBox::setOptionWidgets);
}

KoModeBox::~KoModeBox() = default;

void KoModeBox::addToolAction(KoToolAction *toolAction)
{
    d->toolActions.append(toolAction);
}

void KoModeBox::setActiveTool(KoCanvasController *canvas, int id)
{
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
    }
}

QIcon KoModeBox::createTextIcon(KoToolAction *toolAction) const
{
    QSize iconSize = d->tabBar->iconSize();
    QFont smallFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    qreal pointSize = QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont).pointSizeF();
    smallFont.setPointSizeF(pointSize);
    // This must be a QImage, as drawing to a QPixmap outside the
    // UI thread will cause sporadic crashes.
    QImage pm(iconSize, QImage::Format_ARGB32_Premultiplied);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.rotate(90);
    p.translate(0, -iconSize.width());

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
    if (d->horizontalTabsSide == LeftSide) {
        p.rotate(90);
        p.translate(0, -iconSize.width());
    } else {
        p.rotate(-90);
        p.translate(-iconSize.height(), 0);
    }

    QIcon::fromTheme(toolAction->iconName()).paint(&p, 0, 0, iconSize.height(), iconSize.width());

    return QIcon(QPixmap::fromImage(pm));
}

void KoModeBox::addItem(KoToolAction *toolAction)
{
    QWidget *oldwidget = d->addedWidgets[toolAction->buttonGroupId()];
    QWidget *widget;

    // We need to create a new widget in all cases as QToolBox seems to crash if we reuse
    // a widget (even though the item had been removed)
    QLayout *layout;
    if (!oldwidget) {
        layout = new QVBoxLayout;
    } else {
        layout = oldwidget->layout();
    }
    widget = new QWidget();
    widget->setLayout(layout);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    d->addedWidgets[toolAction->buttonGroupId()] = widget;

    // Create a rotated icon with text
    if (d->iconMode == IconAndText) {
        const int index = d->tabBar->addTab(createTextIcon(toolAction), QString());
        d->tabBar->setTabToolTip(index, toolAction->toolTip());
    } else {
        const int index = d->tabBar->addTab(createSimpleIcon(toolAction), QString());
        d->tabBar->setTabToolTip(index, toolAction->toolTip());
    }
    d->tabBar->blockSignals(false);
    ScrollArea *sa = new ScrollArea();
    sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sa->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sa->setWidgetResizable(true);
    sa->setContentsMargins(0, 0, 0, 0);
    sa->setWidget(widget);
    sa->setFrameShape(QFrame::NoFrame);
    sa->setFocusPolicy(Qt::NoFocus);
    d->stack->addWidget(sa);
    d->addedToolActions.append(toolAction);
}

void KoModeBox::updateShownTools(const QList<QString> &codes)
{
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
        } else if (!toolAction->section().contains("dynamic") && !toolAction->section().contains("main")) {
            continue;
        }
        if (toolCodes.startsWith(QLatin1String("flake/"))) {
            addItem(toolAction);
            continue;
        }

        if (toolCodes.endsWith(QLatin1String("/always"))) {
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

    if (!d->iconTextFitted && d->fittingIterations++ < 8) {
        updateShownTools(codes);
    }
    d->iconTextFitted = true;
}

void KoModeBox::setOptionWidgets(const QList<QPointer<QWidget>> &optionWidgetList)
{
    if (!d->addedWidgets.contains(d->activeId))
        return;

    // For some reason we need to set some attr on our placeholder widget here
    // even though these settings should be default
    // Otherwise Sheets' celltool's optionwidget looks ugly
    d->addedWidgets[d->activeId]->setAutoFillBackground(false);
    d->addedWidgets[d->activeId]->setBackgroundRole(QPalette::NoRole);

    qDeleteAll(d->currentAuxWidgets);
    d->currentAuxWidgets.clear();

    auto layout = qobject_cast<QVBoxLayout *>(d->addedWidgets[d->activeId]->layout());
    Q_ASSERT(layout);
    layout->setSpacing(2);
    QList<QLayoutItem *> spacerItems;
    for (int i = 0, count = layout->count(); i < count; i++) {
        const auto item = layout->itemAt(i);
        const auto spacer = item->spacerItem();
        if (spacer) {
            spacerItems << spacer;
        }
    }
    for (const auto spacer : std::as_const(spacerItems)) {
        layout->removeItem(spacer);
        delete spacer;
    }

    auto title = new KTitleWidget;
    title->setText(d->tabBar->tabToolTip(d->tabBar->currentIndex()));
    layout->addWidget(title);
    d->currentAuxWidgets.insert(title);

    for (QWidget *widget : optionWidgetList) {
        if (!widget->windowTitle().isEmpty()) {
            if (widget != optionWidgetList.constFirst()) {
                layout->addSpacing(style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));
            }
            auto sectionTitle = new QLabel(widget->windowTitle());
            QPalette palette = sectionTitle->palette();
            auto foregroundColor = palette.color(QPalette::WindowText);
            foregroundColor.setAlphaF(0.8);
            palette.setColor(QPalette::WindowText, foregroundColor);
            sectionTitle->setPalette(palette);
            layout->addWidget(sectionTitle);
            d->currentAuxWidgets.insert(sectionTitle);

            auto separator = new QFrame;
            separator->setFrameStyle(QFrame::HLine);
            separator->setFixedHeight(1);
            separator->setPalette(palette);
            layout->addWidget(separator);
            d->currentAuxWidgets.insert(separator);

            layout->addSpacing(style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing) / 2);
        }
        layout->addWidget(widget);
        widget->show();
    }

    layout->addStretch();
}

void ScrollArea::showEvent(QShowEvent *e)
{
    QScrollArea::showEvent(e);
    if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {
        setMinimumWidth(widget()->minimumSizeHint().width() + (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0));
    } else {
        setMinimumHeight(widget()->minimumSizeHint().height() + (horizontalScrollBar()->isVisible() ? horizontalScrollBar()->height() : 0));
    }
}

void KoModeBox::setCurrentLayer(const KoCanvasController *canvas, const KoShapeLayer *layer)
{
    Q_UNUSED(canvas);
    Q_UNUSED(layer);
    // Since targeted application don't use this we won't bother implementing
}

void KoModeBox::setCanvas(KoCanvasBase *canvas)
{
    KoCanvasControllerWidget *ccwidget;

    if (d->canvas) {
        ccwidget = dynamic_cast<KoCanvasControllerWidget *>(d->canvas->canvasController());
        disconnect(ccwidget, &KoCanvasControllerWidget::toolOptionWidgetsChanged, this, &KoModeBox::setOptionWidgets);
    }

    d->canvas = canvas;

    ccwidget = dynamic_cast<KoCanvasControllerWidget *>(d->canvas->canvasController());
    connect(ccwidget, &KoCanvasControllerWidget::toolOptionWidgetsChanged, this, &KoModeBox::setOptionWidgets);
}

void KoModeBox::unsetCanvas()
{
    d->canvas = nullptr;
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
    QMenu menu;
    KSelectAction *textAction = new KSelectAction(i18n("Text"), &menu);
    connect(textAction, &KSelectAction::indexTriggered, this, &KoModeBox::switchIconMode);
    menu.addAction(textAction);
    textAction->addAction(i18n("Icon and Text"));
    textAction->addAction(i18n("Icon only"));
    textAction->setCurrentItem(d->iconMode);

    KSelectAction *buttonPositionAction = new KSelectAction(i18n("Sidebar side"), &menu);
    connect(buttonPositionAction, &KSelectAction::indexTriggered, this, [this](int side) {
        d->horizontalTabsSide = static_cast<HorizontalTabsSide>(side);
        switchTabsSide(d->horizontalTabsSide);
        KConfigGroup cfg = KSharedConfig::openConfig()->group("calligra");
        cfg.writeEntry("ModeBoxHorizontalTabsSide", (int)d->horizontalTabsSide);
        updateShownTools({});
    });
    menu.addAction(buttonPositionAction);
    buttonPositionAction->addAction(i18n("Left side"));
    buttonPositionAction->addAction(i18n("Right side"));
    buttonPositionAction->setCurrentItem(d->horizontalTabsSide);

    menu.exec(d->tabBar->mapToGlobal(pos));
}

void KoModeBox::switchIconMode(int mode)
{
    d->iconMode = static_cast<IconMode>(mode);
    setIconSize();
    updateShownTools(QList<QString>());

    KConfigGroup cfg = KSharedConfig::openConfig()->group("calligra");
    cfg.writeEntry("ModeBoxIconMode", (int)d->iconMode);
}

void KoModeBox::setIconSize() const
{
    if (d->iconMode == IconAndText) {
        d->tabBar->setIconSize(QSize(32, 64));
    } else {
        d->tabBar->setIconSize(QSize(32, 32));
    }
}

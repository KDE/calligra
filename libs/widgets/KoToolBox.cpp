/*
 * SPDX-FileCopyrightText: 2005-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoToolBoxButton_p.h"
#include "KoToolBoxLayout_p.h"
#include "KoToolBox_p.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QButtonGroup>
#include <QHash>
#include <QMenu>
#include <QPainter>
#include <QScreen>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QTimer>
#include <QToolButton>

#include <KActionCollection>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <KoCanvasController.h>
#include <KoShapeLayer.h>
#include <WidgetsDebug.h>

#include <KoCanvasController.h>
#include <KoShapeLayer.h>

#define BUTTON_MARGIN 10

static int buttonSize()
{
    return 22;
}

class KoToolBox::Private
{
public:
    void addSection(Section *section, const QString &name);

    QList<QToolButton *> buttons;
    KoToolBoxButton *selectedButton{nullptr};
    QHash<QString, KoToolBoxButton *> buttonsByToolId;
    QMap<QString, Section *> sections;
    KoToolBoxLayout *layout{nullptr};
    QButtonGroup *buttonGroup{nullptr};
    QHash<QToolButton *, QString> visibilityCodes;
    bool floating{false};
    int iconSize{0};
    QMap<QAction *, int> contextIconSizes;
    QAction *defaultIconSizeAction{nullptr};
    Qt::Orientation orientation{Qt::Vertical};
};

void KoToolBox::Private::addSection(Section *section, const QString &name)
{
    section->setName(name);
    layout->addSection(section);
    sections.insert(name, section);
}

KoToolBox::KoToolBox()
    : d(new Private)
{
    d->layout = new KoToolBoxLayout(this);
    // add defaults
    d->addSection(new Section(this), "main");
    d->addSection(new Section(this), "dynamic");

    d->buttonGroup = new QButtonGroup(this);

    // Get screen the widget exists in, but fall back to primary screen if invalid.
    const int primaryScreen = 0; // In QT, primary screen should always be the first index of QGuiApplication::screens()
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
    d->iconSize = cfg.readEntry("iconSize", buttonSize());

    const auto tools = KoToolManager::instance()->toolActionList();
    for (KoToolAction *toolAction : tools) {
        addButton(toolAction);
    }

    applyIconSize();

    // Update visibility of buttons
    setButtonsVisible(QList<QString>());

    connect(KoToolManager::instance(), &KoToolManager::changedTool, this, &KoToolBox::setActiveTool);
    connect(KoToolManager::instance(), &KoToolManager::currentLayerChanged, this, &KoToolBox::setCurrentLayer);
    connect(KoToolManager::instance(), &KoToolManager::toolCodesSelected, this, &KoToolBox::setButtonsVisible);
    connect(KoToolManager::instance(), &KoToolManager::addedTool, this, &KoToolBox::toolAdded);
}

KoToolBox::~KoToolBox() = default;

void KoToolBox::applyIconSize()
{
    for (QToolButton *button : std::as_const(d->buttons)) {
        button->setIconSize(QSize(d->iconSize, d->iconSize));
    }

    const auto sections = d->sections.values();
    for (Section *section : sections) {
        section->setButtonSize(QSize(d->iconSize + BUTTON_MARGIN, d->iconSize + BUTTON_MARGIN));
    }
}

void KoToolBox::addButton(KoToolAction *toolAction)
{
    KoToolBoxButton *button = new KoToolBoxButton(toolAction, this);

    d->buttons << button;

    QString sectionToBeAddedTo;
    const QString section = toolAction->section();
    if (section.contains(qApp->applicationName())) {
        sectionToBeAddedTo = "main";
    } else if (section.contains("main")) {
        sectionToBeAddedTo = "main";
    } else if (section.contains("dynamic")) {
        sectionToBeAddedTo = "dynamic";
    } else {
        sectionToBeAddedTo = section;
    }

    Section *sectionWidget = d->sections.value(sectionToBeAddedTo);
    if (sectionWidget == nullptr) {
        sectionWidget = new Section(this);
        d->addSection(sectionWidget, sectionToBeAddedTo);
    }
    sectionWidget->addButton(button, toolAction->priority());

    d->buttonGroup->addButton(button);
    d->buttonsByToolId.insert(toolAction->id(), button);

    d->visibilityCodes.insert(button, toolAction->visibilityCode());
}

void KoToolBox::setActiveTool(KoCanvasController *canvas)
{
    Q_UNUSED(canvas);

    QString id = KoToolManager::instance()->activeToolId();
    KoToolBoxButton *button = d->buttonsByToolId.value(id);
    if (button) {
        button->setChecked(true);
        button->setHighlightColor();
        if (d->selectedButton) {
            d->selectedButton->setHighlightColor();
        }
        d->selectedButton = button;
    } else {
        warnWidgets << "KoToolBox::setActiveTool(" << id << "): no such button found";
    }
}

void KoToolBox::setButtonsVisible(const QList<QString> &codes)
{
    const auto buttons = d->visibilityCodes.keys();
    for (QToolButton *button : buttons) {
        QString code = d->visibilityCodes.value(button);

        if (code.startsWith(QLatin1String("flake/"))) {
            continue;
        }

        if (code.endsWith(QLatin1String("/always"))) {
            button->setVisible(true);
            button->setEnabled(true);
        } else if (code.isEmpty()) {
            button->setVisible(true);
            button->setEnabled(codes.count() != 0);
        } else {
            button->setVisible(codes.contains(code));
        }
    }
    layout()->invalidate();
    update();
}

void KoToolBox::setCurrentLayer(const KoCanvasController *canvas, const KoShapeLayer *layer)
{
    Q_UNUSED(canvas);
    const bool enabled = layer == nullptr || (layer->isEditable() && layer->isVisible());
    const auto buttons = d->visibilityCodes.keys();
    for (QToolButton *button : buttons) {
        if (d->visibilityCodes[button].endsWith(QLatin1String("/always"))) {
            continue;
        }
        button->setEnabled(enabled);
    }
}

void KoToolBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    const QList<Section *> sections = d->sections.values();
    QList<Section *>::const_iterator iterator = sections.begin();
    int halfSpacing = layout()->spacing();
    if (halfSpacing > 0) {
        halfSpacing /= 2;
    }
    while (iterator != sections.end()) {
        Section *section = *iterator;
        QStyleOption styleoption;
        styleoption.palette = palette();

        if (section->separators() & Section::SeparatorTop) {
            int y = section->y() - halfSpacing;
            styleoption.state = QStyle::State_None;
            styleoption.rect = QRect(section->x(), y - 1, section->width(), 2);

            style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &styleoption, &painter);
        }

        if (section->separators() & Section::SeparatorLeft && section->isLeftToRight()) {
            int x = section->x() - halfSpacing;
            styleoption.state = QStyle::State_Horizontal;
            styleoption.rect = QRect(x - 1, section->y(), 2, section->height());

            style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &styleoption, &painter);
        } else if (section->separators() & Section::SeparatorLeft && section->isRightToLeft()) {
            int x = section->x() + section->width() + halfSpacing;
            styleoption.state = QStyle::State_Horizontal;
            styleoption.rect = QRect(x - 1, section->y(), 2, section->height());

            style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &styleoption, &painter);
        }

        ++iterator;
    }

    painter.end();
}

void KoToolBox::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) {
        for (QToolButton *button : std::as_const(d->buttons)) {
            KoToolBoxButton *toolBoxButton = qobject_cast<KoToolBoxButton *>(button);
            if (toolBoxButton) {
                toolBoxButton->setHighlightColor();
            }
        }
    }
}

void KoToolBox::setOrientation(Qt::Orientation orientation)
{
    d->orientation = orientation;
    d->layout->setOrientation(orientation);
    QTimer::singleShot(0, this, QOverload<>::of(&KoToolBox::update));
    for (Section *section : std::as_const(d->sections)) {
        section->setOrientation(orientation);
    }
}

void KoToolBox::setFloating(bool v)
{
    d->floating = v;
}

void KoToolBox::toolAdded(KoToolAction *toolAction, KoCanvasController *canvas)
{
    Q_UNUSED(canvas);
    addButton(toolAction);
    setButtonsVisible(QList<QString>());
}

void KoToolBox::slotContextIconSize()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int iconSize = -1;
        if (action == d->defaultIconSizeAction) {
            iconSize = buttonSize();
            QAction *action = d->contextIconSizes.key(iconSize);
            if (action) {
                action->setChecked(true);
            }
        } else if (d->contextIconSizes.contains(action)) {
            iconSize = d->contextIconSizes.value(action);
        }
        if (iconSize < 0) {
            Q_ASSERT(false);
            iconSize = 16;
        }

        KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
        cfg.writeEntry("iconSize", iconSize);
        d->iconSize = iconSize;

        applyIconSize();
    }
}

void KoToolBox::setupIconSizeMenu(QMenu *menu)
{
    if (d->contextIconSizes.isEmpty()) {
        d->defaultIconSizeAction = menu->addAction(i18nc("@item:inmenu Icon size", "Default"), this, &KoToolBox::slotContextIconSize);

        QActionGroup *sizeGroup = new QActionGroup(menu);
        QList<int> sizes{12, 14, 16, 22, 32, 48, 64}; //<< 96, 128, 192, 256;
        for (int i : sizes) {
            QAction *action = menu->addAction(i18n("%1x%2", i, i), this, SLOT(slotContextIconSize()));
            d->contextIconSizes.insert(action, i);
            action->setActionGroup(sizeGroup);
            action->setCheckable(true);
            if (d->iconSize == i) {
                action->setChecked(true);
            }
        }
    }
}

KoToolBoxLayout *KoToolBox::toolBoxLayout() const
{
    return d->layout;
}

#include "moc_KoToolBoxScrollArea_p.cpp"

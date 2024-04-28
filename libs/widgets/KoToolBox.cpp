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

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <WidgetsDebug.h>

#include <KoCanvasController.h>
#include <KoShapeLayer.h>

#define BUTTON_MARGIN 10

static int buttonSize()
{
    return 16;
}

class KoToolBox::Private
{
public:
    Private()
        : layout(0)
        , buttonGroup(0)
        , floating(false)
        , contextSize(0)
    {
    }

    void addSection(Section *section, const QString &name);

    QList<QToolButton *> buttons;
    QMap<QString, Section *> sections;
    KoToolBoxLayout *layout;
    QButtonGroup *buttonGroup;
    QHash<QToolButton *, QString> visibilityCodes;
    bool floating;
    QMap<QAction *, int> contextIconSizes;
    QMenu *contextSize;
    Qt::Orientation orientation;
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
    setLayout(d->layout);
    foreach (KoToolAction *toolAction, KoToolManager::instance()->toolActionList()) {
        addButton(toolAction);
    }

    // Update visibility of buttons
    setButtonsVisible(QList<QString>());

    connect(KoToolManager::instance(), &KoToolManager::changedTool, this, &KoToolBox::setActiveTool);
    connect(KoToolManager::instance(), &KoToolManager::currentLayerChanged, this, &KoToolBox::setCurrentLayer);
    connect(KoToolManager::instance(), &KoToolManager::toolCodesSelected, this, &KoToolBox::setButtonsVisible);
    connect(KoToolManager::instance(), &KoToolManager::addedTool, this, &KoToolBox::toolAdded);
}

KoToolBox::~KoToolBox()
{
    delete d;
}

void KoToolBox::addButton(KoToolAction *toolAction)
{
    KoToolBoxButton *button = new KoToolBoxButton(toolAction, this);

    d->buttons << button;

    int toolbuttonSize = buttonSize();
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
    int iconSize = cfg.readEntry("iconSize", toolbuttonSize);
    button->setIconSize(QSize(iconSize, iconSize));
    foreach (Section *section, d->sections) {
        section->setButtonSize(QSize(iconSize + BUTTON_MARGIN, iconSize + BUTTON_MARGIN));
    }

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
    if (sectionWidget == 0) {
        sectionWidget = new Section(this);
        d->addSection(sectionWidget, sectionToBeAddedTo);
    }
    sectionWidget->addButton(button, toolAction->priority());

    d->buttonGroup->addButton(button, toolAction->buttonGroupId());

    d->visibilityCodes.insert(button, toolAction->visibilityCode());
}

void KoToolBox::setActiveTool(KoCanvasController *canvas, int id)
{
    Q_UNUSED(canvas);

    QAbstractButton *button = d->buttonGroup->button(id);
    if (button) {
        button->setChecked(true);
        (qobject_cast<KoToolBoxButton *>(button))->setHighlightColor();
    } else {
        warnWidgets << "KoToolBox::setActiveTool(" << id << "): no such button found";
    }
}

void KoToolBox::setButtonsVisible(const QList<QString> &codes)
{
    for (auto i = d->visibilityCodes.constBegin(); i != d->visibilityCodes.constEnd(); ++i) {
        QToolButton *button = i.key();
        QString code = i.value();

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
    for (auto i = d->visibilityCodes.constBegin(); i != d->visibilityCodes.constEnd(); ++i) {
        if (i.value().endsWith(QLatin1String("/always"))) {
            continue;
        }
        i.key()->setEnabled(enabled);
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

        if (section->separators() & Section::SeparatorLeft) {
            int x = section->x() - halfSpacing;
            styleoption.state = QStyle::State_Horizontal;
            styleoption.rect = QRect(x - 1, section->y(), 2, section->height());

            style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &styleoption, &painter);
        }

        ++iterator;
    }

    painter.end();
}

void KoToolBox::setOrientation(Qt::Orientation orientation)
{
    d->orientation = orientation;
    d->layout->setOrientation(orientation);
    QTimer::singleShot(0, this, QOverload<>::of(&KoToolBox::update));
    foreach (Section *section, d->sections) {
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
    if (action && d->contextIconSizes.contains(action)) {
        const int iconSize = d->contextIconSizes.value(action);

        KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
        cfg.writeEntry("iconSize", iconSize);

        foreach (QToolButton *button, d->buttons) {
            button->setIconSize(QSize(iconSize, iconSize));
        }

        foreach (Section *section, d->sections) {
            section->setButtonSize(QSize(iconSize + BUTTON_MARGIN, iconSize + BUTTON_MARGIN));
        }
    }
}

void KoToolBox::contextMenuEvent(QContextMenuEvent *event)
{
    int toolbuttonSize = buttonSize();

    if (!d->contextSize) {
        d->contextSize = new QMenu(i18n("Icon Size"), this);
        d->contextIconSizes.insert(d->contextSize->addAction(i18nc("@item:inmenu Icon size", "Default"), this, &KoToolBox::slotContextIconSize),
                                   toolbuttonSize);

        QList<int> sizes;
        sizes << 12 << 14 << 16 << 22 << 32 << 48 << 64; //<< 96 << 128 << 192 << 256;
        foreach (int i, sizes) {
            d->contextIconSizes.insert(d->contextSize->addAction(i18n("%1x%2", i, i), this, &KoToolBox::slotContextIconSize), i);
        }

        QActionGroup *sizeGroup = new QActionGroup(d->contextSize);
        foreach (QAction *action, d->contextSize->actions()) {
            action->setActionGroup(sizeGroup);
            action->setCheckable(true);
        }
    }
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
    toolbuttonSize = cfg.readEntry("iconSize", toolbuttonSize);

    QMapIterator<QAction *, int> it = d->contextIconSizes;
    while (it.hasNext()) {
        it.next();
        if (it.value() == toolbuttonSize) {
            it.key()->setChecked(true);
            break;
        }
    }

    d->contextSize->exec(event->globalPos());
}
KoToolBoxLayout *KoToolBox::toolBoxLayout() const
{
    return d->layout;
}

#include "moc_KoToolBoxScrollArea_p.cpp"

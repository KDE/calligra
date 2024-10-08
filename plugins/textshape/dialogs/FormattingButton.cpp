/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "FormattingButton.h"

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QResizeEvent>
#include <QWidgetAction>

#include <QDebug>

class ContentWidget : public QFrame
{
    Q_OBJECT
public:
    ContentWidget()
        : QFrame()
    {
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QFrame::resizeEvent(event);
        if (event->oldSize().isValid())
            Q_EMIT readyAfterResize();
    }

Q_SIGNALS:
    void readyAfterResize();
};

// This class is a helper to add a label
class LabelAction : public QWidgetAction
{
public:
    LabelAction(const QString &label);
    QLabel *m_label;
};

LabelAction::LabelAction(const QString &label)
    : QWidgetAction(nullptr)
{
    m_label = new QLabel(label);
    setDefaultWidget(m_label);
}

// This class is the main place where the expanding grid is done
class ItemChooserAction : public QWidgetAction
{
    Q_OBJECT
public:
    ItemChooserAction(int columns);
    QWidget *m_widget;
    QGridLayout *m_containerLayout;
    int m_cnt;
    int m_columns;
    QToolButton *addItem(QPixmap pm);
    QToolButton *removeLastItem();
    void addBlanks(int n);
};

ItemChooserAction::ItemChooserAction(int columns)
    : QWidgetAction(nullptr)
    , m_cnt(0)
    , m_columns(columns)
{
    QFrame *ow = new ContentWidget;
    QGridLayout *l = new QGridLayout();
    l->setSpacing(0);
    l->setContentsMargins({});
    l->setSizeConstraint(QLayout::SetMinAndMaxSize);
    ow->setLayout(l);

    m_widget = new QWidget();
    l->addWidget(m_widget);

    m_containerLayout = new QGridLayout();
    m_containerLayout->setSpacing(4);
    m_containerLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    m_containerLayout->setColumnStretch(columns - 1, 1); // make sure the items are left when less than full row
    m_widget->setLayout(m_containerLayout);

    setDefaultWidget(ow);
}

QToolButton *ItemChooserAction::addItem(QPixmap pm)
{
    QToolButton *b = new QToolButton();
    b->setIcon(QIcon(pm));
    b->setIconSize(pm.size());
    b->setAutoRaise(true);
    b->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    b->setContextMenuPolicy(Qt::ActionsContextMenu);
    b->setPopupMode(static_cast<QToolButton::ToolButtonPopupMode>(10));
    m_containerLayout->addWidget(b, m_cnt / m_columns, m_cnt % m_columns);
    ++m_cnt;
    return b;
}

QToolButton *ItemChooserAction::removeLastItem()
{
    --m_cnt;
    QLayoutItem *li = m_containerLayout->itemAtPosition(m_cnt / m_columns, m_cnt % m_columns);
    QToolButton *b = nullptr;
    if (li) {
        m_containerLayout->removeItem(li);
        b = qobject_cast<QToolButton *>(li->widget());
        delete li;
    }
    return b;
}

void ItemChooserAction::addBlanks(int n)
{
    m_cnt += n;
}

FormattingButton::FormattingButton(QWidget *parent)
    : QToolButton(parent)
    , m_lastId(0)
    , m_menuShownFirstTime(true)
{
    m_menu = new QMenu(this);
    setPopupMode(MenuButtonPopup);
    setMenu(m_menu);
    connect(this, &QAbstractButton::released, this, &FormattingButton::itemSelected);
    connect(m_menu, &QMenu::aboutToHide, this, &FormattingButton::doneWithFocus);
    connect(m_menu, &QMenu::aboutToShow, this, &FormattingButton::aboutToShowMenu);
    connect(m_menu, &QMenu::aboutToHide, this, &FormattingButton::menuShown);
}

void FormattingButton::setItemsBackground(ItemChooserAction *chooser, const QColor &color)
{
    if (chooser) {
        foreach (QObject *o, chooser->defaultWidget()->children()) {
            QWidget *w = qobject_cast<QWidget *>(o);
            if (w) {
                QPalette p = w->palette();
                p.setColor(QPalette::Window, color);
                w->setPalette(p);
                w->setAutoFillBackground(true);
                break;
            }
        }
        qobject_cast<QFrame *>(chooser->defaultWidget())->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    }
}

ItemChooserAction *FormattingButton::addItemChooser(int columns, const QString &title)
{
    m_menu->addSection(title);

    ItemChooserAction *styleAction = new ItemChooserAction(columns);

    m_menu->addAction(styleAction);
    connect(m_menu, &QMenu::aboutToShow, this, &FormattingButton::recalcMenuSize);
    connect(styleAction->defaultWidget(), SIGNAL(readyAfterResize()), this, SLOT(recalcMenuSize()));
    return styleAction;
}

void FormattingButton::addItem(ItemChooserAction *chooser, const QPixmap &pm, int id, const QString &toolTip)
{
    // Note: Do not use 0 as the item id, because that will break the m_lastId functionality
    Q_ASSERT(id != 0);

    if (m_styleMap.contains(id)) {
        QToolButton *button = dynamic_cast<QToolButton *>(m_styleMap.value(id));
        if (button) {
            button->setIcon(QIcon(pm));
            button->setIconSize(pm.size());
        }
    } else {
        QToolButton *b = chooser->addItem(pm);
        b->setToolTip(toolTip);
        m_styleMap.insert(id, b);
        connect(b, &QAbstractButton::released, this, &FormattingButton::itemSelected);
    }
    if (!m_lastId) {
        m_lastId = id;
    }
}

QAction *FormattingButton::addItemMenuItem(ItemChooserAction *chooser, int id, const QString &text)
{
    Q_UNUSED(chooser);
    // Note: Do not use 0 as the item id, because that will break the m_lastId functionality
    Q_ASSERT(id != 0);

    if (m_styleMap.contains(id)) {
        QToolButton *button = dynamic_cast<QToolButton *>(m_styleMap.value(id));
        if (button) {
            QAction *a = new QAction(text, nullptr);
            button->addAction(a);
            return a;
        }
    }
    return nullptr;
}

void FormattingButton::addBlanks(ItemChooserAction *chooser, int n)
{
    chooser->addBlanks(n);
}

void FormattingButton::removeLastItem(ItemChooserAction *chooser)
{
    QToolButton *b = chooser->removeLastItem();

    int id = m_styleMap.key(b);
    m_styleMap.remove(id);
    b->deleteLater();
    if (m_lastId == id) {
        Q_ASSERT(false); // oops
    }
}

void FormattingButton::addAction(QAction *action)
{
    m_menu->addAction(action);
}

void FormattingButton::addSeparator()
{
    m_menu->addSeparator();
}

void FormattingButton::itemSelected()
{
    if (sender() != this && m_styleMap.key(sender()) == 0) {
        // this means that the sender() is not in the m_styleMap. Have you missed something?
        return;
    }

    if (sender() == this && m_lastId == 0) {
        // menu not yet populated
        return;
    }

    if (sender() != this) {
        m_lastId = m_styleMap.key(sender());
    }
    m_menu->hide();
    Q_EMIT itemTriggered(m_lastId);
}

bool FormattingButton::hasItemId(int id)
{
    return m_styleMap.contains(id);
}

void FormattingButton::menuShown()
{
    m_menuShownFirstTime = false;
}

void FormattingButton::recalcMenuSize()
{
    m_menu->setSeparatorsCollapsible(!m_menu->separatorsCollapsible()); // invalidates menu cache
    m_menu->setSeparatorsCollapsible(!m_menu->separatorsCollapsible()); // of action rects
    m_menu->grab(); // helps recalc size
    m_menu->setMaximumSize(m_menu->sizeHint());
}

bool FormattingButton::isFirstTimeMenuShown()
{
    return m_menuShownFirstTime;
}

#include "FormattingButton.moc"

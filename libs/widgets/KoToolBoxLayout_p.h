/*
 * SPDX-FileCopyrightText: 2005-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _KO_TOOLBOX_LAYOUT_H_
#define _KO_TOOLBOX_LAYOUT_H_

#include <QAbstractButton>
#include <QApplication>
#include <QLayout>
#include <QMap>
#include <QMouseEvent>
#include <QRect>
#include <WidgetsDebug.h>

class SectionLayout : public QLayout
{
    Q_OBJECT
public:
    explicit SectionLayout(QWidget *parent)
        : QLayout(parent)
        , m_orientation(Qt::Vertical)
    {
    }

    ~SectionLayout() override
    {
        qDeleteAll(m_items);
        m_items.clear();
    }

    void addButton(QAbstractButton *button, int priority)
    {
        addChildWidget(button);
        if (m_priorities.values().contains(priority)) {
            qWarning() << "Button" << button << "has a conflicting priority";
        }

        m_priorities.insert(button, priority);
        int index = 1;
        for (QWidgetItem *item : std::as_const(m_items)) {
            if (m_priorities.value(static_cast<QAbstractButton *>(item->widget())) > priority)
                break;
            index++;
        }
        m_items.insert(index - 1, new QWidgetItem(button));
    }

    QSize sizeHint() const override
    {
        // This is implemented just to not freak out GammaRay, in practice
        // this doesn't have any effect on the layout.
        if (m_orientation == Qt::Vertical) {
            return QSize(m_buttonSize.width(), m_buttonSize.height() * count());
        } else {
            return QSize(m_buttonSize.width() * count(), m_buttonSize.height());
        }
    }

    void addItem(QLayoutItem *) override
    {
        Q_ASSERT(0);
    }

    QLayoutItem *itemAt(int i) const override
    {
        if (m_items.count() <= i)
            return nullptr;
        return m_items.at(i);
    }

    QLayoutItem *takeAt(int i) override
    {
        return m_items.takeAt(i);
    }

    int count() const override
    {
        return m_items.count();
    }

    void setGeometry(const QRect &rect) override
    {
        int x = 0;
        int y = 0;
        const QSize &size = buttonSize();
        if (m_orientation == Qt::Vertical) {
            for (QWidgetItem *w : std::as_const(m_items)) {
                if (w->isEmpty())
                    continue;
                int realX;
                if (parentWidget()->isLeftToRight()) {
                    realX = x;
                } else {
                    realX = rect.width() - x - size.width();
                }
                w->widget()->setGeometry(QRect(realX, y, size.width(), size.height()));
                x += size.width();
                if (x + size.width() > rect.width()) {
                    x = 0;
                    y += size.height();
                }
            }
        } else {
            for (QWidgetItem *w : std::as_const(m_items)) {
                if (w->isEmpty())
                    continue;
                int realX;
                if (parentWidget()->isLeftToRight()) {
                    realX = x;
                } else {
                    realX = rect.width() - x - size.width();
                }
                w->widget()->setGeometry(QRect(realX, y, size.width(), size.height()));
                y += size.height();
                if (y + size.height() > rect.height()) {
                    x += size.width();
                    y = 0;
                }
            }
        }
    }

    void setButtonSize(const QSize size)
    {
        m_buttonSize = size;
    }

    const QSize &buttonSize() const
    {
        return m_buttonSize;
    }

    void setOrientation(Qt::Orientation orientation)
    {
        m_orientation = orientation;
    }

private:
    QSize m_buttonSize;
    QMap<QAbstractButton *, int> m_priorities;
    QList<QWidgetItem *> m_items;
    Qt::Orientation m_orientation;
};

class Section : public QWidget
{
    Q_OBJECT
public:
    enum SeparatorFlag { SeparatorTop = 0x0001, /* SeparatorBottom = 0x0002, SeparatorRight = 0x0004,*/ SeparatorLeft = 0x0008 };
    Q_DECLARE_FLAGS(Separators, SeparatorFlag)
    explicit Section(QWidget *parent = nullptr)
        : QWidget(parent)
        , m_layout(new SectionLayout(this))
    {
        setLayout(m_layout);
    }

    void addButton(QAbstractButton *button, int priority)
    {
        m_layout->addButton(button, priority);
    }

    void setName(const QString &name)
    {
        m_name = name;
    }

    QString name() const
    {
        return m_name;
    }

    void setButtonSize(const QSize &size)
    {
        m_layout->setButtonSize(size);
    }

    QSize iconSize() const
    {
        return m_layout->buttonSize();
    }

    int visibleButtonCount() const
    {
        int count = 0;
        for (int i = m_layout->count() - 1; i >= 0; --i) {
            if (!static_cast<QWidgetItem *>(m_layout->itemAt(i))->isEmpty())
                ++count;
        }
        return count;
    }

    void setSeparator(Separators separators)
    {
        m_separators = separators;
    }

    Separators separators() const
    {
        return m_separators;
    }

    void setOrientation(Qt::Orientation orientation)
    {
        m_layout->setOrientation(orientation);
    }

private:
    SectionLayout *m_layout;
    QString m_name;
    Separators m_separators;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Section::Separators)

class KoToolBoxLayout : public QLayout
{
    Q_OBJECT
public:
    explicit KoToolBoxLayout(QWidget *parent)
        : QLayout(parent)
        , m_orientation(Qt::Vertical)
    {
        setSpacing(6);
    }

    ~KoToolBoxLayout() override;

    QSize sizeHint() const override
    {
        // Prefer showing one row/column by default
        const QSize minSize = minimumSize();
        if (!minSize.isValid()) {
            return minSize;
        }
        if (m_orientation == Qt::Vertical) {
            return QSize(minSize.width(), minSize.height() + spacing());
        } else {
            return QSize(minSize.height() + spacing(), minSize.width());
        }
    }

    QSize minimumSize() const override
    {
        if (m_sections.isEmpty())
            return QSize();
        QSize oneIcon = static_cast<Section *>(m_sections[0]->widget())->iconSize();
        return oneIcon;
    }

    void addSection(Section *section)
    {
        addChildWidget(section);

        QList<QWidgetItem *>::iterator iterator = m_sections.begin();
        int defaults = 2; // skip the first two as they are the 'main' and 'dynamic' sections.
        while (iterator != m_sections.end()) {
            if (--defaults < 0 && static_cast<Section *>((*iterator)->widget())->name() > section->name())
                break;
            ++iterator;
        }
        m_sections.insert(iterator, new QWidgetItem(section));
    }

    void addItem(QLayoutItem *) override
    {
        Q_ASSERT(0); // don't let anything else be added. (code depends on this!)
    }

    QLayoutItem *itemAt(int i) const override
    {
        return m_sections.value(i);
    }
    QLayoutItem *takeAt(int i) override
    {
        return m_sections.takeAt(i);
    }
    int count() const override
    {
        return m_sections.count();
    }

    void setGeometry(const QRect &rect) override
    {
        QLayout::setGeometry(rect);
        doLayout(rect.size(), true);
    }

    bool hasHeightForWidth() const override
    {
        return m_orientation == Qt::Vertical;
    }

    int heightForWidth(int width) const override
    {
        if (m_orientation == Qt::Vertical) {
            const int height = doLayout(QSize(width, 0), false);
            return height;
        } else {
            return -1;
        }
    }

    /**
     * For calculating the width from height by KoToolBoxScrollArea.
     * QWidget doesn't actually support trading width for height, so it needs to
     * be handled specifically.
     */
    int widthForHeight(int height) const
    {
        if (m_orientation == Qt::Horizontal) {
            const int width = doLayout(QSize(0, height), false);
            return width;
        } else {
            return -1;
        }
    }

    void setOrientation(Qt::Orientation orientation)
    {
        m_orientation = orientation;
        invalidate();
    }

private:
    int doLayout(const QSize &size, bool applyGeometry) const
    {
        // nothing to do?
        if (m_sections.isEmpty()) {
            return 0;
        }

        // the names of the variables assume a vertical orientation,
        // but all calculations are done based on the real orientation
        const bool isVertical = m_orientation == Qt::Vertical;

        const QSize iconSize = static_cast<Section *>(m_sections.first()->widget())->iconSize();

        const int maxWidth = isVertical ? size.width() : size.height();
        // using min 1 as width to e.g. protect against div by 0 below
        const int iconWidth = qMax(1, isVertical ? iconSize.width() : iconSize.height());
        const int iconHeight = qMax(1, isVertical ? iconSize.height() : iconSize.width());

        const int maxColumns = qMax(1, (maxWidth / iconWidth));

        int x = 0;
        int y = 0;
        bool firstSection = true;
        if (!applyGeometry) {
            foreach (QWidgetItem *wi, m_sections) {
                Section *section = static_cast<Section *>(wi->widget());
                const int buttonCount = section->visibleButtonCount();
                if (buttonCount == 0) {
                    continue;
                }

                // rows needed for the buttons (calculation gets the ceiling value of the plain div)
                const int neededRowCount = ((buttonCount - 1) / maxColumns) + 1;

                if (firstSection) {
                    firstSection = false;
                } else {
                    // start on a new row, set separator
                    x = 0;
                    y += iconHeight + spacing();
                }

                // advance by the icons in the last row
                const int lastRowColumnCount = buttonCount - ((neededRowCount - 1) * maxColumns);
                x += (lastRowColumnCount * iconWidth) + spacing();
                // advance by all but the last used row
                y += (neededRowCount - 1) * iconHeight;
            }
        } else {
            foreach (QWidgetItem *wi, m_sections) {
                Section *section = static_cast<Section *>(wi->widget());
                const int buttonCount = section->visibleButtonCount();
                if (buttonCount == 0) {
                    section->hide();
                    continue;
                }

                // rows needed for the buttons (calculation gets the ceiling value of the plain div)
                const int neededRowCount = ((buttonCount - 1) / maxColumns) + 1;

                if (firstSection) {
                    firstSection = false;
                } else {
                    // start on a new row, set separator
                    x = 0;
                    y += iconHeight + spacing();
                    const Section::Separators separator = isVertical ? Section::SeparatorTop : Section::SeparatorLeft;
                    section->setSeparator(separator);
                }

                const int usedColumns = qMin(buttonCount, maxColumns);
                if (isVertical) {
                    section->setGeometry(x, y, usedColumns * iconWidth, neededRowCount * iconHeight);
                } else {
                    section->setGeometry(y, x, neededRowCount * iconHeight, usedColumns * iconWidth);
                }

                // advance by the icons in the last row
                const int lastRowColumnCount = buttonCount - ((neededRowCount - 1) * maxColumns);
                x += (lastRowColumnCount * iconWidth) + spacing();
                // advance by all but the last used row
                y += (neededRowCount - 1) * iconHeight;
            }
        }

        return y + iconHeight;
    }

    QList<QWidgetItem *> m_sections;
    Qt::Orientation m_orientation;
};

#endif

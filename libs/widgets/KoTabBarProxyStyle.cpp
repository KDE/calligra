// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KoTabBarProxyStyle.h"

#include <KColorUtils>

#include <QApplication>
#include <QPainter>
#include <QStyleOptionTab>
#include <qnamespace.h>

KoTabBarProxyStyle::KoTabBarProxyStyle()
    : QProxyStyle()
{
    _viewHoverBrush = KStatefulBrush(KColorScheme::View, KColorScheme::HoverColor);
}

QColor KoTabBarProxyStyle::alphaColor(QColor color, qreal alpha) const
{
    if (alpha >= 0 && alpha < 1.0) {
        color.setAlphaF(alpha * color.alphaF());
    }
    return color;
}

int KoTabBarProxyStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    case PM_TabBarTabShiftVertical:
        return 0;
    case PM_TabBarTabShiftHorizontal:
        return 0;
    case PM_TabBarTabOverlap:
        return 0;
    case PM_TabBarBaseOverlap:
        return 0;
    case PM_TabBarTabHSpace:
        return 2 * TabBar_TabMarginWidth;
    case PM_TabBarTabVSpace:
        return 2 * TabBar_TabMarginHeight;
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

QSize KoTabBarProxyStyle::sizeFromContents(ContentsType element, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    if (element == CT_TabBarTab) {
        const auto tabOption(qstyleoption_cast<const QStyleOptionTab *>(option));
        const bool hasText(tabOption && !tabOption->text.isEmpty());
        const bool hasIcon(tabOption && !tabOption->icon.isNull());
        const bool hasLeftButton(tabOption && !tabOption->leftButtonSize.isEmpty());
        const bool hasRightButton(tabOption && !tabOption->leftButtonSize.isEmpty());

        // calculate width increment for horizontal tabs
        int widthIncrement = 0;
        if (hasIcon && !(hasText || hasLeftButton || hasRightButton)) {
            widthIncrement -= 4;
        }
        if (hasText && hasIcon) {
            widthIncrement += TabBar_TabItemSpacing;
        }
        if (hasLeftButton && (hasText || hasIcon)) {
            widthIncrement += TabBar_TabItemSpacing;
        }
        if (hasRightButton && (hasText || hasIcon || hasLeftButton)) {
            widthIncrement += TabBar_TabItemSpacing;
        }

        // add margins
        QSize contentSize(size);

        // compare to minimum size
        contentSize.rheight() += widthIncrement;
        if (hasIcon && !hasText) {
            contentSize = contentSize.expandedTo(QSize(TabBar_TabMinHeight, 0));
        } else {
            contentSize = contentSize.expandedTo(QSize(TabBar_TabMinHeight, TabBar_TabMinWidth));
        }

        return size;
    }
    return QProxyStyle::sizeFromContents(element, option, size, widget);
}

void KoTabBarProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == PE_FrameTabWidget || element == PE_FrameTabBarBase) {
        // don't draw frame
        return;
    }

    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

qreal KoTabBarProxyStyle::devicePixelRatio(QPainter *painter) const
{
    return painter->device() ? painter->device()->devicePixelRatioF() : qApp->devicePixelRatio();
}

QRectF KoTabBarProxyStyle::strokedRect(const QRectF &rect, const qreal penWidth) const
{
    /* With a pen stroke width of 1, the rectangle should have each of its
     * sides moved inwards by half a pixel. This allows the stroke to be
     * pixel perfect instead of blurry from sitting between pixels and
     * prevents the rectangle with a stroke from becoming larger than the
     * original size of the rectangle.
     */
    qreal adjustment = 0.5 * penWidth;
    return rect.adjusted(adjustment, adjustment, -adjustment, -adjustment);
}

void KoTabBarProxyStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == CE_TabBarTabShape) {
        const auto tabOption(qstyleoption_cast<const QStyleOptionTab *>(option));
        if (!tabOption) {
            return;
        }

        // palette and state
        const bool enabled = option->state & State_Enabled;
        const bool hovered = option->state & State_MouseOver;
        const bool selected = option->state & State_Selected;
        const bool north = tabOption->shape == QTabBar::RoundedNorth || tabOption->shape == QTabBar::TriangularNorth;
        const bool south = tabOption->shape == QTabBar::RoundedSouth || tabOption->shape == QTabBar::TriangularSouth;
        const bool west = tabOption->shape == QTabBar::RoundedWest || tabOption->shape == QTabBar::TriangularWest;
        const bool east = tabOption->shape == QTabBar::RoundedEast || tabOption->shape == QTabBar::TriangularEast;
        const QStyleOptionTab::TabPosition &position = tabOption->position;
        const bool isSingle(position == QStyleOptionTab::OnlyOneTab);
        bool isFirst(isSingle || position == QStyleOptionTab::Beginning);
        bool isLast(isSingle || position == QStyleOptionTab::End);
        bool isLeftOfSelected(tabOption->selectedPosition == QStyleOptionTab::NextIsSelected);
        bool isRightOfSelected(tabOption->selectedPosition == QStyleOptionTab::PreviousIsSelected);
        const bool verticalTabs(isVerticalTab(tabOption));

        const auto tabBar = qobject_cast<const QTabBar *>(widget);
        const auto expanding = tabBar && tabBar->expanding();

        isFirst &= !isRightOfSelected;
        isLast &= !isLeftOfSelected;

        const bool reverseLayout(option->direction == Qt::RightToLeft);
        if (reverseLayout && !verticalTabs) {
            qSwap(isFirst, isLast);
            qSwap(isLeftOfSelected, isRightOfSelected);
        }

        QRectF frameRect = option->rect;
        switch (tabOption->shape) {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:
            frameRect.adjust(0, 0, 0, 1);
            break;
        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            frameRect.adjust(0, 0, 0, -1);
            break;
        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            frameRect.adjust(0, 0, 1, 0);
            break;
        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            frameRect.adjust(0, 0, -1, 0);
            break;
        }

        // setup painter
        painter->setRenderHint(QPainter::Antialiasing, true);
        QColor bgBrush;

        const auto palette = option->palette;
        const auto windowColor = palette.color(QPalette::Base);

        if (selected) {
            // overlap border
            bgBrush = windowColor;
            QColor penBrush = KColorUtils::mix(bgBrush, palette.color(QPalette::WindowText), Bias_Default);
            painter->setPen(Qt::NoPen);
            painter->setBrush(bgBrush);
            painter->drawRect(frameRect.adjusted(0, 0, 0, east || west ? -1 : 0));

            const auto hover = alphaColor(palette.color(QPalette::Highlight), 0.2);
            bgBrush = hover;

            penBrush = KColorUtils::mix(bgBrush, palette.color(QPalette::WindowText), Bias_Default);
            painter->setBrush(bgBrush);
            painter->setPen(QPen(penBrush, PenWidthFrame));
            painter->setPen(Qt::NoPen);
            QRectF highlightRect = frameRect;
            if (north || south) {
                highlightRect.setHeight(TabBar_ActiveEffectSize);
            } else if (west || east) {
                highlightRect.setWidth(TabBar_ActiveEffectSize);
            }
            if (south) {
                highlightRect.moveBottom(frameRect.bottom());
            } else if (east) {
                highlightRect.moveRight(frameRect.right());
            }
            painter->drawRect(strokedRect(frameRect));
            painter->setBrush(palette.color(QPalette::Highlight));
            painter->setPen(Qt::NoPen);
            painter->drawRect(highlightRect);
        } else {
            bgBrush = windowColor;
            const auto hover = alphaColor(hoverColor(palette), 0.2);
            if (enabled && hovered && !selected) {
                bgBrush = hover;
            }
            painter->setBrush(bgBrush);
            painter->setPen(Qt::NoPen);
            painter->drawRect(frameRect);
        }

        if (!isFirst && (north || south)) {
            auto penColor = KColorUtils::mix(windowColor, palette.color(QPalette::WindowText), 0.20);
            QRectF lineRect = frameRect;
            lineRect.setRight(lineRect.x() + 1);
            lineRect.adjust(0, 0, 0, -1);

            // ensure the background it the right color
            painter->setBrush(bgBrush);
            painter->drawRect(lineRect);

            // draw separator
            if (!isRightOfSelected && !selected) {
                lineRect.adjust(0, 4, 0, -3);
            }
            painter->setBrush(penColor);
            painter->drawRect(lineRect);

            if (selected && !expanding) {
                lineRect.setX(frameRect.x() + frameRect.width() - 1);
                lineRect.setWidth(1);
                painter->drawRect(lineRect);
            }
        } else if (!isFirst && (east || west)) {
            auto penColor = KColorUtils::mix(windowColor, palette.color(QPalette::WindowText), 0.20);
            QRectF lineRect = frameRect;
            lineRect.setBottom(lineRect.y() - 1);
            lineRect.adjust(0, 0, -1, 0);

            // ensure the background it the right color
            painter->setBrush(bgBrush);
            painter->drawRect(lineRect);

            // draw separator
            if (!isRightOfSelected && !selected) {
                lineRect.adjust(4, 0, -3, 0);
            }
            painter->setBrush(penColor);
            painter->drawRect(lineRect);

            if (selected && !expanding) {
                lineRect.setY(frameRect.y() + frameRect.height() - 1);
                lineRect.setHeight(1);
                painter->drawRect(lineRect);
            }
        }
        return;
    }

    QProxyStyle::drawControl(element, option, painter, widget);
}

QColor KoTabBarProxyStyle::hoverColor(const QPalette &palette) const
{
    return _viewHoverBrush.brush(palette).color();
}

QRect KoTabBarProxyStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    if (element == SE_TabWidgetTabBar) {
        // cast option and check
        const auto tabOption = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option);
        if (!tabOption) {
            return proxy()->subElementRect(SE_TabWidgetTabBar, option, widget);
        }

        const auto tabWidget = qobject_cast<const QTabWidget *>(widget);
        const auto documentMode = tabWidget->documentMode();

        // do nothing if tabbar is hidden
        const QSize tabBarSize(tabOption->tabBarSize);

        auto rect(option->rect);
        QRect tabBarRect(QPoint(0, 0), tabBarSize);

        Qt::Alignment tabBarAlignment(styleHint(SH_TabBar_Alignment, option, widget));

        // horizontal positioning
        const bool verticalTabs(isVerticalTab(tabOption->shape));
        if (verticalTabs) {
            tabBarRect.setHeight(qMin(tabBarRect.height(), rect.height() - (documentMode ? 0 : 2)));
            if (tabBarAlignment == Qt::AlignCenter) {
                tabBarRect.moveTop(rect.top() + (rect.height() - tabBarRect.height()) / 2);
            } else {
                tabBarRect.moveTop(rect.top() + (documentMode ? 0 : 1));
            }

        } else {
            // account for corner rects
            // need to re-run visualRect to remove right-to-left handling, since it is re-added on tabBarRect at the end
            const auto leftButtonRect(visualRect(option, subElementRect(SE_TabWidgetLeftCorner, option, widget)));
            const auto rightButtonRect(visualRect(option, subElementRect(SE_TabWidgetRightCorner, option, widget)));

            rect.setLeft(leftButtonRect.width());
            rect.setRight(rightButtonRect.left());

            tabBarRect.setWidth(qMin(tabBarRect.width(), rect.width() - (documentMode ? 0 : 2)));
            if (tabBarAlignment == Qt::AlignCenter) {
                tabBarRect.moveLeft(rect.left() + (rect.width() - tabBarRect.width()) / 2);
            } else {
                tabBarRect.moveLeft(rect.left() + (documentMode ? 0 : 1));
            }

            tabBarRect = visualRect(option, tabBarRect);
        }

        // expand the tab bar towards the frame to cover the frame's border
        switch (tabOption->shape) {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:
            tabBarRect.moveTop(rect.top());
            tabBarRect.setBottom(tabBarRect.bottom() + 1);
            break;

        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            tabBarRect.moveBottom(rect.bottom());
            tabBarRect.setTop(tabBarRect.top() - 1);
            break;

        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            tabBarRect.moveLeft(rect.left());
            tabBarRect.setRight(tabBarRect.right() + 1);
            break;

        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            tabBarRect.moveRight(rect.right());
            tabBarRect.setLeft(tabBarRect.left() - 1);
            break;

        default:
            break;
        }

        return tabBarRect;
    }

    return QProxyStyle::subElementRect(element, option, widget);
}

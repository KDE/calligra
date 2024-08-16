// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <KStatefulBrush>
#include <QProxyStyle>
#include <QStyleOptionTab>
#include <qproxystyle.h>

class KoTabBarProxyStyle : public QProxyStyle
{
public:
    explicit KoTabBarProxyStyle();

    KStatefulBrush _viewHoverBrush;
    static constexpr int TabBar_TabMarginHeight = 8;
    static constexpr int TabBar_TabMarginWidth = 8;
    static constexpr int TabBar_TabItemSpacing = 8;
    static constexpr int TabBar_TabMinWidth = 80;
    static constexpr int TabBar_TabMinHeight = 30;
    static constexpr int TabBar_TabOverlap = 0;
    static constexpr int TabBar_BaseOverlap = 0;
    static constexpr int TabBar_ActiveEffectSize = 3;
    static constexpr double PenWidthFrame = 1.001;
    static constexpr double Bias_Default = 0.2;

    QRectF strokedRect(const QRectF &rect, const qreal penWidth = PenWidthFrame) const;

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;

    QSize sizeFromContents(ContentsType element, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;

    bool isVerticalTab(const QStyleOptionTab *option) const
    {
        return isVerticalTab(option->shape);
    }

    bool isVerticalTab(const QTabBar::Shape &shape) const
    {
        return shape == QTabBar::RoundedEast || shape == QTabBar::RoundedWest || shape == QTabBar::TriangularEast || shape == QTabBar::TriangularWest;
    }

    using QProxyStyle::visualRect;
    QRect visualRect(const QStyleOption *opt, const QRect &subRect) const
    {
        return QProxyStyle::visualRect(opt->direction, opt->rect, subRect);
    }

    qreal devicePixelRatio(QPainter *painter) const;

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

    QColor alphaColor(QColor color, qreal alpha) const;
    QColor hoverColor(const QPalette &palette) const;
};

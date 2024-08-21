/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoDockWidgetTitleBarButton.h"

#include <WidgetsDebug.h>

#include <QAbstractButton>
#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QStylePainter>

class Q_DECL_HIDDEN KoDockWidgetTitleBarButton::Private
{
public:
    Private()
        : styleSize(0, 0)
        , iconSize(0)
    {
    }
    QSize styleSize;
    int iconSize;
};

KoDockWidgetTitleBarButton::KoDockWidgetTitleBarButton(QWidget *parent)
    : QAbstractButton(parent)
    , d(new Private())
{
    setFocusPolicy(Qt::NoFocus);
}

KoDockWidgetTitleBarButton::~KoDockWidgetTitleBarButton()
{
    delete d;
}

QSize KoDockWidgetTitleBarButton::sizeHint() const
{
    ensurePolished();

    const int margin = style()->pixelMetric(QStyle::PM_DockWidgetTitleBarButtonMargin, nullptr, this);
    if (icon().isNull()) {
        return QSize(18, 18); // TODO: was QSize(margin, margin), hardcoded values seem bad
    }

    int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this);
    if (iconSize != d->iconSize) {
        const_cast<KoDockWidgetTitleBarButton *>(this)->d->iconSize = iconSize;
        const QPixmap pm = icon().pixmap(iconSize);
        const_cast<KoDockWidgetTitleBarButton *>(this)->d->styleSize = QSize(pm.width() + margin, pm.height() + margin);
    }
    return d->styleSize;
}

QSize KoDockWidgetTitleBarButton::minimumSizeHint() const
{
    return sizeHint();
}

// redraw the button when the mouse enters or leaves it
void KoDockWidgetTitleBarButton::enterEvent(QEnterEvent *event)
{
    if (isEnabled())
        update();
    QAbstractButton::enterEvent(event);
}

void KoDockWidgetTitleBarButton::leaveEvent(QEvent *event)
{
    if (isEnabled())
        update();
    QAbstractButton::leaveEvent(event);
}

void KoDockWidgetTitleBarButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QStyleOptionToolButton opt;
    opt.initFrom(this);
    opt.state |= QStyle::State_AutoRaise;

    if (isEnabled() && underMouse() && !isChecked() && !isDown())
        opt.state |= QStyle::State_Raised;
    if (isChecked())
        opt.state |= QStyle::State_On;
    if (isDown())
        opt.state |= QStyle::State_Sunken;
    style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);

    opt.icon = icon();
    opt.subControls = {};
    opt.activeSubControls = {};
    opt.features = QStyleOptionToolButton::None;
    opt.arrowType = Qt::NoArrow;
    int size = style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this);
    opt.iconSize = QSize(size, size);
    style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &p, this);
}

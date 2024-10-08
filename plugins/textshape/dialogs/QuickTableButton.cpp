/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "QuickTableButton.h"

#include <KLocalizedString>
#include <KoIcon.h>
#include <QDebug>

#include <QFrame>
#include <QGridLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QWidgetAction>

// This class is the main place where the expanding grid is done
class SizeChooserGrid : public QFrame
{
public:
    SizeChooserGrid(QuickTableButton *button, QAction *action);
    QSize sizeHint() const override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void enterEvent(QEnterEvent *ev) override;
    void leaveEvent(QEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void paintEvent(QPaintEvent *event) override;

private:
    int m_column;
    int m_row;
    qreal m_columnWidth;
    qreal m_rowHeight;
    int m_leftMargin;
    int m_topMargin;
    int m_extraWidth;
    int m_extraHeight;
    QuickTableButton *m_button;
    QAction *m_action;
};

SizeChooserGrid::SizeChooserGrid(QuickTableButton *button, QAction *action)
    : QFrame()
    , m_column(0)
    , m_row(0)
    , m_columnWidth(30)
    , m_button(button)
    , m_action(action)
{
    setFrameShadow(Sunken);
    setBackgroundRole(QPalette::Base);
    setFrameShape(StyledPanel);
    setMouseTracking(true);

    QFontMetrics metrics(font());
    m_rowHeight = metrics.height() + 2;
    m_columnWidth = metrics.boundingRect("8x22").width() + 2;

    auto margins = contentsMargins();
    m_leftMargin = margins.left() + 4;
    m_topMargin = margins.top() + 4;
    m_extraWidth = margins.right() + m_leftMargin + 4 + 1;
    m_extraHeight = margins.bottom() + m_topMargin + 4 + 1;
}

QSize SizeChooserGrid::sizeHint() const
{
    return QSize(m_extraWidth + 8 * m_columnWidth, m_extraHeight + 8 * m_rowHeight);
}

void SizeChooserGrid::mouseMoveEvent(QMouseEvent *ev)
{
    m_column = qMin(qreal(7.0), (ev->x() - m_leftMargin) / m_columnWidth);
    m_row = qMin(qreal(7.0), (ev->y() - m_topMargin) / m_rowHeight);
    repaint();
}

void SizeChooserGrid::enterEvent(QEnterEvent *event)
{
    m_action->activate(QAction::Hover);
    QFrame::enterEvent(event);
}

void SizeChooserGrid::leaveEvent(QEvent *)
{
    m_column = -1;
    m_row = -1;
    repaint();
}

void SizeChooserGrid::mouseReleaseEvent(QMouseEvent *ev)
{
    if (contentsRect().contains(ev->pos())) {
        m_button->emitCreate(m_row + 1, m_column + 1);
    }
    QFrame::mouseReleaseEvent(ev);
}

void SizeChooserGrid::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(contentsRect(), palette().brush(QPalette::Base));
    painter.translate(m_leftMargin, m_topMargin);
    painter.translate(0.5, 0.5);
    QPen pen = painter.pen();
    pen.setWidthF(0.5);
    painter.setPen(pen);
    painter.fillRect(QRectF(0.0, 0.0, (m_column + 1) * m_columnWidth, (m_row + 1) * m_rowHeight), palette().brush(QPalette::Highlight));
    for (int c = 0; c <= 8; c++) {
        painter.drawLine(QPointF(c * m_columnWidth, 0.0), QPointF(c * m_columnWidth, 8 * m_rowHeight));
    }
    for (int r = 0; r <= 8; r++) {
        painter.drawLine(QPointF(0.0, r * m_rowHeight), QPointF(8 * m_columnWidth, r * m_rowHeight));
    }
    QTextOption option(Qt::AlignCenter);
    option.setUseDesignMetrics(true);
    painter.drawText(QRectF(0.0, 0.0, m_columnWidth, m_rowHeight), QString("%1x%2").arg(m_column + 1).arg(m_row + 1), option);
    painter.end();
}

// This class is the main place where the expanding grid is done
class SizeChooserAction : public QWidgetAction
{
public:
    SizeChooserAction(QuickTableButton *button);
    SizeChooserGrid *m_widget;
};

SizeChooserAction::SizeChooserAction(QuickTableButton *button)
    : QWidgetAction(nullptr)
{
    m_widget = new SizeChooserGrid(button, this);
    setDefaultWidget(m_widget);
}

// And now for the button itself
QuickTableButton::QuickTableButton(QWidget *parent)
    : QToolButton(parent)
{
    setToolTip(i18n("Insert a table"));
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIcon(koIcon("insert-table"));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_menu = new QMenu(this);
    setMenu(m_menu);
    setPopupMode(InstantPopup);
}

void QuickTableButton::addAction(QAction *action)
{
    m_menu->addAction(action);
    m_menu->addAction(new SizeChooserAction(this));
}

void QuickTableButton::emitCreate(int rows, int columns)
{
    m_menu->hide();
    Q_EMIT create(rows, columns);
}

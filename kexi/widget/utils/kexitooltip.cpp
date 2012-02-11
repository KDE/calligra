/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexitooltip.h"

#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QImage>
#include <QFont>
#include <QFontMetrics>
#include <QTimer>
#include <QToolTip>

#ifdef __GNUC__
#warning KexiToolTip ported to Qt4 but not tested
#else
#pragma WARNING( KexiToolTip ported to Qt4 but not tested )
#endif

KexiToolTip::KexiToolTip(const QVariant& value, QWidget* parent)
        : QWidget(parent)
        , m_value(value)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint
                   | Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setPalette(QToolTip::palette());
    setFocusPolicy(Qt::NoFocus);
}

KexiToolTip::~KexiToolTip()
{
}

QSize KexiToolTip::sizeHint() const
{
    QSize sz(fontMetrics().boundingRect(m_value.toString()).size());
    return sz;
}

void KexiToolTip::show()
{
    updateGeometry();
    QWidget::show();
}

void KexiToolTip::paintEvent(QPaintEvent *pev)
{
    QWidget::paintEvent(pev);
    QPainter p(this);
    drawFrame(p);
    drawContents(p);
}

void KexiToolTip::drawFrame(QPainter& p)
{
//!TODO check
    p.setPen(QPen(palette().foreground().color(), 1));
    p.drawRect(rect());
}

void KexiToolTip::drawContents(QPainter& p)
{
    p.drawText(rect(), Qt::AlignCenter, m_value.toString());
}

#include "kexitooltip.moc"

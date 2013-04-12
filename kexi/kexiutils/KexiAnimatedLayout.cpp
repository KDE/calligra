/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#include "KexiAnimatedLayout.h"
#include "KexiAnimatedLayout_p.h"
 
#include <kglobalsettings.h>

#include <QPainter>
#include <QPaintEvent>

KexiAnimatedLayout::Private::Private(KexiAnimatedLayout * qq)
 : QWidget(), q(qq), animation(this, "pos")
{
    hide();
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    animation.setEasingCurve(QEasingCurve::InOutQuart);
    animation.setDuration(500);
    connect(&animation, SIGNAL(finished()), this, SLOT(animationFinished()));
}

void KexiAnimatedLayout::Private::animateTo(QWidget* destination)
{
    QWidget *from = q->currentWidget();
    Q_ASSERT(destination && from);
    destinationWidget = destination;
    if (from == destinationWidget)
        return;
    if (!(KGlobalSettings::self()->graphicEffectsLevel()
          & KGlobalSettings::SimpleAnimationEffects))
    {
        // animations not allowed: switch to destination widget immediately
        animationFinished();
        return;
    }
    bool toRight = q->currentIndex() < q->indexOf(destinationWidget);
    hide();
    setParent(from);
    QSize s(from->parentWidget()->size());
    QPoint startPos(toRight ? QPoint(0, 0) : QPoint(s.width(), 0));
    QPoint endPos(toRight ? QPoint(s.width(), 0) : QPoint(0, 0));
    animation.setStartValue(-startPos);
    animation.setEndValue(-endPos);
    buffer = QPixmap(s.width() * 2, s.height());
    buffer.fill(Qt::white);
    from->render(&buffer, startPos);
    //kDebug() << s << from->geometry() << destinationWidget->geometry();
    destinationWidget->resize(from->size()); // needed because destination could
                                               // have been never displayed
    destinationWidget->render(&buffer, endPos);
    resize(buffer.size());
    move(animation.startValue().toPoint().x(), animation.startValue().toPoint().y());
    show();
    animation.start();
}

void KexiAnimatedLayout::Private::paintEvent(QPaintEvent* event)
{
    if (buffer.isNull())
        return;
    QPainter p(this);
    p.drawPixmap(event->rect(), buffer, event->rect());
}

void KexiAnimatedLayout::Private::animationFinished()
{
    if (destinationWidget) {
        static_cast<QStackedLayout*>(q)->setCurrentWidget(destinationWidget);
    }
    hide();
    destinationWidget = 0;
    buffer = QPixmap();
}

// ----

KexiAnimatedLayout::KexiAnimatedLayout(QWidget* parent)
 : QStackedLayout(parent)
 , d(new Private(this))
{
}

KexiAnimatedLayout::~KexiAnimatedLayout()
{
    delete d;
}

void KexiAnimatedLayout::setCurrentWidget(QWidget* widget)
{
    if (indexOf(widget) < 0) {
        return;
    }
    if (!currentWidget()) {
        QStackedLayout::setCurrentWidget(widget);
        return;
    }
    d->animateTo(widget);
}

void KexiAnimatedLayout::setCurrentIndex(int index)
{
    QWidget *w = widget(index);
    if (!w)
        return;
    
    setCurrentWidget(w);
}

#include "KexiAnimatedLayout.moc"
#include "KexiAnimatedLayout_p.moc"

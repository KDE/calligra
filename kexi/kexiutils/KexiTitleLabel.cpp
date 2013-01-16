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

#include "KexiTitleLabel.h"
#include "utils.h"

#include <KDebug>

#include <QApplication>
#include <QDesktopWidget>

class KexiTitleLabel::Private {
public:
    Private() : updateFontEnabled(true) {}
    bool updateFontEnabled;
};

// ----

KexiTitleLabel::KexiTitleLabel(QWidget * parent, Qt::WindowFlags f)
 : QLabel(parent, f)
 , d(new Private)
{
    init();
}

KexiTitleLabel::KexiTitleLabel(const QString & text, QWidget * parent, Qt::WindowFlags f)
 : QLabel(text, parent, f)
 , d(new Private)
{
    init();
}

void KexiTitleLabel::init()
{
    setWordWrap(true);
    updateFont();
}

KexiTitleLabel::~KexiTitleLabel()
{
    delete d;
}

void KexiTitleLabel::updateFont()
{
    if (!d->updateFontEnabled)
        return;
    KexiUtils::BoolBlocker guard(d->updateFontEnabled, false);
    
    qreal factor;
    QRect geo = QApplication::desktop()->availableGeometry(this);
    QFont f = font();
    if (geo.width() > 600 && geo.height() > 600) {
        factor = 2.0;
    }
    else {
        factor = 1.2;
        f.setBold(true);
    }
    //kDebug() << f.pointSize() << f.pixelSize();
    if (f.pointSize() == -1) {
        f.setPixelSize(qreal(f.pixelSize()) * factor);
    }
    else {
        f.setPointSize(f.pointSizeF() * factor);
    }
    setFont(f);
}

void KexiTitleLabel::changeEvent(QEvent* event)
{
    QLabel::changeEvent(event);
    if (event->type() == QEvent::FontChange) {
        updateFont();
    }
}

#include "KexiTitleLabel.moc"

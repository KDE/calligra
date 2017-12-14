/* This file is part of the KDE project
  Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "Help.h"

#include <QEvent>
#include <QWhatsThisClickedEvent>
#include <QDesktopServices>
#include <QWidget>
#include <QDebug>

namespace KPlato
{

Help* Help::self = 0;

Help::Help(const QString &docpath)
{
    if (!self) {
        self = this;
        m_docpath = docpath;
    }
}

Help::~Help()
{
    self = 0;
}

void Help::add(QWidget *widget, const QString &text)
{
    widget->installEventFilter(new WhatsThisClickedEventHandler(widget));
    widget->setWhatsThis(text);
}

QString Help::page(const QString &page)
{
    if (!self) {
        new Help("https://userbase.kde.org/Plan");
    }
    QString url = self->m_docpath;
    if (!page.isEmpty()) {
        url = QString("%1/%2").arg(url, page);
    }
    return url;
}


WhatsThisClickedEventHandler::WhatsThisClickedEventHandler(QObject *parent)
    : QObject(parent)
{

}

bool WhatsThisClickedEventHandler::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::WhatsThisClicked) {
        QWhatsThisClickedEvent *e = static_cast<QWhatsThisClickedEvent*>(event);
        QUrl url(e->href());
        if (url.isValid()) {
            QDesktopServices::openUrl(url);
        }
        return true;
    }
    return false;
}

} // namespace KPlato

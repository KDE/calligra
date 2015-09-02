/*
 *  Copyright (c) 1998, 1999 Torben Weis <weis@kde.org>
 *  Copyright (c) 2007 Thomas Zander <zander@kde.org>
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _STATUSBARITEM_H_
#define _STATUSBARITEM_H_

#include "MainWindow.h"

#include <QStatusBar>

struct MainWindow::StatusBarItem {
    StatusBarItem(QWidget* _widget, int _strech, bool _permanent) : m_widget(_widget), m_stretch(_strech), m_permanent(_permanent), m_visible(false) {
    }
    void ensureItemShown(QStatusBar * sb) {
        Q_ASSERT(m_widget);
        Q_ASSERT(sb);
        if(!m_visible) {
            if(m_permanent) {
                sb->addPermanentWidget(m_widget, m_stretch);
            } else {
                sb->addWidget(m_widget, m_stretch);
            }
            m_visible = true;
            m_widget->show();
        }
    }
    void ensureItemHidden(QStatusBar * sb) {
        Q_ASSERT(sb);
        if(m_visible) {
            sb->removeWidget(m_widget);
            m_visible = false;
            m_widget->hide();
        }
    }
    QWidget* m_widget;
    int m_stretch;
    bool m_permanent;
    bool m_visible;
};

#endif

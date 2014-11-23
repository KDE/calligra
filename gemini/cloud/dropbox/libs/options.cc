/*

    Copyright 2011 Cuong Le <metacuong@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "options.h"

#include <QSettings>
#include <QStringList>
#include <QDebug>

Options::Options(QObject *parent) :
    QObject(parent)
{
    get_transfers_auto();
    get_screen_orientation();
    get_push_notification();
}

bool Options::is_transfers_auto() const {
    return m_is_transfers_auto;
}

int  Options::screen_orientation() const {
    return m_screen_orientation;
}

bool  Options::is_push_notification() const {
    return m_is_push_notification;
}


void Options::get_transfers_auto() {
    QSettings settings;
    settings.beginGroup("transfers");

    if (settings.childKeys().indexOf("type") == -1){
        m_is_transfers_auto = false;
        return; //default is manual
    }
    m_is_transfers_auto = settings.value("type").toBool();
}

void Options::get_screen_orientation() {
    QSettings settings;
    settings.beginGroup("screen_orientation");

    if(settings.childKeys().indexOf("type") == -1){
        m_screen_orientation = 2;
        return;//default is auto
    }

    m_screen_orientation = settings.value("type").toInt();
}

void Options::get_push_notification() {
    QSettings settings;
    settings.beginGroup("push_notification");

    if(settings.childKeys().indexOf("type") == -1){
        m_is_push_notification = true;
        return;//default is auto
    }

    m_is_push_notification = settings.value("type").toBool();
}

void Options::set_transfers_auto(const bool &val){
    QSettings settings;
    settings.setValue("transfers/type", val);
    m_is_transfers_auto = val;
}

void Options::set_screen_orientation(const int &val){
    QSettings settings;
    settings.setValue("screen_orientation/type", val);
    m_screen_orientation = val;
}

void Options::set_push_notification(const bool &val){
    QSettings settings;
    settings.setValue("push_notification/type", val);
    m_is_push_notification = val;
}

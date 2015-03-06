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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QObject>

class Options : public QObject
{
    Q_OBJECT
public:
    explicit Options(QObject *parent = 0);

Q_SIGNALS:

public Q_SLOTS:
    bool is_transfers_auto() const;
    int screen_orientation() const;
    bool is_push_notification() const;

    void set_transfers_auto(const bool&);
    void set_screen_orientation(const int&);
    void set_push_notification(const bool&);

private:
    void get_transfers_auto();
    void get_screen_orientation();
    void get_push_notification();

    bool m_is_transfers_auto;
    int m_screen_orientation;
    bool m_is_push_notification;
};

#endif // OPTIONS_H

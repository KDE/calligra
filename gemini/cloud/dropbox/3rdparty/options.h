/*

    SPDX-FileCopyrightText: 2011 Cuong Le <metacuong@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QObject>

class Options : public QObject
{
    Q_OBJECT
public:
    explicit Options(QObject *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:
    bool is_transfers_auto() const;
    int screen_orientation() const;
    bool is_push_notification() const;

    void set_transfers_auto(const bool &);
    void set_screen_orientation(const int &);
    void set_push_notification(const bool &);

private:
    void get_transfers_auto();
    void get_screen_orientation();
    void get_push_notification();

    bool m_is_transfers_auto;
    int m_screen_orientation;
    bool m_is_push_notification;
};

#endif // OPTIONS_H

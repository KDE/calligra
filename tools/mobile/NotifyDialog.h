/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef NOTIFYDIALOG_H
#define NOTIFYDIALOG_H

#include <QDialog>

class QVBoxLayout;
class QLabel;

namespace Ui
{
class NotifyDialog;
}
/*!
 * \brief Dialog is to notify user that selected document is not supported by Office
 */
class NotifyDialog : public QDialog
{
    Q_OBJECT

public:
    NotifyDialog(const QString& messageText, QWidget *parent = 0);
    ~NotifyDialog();

private:
    /*!
     * Layout on notifydialog
     */
    QVBoxLayout *m_layout;
    /*!
     * Label for notify text
     */
    QLabel *m_label;
};

#endif // NOTIFYDIALOG_H

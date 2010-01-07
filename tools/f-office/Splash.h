/*
 * This file is part of Maemo 5 Office UI for KOffice
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

#ifndef SPLASH_H
#define SPLASH_H

#include <QSplashScreen>

class QVBoxLayout;
class QLabel;

namespace Ui
{
class Splash;
}

/*!
 * \brief Splash screen to display KOffice Icon
 */
class Splash : public QSplashScreen
{
    Q_OBJECT

public:
    Splash(QWidget *parent = 0);
    ~Splash();
private:
    /*!
     * Layout on splashscreen
     */
    QVBoxLayout *m_layout;
    /*!
     * Label for koffice logo
     */
    QLabel *m_label;
    /*!
     * Pixmap for KOffice png
     */
    QPixmap m_kOfficeLogo;
};

#endif // SPLASH_H


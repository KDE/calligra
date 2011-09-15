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

#include "Splash.h"
#include "Common.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialog>
#include<QGridLayout>
#include <klocalizedstring.h>

Splash::Splash(QWidget *parent)
        : QSplashScreen(parent),
        m_layout(0),
        m_label(0),
        m_calligraLogo(SPLASH_LOGO_CALLIGRA_PATH)
{
    m_layout= new QVBoxLayout(this);
    m_label=new QLabel(this);
    m_label->setPixmap(m_calligraLogo);
    m_layout->addWidget(m_label);
    m_label->setAlignment(Qt::AlignCenter);
    this->setLayout(m_layout);
    this->setWindowTitle(i18n("Office"));
}

Splash::~Splash()
{
}

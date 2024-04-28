/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2003 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoDpi.h"

#include <QFontInfo>

#include <QApplication>
#include <QWidget>

#include <QGlobalStatic>

Q_GLOBAL_STATIC(KoDpi, s_instance)

KoDpi *KoDpi::self()
{
    return s_instance;
}

KoDpi::KoDpi()
{
    // Another way to get the DPI of the display would be QPaintDeviceMetrics,
    // but we have no widget here (and moving this to KoView wouldn't allow
    // using this from the document easily).
    auto window = qApp->activeWindow();
    if (window) {
        m_dpiX = window->logicalDpiX();
        m_dpiY = window->logicalDpiY();
    } else {
        m_dpiX = 75;
        m_dpiY = 75;
    }
}

void KoDpi::setDPI(int x, int y)
{
    // debugWidgets << x <<"," << y;
    KoDpi *s = self();
    s->m_dpiX = x;
    s->m_dpiY = y;
}

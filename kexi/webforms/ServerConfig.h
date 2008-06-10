/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIWEBFORMS_SERVERCONFIG_H
#define KEXIWEBFORMS_SERVERCONFIG_H

#include <QString>

namespace KexiWebForms {

    /*!
     * A structure representing HTTP server configuration
     */
    struct ServerConfig {

        /*! A string to specify listen ports */
        QString ports;

        /*!
         * A string specifying the webroot, it must contain at least
         * the required template files
         */
        QString webRoot;

        /*! Wether or not enable directory listing */
        bool dirList;

        /*! Wether or not enable SSL support */
        QString https;

        /*!
         * If SSL support is enabled, this string should point to a valid
         * certificate file
         */
        QString certPath;

        /*!
         * Path to the Kexi database file, Kexi shortcut file or Kexi connection file
         */
        QString dbPath;
    };

}

#endif /* KEXIWEBFORMS_SERVERCONFIG_H */

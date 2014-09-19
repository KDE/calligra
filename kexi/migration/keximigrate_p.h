/* This file is part of the KDE project
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXI_MIGRATE_P_H
#define KEXI_MIGRATE_P_H

#include <db/pluginloader.h>

//! Implementation of driver's static version information and plugin entry point.
#define K_EXPORT_KEXIMIGRATE_DRIVER( class_name, internal_name ) \
    KEXI_EXPORT_PLUGIN("keximigrate", class_name, internal_name, \
                       KEXI_MIGRATION_VERSION_MAJOR, KEXI_MIGRATION_VERSION_MINOR, 0)

#endif

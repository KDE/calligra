/* This file is part of the KDE project
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef MYSQLDRIVER_GLOBAL_H
#define MYSQLDRIVER_GLOBAL_H

#include <QtGlobal>
#ifdef Q_WS_WIN
# define KDEWIN_FCNTL_H
# define KDEWIN_SYS_STAT_H
# define KDEWIN_STDIO_H
# define KDEWIN_MATH_H

# include <../include/fcntl.h>
# include <../include/sys/stat.h>
# include <../include/stdio.h>
# include <../include/math.h>
# include <WinSock2.h>
#endif

#endif

/* This file is part of the KDE project
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __main_h__
#define __main_h__

#include <qobject.h>

/**
 * A tiny class for redirectting the progress signals to stdout. These numbers
 * on stdout are read by the parent process and sent to the progress bar.
 */
class WrapperProgress : public QObject {

    Q_OBJECT

public slots:
    void slotProgress(int p);

};

#endif // __main_h__

/* This file is part of the KDE project
   Copyright (C) 2012 C. Boemann <cbo@kogmbh.com>
   Copyright (C) 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

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

#ifndef CAUPART_H
#define CAUPART_H

#include <KWPart.h>

#include "words_export.h"

class KoMainWindow;

class WORDS_EXPORT CAuPart : public KWPart
{
    Q_OBJECT

public:
    CAuPart(QObject *parent);

    void showStartUpWidget(KoMainWindow *parent, bool alwaysShow = false);

private slots:
     void showErrorAndDie();

protected:

    virtual KoView *createViewInstance(QWidget *parent); ///< reimplemented
};

#endif

/*
 *  kis_gradient.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <qcolor.h>
#include <qclipboard.h>
#include <kapp.h>
#include <kdebug.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_vec.h"
#include "kis_cursor.h"
#include "kis_util.h"
#include "kis_painter.h"


KisPainter::KisPainter(KisDoc *doc)
{
  	pDoc  = doc;
}

KisPainter::~KisPainter()
{
}

void KisPainter::toLayer()
{
}

void KisPainter::clear()
{
}

//#include "kis_painter.moc"


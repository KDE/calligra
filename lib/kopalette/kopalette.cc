/*
 *  Copyright (c) 2005 Boudewijn Rempt <boud@valdyas.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <qwidget.h>
#include <qdockwindow.h>
#include <qvariant.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlayout.h>

#include <klocale.h>
#include <kglobalsettings.h>
#include <kaccelmanager.h>
#include <koView.h>

#include "kopalette.h"



KoPalette::KoPalette(QWidget * parent, const char * name)
    : QDockWindow(parent, name)
{

#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,90)
    KAcceleratorManager::setNoAccel(this);
#endif    
    setCloseMode( QDockWindow::Never);
    setResizeEnabled(true);
    setOpaqueMoving(true);
    setFocusPolicy(QWidget::NoFocus);
    setVerticallyStretchable(false);
    setHorizontallyStretchable(false);
    
    setNewLine(true);
    layout() -> setSpacing(0);
    layout() -> setMargin(0);

    // Compute a font that's smaller that the general font
    m_font  = KGlobalSettings::generalFont();
    float ps = m_font.pointSize() * 0.7;
    m_font.setPointSize((int)ps);
    setFont(m_font);

}

KoPalette::~KoPalette()
{
}

void KoPalette::setMainWidget(QWidget * widget)
{
    setWidget(widget);
    resize( QSize(285, 233).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
    widget->setFont(m_font);
    m_page = widget;
}

#include "kopalette.moc"

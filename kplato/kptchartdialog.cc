/* This file is part of the KDE project
   Copyright (C) 2005 Frédéric Lambert <konkistadorr@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptchartdialog.h"
#include "kptchartpanel.h"
#include <klocale.h>

namespace KPlato
{

ChartDialog::ChartDialog(Project &p, QWidget *parent, const char *n) : KDialog(parent)
{
    setCaption( i18n("Project Charts Indicators") );
    setButtons( KDialog::Ok );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_panel = new ChartPanel(p,this);
    setMainWidget(m_panel);
    m_panel->show();
    resize(610,550);
    setMinimumSize(610,550);
}

ChartDialog::~ChartDialog()
{
	//delete m_panel;
}

void ChartDialog::resizeEvent(QResizeEvent* ev)
{
	m_panel->setMaximumWidth((ev->size()).width()-10);
	m_panel->setMaximumHeight((ev->size()).height()-100);
}

} //namespace KPlato

#include "kptchartdialog.moc"


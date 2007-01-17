/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

ChartDialog::ChartDialog(QWidget *p, const char *n) : KDialog(p)
{
    kDebug()<<"HELOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO WORRRRRRRRRLLLLLLLLLLDDDDDDDD";
    setCaption( i18n("Project Charts Indicators") );
    //setDefaultButton( CloseButton );
    showButtonSeparator( true );
    m_panel = new ChartPanel(this);
    setMainWidget(m_panel);
    m_panel->show();
    //enableButtonOk(false);
   // connect(m_panel, SIGNAL(changed(bool)), SLOT(enableButtonClose(bool)));
   // connect(this,SIGNAL(CloseClicked()),this,SLOT(slotClose()));
}

KCommand *ChartDialog::buildCommand(Part *part) {
	kDebug()<< "Chart Dialog : buildCommand";
    //return m_panel->buildCommand(part);
}

void ChartDialog::slotClose() {
    kDebug()<< "Chart Dialog : slotClose";
    //m_panel->slotClose();

}

} //namespace KPlato

#include "kptchartdialog.moc"

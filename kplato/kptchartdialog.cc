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

ChartDialog::ChartDialog(QWidget *p, const char *n) : KDialog(p)
{
    setCaption( i18n("Project Charts Indicators") );
    setButtons( KDialog::Ok );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_panel = new ChartPanel(this);
    setMainWidget(m_panel);
    m_panel->show();

    resize(610,550);

   // connect(m_panel, SIGNAL(changed(bool)), SLOT(enableButtonClose(bool)));
   // connect(this,SIGNAL(CloseClicked()),this,SLOT(slotClose()));
}

KCommand *ChartDialog::buildCommand(Part *part) {
	kDebug()<< "Chart Dialog : buildCommand";
    //return m_panel->buildCommand(part);
    return 0;
}

void ChartDialog::slotClose() {
    kDebug()<< "Chart Dialog : slotClose";
    //m_panel->slotClose();

}

} //namespace KPlato

#include "kptchartdialog.moc"


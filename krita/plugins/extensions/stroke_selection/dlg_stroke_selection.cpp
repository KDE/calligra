/*
 *  Copyright (c) 2012 Boudewijn Rempt <boud@valdyas.org>
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

#include "dlg_stroke_selection.h"

#include <math.h>

#include <klocale.h>
#include <kis_debug.h>

DlgStrokeSelection::DlgStrokeSelection(QWidget *  parent, const char * name)
        : KDialog(parent)
{
    setCaption(i18n("Stroke Selection"));
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    setObjectName(name);

    m_page = new WdgStrokeSelection(this);
    Q_CHECK_PTR(m_page);
    m_page->setObjectName("stroke_selection");

    setMainWidget(m_page);
    resize(m_page->sizeHint());

    connect(this, SIGNAL(okClicked()), this, SLOT(okClicked()));
}

DlgStrokeSelection::~DlgStrokeSelection()
{
    delete m_page;
}

void DlgStrokeSelection::okClicked()
{
    accept();
}

#include "dlg_stroke_selection.moc"

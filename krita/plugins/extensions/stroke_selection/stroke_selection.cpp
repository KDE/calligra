/*
 * Copyright (c) 2012 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or stroke
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

#include "stroke_selection.h"

#include <klocale.h>
#include <kcomponentdata.h>
#include <kstandarddirs.h>
#include <kpluginfactory.h>
#include <kstandardaction.h>
#include <kactioncollection.h>

#include <kis_debug.h>
#include "kis_config.h"
#include "kis_image.h"
#include "kis_layer.h"
#include "kis_global.h"
#include "kis_types.h"
#include "kis_view2.h"
#include "kis_selection.h"
#include "kis_selection_manager.h"
#include "kis_transaction.h"

#include "dlg_stroke_selection.h"

K_PLUGIN_FACTORY(StrokeSelectionFactory, registerPlugin<StrokeSelection>();)
K_EXPORT_PLUGIN(StrokeSelectionFactory("krita"))

StrokeSelection::StrokeSelection(QObject *parent, const QVariantList &)
        : KParts::Plugin(parent)
{
    if (parent->inherits("KisView2")) {
        setXMLFile(KStandardDirs::locate("data", "kritaplugins/stroke_selection.rc"),
                   true);

        m_view = (KisView2*) parent;

        m_strokeSelection = new KAction(i18n("Stroke Selection..."), this);
        actionCollection()->addAction("strokeselection", m_strokeSelection);

        Q_CHECK_PTR(m_strokeSelection);
        
        connect(m_strokeSelection, SIGNAL(triggered()), this, SLOT(slotStrokeSelection()));
        
        m_view->selectionManager()->addSelectionAction(m_strokeSelection);

        connect(m_view->selectionManager(), SIGNAL(signalUpdateGUI()), SLOT(slotUpdateGUI()));
    }
}

StrokeSelection::~StrokeSelection()
{
}

void StrokeSelection::slotUpdateGUI()
{
    bool enable = m_view->selectionManager()->haveEditablePixelSelectionWithPixels();

    m_strokeSelection->setEnabled(enable);
}

void StrokeSelection::slotStrokeSelection()
{
    KisImageWSP image = m_view->image();

    if (!image) return;

    DlgStrokeSelection *dlgStrokeSelection = new DlgStrokeSelection(m_view, "StrokeSelection");

    dlgStrokeSelection->setCaption(i18n("Stroke Selection"));

    if (dlgStrokeSelection->exec() == QDialog::Accepted) {
        // XXX: stroke selection
        KisSel
    }

    delete dlgStrokeSelection;
}

#include "stroke_selection.moc"

/*
 *  Copyright (c) 2014 Spencer Brown <sbrown655@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <opengl/kis_opengl.h>

#ifdef HAVE_OPENGL

#include "previewrenderdocker_dock.h"

#include <QHBoxLayout>
#include <QPushButton>

#include <klocale.h>

#include <KoCanvasResourceManager.h>
#include <KoCanvasBase.h>

#include "kis_canvas2.h"
#include "kis_view2.h"
#include "kis_paintop_box.h"
#include "kis_paintop_presets_chooser_popup.h"
#include "kis_canvas_resource_provider.h"
#include <kis_paintop_preset.h>

#include "kis_previewrenderer_widget.h"


PreviewRenderDockerDock::PreviewRenderDockerDock( )
    : QDockWidget(i18n("Preview Render"))
{
    m_previewRenderer = new KisPreviewRendererWidget(this->widget());
    setWidget(m_previewRenderer);
}


#include "previewrenderdocker_dock.moc"
#endif //HAVE_OPENGL

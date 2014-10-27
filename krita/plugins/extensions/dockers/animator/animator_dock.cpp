/*
 *  Copyright (c) 2013 Somsubhra Bairi <somsubhra.bairi@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License, or(at you option)
 *  any later version.
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

#include "animator_dock.h"

#include <klocale.h>

#include "kis_view2.h"
#include "kis_animation.h"
#include "kis_canvas2.h"
#include "kis_animation_doc.h"
#include "kis_part2.h"
#include "kis_timeline.h"

AnimatorDock::AnimatorDock() : QDockWidget(i18n("Animator")), m_canvas(0), m_animation(0)
{
    this->setMinimumHeight(120);
    m_timelineWidget = new KisTimelineWidget(this);
    this->setWidget(m_timelineWidget);
}

void AnimatorDock::setCanvas(KoCanvasBase *canvas)
{
    m_canvas = dynamic_cast<KisCanvas2*>(canvas);
    if(m_canvas && m_canvas->view() && m_canvas->view()->document()) {
        KisAnimationDoc *doc = qobject_cast<KisAnimationDoc*>(m_canvas->view()->document());
        if (doc) {
            m_animation = doc->getAnimation();
            if(m_animation) {
                m_timelineWidget->setCanvas(m_canvas);
                m_timelineWidget->setAnimation(m_animation);
            }
        }
    }
}

#include "animator_dock.moc"

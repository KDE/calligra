/*
 *  Exporter
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "animator_exporter.h"

#include <KoFilterManager.h>

#include <kis_canvas2.h>
#include <kis_view2.h>

#include "animator_manager.h"
#include "animator_updater.h"
#include "animator_switcher.h"

#include <sstream>
#include <iomanip>

AnimatorExporter::AnimatorExporter(AnimatorManager* manager)
{
    m_filter_manager = 0;
    m_manager = manager;
    setDocument(0);
    
    setPrefix("");
    setSuffix(".png");
    setNumberWidth(4);
}

AnimatorExporter::~AnimatorExporter()
{

}

void AnimatorExporter::setCanvas(KoCanvasBase* canvas)
{
    KisCanvas2* kcanvas = dynamic_cast<KisCanvas2*>(canvas);
    setDocument(kcanvas->view()->document());
}

void AnimatorExporter::unsetCanvas()
{
    setDocument(0);
}

void AnimatorExporter::setDocument(KisDoc2* document)
{
    if (document != m_document)
    {
        if (m_filter_manager)
            delete m_filter_manager;
        m_document = document;
        if (document)
        {
            setPrefix(m_document->url().path());
            m_filter_manager = new KoFilterManager(m_document);
        }
    }
}

void AnimatorExporter::exportFrame(int num)
{
    m_manager->getSwitcher()->goFrame(num);
    m_manager->image()->waitForDone();
    
    QByteArray* mime = new QByteArray("image/png");
    KoFilter::ConversionStatus status =  m_filter_manager->exportDocument(getName(num), *mime);
}

void AnimatorExporter::exportAll()
{
    m_manager->getUpdater()->playerModeOn();
    for (int i = 0; i < m_manager->framesNumber(); ++i)
    {
        exportFrame(i);
    }
    m_manager->getUpdater()->playerModeOff();
}

void AnimatorExporter::setPrefix(const QString& prefix)
{
    m_prefix = prefix;
}

void AnimatorExporter::setSuffix(const QString& suffix)
{
    m_suffix = suffix;
}

const QString AnimatorExporter::getName(int i)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(m_leadingZeros) << i;
    std::string s;
    ss >> s;
    return m_prefix+QString(s.data())+m_suffix;
}

void AnimatorExporter::setNumberWidth(int n)
{
    if (n > 0)
    {
        m_leadingZeros = n;
    }
}

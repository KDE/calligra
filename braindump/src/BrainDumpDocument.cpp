/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "BrainDumpDocument.h"

#include <kdebug.h>
#include <klocale.h>

#include "BrainDumpView.h"

// KComponentData* BrainDumpFactory::s_instance = 0;
// KAboutData* BrainDumpFactory::s_aboutData = 0;

BrainDumpDocument::BrainDumpDocument(QWidget* parentWidget, QObject* parent, bool singleViewMode)
  : KoPADocument(parentWidget, parent, singleViewMode)
{
//     setComponentData(BrainDumpFactory::componentData(), false);
//     setTemplateType("braindump_template");
}

BrainDumpDocument::~BrainDumpDocument()
{
}

KoOdf::DocumentType BrainDumpDocument::documentType() const
{
    return KoOdf::Graphics;
}

KoView* BrainDumpDocument::createViewInstance(QWidget* parent)
{
    return new BrainDumpView(this, parent);
}

const char * BrainDumpDocument::odfTagName( bool withNamespace )
{
    return withNamespace ? "office:drawing": "drawing";
}


#include "BrainDumpDocument.moc"

/*
 * Kexi Report Plugin
 * Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ReportODTRenderer.h"
#include "KoSimpleOdtDocument.h"
#include "KoSimpleOdtTextBox.h"
#include "KoSimpleOdtImage.h"
#include "KoSimpleOdtPicture.h"
#include "KoSimpleOdtLine.h"
#include "KoSimpleOdtCheckBox.h"
#include "renderobjects.h"
#include <kdebug.h>

extern int planDbg();

ReportODTRenderer::ReportODTRenderer()
{

}

ReportODTRenderer::~ReportODTRenderer()
{
}

bool ReportODTRenderer::render(const KoReportRendererContext& context, ORODocument* document, int /*page*/)
{
    int uid = 1;
    KoSimpleOdtDocument doc;
    doc.setPageOptions(document->pageOptions());
    for (int page = 0; page < document->pages(); page++) {
        OROPage *p = document->page(page);
        for (int i = 0; i < p->primitives(); i++) {
            OROPrimitive *prim = p->primitive(i);
            if (prim->type() == OROTextBox::TextBox) {
                KoSimpleOdtPrimitive *sp = new KoSimpleOdtTextBox(static_cast<OROTextBox*>(prim));
                sp->setUID(uid++);
                doc.addPrimitive(sp);
            } else if (prim->type() == OROImage::Image) {
                KoSimpleOdtPrimitive *sp = new KoSimpleOdtImage(static_cast<OROImage*>(prim));
                sp->setUID(uid++);
                doc.addPrimitive(sp);
            } else if (prim->type() == OROPicture::Picture) {
                KoSimpleOdtPrimitive *sp = new KoSimpleOdtPicture(static_cast<OROPicture*>(prim));
                sp->setUID(uid++);
                doc.addPrimitive(sp);
            } else if (prim->type() == OROLine::Line) {
                KoSimpleOdtPrimitive *sp = new KoSimpleOdtLine(static_cast<OROLine*>(prim));
                sp->setUID(uid++);
                doc.addPrimitive(sp);
            } else if (prim->type() == OROCheck::Check) {
                KoSimpleOdtPrimitive *sp = new KoSimpleOdtCheckBox(static_cast<OROCheck*>(prim));
                sp->setUID(uid++);
                doc.addPrimitive(sp);
            } else if (prim->type() == ORORect::Rect) {
                // TODO: section background
            } else {
                kDebug(planDbg()) << "unhandled primitive type."<<prim->type();
            }
        }
    }
    return doc.saveDocument(context.destinationUrl.path()) == QFile::NoError;
}


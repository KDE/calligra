/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 2002 Igor Janssen (rm@linux.ru.net)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "InsertImageTool.h"

#include <kaction.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kdebug.h>

#include "kontour_view.h"
#include "kontour_doc.h"
#include "GDocument.h"
#include "GImage.h"
#include "Canvas.h"
#include "ToolController.h"
#include "CreateImageCmd.h"

InsertImageTool::InsertImageTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction *insertimage = new ToolSelectAction(actionCollection(), "ToolAction");
  KAction *mT1 = new KAction(i18n("Insert Image"), "frame_image", 0, actionCollection());
  insertimage->insert(mT1);
}

void InsertImageTool::activate()
{
  url = KFileDialog::getImageOpenURL();
  kdDebug(38000) << "URL=" << url.url() << endl;
}

void InsertImageTool::deactivate()
{
}

void InsertImageTool::processEvent(QEvent *e)
{
  KontourDocument *doc = static_cast<KontourDocument *>(toolController()->view()->koDocument());
//  GPage *page = toolController()->view()->activeDocument()->activePage();
  Canvas *canvas = toolController()->view()->canvas();

  if(!doc->isReadWrite())
    return;

  if(e->type() == QEvent::MouseButtonPress)
  {
  }
  else if(e->type() == QEvent::MouseMove)
  {
  }
  else if(e->type() == QEvent::MouseButtonRelease)
  {
    GImage *img = new GImage(url);
    double zoom = toolController()->view()->activeDocument()->zoomFactor();
    QWMatrix m;
    m = m.translate((static_cast<QMouseEvent *>(e)->x() - canvas->xOffset()) / zoom, (static_cast<QMouseEvent *>(e)->y() - canvas->yOffset()) / zoom);
    img->transform(m);
    CreateImageCmd *cmd = new CreateImageCmd(toolController()->view()->activeDocument(), img);
    doc->history()->addCommand(cmd);
  }
  else if(e->type() == QEvent::KeyPress)
  {
  }
}

#include "InsertImageTool.moc"

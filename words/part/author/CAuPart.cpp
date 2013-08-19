/* This file is part of the KDE project
   Copyright (C) 2012 C. Boemann <cbo@kogmbh.com>
   Copyright (C) 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "CAuPart.h"

#include "CAuView.h"
#include "CAuFactory.h"

#include <KoShapeRegistry.h>
#include <KoShapeManager.h>

#include <kmessagebox.h>

CAuPart::CAuPart(const KComponentData &componentData, QObject *parent)
    : KWPart(componentData, parent)
{
    setComponentData(componentData);
    setTemplateType("words_template");
}


KoView *CAuPart::createViewInstance(KoDocument *document, QWidget *parent)
{
    CAuView *view = new CAuView(this, qobject_cast<KWDocument*>(document), parent);
    setupViewInstance(document, view);
    return view;
}

void CAuPart::showStartUpWidget(KoMainWindow *parent, bool alwaysShow)
{
    // print error if kotext not available
    if (KoShapeRegistry::instance()->value(TextShape_SHAPEID) == 0)
        // need to wait 1 event since exiting here would not work.
        QTimer::singleShot(0, this, SLOT(showErrorAndDie()));
    else
        KoPart::showStartUpWidget(parent, alwaysShow);
}

void CAuPart::showErrorAndDie()
{
    KMessageBox::error(0,
                       i18n("Can not find needed text component, Author will quit now"),
                       i18n("Installation Error"));
    QCoreApplication::exit(10);
}

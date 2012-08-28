/* This file is part of the KDE project
 * Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
 * Copyright (C) 2001-2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2002-2006 Laurent Montel <montel@kde.org>
 * Copyright (C) 2002 Werner Trobin <trobin@kde.org>
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2002 Stephan Kulow <coolo@kde.org>
 * Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * Copyright (C) 2003 Thomas Nagy <tnagyemail-mail@yahoo.fr>
 * Copyright (C) 2003,2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2004 Brad Hards <bradh@frogmouth.net>
 * Copyright (C) 2004-2006 Peter Simonsson <psn@linux.se>
 * Copyright (C) 2004-2005 Fredrik Edemar <f_edemar@linux.se>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005 Sven Langkamp <sven.langkamp@gmail.com>
 * Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2005 Johannes Schaub <johannes.schaub@kdemail.net>
 * Copyright (C) 2006 Gabor Lehel <illissius@gmail.com>
 * Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * Copyright (C) 2006 Jaison Lee <lee.jaison@gmail.com>
 * Copyright (C) 2006,2012 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 * Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KarbonPart.h"

#include "KarbonView.h"
#include "KarbonDocument.h"
#include "KarbonKoDocument.h"
#include "KarbonFactory.h"
#include "KarbonCanvas.h"

#include <KoCanvasResourceManager.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoInteractionTool.h>
#include <KoShapeRegistry.h>
#include <KoShapeManager.h>

#include <kglobal.h>
#include <kconfiggroup.h>
#include <KMessageBox>

KarbonPart::KarbonPart(QObject *parent)
    : KoPart(parent)
{
    setTemplateType("karbon_template");
    setComponentData(KarbonFactory::componentData(), false);
}

KarbonPart::~KarbonPart()
{
}

void KarbonPart::setDocument(KoDocument *document)
{
    KoPart::setDocument(document);
    KarbonKoDocument *doc = qobject_cast<KarbonKoDocument*>(document);
    connect(doc, SIGNAL(applyCanvasConfiguration(KarbonCanvas*)), SLOT(applyCanvasConfiguration(KarbonCanvas*)));
}

KoView * KarbonPart::createViewInstance(QWidget *parent)
{
    KarbonKoDocument *doc = qobject_cast<KarbonKoDocument*>(document());

    KarbonView *result = new KarbonView(this, doc, parent);

    KoCanvasResourceManager * provider = result->canvasWidget()->resourceManager();
    provider->setResource(KoCanvasResourceManager::PageSize, doc->document().pageSize());

    applyCanvasConfiguration(result->canvasWidget());

    return result;
}

void KarbonPart::openTemplate(const KUrl& url)
{
    KoPart::openTemplate(url);

    // explicitly set the output mimetype to our native mimetype
    // so that autosaving works for not yet saved templates as well
    if (document()->outputMimeType().isEmpty()) {
        document()->setOutputMimeType("application/vnd.oasis.opendocument.graphics");
    }
}


void KarbonPart::applyCanvasConfiguration(KarbonCanvas *canvas)
{
    KSharedConfigPtr config = componentData().config();

    QColor color(Qt::white);
    if (config->hasGroup("Interface")) {
        color = config->group("Interface").readEntry("CanvasColor", color);
    }
    canvas->setBackgroundColor(color);
}


#include "KarbonPart.moc"

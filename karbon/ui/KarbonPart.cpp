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
#include "ProxyView.h"
#include "KarbonDocument.h"
#include "KarbonFactory.h"
#include "KarbonPaletteBarWidget.h"

#include <KoCanvasResourceManager.h>
#include <KoCanvasBase.h>
#include <KoComponentData.h>

#include <kconfiggroup.h>

#include <QVBoxLayout>

KarbonPart::KarbonPart(QObject *parent)
    : KoPart(KarbonFactory::global(), parent)
{
    setTemplatesResourcePath(QLatin1String("karbon/templates/"));
}

KarbonPart::~KarbonPart()
{
}

KoView * KarbonPart::createViewInstance(KoDocument *_document, QWidget *parent)
{
    KarbonDocument *doc = qobject_cast<KarbonDocument*>(_document);
    ProxyView *view = new ProxyView(this, doc, parent);

    KarbonView *result = new KarbonView(this, doc, view);
    view->view = result;

    // Add the color bar below the karbon view (the reason for the ProxyView)
    QVBoxLayout *layout = new QVBoxLayout(view);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(result);
    layout->addWidget(result->colorBar());

    connect(doc, SIGNAL(replaceActivePage(KoPAPageBase*,KoPAPageBase*)), result, SLOT(replaceActivePage(KoPAPageBase*,KoPAPageBase*)));

    return view;
}

KoMainWindow *KarbonPart::createMainWindow()
{
    return new KoMainWindow(KARBON_MIME_TYPE, componentData());
}

void KarbonPart::openTemplate(const QUrl& url)
{
    KoPart::openTemplate(url);

    // explicitly set the output mimetype to our native mimetype
    // so that autosaving works for not yet saved templates as well
    if (document()->outputMimeType().isEmpty()) {
        document()->setOutputMimeType("application/vnd.oasis.opendocument.graphics");
    }
}

/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001-2002 Lennart Kudling <kudling@kde.org>
 * SPDX-FileCopyrightText: 2001-2007 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2002-2006 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2002 Werner Trobin <trobin@kde.org>
 * SPDX-FileCopyrightText: 2002-2006 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2002 Stephan Kulow <coolo@kde.org>
 * SPDX-FileCopyrightText: 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * SPDX-FileCopyrightText: 2003 Thomas Nagy <tnagyemail-mail@yahoo.fr>
 * SPDX-FileCopyrightText: 2003, 2006 Dirk Mueller <mueller@kde.org>
 * SPDX-FileCopyrightText: 2004 Brad Hards <bradh@frogmouth.net>
 * SPDX-FileCopyrightText: 2004-2006 Peter Simonsson <psn@linux.se>
 * SPDX-FileCopyrightText: 2004-2005 Fredrik Edemar <f_edemar@linux.se>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005 Sven Langkamp <sven.langkamp@gmail.com>
 * SPDX-FileCopyrightText: 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2005-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2005 Johannes Schaub <johannes.schaub@kdemail.net>
 * SPDX-FileCopyrightText: 2006 Gabor Lehel <illissius@gmail.com>
 * SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * SPDX-FileCopyrightText: 2006 Jaison Lee <lee.jaison@gmail.com>
 * SPDX-FileCopyrightText: 2006, 2012 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 * SPDX-FileCopyrightText: 2007 Matthias Kretz <kretz@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonPart.h"

#include "KarbonDocument.h"
#include "KarbonFactory.h"
#include "KarbonPaletteBarWidget.h"
#include "KarbonView.h"
#include "ProxyView.h"

#include <KConfigGroup>
#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoComponentData.h>
#include <KoPAPageBase.h>

#include <QVBoxLayout>

KarbonPart::KarbonPart(QObject *parent)
    : KoPart(KarbonFactory::global(), parent)
{
    setTemplatesResourcePath(QLatin1String("karbon/templates/"));
}

KarbonPart::~KarbonPart() = default;

KoView *KarbonPart::createViewInstance(KoDocument *_document, QWidget *parent)
{
    KarbonDocument *doc = qobject_cast<KarbonDocument *>(_document);
    ProxyView *view = new ProxyView(this, doc, parent);

    KarbonView *result = new KarbonView(this, doc, view);
    view->view = result;

    // Add the color bar below the karbon view (the reason for the ProxyView)
    QVBoxLayout *layout = new QVBoxLayout(view);
    layout->setContentsMargins({});
    layout->setSpacing(0);
    layout->addWidget(result);
    layout->addWidget(result->colorBar());

    connect(doc, &KoPADocument::replaceActivePage, result, &KarbonView::replaceActivePage);

    return view;
}

KoMainWindow *KarbonPart::createMainWindow()
{
    return new KoMainWindow(KARBON_MIME_TYPE, componentData());
}

void KarbonPart::openTemplate(const QUrl &url)
{
    KoPart::openTemplate(url);

    // explicitly set the output mimetype to our native mimetype
    // so that autosaving works for not yet saved templates as well
    if (document()->outputMimeType().isEmpty()) {
        document()->setOutputMimeType("application/vnd.oasis.opendocument.graphics");
    }
}

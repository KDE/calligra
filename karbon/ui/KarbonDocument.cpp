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
 * Copyright (C) 2005-2013 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2005 Johannes Schaub <johannes.schaub@kdemail.net>
 * Copyright (C) 2006 Gabor Lehel <illissius@gmail.com>
 * Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * Copyright (C) 2006 Jaison Lee <lee.jaison@gmail.com>
 * Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
 * Copyright (C) 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 * Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
 * Copyright (C) 2012 Yue Liu <yue.liu@mail.com>
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

#include "KarbonDocument.h"
#include "KarbonPart.h"
#include "KarbonFactory.h"
#include "KarbonView.h"
#include "KarbonUiDebug.h"

#include <KoImageCollection.h>
#include <KoText.h>
#include <KoSelection.h>
#include <KoStyleManager.h>
#include <KoTextSharedLoadingData.h>
#include <KoOdfStylesReader.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoGridData.h>
#include <KoGuidesData.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoGenStyles.h>
#include <KoOasisSettings.h>
#include <KoMainWindow.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>
#include <KoCanvasResourceManager.h>
#include <KoDocumentResourceManager.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoStoreDevice.h>
#include <KoStore.h>
#include <KoUnit.h>
#include <KoShapePainter.h>
#include <SvgShapeFactory.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <kundo2stack.h>

#include <QLocale>
#include <QRectF>
#include <QPainter>

// Make sure an appropriate DTD is available in www/calligra/DTD if changing this value
// static const char * CURRENT_DTD_VERSION = "1.2";

class Q_DECL_HIDDEN KarbonDocument::Private
{
public:
    Private()
            : showStatusBar(true),
              merge(false),
              maxRecentFiles(10)
    {}

    // KarbonDocument document;  ///< store non-visual doc info

    bool showStatusBar;       ///< enable/disable status bar in attached view(s)
    bool merge;
    uint maxRecentFiles;      ///< max. number of files shown in open recent menu item
};


KarbonDocument::KarbonDocument(KarbonPart* part)
    : KoPADocument(part)
    , d(new Private())
{
    Q_ASSERT(part);

    initConfig();

    SvgShapeFactory::addToRegistry();

    // set as default paper
    KoPageLayout pl = pageLayout();
    pl.format = KoPageFormat::defaultFormat();
    pl.orientation = KoPageFormat::Portrait;
    pl.width = MM_TO_POINT(KoPageFormat::width(pl.format, pl.orientation));
    pl.height = MM_TO_POINT(KoPageFormat::height(pl.format, pl.orientation));
    setPageLayout(pl);
}

KarbonDocument::~KarbonDocument()
{
    delete d;
}

void KarbonDocument::slotDocumentRestored()
{
    setModified(false);
}

bool KarbonDocument::showStatusBar() const
{
    return d->showStatusBar;
}

void KarbonDocument::setShowStatusBar(bool b)
{
    d->showStatusBar = b;
}

uint KarbonDocument::maxRecentFiles() const
{
    return d->maxRecentFiles;
}

void KarbonDocument::reorganizeGUI()
{
    foreach(KoView* view, documentPart()->views()) {
        KarbonView * kv = qobject_cast<KarbonView*>(view);
        if (kv) {
            kv->reorganizeGUI();
        }
    }
}

void KarbonDocument::initConfig()
{
    KSharedConfigPtr config = KarbonFactory::karbonConfig();

    // disable grid by default
    gridData().setShowGrid(false);

    if (config->hasGroup("Interface")) {
        KConfigGroup interfaceGroup = config->group("Interface");
        setAutoSave(interfaceGroup.readEntry("AutoSave", defaultAutoSave() / 60) * 60);
        d->maxRecentFiles = interfaceGroup.readEntry("NbRecentFile", 10);
        setShowStatusBar(interfaceGroup.readEntry("ShowStatusBar" , true));
        setBackupFile(interfaceGroup.readEntry("BackupFile", true));
    }
    int undos = 30;

    QString defaultUnitSymbol =
        QLatin1String((QLocale().measurementSystem() == QLocale::ImperialSystem)?"in":"cm");

    if (config->hasGroup("Misc")) {
        KConfigGroup miscGroup = config->group("Misc");
        undos = miscGroup.readEntry("UndoRedo", -1);
        defaultUnitSymbol = miscGroup.readEntry("Units", defaultUnitSymbol);
    }
    undoStack()->setUndoLimit(undos);
    setUnit(KoUnit::fromSymbol(defaultUnitSymbol));

}

bool KarbonDocument::mergeNativeFormat(const QString &file)
{
    d->merge = true;
    bool result = loadNativeFormat(file);
    if (!result)
        showLoadingErrorDialog();
    d->merge = false;
    return result;
}

KoOdf::DocumentType KarbonDocument::documentType() const
{
    return KoOdf::Graphics;
}

const char * KarbonDocument::odfTagName(bool withNamespace)
{
    return withNamespace ? "office:drawing": "drawing";
}

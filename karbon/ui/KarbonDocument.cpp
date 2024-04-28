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
 * SPDX-FileCopyrightText: 2005-2013 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2005 Johannes Schaub <johannes.schaub@kdemail.net>
 * SPDX-FileCopyrightText: 2006 Gabor Lehel <illissius@gmail.com>
 * SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * SPDX-FileCopyrightText: 2006 Jaison Lee <lee.jaison@gmail.com>
 * SPDX-FileCopyrightText: 2006 Casper Boemann <cbr@boemann.dk>
 * SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 * SPDX-FileCopyrightText: 2007 Matthias Kretz <kretz@kde.org>
 * SPDX-FileCopyrightText: 2012 Yue Liu <yue.liu@mail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonDocument.h"
#include "KarbonFactory.h"
#include "KarbonPart.h"
#include "KarbonUiDebug.h"
#include "KarbonView.h"

#include <KoCanvasController.h>
#include <KoCanvasResourceManager.h>
#include <KoDocumentResourceManager.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoGenStyles.h>
#include <KoGridData.h>
#include <KoGuidesData.h>
#include <KoImageCollection.h>
#include <KoMainWindow.h>
#include <KoOasisSettings.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfStylesReader.h>
#include <KoOdfWriteStore.h>
#include <KoPageLayout.h>
#include <KoSelection.h>
#include <KoShapeLayer.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeManager.h>
#include <KoShapePainter.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoStyleManager.h>
#include <KoText.h>
#include <KoTextSharedLoadingData.h>
#include <KoToolManager.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <SvgShapeFactory.h>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <kundo2stack.h>

#include <QLocale>
#include <QPainter>
#include <QRectF>

// Make sure an appropriate DTD is available in www/calligra/DTD if changing this value
// static const char * CURRENT_DTD_VERSION = "1.2";

class Q_DECL_HIDDEN KarbonDocument::Private
{
public:
    Private()
        : showStatusBar(true)
        , merge(false)
        , maxRecentFiles(10)
    {
    }

    // KarbonDocument document;  ///< store non-visual doc info

    bool showStatusBar; ///< enable/disable status bar in attached view(s)
    bool merge;
    uint maxRecentFiles; ///< max. number of files shown in open recent menu item
};

KarbonDocument::KarbonDocument(KarbonPart *part)
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
    foreach (KoView *view, documentPart()->views()) {
        KarbonView *kv = qobject_cast<KarbonView *>(view);
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
        setShowStatusBar(interfaceGroup.readEntry("ShowStatusBar", true));
        setBackupFile(interfaceGroup.readEntry("BackupFile", true));
    }
    int undos = 30;

    QString defaultUnitSymbol = QLatin1String((QLocale().measurementSystem() == QLocale::ImperialSystem) ? "in" : "cm");

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

const char *KarbonDocument::odfTagName(bool withNamespace)
{
    return withNamespace ? "office:drawing" : "drawing";
}

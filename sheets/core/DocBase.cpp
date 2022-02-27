/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2016 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Phillip Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "DocBase.h"

#include <QAbstractItemModel>
#include <QDomDocument>
#include <QPointer>
#include <QUrl>

#include <KoCanvasResourceIdentities.h>
#include <KoDocumentResourceManager.h>

#include "engine/ElapsedTime_p.h"
#include "engine/CalculationSettings.h"
#include "Map.h"

#include "odf/SheetsOdf.h"
#include "ksp/SheetsKsp.h"

namespace Calligra {
namespace Sheets {

class Q_DECL_HIDDEN DocBase::Private
{
public:
    Map *map;
    static QList<DocBase*> s_docs;
    static int s_docId;

    // document properties
    bool configLoadFromFile       : 1;
    QStringList spellListIgnoreAll;
    KoDocumentResourceManager *resourceManager;
};

} // namespace Sheets
} // namespace Calligra


using namespace Calligra::Sheets;

QList<DocBase*> DocBase::Private::s_docs;
int DocBase::Private::s_docId = 0;

Q_DECLARE_METATYPE(QPointer<QAbstractItemModel>)

DocBase::DocBase(KoPart *part)
    : KoDocument(part)
    , d(new Private)
{
    Q_ASSERT(part);
    d->resourceManager = new KoDocumentResourceManager();
    d->map = new Map(this, CURRENT_SYNTAX_VERSION);

    d->resourceManager->setUndoStack(undoStack());
    QVariant variant;
    variant.setValue<void*>(d->map->sheetAccessModel());
    d->resourceManager->setResource(::Sheets::CanvasResource::AccessModel, variant);

    // Document Url for FILENAME function and page header/footer.
    d->map->calculationSettings()->setFileName(url().toDisplayString());

    d->configLoadFromFile = false;

    documents().append(this);
}

DocBase::~DocBase()
{
    delete d->map;
    delete d->resourceManager;
    delete d;
}

QList<DocBase*> DocBase::documents()
{
    return Private::s_docs;
}

void DocBase::setReadWrite(bool readwrite)
{
    map()->setReadWrite(readwrite);
    KoDocument::setReadWrite(readwrite);
}

Map *DocBase::map() const
{
    return d->map;
}

KoDocumentResourceManager* DocBase::resourceManager() const
{
    return d->resourceManager;
}

void DocBase::initConfig()
{
}

QStringList DocBase::spellListIgnoreAll() const {
    return d->spellListIgnoreAll;
}

void DocBase::setSpellListIgnoreAll(const QStringList &list) {
    d->spellListIgnoreAll = list;
}

bool DocBase::saveOdf(SavingContext &documentContext)
{
    ElapsedTime et("OpenDocument Saving", ElapsedTime::PrintOnlyTime);
    return Odf::saveDocument(this, documentContext);
}

bool DocBase::loadOdf(KoOdfReadStore & odfStore)
{
    return Odf::loadDocument(this, odfStore);
}

void DocBase::paintContent(QPainter &, const QRect &)
{
}

bool DocBase::loadXML(const KoXmlDocument &doc, KoStore *)
{
    return Ksp::loadDoc (this, doc);
}

QDomDocument DocBase::saveXML()
{
    return Ksp::saveDoc(this);
}


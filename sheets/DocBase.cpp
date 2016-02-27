/* This file is part of the KDE project
   Copyright 2016 Tomas Mecir <mecirt@gmail.com>
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/
#include "DocBase.h"
#include "DocBase_p.h"

#include <KoDocumentResourceManager.h>
#include <KoShapeRegistry.h>
#include <KoPart.h>

#include "calligra_sheets_limits.h"
#include "CalculationSettings.h"
#include "Map.h"
#include "SheetAccessModel.h"

#include "ElapsedTime_p.h"
#include "odf/SheetsOdf.h"

#include "part/View.h" // TODO: get rid of this dependency

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

    // Document Url for FILENAME function and page header/footer.
    d->map->calculationSettings()->setFileName(url().toDisplayString());

    KoShapeRegistry *registry = KoShapeRegistry::instance();
    foreach (const QString &id, registry->keys()) {
        KoShapeFactoryBase *shapeFactory = registry->value(id);
        shapeFactory->newDocumentResourceManager(d->resourceManager);
    }

    d->configLoadFromFile = false;

    documents().append(this);

    d->sheetAccessModel = new SheetAccessModel(d->map);
}

DocBase::~DocBase()
{
    delete d->map;
    delete d->sheetAccessModel;
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

int DocBase::syntaxVersion() const
{
    return d->map->syntaxVersion();
}

KoDocumentResourceManager* DocBase::resourceManager() const
{
    return d->resourceManager;
}

SheetAccessModel *DocBase::sheetAccessModel() const
{
    return d->sheetAccessModel;
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

bool DocBase::loadXML(const KoXmlDocument &, KoStore *)
{
    return false;
}


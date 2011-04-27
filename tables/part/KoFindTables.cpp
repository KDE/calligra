/* This file is part of the KDE project
 *
 * Copyright (c) 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include "KoFindTables.h"

#include <KoFindOptionSet.h>
#include <tables/Sheet.h>
#include <CellStorage.h>
#include <KoFindOption.h>

using namespace Calligra::Tables;

class KoFindTables::Private
{
public:
    Sheet* currentSheet;
};

KoFindTables::KoFindTables ( QObject* parent )
    : KoFindBase ( parent ), d(new Private)
{
    KoFindOptionSet *options = new KoFindOptionSet();
    options->addOption("caseSensitive", i18n("Case Sensitive"), i18n("Match cases when searching"), QVariant::fromValue<bool>(false));
    setOptions(options);
}

void KoFindTables::setCurrentSheet ( Sheet* sheet )
{
    d->currentSheet = sheet;
}

void KoFindTables::replaceImplementation ( const KoFindMatch& match, const QVariant& value )
{

}

void KoFindTables::findImplementation ( const QString& pattern, KoFindBase::KoFindMatchList& matchList )
{
    Cell cell = d->currentSheet->cellStorage()->firstInRow(0);
    Qt::CaseSensitivity sensitivity = options()->option("caseSensitive")->value().toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    int column = 0;
    while(!cell.isNull()) {
        if(cell.userInput().contains(pattern, sensitivity)) {
            KoFindMatch match;
            match.setContainer(QVariant::fromValue(d->currentSheet));
            match.setLocation(QVariant::fromValue(cell));
            matchList.append(match);
        }
        cell = d->currentSheet->cellStorage()->nextInRow(++column, 0);
    }
}


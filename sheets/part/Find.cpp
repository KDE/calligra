/* This file is part of the KDE project
 *
 * Copyright (c) 2011 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include "Find.h"

#include <QApplication>

#include <KoFindOptionSet.h>
#include <KoFindOption.h>

#include "Sheet.h"
#include "Value.h"
#include "ValueStorage.h"
#include "ui/SheetView.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Find::Private
{
public:
    Private() : currentSheet(0), currentSheetView(0) { }
    
    Sheet *currentSheet;
    SheetView *currentSheetView;
};

Find::Find(QObject *parent)
    : KoFindBase(parent), d(new Private)
{
    KoFindOptionSet *options = new KoFindOptionSet();
    options->addOption("caseSensitive", i18n("Case Sensitive"), i18n("Match cases when searching"), QVariant::fromValue<bool>(false));
    setOptions(options);
    
    connect(this, SIGNAL(matchFound(KoFindMatch)), SLOT(setActiveMatch(KoFindMatch)));
}

void Find::setCurrentSheet( Sheet* sheet, SheetView* view)
{
    if(d->currentSheetView) {
        clearMatches();
    }
    
    d->currentSheet = sheet;
    d->currentSheetView = view;
}

void Find::replaceImplementation(const KoFindMatch &match, const QVariant &value)
{
    //No implementation yet.
    Q_UNUSED(match);
    Q_UNUSED(value);
}

void Find::findImplementation(const QString &pattern, KoFindBase::KoFindMatchList &matchList)
{
    //int row = 1;
    //int column = 1;

    const ValueStorage *values = d->currentSheet->valueStorage();
    Qt::CaseSensitivity sensitivity = options()->option("caseSensitive")->value().toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    for(int i = 0; i < values->count(); ++i) {
        Value val = values->data(i);
        if(val.isString() && val.asString().contains(pattern, sensitivity)) {
            KoFindMatch match;
            match.setContainer(QVariant::fromValue(d->currentSheet));
            Cell cell(d->currentSheet, values->col(i), values->row(i));
            match.setLocation(QVariant::fromValue(cell));
            matchList.append(match);
            d->currentSheetView->setHighlighted(cell.cellPosition(), true);
        }
    }
}

void Find::clearMatches()
{
    KoFindMatchList list = matches();
    foreach(const KoFindMatch &match, list) {
        d->currentSheetView->setHighlighted(match.location().value<Cell>().cellPosition(), false);
    }
}

void Find::setActiveMatch ( const KoFindMatch& match )
{
    d->currentSheetView->setActiveHighlight(match.location().value<Cell>().cellPosition());
}

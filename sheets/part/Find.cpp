/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Find.h"

#include <KoFindOption.h>
#include <KoFindOptionSet.h>

#include <KLocalizedString>

#include "core/Cell.h"
#include "core/Sheet.h"
#include "engine/ValueStorage.h"
#include "ui/SheetView.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Find::Private
{
public:
    Private()
        : currentSheet(nullptr)
        , currentSheetView(nullptr)
    {
    }

    Sheet *currentSheet;
    SheetView *currentSheetView;
};

Find::Find(QObject *parent)
    : KoFindBase(parent)
    , d(new Private)
{
    KoFindOptionSet *options = new KoFindOptionSet();
    options->addOption("caseSensitive", i18n("Case Sensitive"), i18n("Match cases when searching"), QVariant::fromValue<bool>(false));
    setOptions(options);

    connect(this, &KoFindBase::matchFound, this, &Find::setActiveMatch);
}

void Find::setCurrentSheet(Sheet *sheet, SheetView *view)
{
    if (d->currentSheetView) {
        clearMatches();
    }

    d->currentSheet = sheet;
    d->currentSheetView = view;
}

void Find::replaceImplementation(const KoFindMatch &match, const QVariant &value)
{
    // No implementation yet.
    Q_UNUSED(match);
    Q_UNUSED(value);
}

void Find::findImplementation(const QString &pattern, KoFindBase::KoFindMatchList &matchList)
{
    // int row = 1;
    // int column = 1;

    const ValueStorage *values = d->currentSheet->valueStorage();
    Qt::CaseSensitivity sensitivity = options()->option("caseSensitive")->value().toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    for (int i = 0; i < values->count(); ++i) {
        Value val = values->data(i);
        if (val.isString() && val.asString().contains(pattern, sensitivity)) {
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
    foreach (const KoFindMatch &match, list) {
        d->currentSheetView->setHighlighted(match.location().value<Cell>().cellPosition(), false);
    }
}

void Find::setActiveMatch(const KoFindMatch &match)
{
    d->currentSheetView->setActiveHighlight(match.location().value<Cell>().cellPosition());
}

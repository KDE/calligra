/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CALLIGRA_SHEETS_FIND_H
#define CALLIGRA_SHEETS_FIND_H

#include "sheets_part_export.h"

#include <KoFindBase.h>

namespace Calligra
{
namespace Sheets
{

class Sheet;
class SheetView;
/**
 * Searching implementation for searching through spreadsheets.
 *
 * This class implements a KoFind-backend for searching in spreadsheets.
 *
 * Matches found by this class will use Calligra::Sheets::Sheet* as container
 * and Calligra::Sheets::Cell as location.
 *
 * TODO: Add support for searching in notes/comments.
 * TODO: Support searching through all Sheets in a document.
 * TODO: Search within the displayed text or the user input.
 */
class CALLIGRA_SHEETS_PART_EXPORT Find : public KoFindBase
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    explicit Find(QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * Set the current active sheet.
     *
     * Currently this class only searches within the active sheet.
     */
    void setCurrentSheet(Sheet *sheet, SheetView *view);

protected:
    /**
     * Overridden from KoFindBase
     */
    void replaceImplementation(const KoFindMatch &match, const QVariant &value) override;
    /**
     * Overridden from KoFindBase
     */
    void findImplementation(const QString &pattern, KoFindBase::KoFindMatchList &matchList) override;

    /**
     * Overridden from KoFindBase
     */
    void clearMatches() override;

private:
    class Private;
    Private *const d;

private Q_SLOTS:
    void setActiveMatch(const KoFindMatch &match);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FIND_H

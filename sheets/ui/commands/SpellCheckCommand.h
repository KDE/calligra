/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SPELLCHECK_COMMAND
#define CALLIGRA_SHEETS_SPELLCHECK_COMMAND

#include <sonnet/backgroundchecker.h>

class KoCanvasBase;

namespace Calligra
{
namespace Sheets
{
class Region;

/**
 * \ingroup Commands
 * \brief Spellchecks a cell range.
 */
class SpellCheckCommand : public Sonnet::BackgroundChecker
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    SpellCheckCommand(const Region &region, KoCanvasBase* canvasBase);

    /**
     * Destructor.
     */
    ~SpellCheckCommand() override;

protected:
    QString fetchMoreText() override;
    void finishedCurrentFeed() override;

private Q_SLOTS:
    void finishCommand();

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SPELLCHECK_COMMAND

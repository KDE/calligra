/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CELL_TOOL
#define CALLIGRA_SHEETS_CELL_TOOL

#include <ui/CellToolBase.h>

#include "sheets_part_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * The tool to change cell ranges.
 */
class CALLIGRA_SHEETS_PART_EXPORT CellTool : public CellToolBase
{
    Q_OBJECT

public:
    explicit CellTool(KoCanvasBase *canvas);
    ~CellTool() override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;
    SheetView *sheetView(Sheet *sheet) const override;

public Q_SLOTS:
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;

protected:
    Selection *selection() override;
    QPointF offset() const override;
    QSizeF size() const override;
    QPointF canvasOffset() const override;
    int maxCol() const override;
    int maxRow() const override;

protected Q_SLOTS:
    // -- misc actions --
    void definePrintRange();

private:
    Q_DISABLE_COPY(CellTool)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_TOOL

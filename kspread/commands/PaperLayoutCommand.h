/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

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

#ifndef KSPREAD_PAPER_LAYOUT_COMMAND
#define KSPREAD_PAPER_LAYOUT_COMMAND

#include <QRect>
#include <QString>
#include <QUndoCommand>

#include <KoPageLayout.h>
#include <KoUnit.h>

/**
 * The KSpread namespace.
 */
namespace KSpread
{
class Doc;
class Sheet;

class PaperLayoutCommand : public QUndoCommand
{
public:
  explicit PaperLayoutCommand( Sheet* sheet );

  virtual void redo();
  virtual void undo();
  virtual QString name() const;

protected:
    Doc* doc;
    QString sheetName;
    KoPageLayout pl;
    KoPageLayout plRedo;
    KoHeadFoot hf;
    KoHeadFoot hfRedo;
    KoUnit unit;
    KoUnit unitRedo;
    bool printGrid;
    bool printGridRedo;
    bool printCommentIndicator;
    bool printCommentIndicatorRedo;
    bool printFormulaIndicator;
    bool printFormulaIndicatorRedo;
    QRect printRange;
    QRect printRangeRedo;
    QPair<int, int> printRepeatColumns;
    QPair<int, int> printRepeatColumnsRedo;
    QPair<int, int> printRepeatRows;
    QPair<int, int> printRepeatRowsRedo;
    double zoom;
    double zoomRedo;
    int pageLimitX;
    int pageLimitXRedo;
    int pageLimitY;
    int pageLimitYRedo;

};

} // namespace KSpread

#endif // KSPREAD_PAPER_LAYOUT_COMMAND

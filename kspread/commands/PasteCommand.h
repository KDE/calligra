/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_PASTE_COMMAND
#define KSPREAD_PASTE_COMMAND

#include "AbstractRegionCommand.h"

#include <KoXmlReader.h>

#include "Global.h"

class QMimeData;
class KoXmlDocument;

namespace KSpread
{

/**
 * Command to paste cell data.
 */
class PasteCommand : public AbstractRegionCommand
{
public:
    PasteCommand(QUndoCommand *parent = 0);
    virtual ~PasteCommand();

    /**
     * Modes used for paste with insertion.
     */
    enum InsertionMode {
        NoInsertion,     ///< No cell insertion.
        ShiftCells,      ///< Shift cells; determine the direction from the data.
        ShiftCellsRight, ///< Shift cells to the right.
        ShiftCellsDown   ///< Shift cells to the bottom.
    };

    bool setMimeData(const QMimeData *mimeData);
    void setInsertionMode(InsertionMode mode);
    void setMode(Paste::Mode mode);
    void setOperation(Paste::Operation operation);
    void setPasteFC(bool force);

    virtual bool isApproved() const;

    /**
     * \param mimeData the MIME data to check
     * \return \c true , if the MIME type is supported.
     */
    static bool supports(const QMimeData *mimeData);

    /**
     * Checks wether the clipboard data contains columns or rows.
     * Used to decide wether the paste with insertion dialog, where
     * you can choose between shifting cells down or to the right,
     * has to be shown.
     * \param mimeData the MIME data to check
     * \return \c true if the shifting direction is not defined
     * \return \c false if the clipboard data contains a column or a row
     */
    static bool unknownShiftDirection(const QMimeData *mimeData);

protected:
    virtual bool preProcessing();
    virtual bool mainProcessing();
    virtual bool postProcessing();

    /**
     * Creates sub-commands for the region \p element by parsing XML \p data.
     */
    bool processXmlData(Element *element, KoXmlDocument *data);

    /**
     * Creates sub-commands for the region \p element by parsing plain text.
     */
    bool processTextPlain(Element *element);

private:
    const QMimeData *   m_mimeData;
    KoXmlDocument *     m_xmlDocument;
    InsertionMode       m_insertMode;
    Paste::Mode         m_pasteMode;
    Paste::Operation    m_operation;
    bool                m_pasteFC;
};

} // namespace KSpread

#endif // KSPREAD_PASTE_COMMAND

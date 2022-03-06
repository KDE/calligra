/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PASTE_COMMAND
#define CALLIGRA_SHEETS_PASTE_COMMAND

#include "AbstractRegionCommand.h"

#include <KoXmlReader.h>

#include "Global.h"

#include "sheets_common_export.h"

class QMimeData;
class KoXmlDocument;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Command to paste cell data.
 */
class CALLIGRA_SHEETS_COMMON_TEST_EXPORT PasteCommand : public AbstractRegionCommand
{
public:
    PasteCommand(KUndo2Command *parent = 0);
    ~PasteCommand() override;

    /**
     * Modes used for paste with insertion.
     */
    enum InsertionMode {
        NoInsertion,     ///< No cell insertion.
        ShiftCells,      ///< Shift cells; determine the direction from the data.
        ShiftCellsRight, ///< Shift cells to the right.
        ShiftCellsDown   ///< Shift cells to the bottom.
    };

    const QMimeData* mimeData() const;
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
     * Checks whether the clipboard data contains columns or rows.
     * Used to decide whether the paste with insertion dialog, where
     * you can choose between shifting cells down or to the right,
     * has to be shown.
     * \param mimeData the MIME data to check
     * \return \c true if the shifting direction is not defined
     * \return \c false if the clipboard data contains a column or a row
     */
    static bool unknownShiftDirection(const QMimeData *mimeData);

protected:
    bool preProcessing() override;
    bool mainProcessing() override;
    bool postProcessing() override;

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

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PASTE_COMMAND

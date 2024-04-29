/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PASTE_COMMAND
#define CALLIGRA_SHEETS_PASTE_COMMAND

#include "AbstractRegionCommand.h"

#include "../sheets_ui_export.h"

#include "core/Cell.h"

#include <QVector>

class QMimeData;
class KoXmlDocument;

namespace Calligra
{
namespace Sheets
{

class SheetBase;

/**
 * \ingroup Commands
 * \brief Command to paste cell data.
 */
class CALLIGRA_SHEETS_UI_EXPORT PasteCommand : public AbstractRegionCommand
{
public:
    PasteCommand(KUndo2Command *parent = nullptr);
    ~PasteCommand() override;

    bool setMimeData(const QMimeData *mimeData, bool sameApp);
    void setMode(Paste::Mode mode);
    void setOperation(Paste::Operation operation);
    void setPasteFC(bool force);
    void setSourceRegion(const Region &region);
    void setCutMode(bool cut);

    Region sourceRegion() const;

    static QRect adjustPasteArea(QRect sourceRect, QRect pasteArea);

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
     * \return \c true if the shifting direction is not defined
     * \return \c false if the clipboard data contains a column or a row
     */
    bool unknownShiftDirection();

protected:
    bool postProcess() override;
    bool performCommands() override;

    /**
     * Creates sub-commands for the region \p element. \p data.
     */
    bool processSnippetData(Element *element, QRect sourceRect, SheetBase *sourceSheet, bool isCut);

    /**
     * Creates sub-commands for the region \p element by parsing plain text.
     */
    bool processTextPlain(Element *element, const QStringList &data);

    void adjustTargetRegion();
    Region parseSnippet(const QMimeData *mimeData, bool *isCut);

private:
    bool m_haveSource;
    bool m_haveText;
    Region m_sourceRegion;
    QStringList m_text;
    Paste::Mode m_pasteMode;
    Paste::Operation m_operation;
    bool m_pasteFC;
    bool m_sameApp;
    bool m_isCut;
    QVector<AbstractRegionCommand *> m_nestedCommands;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PASTE_COMMAND

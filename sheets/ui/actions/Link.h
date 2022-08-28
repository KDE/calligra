/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_LINK
#define CALLIGRA_SHEETS_ACTION_LINK


#include "CellAction.h"

#include "ui/commands/AbstractRegionCommand.h"


namespace Calligra
{
namespace Sheets
{
class LinkDialog;

class Link : public CellAction {
Q_OBJECT
public:
    Link(Actions *actions);
    virtual ~Link();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    LinkDialog *m_dlg;
    Selection *m_selection;
};

class ClearLink : public CellAction {
Q_OBJECT
public:
    ClearLink(Actions *actions);
    virtual ~ClearLink();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};


/**
 * \ingroup Commands
 * \brief Adds/Removes a hyperlink.
 */
class LinkCommand : public AbstractRegionCommand
{
public:
    LinkCommand(const QString& text, const QString& link);
protected:
    bool process(Element* element) override;

    QString newText;
    QString newLink;
};



} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_LINK

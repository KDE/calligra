/* This file is part of the KDE project
   Copyright 2005,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_ABSTRACT_REGION_COMMAND
#define KSPREAD_ABSTRACT_REGION_COMMAND

#include <QString>
#include <QUndoCommand>

#include "Region.h"

namespace KSpread
{
class Sheet;

/**
 * \class AbstractRegionCommand
 * \brief Abstract base class for all region related operations.
 */
class AbstractRegionCommand : public Region, public QUndoCommand
{
public:
    /**
     * Constructor.
     */
    AbstractRegionCommand();

    /**
     * Destructor.
     */
    virtual ~AbstractRegionCommand();

    /**
     * \return the Sheet this AbstractRegionCommand works on
     */
    Sheet* sheet() const { return m_sheet; }

    /**
     * Sets \p sheet to be the Sheet to work on.
     */
    void setSheet( Sheet* sheet ) { m_sheet = sheet; }

    /**
     * Executes the actual operation and adds the manipulator to the undo history, if desired.
     * \return \c true if the command was executed successfully
     * \return \c false if the command fails or if the command was already executed once
     * \see setRegisterUndo
     */
    bool execute();

    /**
     * Executes the actual operation.
     */
    virtual void redo();

    /**
     * Executes the actual operation in reverse order.
     */
    virtual void undo();

    /**
     * Sets reverse mode to \b reverse .
     * \see redo
     * \see undo
     */
    virtual void setReverse( bool reverse ) { m_reverse = reverse; }

    /**
     * If \p registerUndo is \c true , this manipulator registers an
     * undo operation for the document.
     */
    void setRegisterUndo( bool registerUndo ) { m_register = registerUndo; }

    /**
     * Sets the name to \p name . The name is used for the undo/redo functionality.
     */
    virtual void setName( const QString& name ) { m_name = name; }

    /**
     * \return the manipulator's name
     */
    virtual QString name() const { return m_name; }

protected:
    /**
     * Processes \p element , a Region::Point or a Region::Range .
     * Invoked by mainProcessing() .
     */
    virtual bool process( Element* ) { return true; }

    /**
     * Preprocessing of the region.
     * Checks the protection of each cell in the region.
     * \return \c true if the sheet is not protected or all cells are not protected
     * \return \c false if any of the cells is protected
     */
    virtual bool preProcessing();

    /**
     * Processes the region. Calls process(Element*).
     */
    virtual bool mainProcessing();

    /**
     * Postprocessing of the region.
     */
    virtual bool postProcessing() { return true; }

protected:
    Sheet*  m_sheet;
    QString m_name;
    bool    m_reverse   : 1;
    bool    m_firstrun  : 1;
    bool    m_register  : 1;
    bool    m_success   : 1;
};

} // namespace KSpread

#endif // KSPREAD_ABSTRACT_REGION_COMMAND

/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_MANIPULATOR
#define KSPREAD_MANIPULATOR

#include <QLinkedList>
#include <QRect>
#include <QString>
#include <QUndoCommand>

#include <klocale.h>

#include "kspread_export.h"

#include "Region.h"
#include "Validity.h"

namespace KSpread
{
class Sheet;

/**
 * \class Manipulator
 * \brief Abstract base class for all region related operations.
 */
class Manipulator : public Region, public QUndoCommand
{
public:
    /**
     * Constructor.
     */
    Manipulator();

    /**
     * Destructor.
     */
    virtual ~Manipulator();

    /**
     * \return the Sheet this Manipulator works on
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

/**
 * The macro manipulator holds a set of manipulators and calls them all at once.
 * Each of the manipulators has its own range, MacroManipulator does not take
 * care of that.
 */
class KSPREAD_EXPORT MacroManipulator : public Manipulator {
  public:
    void redo ();
    void undo ();
    void add (Manipulator *manipulator);
  protected:
    QList<Manipulator *> manipulators;
};


/**
 * \class MergeManipulator
 * \brief Merges and splits the cells of a cell region.
 */
class MergeManipulator : public Manipulator
{
public:
  MergeManipulator();
  virtual ~MergeManipulator();

  virtual bool preProcessing();

  virtual void setReverse(bool reverse) { m_merge = !reverse; }
  void setHorizontalMerge(bool state) { m_mergeHorizontal = state; }
  void setVerticalMerge(bool state) { m_mergeVertical = state; }

protected:
  virtual bool process(Element*);

  virtual bool postProcessing();

  virtual QString name() const;

  bool m_merge;
private:
  bool m_mergeHorizontal : 1;
  bool m_mergeVertical   : 1;
  Manipulator* m_unmerger; // to restore old merging
};


/**
 * \class CommentManipulator
 * \brief Adds/Removes comments to/of a cell region.
 */
class CommentManipulator : public Manipulator
{
public:
    CommentManipulator();
    void setComment( const QString& comment ) { m_comment = comment; }

protected:
    virtual bool process(Element* element);
    virtual bool mainProcessing();
    virtual QString name() const;

private:
    QString m_comment;
    QList< QPair<QRectF, QString> > m_undoData;
};

/**
 * \class ConditionalManipulator
 * \brief Adds/Removes condtional formatting to/of a cell region.
 */
class ConditionalManipulator : public Manipulator
{
public:
    ConditionalManipulator();
    void setConditionList( const QLinkedList<Conditional>& list ) { m_conditions.setConditionList( list ); }

protected:
    virtual bool process(Element* element);
    virtual bool mainProcessing();
    virtual QString name() const;

private:
    Conditions m_conditions;
    QList< QPair<QRectF, Conditions> > m_undoData;
};

/**
 * \class ValidityManipulator
 * \brief Adds/Removes validity checks to/of a cell region.
 */
class ValidityManipulator : public Manipulator
{
public:
    ValidityManipulator();
    void setValidity( Validity validity ) { m_validity = validity; }

protected:
    virtual bool process(Element* element);
    virtual bool mainProcessing();
    virtual QString name() const;

private:
    Validity m_validity;
    QList< QPair<QRectF, Validity> > m_undoData;
};

} // namespace KSpread

#endif // KSPREAD_MANIPULATOR

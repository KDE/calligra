/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

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

#include <QHash>
#include <QLinkedList>
#include <QRect>
#include <QString>

#include <kcommand.h>
#include <klocale.h>

#include <koffice_export.h>

#include "Undo.h"
#include "Region.h"
#include "Validity.h"

namespace KSpread
{
class Cell;
class ColumnFormat;
class RowFormat;
class Sheet;

/**
 * \class Manipulator
 * \brief Abstract base class for all region related operations.
 */
class Manipulator : public Region, public KCommand
{
public:
  Manipulator();
  virtual ~Manipulator();

  /**
   * \return the Sheet this Manipulator works on
   */
  Sheet* sheet() const { return m_sheet; }
  /**
   * Sets \p sheet to be the Sheet to work on.
   */
  void setSheet(Sheet* sheet) { m_sheet = sheet; }

  /**
   * \return \c true if cells are created, if it is necessary
   */
  bool creation() { return m_creation; }
  /**
   * Sets cell creatioin for regions containing default cells to \p creation .
   */
  void setCreation(bool creation) { m_creation = creation; }

  /** Is this a formatting manipulator ? If so, execute will call
  process(Format*) for each complete row/column, instead of going
  cell-by-cell. True by default. */
  bool format() { return m_format; };
  void setFormat (bool f) { m_format = f; };

  /**
   * Executes the actual operation.
   */
  virtual void execute();
  /**
   * Executes the actual operation in reverse order.
   */
  virtual void unexecute();

  virtual void setArgument(const QString& /*arg*/, const QString& /*val*/) {};

  /**
   * Sets reverse mode to \b reverse .
   * \see execute
   * \see unexecute
   */
  virtual void setReverse(bool reverse) { m_reverse = reverse; }
  /**
   * If \p registerUndo is \c true , this manipulator registers an
   * undo operation for the document.
   */
  void setRegisterUndo(bool registerUndo) { m_register = registerUndo; }

  /** Set whether the manipulator will automatically bail out with an error
  message box, if the range is protected. */
  void setProtectedCheck (bool protcheck) { m_protcheck = protcheck; };
  
  /**
   * Sets the name to \p name . The name is used for the undo/redo
   * functionality.
   */
  virtual void setName (const QString& name) { m_name = name; }
  /**
   * \return the manipulator's name
   */
  virtual QString name() const { return m_name; };

protected:
  /**
   * Processes \p element , a Region::Point or a Region::Range .
   * Invoked by mainProcessing() .
   * Calls process(Cell*) or process(Format*) depending on \p element .
   */
  virtual bool process(Element* element);
  /**
   * Processes \p cell .
   */
  virtual bool process(Cell* cell) { Q_UNUSED(cell); return true; }
  /**
   * Processes \p format , a row or column format.
   */
  virtual bool process(Format* format) { Q_UNUSED(format); return true; }

  /**
   * Preprocessing the region.
   */
  virtual bool preProcessing() { return true; }
  /**
   * Processes the region. Calls process(Element*).
   */
  virtual bool mainProcessing();
  /**
   * Postprocessing the region.
   */
  virtual bool postProcessing() { return true; }


  Sheet* m_sheet;
  QString m_name;
  bool   m_creation  : 1;
  bool   m_reverse   : 1;
  bool   m_firstrun  : 1;
  bool   m_format    : 1;
  bool   m_register  : 1;
  bool   m_protcheck : 1;
private:
};

/**
 * The macro manipulator holds a set of manipulators and calls them all at once.
 * Each of the manipulators has its own range, MacroManipulator does not take
 * care of that.
 * */
class KSPREAD_EXPORT MacroManipulator : public Manipulator {
  public:
    void execute ();
    void unexecute ();
    void add (Manipulator *manipulator);
  protected:
    QList<Manipulator *> manipulators;
};

/** class ProtectedCheck can be used to check, whether a particular
  range is protected or not */
class KSPREAD_EXPORT ProtectedCheck : public Region {
  public:
    ProtectedCheck ();
    virtual ~ProtectedCheck ();
    void setSheet (Sheet *sheet) { m_sheet = sheet; };
    bool check ();
  protected:
    Sheet *m_sheet;
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


// TODO: storing undo information by doing an XML dump is a bad idea, replace
// with something that only stores relevant data
/**
 * \class RemovalManipulator
 * \brief Abstract class for removing cell attributes.
 */
class RemovalManipulator : public Manipulator
{
public:

protected:
  virtual bool process( Cell* ) = 0;

  virtual bool preProcessing();
  virtual bool mainProcessing();
  virtual bool postProcessing();

  void saveCellRegion( QByteArray& bytearray );

  QByteArray m_redoData;
  QByteArray m_undoData;

private:
};

/**
 * \class CommentRemovalManipulator
 * \brief Removes the comments of a cell region.
 */
class CommentRemovalManipulator : public RemovalManipulator
{
public:

protected:
  virtual bool process( Cell* cell );
  virtual QString name() const { return i18n( "Remove Comment" ); }

private:
};

/**
 * \class ConditionRemovalManipulator
 * \brief Removes the conditional formattings of a cell region.
 */
class ConditionRemovalManipulator : public RemovalManipulator
{
public:

protected:
  virtual bool process( Cell* cell );
  virtual QString name() const { return i18n( "Remove Conditional Formatting" ); }

private:
};

/**
 * \class ValidityManipulator
 * \brief Adds/Removes validity checks to/of a cell region.
 */
class ValidityManipulator : public Manipulator
{
public:
  ValidityManipulator();

  void setValidity( const Validity& validity ) { m_validity = validity; }

protected:
  virtual bool process( Cell* cell );

  virtual QString name() const;

private:
  Validity m_validity;
  QHash<int, QHash<int, Validity> > m_undoData;
};

} // namespace KSpread

#endif // KSPREAD_MANIPULATOR

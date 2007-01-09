/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
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

#ifndef KSPREAD_MANIPULATOR_ROWCOL
#define KSPREAD_MANIPULATOR_ROWCOL

#include "Manipulator.h"
#include "RectStorage.h"
#include "Style.h"

namespace KSpread
{

/**
   * \class ResizeColumnManipulator
   * \brief Resize column operation.
 */
  class ResizeColumnManipulator : public Manipulator
  {
    public:
      ResizeColumnManipulator();
      ~ResizeColumnManipulator();

      void setSize(double size) { m_newSize = size; }
      void setOldSize(double size) { m_oldSize = size; }

    protected:
      virtual bool process(Element*);

      virtual QString name() const { return i18n("Resize Column"); }

    private:
      double m_newSize;
      double m_oldSize;
  };



/**
   * \class ResizeRowManipulator
   * \brief Resize row operation.
 */
class ResizeRowManipulator : public Manipulator
{
  public:
    ResizeRowManipulator();
    ~ResizeRowManipulator();

    void setSize(double size) { m_newSize = size; }
    void setOldSize(double size) { m_oldSize = size; }

  protected:
    virtual bool process(Element*);

    virtual QString name() const { return i18n("Resize Row"); }

  private:
    double m_newSize;
    double m_oldSize;
};


/**
 * \class AdjustColumnRowManipulator
 * \brief Optimizes the height and the width of rows and columns, respectively.
 */
class AdjustColumnRowManipulator : public Manipulator
{
  public:
    AdjustColumnRowManipulator();
    virtual ~AdjustColumnRowManipulator();

    virtual bool process(Element*);
    virtual bool preProcessing();

    void setAdjustColumn(bool state) { m_adjustColumn = state; }
    void setAdjustRow(bool state) { m_adjustRow = state; }

  protected:
    virtual QString name() const;

    double adjustColumnHelper( Cell * c);
    double adjustRowHelper( Cell * c);

  private:
    bool m_adjustColumn : 1;
    bool m_adjustRow    : 1;
    QMap<int, double> m_newWidths;
    QMap<int, double> m_oldWidths;
    QMap<int, double> m_newHeights;
    QMap<int, double> m_oldHeights;
};



/**
 * \class HideShowManipulator
 * \brief Hides and shows columns and rows.
 */
class HideShowManipulator : public Manipulator
{
  public:
    HideShowManipulator();
    virtual ~HideShowManipulator();

    virtual bool process(Element*);
    virtual bool preProcessing();
    virtual bool postProcessing();

    void setManipulateColumns(bool state) { m_manipulateColumns = state; }
    void setManipulateRows(bool state) { m_manipulateRows = state; }

  protected:
    virtual QString name() const;

  private:
    bool m_manipulateColumns : 1;
    bool m_manipulateRows    : 1;
};



/**
 * \class InsertDeleteColumnManipulator
 * \brief Inserts and deletes rows.
 */
class InsertDeleteColumnManipulator : public Manipulator
{
public:
    InsertDeleteColumnManipulator();

protected:
    virtual bool process(Element*);
    virtual bool postProcessing();
    virtual QString name() const;

private:
    QList< QPair<QRectF,SharedSubStyle> > m_undoStyles;
    QList< QPair<QRectF,QString> >        m_undoComment;
    QList< QPair<QRectF,Conditions> >     m_undoConditions;
    QList< QPair<QRectF,Validity> >       m_undoValidity;
    QHash<QPoint,QString>                 m_undoCells;
};



/**
 * \class InsertDeleteRowManipulator
 * \brief Inserts and deletes rows.
 */
class InsertDeleteRowManipulator : public Manipulator
{
public:
    InsertDeleteRowManipulator();

protected:
    virtual bool process(Element*);
    virtual bool postProcessing();
    virtual QString name() const;

private:
    QList< QPair<QRectF,SharedSubStyle> > m_undoStyles;
    QList< QPair<QRectF,QString> >        m_undoComment;
    QList< QPair<QRectF,Conditions> >     m_undoConditions;
    QList< QPair<QRectF,Validity> >       m_undoValidity;
    QHash<QPoint,QString>                 m_undoCells;
};

}  // namespace KSpread

#endif // KSPREAD_MANIPULATOR_ROWCOL

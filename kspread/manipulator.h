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

#include <QLinkedList>
#include <QRect>
#include <QString>

#include <kcommand.h>
#include <klocale.h>

#include <koffice_export.h>

#include "kspread_format.h"
#include "kspread_style.h"
#include "kspread_undo.h"
#include "region.h"

namespace KSpread
{
class Cell;
class ColumnFormat;
class RowFormat;
class Sheet;


// struct layoutCell {
//   int row;
//   int col;
//   Format *l;
// };
//
// struct layoutColumn {
//   int col;
//   ColumnFormat *l;
// };
//
// struct layoutRow {
//   int row;
//   RowFormat *l;
// };



/**
 * \class Manipulator
 * \brief Abstract base class for all region related operations.
 */
class Manipulator : public Region, public KCommand
{
public:
  Manipulator();
  virtual ~Manipulator();

  Sheet* sheet() const { return m_sheet; }
  void setSheet(Sheet* sheet) { m_sheet = sheet; }

  bool creation() { return m_creation; }
  void setCreation(bool creation) { m_creation = creation; }

  /** Is this a formatting manipulator ? If so, execute will call
  process(Format*) for each complete row/column, instead of going
  cell-by-cell. True by default. */
  bool format() { return m_format; };
  void setFormat (bool f) { m_format = f; };

  virtual void execute();
  virtual void unexecute();

  virtual void setArgument(const QString& /*arg*/, const QString& /*val*/) {};

  virtual void setReverse(bool reverse) { m_reverse = reverse; }
  void setRegisterUndo(bool registerUndo) { m_register = registerUndo; }

  virtual void setName (const QString &n) { m_name = n; }
  virtual QString name() const { return m_name; };

protected:
  virtual bool process(Element*);
  virtual bool process(Cell*) { return true; }
  virtual bool process(Format*) { return true; }

  virtual bool preProcessing() { return true; }
  virtual bool mainProcessing();
  virtual bool postProcessing() { return true; }


  Sheet* m_sheet;
  QString m_name;
  bool   m_creation : 1;
  bool   m_reverse  : 1;
  bool   m_firstrun : 1;
  bool   m_format   : 1;
  bool   m_register : 1;
private:
};



/**
 * \class FormatManipulator
 * \brief Manipulates the formatting of a cell region.
 */
class FormatManipulator : public Manipulator
{
public:
  FormatManipulator();
  virtual ~FormatManipulator();

  void setProperty(Style::FlagsSet property) { m_properties |= property; }
  bool isEmpty() { return m_properties == 0; }

  // SetSelectionFontWorker
  // SetSelectionSizeWorker
  void setFontFamily(const QString& font) { m_properties |= Style::SFont; m_font = font; }
  void setFontSize(int size) { m_properties |= Style::SFont; m_size = size; }
  void setFontBold(uint bold) { m_properties |= Style::SFont; m_bold = bold; }
  void setFontItalic(uint italic) { m_properties |= Style::SFont; m_italic = italic; }
  void setFontStrike(uint strike) { m_properties |= Style::SFont; m_strike = strike; }
  void setFontUnderline(uint underline) { m_properties |= Style::SFont; m_underline = underline; }
  // SetSelectionAngleWorker
  void setAngle(int angle) { m_properties |= Style::SAngle; m_angle = angle; }
  // SetSelectionTextColorWorker
  void setTextColor(const QColor& textColor) { m_properties |= Style::STextPen; m_textColor = textColor; }
  // SetSelectionBgColorWorker
  void setBackgroundColor(const QColor& bgColor) { m_properties |= Style::SBackgroundColor; m_backgroundColor = bgColor; }
  // SetSelectionBorderAllWorker
  void setTopBorderPen(const QPen& pen) { m_properties |= Style::STopBorder; m_topBorderPen = pen; }
  void setBottomBorderPen(const QPen& pen) { m_properties |= Style::SBottomBorder; m_bottomBorderPen = pen; }
  void setLeftBorderPen(const QPen& pen) { m_properties |= Style::SLeftBorder; m_leftBorderPen = pen; }
  void setRightBorderPen(const QPen& pen) { m_properties |= Style::SRightBorder; m_rightBorderPen = pen; }
  void setHorizontalPen(const QPen& pen) { m_properties |= Style::STopBorder | Style::SBottomBorder; m_horizontalPen = pen; }
  void setVerticalPen(const QPen& pen) { m_properties |= Style::SLeftBorder | Style::SRightBorder; m_verticalPen = pen; }
  void setFallDiagonalPen(const QPen& pen) { m_properties |= Style::SFallDiagonal; m_fallDiagonalPen = pen; }
  void setGoUpDiagonalPen(const QPen& pen) { m_properties |= Style::SGoUpDiagonal; m_goUpDiagonalPen = pen; }
  // SetSelectionAlignWorker
  void setHorizontalAlignment(Style::HAlign align) { m_properties |= Style::SHAlign; m_horAlign = align; }
  // SetSelectionAlignWorker
  void setVerticalAlignment(Style::VAlign align) { m_properties |= Style::SVAlign; m_verAlign = align; }

  void setBackgroundBrush(const QBrush& brush) { m_properties |= Style::SBackgroundBrush; m_backgroundBrush = brush; }
  void setIndent(double indent) { m_properties |= Style::SIndent; m_indent = indent; }
  void setMultiRow(bool multiRow) { m_properties |= Style::SMultiRow; m_multiRow = multiRow; }
  void setVerticalText(bool verticalText) { m_properties |= Style::SVerticalText; m_verticalText = verticalText; }
  void setDontPrintText(bool dontPrintText) { m_properties |= Style::SDontPrintText; m_dontPrintText = dontPrintText; }
  void setNotProtected(bool notProtected) { m_properties |= Style::SNotProtected; m_notProtected = notProtected; }
  void setHideAll(bool hideAll) { m_properties |= Style::SHideAll; m_hideAll = hideAll; }
  void setHideFormula(bool hideFormula) { m_properties |= Style::SHideFormula; m_hideFormula = hideFormula; }
  void setComment(const QString& comment) { m_properties |= Style::SComment; m_comment = comment; }
  void setPrefix(const QString& prefix) { m_properties |= Style::SPrefix; m_prefix = prefix; }
  void setPostfix(const QString& postfix) { m_properties |= Style::SPostfix; m_postfix = postfix; }
  void setPrecision(int precision) { m_properties |= Style::SPrecision; m_precision = precision; }
  void setFloatFormat(Style::FloatFormat floatFormat) { m_properties |= Style::SFloatFormat; m_floatFormat = floatFormat; }
  void setFloatColor(Style::FloatColor floatColor) { m_properties |= Style::SFloatColor; m_floatColor = floatColor; }
  void setFormatType(FormatType formatType) { m_properties |= Style::SFormatType; m_formatType = formatType; }
  void setCurrency(int type, const QString& symbol) { m_currencyType = type; m_currencySymbol = symbol; }

protected:
  virtual QString name() const { return i18n("Format Change"); }

  virtual bool preProcessing();
  virtual bool process(Element*);

  void copyFormat(QLinkedList<layoutCell> &list,
                  QLinkedList<layoutColumn> &listCol,
                  QLinkedList<layoutRow> &listRow);
  bool testCondition(RowFormat*);
  void doWork(Format*, bool isTop, bool isBottom, bool isLeft, bool isRight);
  void prepareCell(Cell*);

private:
  quint32 m_properties;

  // TODO Stefan: find a more elegant way to store the format
  QLinkedList<layoutCell> m_lstFormats;
  QLinkedList<layoutCell> m_lstRedoFormats;
  QLinkedList<layoutColumn> m_lstColFormats;
  QLinkedList<layoutColumn> m_lstRedoColFormats;
  QLinkedList<layoutRow> m_lstRowFormats;
  QLinkedList<layoutRow> m_lstRedoRowFormats;

  // SetSelectionFontWorker
  // SetSelectionSizeWorker
  QString m_font;
  int m_size;
  signed char m_bold;
  signed char m_italic;
  signed char m_strike;
  signed char m_underline;
  // SetSelectionAngleWorker
  int m_angle;
  int m_precision;
  int m_currencyType;
  double m_indent;
  bool m_multiRow;
  bool m_verticalText;
  bool m_dontPrintText;
  bool m_notProtected;
  bool m_hideAll;
  bool m_hideFormula;

  // SetSelectionTextColorWorker
  QColor m_textColor;
  // SetSelectionBgColorWorker
  QColor m_backgroundColor;
  // SetSelectionBorderAllWorker
  QPen m_topBorderPen;
  QPen m_bottomBorderPen;
  QPen m_leftBorderPen;
  QPen m_rightBorderPen;
  QPen m_horizontalPen;
  QPen m_verticalPen;
  QPen m_fallDiagonalPen;
  QPen m_goUpDiagonalPen;

  QBrush m_backgroundBrush;
  QString m_comment;
  QString m_prefix;
  QString m_postfix;
  QString m_currencySymbol;

  // SetSelectionAlignWorker
  Style::HAlign m_horAlign;
  // SetSelectionAlignWorker
  Style::VAlign m_verAlign;
  Style::FloatFormat m_floatFormat;
  Style::FloatColor m_floatColor;
  FormatType m_formatType;
};



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
 * \class BorderManipulator
 * \brief Manipulates the border of a cell region.
 */
class BorderManipulator : public FormatManipulator
{
public:
  BorderManipulator() {}
  ~BorderManipulator() {}

protected:
  virtual QString name() const { return i18n("Change Border"); }

private:
};



/**
 * \class BackgroundColorManipulator
 * \brief Manipulates the background of a cell region.
 */
class BackgroundColorManipulator : public FormatManipulator
{
public:
  BackgroundColorManipulator() {}
  ~BackgroundColorManipulator() {}

protected:
  virtual QString name() const { return i18n("Change Background Color"); }

private:
};



/**
 * \class FontColorManipulator
 * \brief Manipulates the font color of a cell region.
 */
class FontColorManipulator : public FormatManipulator
{
public:
  FontColorManipulator() {}
  ~FontColorManipulator() {}

protected:
  virtual QString name() const { return i18n("Change Text Color"); }

private:
};



/**
 * \class FontManipulator
 * \brief Manipulates the font of a cell region.
 */
class FontManipulator : public FormatManipulator
{
public:
  FontManipulator() {}
  ~FontManipulator() {}

protected:
  virtual QString name() const { return i18n("Change Font"); }

private:
};



/**
 * \class AngleManipulator
 * \brief Manipulates the text angle of a cell region.
 */
class AngleManipulator : public FormatManipulator
{
  public:
    AngleManipulator() {}
    ~AngleManipulator() {}

  protected:
    virtual QString name() const { return i18n("Change Angle"); }

  private:
};



/**
 * \class HorAlignManipulator
 * \brief Manipulates the horizontal text alignment of a cell region.
 */
class HorAlignManipulator : public FormatManipulator
{
  public:
    HorAlignManipulator() {}
    ~HorAlignManipulator() {}

  protected:
    virtual QString name() const { return i18n("Change Horizontal Alignment"); }

  private:
};



/**
 * \class VerAlignManipulator
 * \brief Manipulates the vertical text alignment of a cell region.
 */
class VerAlignManipulator : public FormatManipulator
{
  public:
    VerAlignManipulator() {}
    ~VerAlignManipulator() {}

  protected:
    virtual QString name() const { return i18n("Change Vertical Alignment"); }

  private:
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
 * \class DilationManipulator
 * \brief Dilates a cell region by one cell in each direction.
 */
class DilationManipulator : public Manipulator
{
public:
  DilationManipulator();
  virtual ~DilationManipulator();

  virtual void execute();
  virtual void unexecute();

protected:
  virtual QString name() const { return i18n("Dilate Region"); }

private:
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

  double adjustColumnHelper( Cell * c, int _col, int _row );
  double adjustRowHelper( Cell * c, int _col, int _row );

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
 * \class InsertDeleteManipulator
 * \brief Inserts and deletes columns and rows.
 */
class InsertDeleteManipulator : public Manipulator
{
public:
  InsertDeleteManipulator();
  ~InsertDeleteManipulator();

protected:

private:
  bool m_manipulateColumns : 1;
  bool m_manipulateRows    : 1;
};



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
 * \class TextRemovalManipulator
 * \brief Removes the texts of a cell region.
 */
class TextRemovalManipulator : public RemovalManipulator
{
public:

protected:
  virtual bool process( Cell* cell );
  virtual QString name() const { return i18n( "Remove Text" ); }

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
 * \class ValidityRemovalManipulator
 * \brief Removes the validity checks of a cell region.
 */
class ValidityRemovalManipulator : public RemovalManipulator
{
public:

protected:
  virtual bool process( Cell* cell );
  virtual QString name() const { return i18n( "Remove Validity" ); }

private:
};




/**
 * \class ManipulatorManager
 * \todo To implement of not to implement. That's the question.
 */
class ManipulatorManager
{
  public:
    static ManipulatorManager* self();
    ~ManipulatorManager();
    Manipulator* create(const QString&);

  private:
    ManipulatorManager();
    static ManipulatorManager* m_self;
};

} // namespace KSpread

#endif // KSPREAD_MANIPULATOR

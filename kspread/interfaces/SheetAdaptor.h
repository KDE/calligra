/* This file is part of the KDE project

   Copyright 2006 Fredrik Edemar <f_edemar@linux.se>
   Copyright 2000, 2003 Laurent Montel <montel@kde.org>
   Copyright 2002-2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_SHEET_ADAPTOR
#define KSPREAD_SHEET_ADAPTOR

#include <QtDBus/QtDBus>
#include "kspread_export.h"
#include <QString>

namespace KSpread
{
class Damage;
class Sheet;
class CellProxy;

class KSPREAD_EXPORT SheetAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.spreadsheet.sheet")
public:
    explicit SheetAdaptor(Sheet*);
    ~SheetAdaptor();

public Q_SLOTS: // METHODS

    /** Return the cellname for x,y (where x is the row and y is the column). For
    example for x=5 and y=2 the string "B5" got returned. */
    virtual QString cellName(int x, int y);
    /** Return the x,y-point for the cellname. */
    virtual QPoint cellLocation(const QString& cellname);
    /** Return the row-number for the cellname. For example for the cellname "B5"
    the integer 5 got returned. */
    virtual int cellRow(const QString& cellname);
    /** Return the column-number for the cellname. For example for the cellname "B5"
    the integer 2 got returned. */
    virtual int cellColumn(const QString& cellname);

    /** Return the text for the row x and for the column y. */
    virtual QString text(int x, int y);
    /** Return the text for a cellname. */
    virtual QString text(const QString& cellname);
    /** Set the text of the cell with row x and column y. If the parse argument is
    true, the passed text got parsed (e.g. a text like "123" will be recognised
    as numeric value. */
    virtual bool setText(int x, int y, const QString& text, bool parse = true);
    /** Set the text of the cell defined with cellname. */
    virtual bool setText(const QString& cellname, const QString& text, bool parse = true);

    /** Return the value the cell at row x and column y has. The returned value is
    a variant and could be e.g. a number, a bool or a text depending on the format
    and the content the cell has. */
    virtual QVariant value(int x, int y);
    /** Return the value for the cell defined with cellname. */
    virtual QVariant value(const QString& cellname);
    /** Set the value in the cell at row x and column y. */
    virtual bool setValue(int x, int y, const QVariant& value);
    /** Set the value in the cell defined with cellname. */
    virtual bool setValue(const QString& cellname, const QVariant& value);

    /** Return the name of the sheet. */
    virtual QString sheetName() const;
    /** Set the name of the sheet. */
    virtual bool setSheetName(const QString & name);

    //virtual QString column( int _col );
    //virtual QString row( int _row );

    /** Return the position the last column on this sheet has. */
    virtual int lastColumn() const;
    /** Return the position the last row on this sheet has. */
    virtual int lastRow() const;

    /** Inserts nbCol number of new columns at the position col. All columns which
    are >= col are moved to the right. */
    virtual void insertColumn(int col, int nbCol = 1);
    /** Inserts nbRow number of new rows at the position row. All rows which
    are >= row are moved down. */
    virtual void insertRow(int row, int nbRow = 1);
    /** Remove nbCol number of columns from the position col. */
    virtual void removeColumn(int col, int nbCol = 1);
    /** Remove nbRow number of columns from the position row. */
    virtual void removeRow(int row, int nbRow = 1);

    /** Returns true if the sheet is hidden else false is returned. */
    virtual bool isHidden() const;
    /** Hide the sheet if the argument hidden is true or show it if hidden is false. */
    virtual void setHidden(bool hidden);

    //virtual bool showGrid() const;
    //virtual bool showFormula() const;
    //virtual bool lcMode() const;
    //virtual bool autoCalc() const;
    //virtual bool showColumnNumber() const;
    //virtual bool hideZero() const;
    //virtual bool firstLetterUpper() const;
    //virtual void setShowPageBorders( bool b );

    /** Return the height the paper of the printer has. */
    virtual float paperHeight() const;
    /** Set the height the paper of the printer has. */
    virtual void setPrinterHeight(float height);
    /** Return the width the paper of the printer has. */
    virtual float paperWidth() const;
    /** Set the width the paper of the printer has. */
    virtual void setPaperWidth(float width);
    /** Return the left border the paper of the printer has. */
    virtual float paperLeftBorder() const;
    /** Return the right border the paper of the printer has. */
    virtual float paperRightBorder() const;
    /** Return the top border the paper of the printer has. */
    virtual float paperTopBorder() const;
    /** Return the bottom border the paper of the printer has. */
    virtual float paperBottomBorder() const;
    /** Return the name of the paper format (like "A4" or "Letter"). */
    virtual QString paperFormat() const;
    /** Return the name of the paper orientation (like "Portrait" or "Landscape"). */
    virtual QString paperOrientation() const;
    /** Set the left, top, right and bottom border as well as the page format and
    orientation the paper of the printer has. */
    virtual void setPaperLayout(float leftBorder, float topBorder, float rightBorder, float bottomBoder, const QString& format, const QString& orientation);

    //QString printHeadLeft() const;
    //QString printHeadMid() const;
    //QString printHeadRight() const;
    //QString printFootLeft() const;
    //QString printFootMid() const;
    //QString printFootRight() const;
    //void setPrintHeaderLeft(const QString & text);
    //void setPrintHeaderMiddle(const QString & text);
    //void setPrintHeaderRight(const QString & text);
    //void setPrintFooterLeft(const QString & text);
    //void setPrintFooterMiddle(const QString & text);
    //void setPrintFooterRight(const QString & text);

    //NOTE: don't publish "QByteArray password()" cause that may introduce insecure situations...
    /** Return true if passwd is the correct password. */
    virtual bool checkPassword(const QByteArray& passwd) const;
    /** Return true if the sheet/document is protected. */
    virtual bool isProtected() const;
    /** Protect the document with the password passwd. */
    virtual void setProtected(const QByteArray& passwd);

Q_SIGNALS:

    /** This signal got emitted if the name of the sheet changed. */
    void nameChanged();
    /** This signal got emitted if the sheet got shown. */
    void showChanged();
    /** This signal got emitted if the sheet got hidden. */
    void hideChanged();

private Q_SLOTS:
    /**
     * \ingroup Damages
     * Handles changes of the sheet name and visibility.
     */
    void handleDamages(const QList<Damage*>& damages);

private:
    Sheet* m_sheet;
    QByteArray ident;
};

} // namespace KSpread

#endif // KSPREAD_SHEET_ADAPTOR

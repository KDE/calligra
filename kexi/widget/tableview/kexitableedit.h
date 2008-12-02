/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
   Copyright (C) 2003-2006 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _KEXITABLEEDIT_H_
#define _KEXITABLEEDIT_H_

#include <kexidataiteminterface.h>

#include <qvariant.h>
#include <q3scrollview.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QEvent>

#include "kexitableviewdata.h"

namespace KexiDB
{
class Field;
class QueryColumnInfo;
}

/*! @short Abstract class for a cell editor.
 Handles cell painting and displaying the editor widget.
*/
class KEXIDATATABLE_EXPORT KexiTableEdit : public QWidget, public KexiDataItemInterface
{
    Q_OBJECT

public:
    KexiTableEdit(KexiTableViewColumn &column, QWidget* parent = 0);

    virtual ~KexiTableEdit();

    //! Implemented for KexiDataItemInterface.
    //! \return field information for this item
    virtual KexiDB::Field *field() const {
        return m_column->field();
    }

    /*! A rich field information for db-aware data.
     For not-db-aware data it is always 0 (use field() instead. */
    virtual KexiDB::QueryColumnInfo *columnInfo() const {
        return m_column->columnInfo();
    }

    //! Implemented for KexiDataItemInterface.
    //! Does nothing because instead KexiTableViewColumn is used to get field's schema.
    virtual void setColumnInfo(KexiDB::QueryColumnInfo *) { }

    //! \return column information for this item
    //! (extended information, comparing to field()).
    inline KexiTableViewColumn *column() const {
        return m_column;
    }

    /*! \return displayed field. This is equal to field() in typical case but can return a different field
     definition if the column contains a lookup field. This distiction is especially used for
     displaying data dependent on the type and specifics of the field definition
     (e.g. text type versus integer type). Note that to compute the editor's value
     we still use field(). */
    KexiDB::Field *displayedField() const;

    /*! Reimplemented: resizes a view(). */
    virtual void resize(int w, int h);

    /*! \return the view widget of this editor, e.g. line edit widget. */
    virtual QWidget* widget() {
        return m_view;
    }

    /*! Hides item's widget, if available. */
    inline virtual void hideWidget() {
        hide();
    }

    /*! Shows item's widget, if available. */
    inline virtual void showWidget() {
        show();
    }

    /*! Paints a border for the cell described by \a x, \a y, \a w, \a h on \a p painter.
     The cell's value is \a val (may be useful if you want to reimplement this method).
    */
    virtual void paintFocusBorders(QPainter *p, QVariant &cal, int x, int y, int w, int h);

    /*! For reimplementation.
     Sets up and paints cell's contents using context of \a val value.
     \a focused is true if the cell is focused. \a align is set using Qt::AlignmentFlags.
     Some additional things may be painted using \a p,
     but it is not needed to paint the text (this is done automatically outside of this method).

     Before calling, \a x, \a y_offset, \a w, \a h parameters are initialized,
     but you can tune these values depending on the context.
     You should set \a txt to a text representation of \a val,
     otherwise no text will be painted.

     \a p can be 0 - in this case no painting should be performed, becasue caller only expects
     that \a x, \a y_offset, \a w, \a h, \a txt parameters are tuned, if needed.
     \a p painter's pen is set to foreground color (usually black) that should be used to paint
     foreground information, if needed. For example boolean editor widget paints
     a rectangle using this color. */
    virtual void setupContents(QPainter *p, bool focused, const QVariant& val,
                               QString &txt, int &align, int &x, int &y_offset, int &w, int &h);

    /*! \return true if "selected text" color should be used to paint contents of the editor.
     True by default. It's false e.g. in boolean editor, where no selection is painted
     using paintSelectionBackground().
     This flag is set in editor's constructor and checked in KexiTableView::paintCell().
     Depending on it, appropriate ("text" or "selected text" color is set for painter) before
     setupContents() is called. */
    bool usesSelectedTextColor() const {
        return m_usesSelectedTextColor;
    }

    /*! For reimplementation.
     Paints selection's background using \a p. Most parameters are similar to these from
     setupContents(). */
    virtual void paintSelectionBackground(QPainter *p, bool focused, const QString& txt,
                                          int align, int x, int y_offset, int w, int h, const QColor& fillColor,
                                          const QFontMetrics &fm, bool readOnly, bool fullRowSelection);

    /*! Sometimes, editor can contain non-standard margin, for example combobox editor contains
     dropdown button at the right side. \return left margin's size;
     0 by default. For reimplementation.  */
    int leftMargin() const {
        return m_leftMargin;
    }

    /*! Sometimes, editor can contain non-standard margin, for example combobox editor contains
     dropdown button at the right side. THe dropdown button's width is counted only if \a focused is true.
     \return right margin's size; 0 by default. For reimplementation.  */
    int rightMargin(bool focused) const;

    /*! Handles \a ke key event that came over the column that is bound to this editor.
     For implementation: true should be returned if \a ke should be accepted.
     If \a editorActive is true, this editor is currently active, i.e. the table view is in edit mode.
     By default false is returned. */
    virtual bool handleKeyPress(QKeyEvent* ke, bool editorActive) {
        Q_UNUSED(ke); Q_UNUSED(editorActive); return false;
    }

    /*! Handles double click request coming from the table view.
     \return true if it has been consumed.
     Reimplemented in KexiBlobTableEdit (to execute "insert file" action. */
    virtual bool handleDoubleClick() {
        return false;
    }

    /*! Handles copy action for value. The \a value is copied to clipboard in format appropriate
     for the editor's impementation, e.g. for image cell it can be a pixmap.
     For a special case (combo box), \a visibleValue can be provided,
     so it can be copied to the clipboard instead of unreadable \a value.
     For reimplementation. */
    virtual void handleCopyAction(const QVariant& value, const QVariant& visibleValue) = 0;

    /*! \return width of \a value. For the default implementation \a val is converted to a string
     and width of this string is returned. */
    virtual int widthForValue(const QVariant &val, const QFontMetrics &fm);

    /*! \return total size of this editor, including any buttons, etc. (if present).
     Reimpelment this if you want to return more appropriate size. This impelmentation just
     returns QWidget::size(). */
    virtual QSize totalSize() const {
        return QWidget::size();
    }

    /*! Shows a special tooltip for \a value if needed, i.e. if the value could not fit inside \a rect
     for a given font metrics \a fm.
     \return true a normal tooltip should be displayed (using QToolTip,) and false if
     no tooltip should be displayed or a custom tooltip was displayed internally (not yet supported).
     Default implementation does nothing and returns false.
     If the cell is currentl focused (selected), \a focused is true. */
    virtual bool showToolTipIfNeeded(const QVariant& value, const QRect& rect, const QFontMetrics& fm,
                                     bool focused);

    /*! Created internal editor for this editor is needed. This method is only implemented
     in KexiComboBoxTableEdit since it's visible value differs from internal value,
     so a different KexiTableEdit object is used to displaying the data. */
    virtual void createInternalEditor(KexiDB::QuerySchema& schema) {
        Q_UNUSED(schema);
    }

signals:
    void editRequested();
    void cancelRequested();
    void acceptRequested();

protected:
    virtual bool eventFilter(QObject* watched, QEvent* e);

    /*! Sets \a v as view widget for this editor. The view will be assigned as focus proxy
     for the editor, its events will be filtered, it will be resized when neede, and so on. */
    void setViewWidget(QWidget *v);

    /*! Moves child widget within the viewport if the parent is scrollview (otherwise does nothing).
     Use this for child widgets that are outside of this editor widget, instead of calling QWidget::move(). */
    void moveChild(QWidget * child, int x, int y);

    /*! Allows to force redrawing the related cell by the editor itself. Usable when the editor is not
     displayed by a QWidget but rather by table view cell itself, for example KexiBlobTableEdit. */
    void repaintRelatedCell();

    KexiTableViewColumn *m_column;
    int m_leftMargin;
    int m_rightMargin, m_rightMarginWhenFocused;
    Q3ScrollView* m_scrollView; //!< may be 0 if the parent is not a scrollview
    bool m_usesSelectedTextColor : 1; //!< set in ctor, @see usesSelectedTextColor()

private:
    QWidget* m_view;
};

//! Declaration of cell editor factory
#define KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(factoryclassname) \
    class factoryclassname : public KexiCellEditorFactoryItem \
    { \
    public: \
        factoryclassname(); \
        virtual ~factoryclassname(); \
        \
    protected: \
        virtual KexiTableEdit* createEditor(KexiTableViewColumn &column, QWidget* parent = 0); \
    };

//! Implementation of cell editor factory
#define KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(factoryclassname, itemclassname) \
    factoryclassname::factoryclassname() \
            : KexiCellEditorFactoryItem() \
    { \
        m_className = "" #itemclassname ""; \
    } \
    \
    factoryclassname::~factoryclassname() \
    {} \
    \
    KexiTableEdit* factoryclassname::createEditor( \
            KexiTableViewColumn &column, QWidget* parent) \
    { \
        return new itemclassname(column, parent); \
    }

#endif

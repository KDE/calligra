/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIINPUTTABLEEDIT_H
#define KEXIINPUTTABLEEDIT_H

#include <klineedit.h>
#include <qvariant.h>
//Added by qt3to4:
#include <QPaintEvent>

#include "kexitableedit.h"
#include "kexicelleditorfactory.h"
#include "kexitextformatter.h"

/*! @short General purpose cell editor using line edit widget.
*/
class KEXIDATATABLE_EXPORT KexiInputTableEdit : public KexiTableEdit
{
    Q_OBJECT

public:
    KexiInputTableEdit(KexiTableViewColumn &column, QWidget *parent = 0);

    virtual ~KexiInputTableEdit();

    virtual bool valueChanged();

    //! \return true if editor's value is null (not empty)
    virtual bool valueIsNull();

    //! \return true if editor's value is empty (not null).
    //! Only few field types can accept "EMPTY" property
    //! (check this with KexiDB::Field::hasEmptyProperty()),
    virtual bool valueIsEmpty();

    virtual QVariant value();

    virtual bool cursorAtStart();
    virtual bool cursorAtEnd();

//  virtual bool eventFilter(QObject* watched, QEvent* e);
//js  void end(bool mark);
//js  void backspace();
    virtual void clear();

    /*! \return total size of this editor, including any buttons, etc. (if present). */
    virtual QSize totalSize() const;

    /*! Handles action having standard name \a actionName.
     Action could be: "edit_cut", "edit_paste", etc. */
    virtual void handleAction(const QString& actionName);

    /*! Handles copy action for value. The \a value is copied to clipboard in format appropriate
     for the editor's impementation, e.g. for image cell it can be a pixmap.
     \a visibleValue is unused here. Reimplemented after KexiTableEdit. */
    virtual void handleCopyAction(const QVariant& value, const QVariant& visibleValue);

    /*! Shows a special tooltip for \a value if needed, i.e. if the value could not fit
     inside \a rect for a given font metrics \a fm.
     \return true a normal tooltip should be displayed (using QToolTip,) and false if
     no tooltip should be displayed or a custom tooltip was displayed internally (not yet supported).
     This implementation converts the value to text using KexiTextFormatter::toString()
     if \a value is not string to see whether it can fit inside the cell's \a rect.
     If the cell is currentl focused (selected), \a focused is true. */
    virtual bool showToolTipIfNeeded(const QVariant& value, const QRect& rect, const QFontMetrics& fm,
                                     bool focused);

public slots:
    //! Implemented for KexiDataItemInterface
    virtual void moveCursorToEnd();

    //! Implemented for KexiDataItemInterface
    virtual void moveCursorToStart();

    //! Implemented for KexiDataItemInterface
    virtual void selectAll();

protected slots:
    void setRestrictedCompletion();
    void completed(const QString &);
    void slotTextEdited(const QString&);

protected:
    //! initializes this editor with \a add value
    virtual void setValueInternal(const QVariant& add, bool removeOld);

    void showHintButton();
    void init();
    virtual void paintEvent(QPaintEvent *e);

    KexiTextFormatter m_textFormatter;
    bool m_calculatedCell;
    QString m_decsym; //! decimal symbol
    QString m_origText; //! orig. Line Edit's text after conversion - for easy comparing
    KLineEdit *m_lineedit;

signals:
    void hintClicked();
};

KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(KexiInputEditorFactoryItem)

#endif

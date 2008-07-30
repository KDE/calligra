/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIBOOLTABLEEDIT_H
#define KEXIBOOLTABLEEDIT_H

#include <qvariant.h>

#include "kexitableedit.h"
#include "kexicelleditorfactory.h"

/*! @short Cell editor for boolean type.
*/
class KexiBoolTableEdit : public KexiTableEdit
{
  Q_OBJECT

  public:
    KexiBoolTableEdit(KexiTableViewColumn &column, QWidget *parent=0);

    virtual ~KexiBoolTableEdit();

    //! \return true if editor's value is null (not empty)
    virtual bool valueIsNull();

    //! \return true if editor's value is empty (not null). 
    //! Only few field types can accept "EMPTY" property 
    //! (check this with KexiDB::Field::hasEmptyProperty()), 
    virtual bool valueIsEmpty();

    virtual QVariant value();

    virtual bool cursorAtStart();
    virtual bool cursorAtEnd();

    virtual void clear();

    virtual void setupContents( QPainter *p, bool focused, const QVariant& val, 
      QString &txt, int &align, int &x, int &y_offset, int &w, int &h );

    virtual void clickedOnContents();

    /*! Handles action having standard name \a actionName. 
     Action could be: "edit_cut", "edit_paste", etc. */
    virtual void handleAction(const QString& actionName);

    /*! Handles copy action for value. Copies empty string for null, "1" for true, "0" for false.
     \a visibleValue is unused here. Reimplemented after KexiTableEdit. */
    virtual void handleCopyAction(const QVariant& value, const QVariant& visibleValue);

    /*! \return width of \a value. Reimplemented  after KexiTableEdit. */
    virtual int widthForValue( const QVariant &val, const QFontMetrics &fm );

  protected slots:

  protected:
    //! initializes this editor with \a add value
    virtual void setValueInternal(const QVariant& add, bool removeOld);

    void showHintButton();

    //! We've no editor widget that would store current value, so we do this here
    QVariant m_currentValue;

  signals:
    void hintClicked();
};

KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(KexiBoolEditorFactoryItem)

#endif

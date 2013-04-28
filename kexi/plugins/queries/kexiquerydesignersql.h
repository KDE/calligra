/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2012 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIQUERYDESIGNERSQL_H
#define KEXIQUERYDESIGNERSQL_H

#include "kexiquerypart.h"

#include <KexiView.h>
#include <ktitlewidget.h>
#include <QEvent>

class KexiQueryDesignerSQLEditor;

//! The KexiQueryDesignerSQLView class for editing Queries in text mode.
/*! It is a view containing SQL text editor
 and SQL status widget split vertically. */
class KexiQueryDesignerSQLView : public KexiView
{
    Q_OBJECT

public:
    KexiQueryDesignerSQLView(QWidget *parent);
    virtual ~KexiQueryDesignerSQLView();

    QString sqlText() const;
    KexiQueryDesignerSQLEditor *editor() const;

protected:
    KexiQueryPart::TempData * tempData() const;

    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore);
    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);
    virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata,
                                             KexiView::StoreNewDataOptions options,
                                             bool &cancel);
    virtual tristate storeData(bool dontAsk = false);

    void setStatusOk();
    void setStatusError(const QString& msg);
    void setStatusEmpty();
    void setStatusText(const QString& text);

    virtual void updateActions(bool activated);

protected slots:
    /*! Performs query checking (by text parsing). \return true and sets d->parsedQuery
     to the new query schema object on success. */
    bool slotCheckQuery();
    void slotUpdateMode();
    void slotTextChanged();

signals:
    void queryShortcut();

private:
    class Private;
    Private * const d;

    friend class KexiQueryView; // for storeNewData() and storeData() only
};

#endif

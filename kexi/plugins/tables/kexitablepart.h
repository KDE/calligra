/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXITABLEPART_H
#define KEXITABLEPART_H

#include <kexi.h>
#include <kexipart.h>
#include <KexiWindow.h>
#include <KexiWindowData.h>
#include <kexipartitem.h>
#include <db/fieldlist.h>

class KexiLookupColumnPage;

class KexiTablePart : public KexiPart::Part
{
    Q_OBJECT

public:
    KexiTablePart(QObject *parent, const QVariantList &);
    virtual ~KexiTablePart();

    virtual tristate remove(KexiPart::Item &item);

    virtual tristate rename(KexiPart::Item &item,
                            const QString& newName);

//  virtual KexiPart::DataSource *dataSource();

    class TempData : public KexiWindowData
    {
    public:
        TempData(QObject* parent);
        KexiDB::TableSchema *table;
        /*! true, if \a table member has changed in previous view. Used on view switching.
         We're checking this flag to see if we should refresh data for DataViewMode. */
    bool tableSchemaChangedInPreviousView : 1;
    };

    static tristate askForClosingObjectsUsingTableSchema(
        QWidget *parent, KexiDB::Connection& conn,
        KexiDB::TableSchema& table, const QString& msg);

    virtual KLocalizedString i18nMessage(const QString& englishMessage,
                                         KexiWindow* window) const;

    KexiLookupColumnPage* lookupColumnPage() const;

protected:
    virtual KexiWindowData* createWindowData(KexiWindow* window);

    virtual KexiView* createView(QWidget *parent, KexiWindow* window,
                                 KexiPart::Item &item, Kexi::ViewMode viewMode = Kexi::DataViewMode, QMap<QString, QVariant>* staticObjectArgs = 0);

    virtual void initPartActions();
    virtual void initInstanceActions();

    virtual void setupCustomPropertyPanelTabs(KTabWidget *tab);

    virtual KexiDB::SchemaData* loadSchemaData(KexiWindow *window, const KexiDB::SchemaData& sdata,
            Kexi::ViewMode viewMode, bool *ownedByWindow);

private:
    class Private;
    Private* const d;
};

#if 0
class KexiTableDataSource : public KexiPart::DataSource
{
public:
    KexiTableDataSource(KexiPart::Part *part);
    ~KexiTableDataSource();

    virtual KexiDB::FieldList *fields(KexiProject *project, const KexiPart::Item &item);
    virtual KexiDB::Cursor *cursor(KexiProject *project, const KexiPart::Item &item, bool buffer);
};
#endif

#endif


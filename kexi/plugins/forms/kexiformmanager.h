/* This file is part of the KDE project
   Copyright (C) 2005-2009 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIFORMMANAGER_H
#define KEXIFORMMANAGER_H

#include <QObject>
#include "kexiformpart.h"

class KexiFormView;
class KActionCollection;
class KexiFormManagerPrivate;

namespace KFormDesigner {
class WidgetLibrary;
class ObjectTreeView;
class ActionGroup;
class Command;
}

//! @internal
class KEXIFORMUTILS_EXPORT KexiFormManager : public QObject
{
    Q_OBJECT

public:
    static KexiFormManager* self();

    //! Called by KexiFormPart()
    void init(KexiFormPart *part, KFormDesigner::ObjectTreeView *treeView);

    virtual QAction* action(const char* name);
    virtual void enableAction(const char* name, bool enable);

    KFormDesigner::WidgetLibrary* library() const;

//moved from KFormDesigner::FormManager
    /*! @return action group containing "insert widget" actions for each widget. */
    KFormDesigner::ActionGroup* widgetActionGroup() const;

public slots:
    //! Receives signal from KexiDataSourcePage about changed form's data source
    void setFormDataSource(const QString& mime, const QString& name);

    /*! Receives signal from KexiDataSourcePage about changed widget's data source.
     This is because we couldn't pass objects like KexiDB::QueryColumnInfo.

     Also sets following things in KexiDBAutoField:
     - caption related to the data source
     - data type related to the data source */
    void setDataSourceFieldOrExpression(const QString& string, const QString& caption,
                                        KexiDB::Field::Type type);

    /*! Receives signal from KexiDataSourcePage and inserts autofields onto the current form. */
    void insertAutoFields(const QString& sourcePartClass, const QString& sourceName,
                          const QStringList& fields);

// moved from FormManager
    /*! For debugging purposes only:
     shows a text window containing contents of .ui XML definition of the current form. */
    void showFormUICode();

protected slots:
    void slotHistoryCommandExecuted(KFormDesigner::Command *command);
// 2.0 moved from KexiFormPart
    void slotWidgetCreatedByFormsLibrary(QWidget* widget);
    void slotWidgetActionToggled(const QByteArray& action);
    void slotAssignAction();
    void slotPointerClicked();

protected:
    inline QString translateName(const char* name) const;

private:
    KexiFormManager(KexiFormManagerPrivate * p);
    
    virtual ~KexiFormManager();

    //! Helper: return active form's view widget or 0 if there's no active form having such widget
    KexiFormView* activeFormViewWidget() const;

//moved from KFormDesigner::FormManager
    //! Called by init()
    void createActions(KActionCollection* collection);

    friend class KexiFormManagerPrivate;
    KexiFormManagerPrivate * const d;
};

QString KexiFormManager::translateName(const char* name) const
{
    QString n(QString::fromLatin1(name));
    //translate to our name space:
    if (n.startsWith("align_") || n.startsWith("adjust_") || n.startsWith("layout_")
            || n == "format_raise" || n == "format_raise" || n == "taborder" || n == "break_layout")
    {
        n.prepend("formpart_");
    }
    return n;
}

#endif

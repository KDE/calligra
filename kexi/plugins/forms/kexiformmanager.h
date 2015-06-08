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

#include "kexiformpart.h"

class KexiFormView;
class KActionCollection;
class KexiFormManagerPrivate;

namespace KFormDesigner {
class WidgetLibrary;
class WidgetTreeWidget;
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
    void init(KexiFormPart *part, KFormDesigner::WidgetTreeWidget *widgetTree);

    virtual QAction* action(const char* name);
    virtual void enableAction(const char* name, bool enable);

    KFormDesigner::WidgetLibrary* library() const;

    /*! @return action group containing "insert widget" actions for each widget. */
    KFormDesigner::ActionGroup* widgetActionGroup() const;

    //! @internal
    KexiFormManager();

    virtual ~KexiFormManager();

public Q_SLOTS:
    //! Receives signal from KexiDataSourcePage about changed form's data source
    void setFormDataSource(const QString& mime, const QString& name);

    /*! Receives signal from KexiDataSourcePage about changed widget's data source.
     This is because we couldn't pass objects like KDbQueryColumnInfo.

     Also sets following things in KexiDBAutoField:
     - caption related to the data source
     - data type related to the data source */
    void setDataSourceFieldOrExpression(const QString& string, const QString& caption,
                                        KDbField::Type type);

    /*! Receives signal from KexiDataSourcePage and inserts autofields onto the current form. */
    void insertAutoFields(const QString& sourcePartClass, const QString& sourceName,
                          const QStringList& fields);

    /*! For debugging purposes only:
     shows a text window containing contents of .ui XML definition of the current form. */
    void showFormUICode();

protected Q_SLOTS:
    void slotHistoryCommandExecuted(KFormDesigner::Command *command);
    void slotWidgetCreatedByFormsLibrary(QWidget* widget);
    void slotWidgetActionToggled(const QByteArray& action);
    void slotAssignAction();
    void slotPointerClicked();

protected:
    QString translateName(const char* name) const;

private:
    //! Helper: return active form's view widget or 0 if there's no active form having such widget
    KexiFormView* activeFormViewWidget() const;

    //! Called by init()
    void createActions(KActionCollection* collection);

    friend class KexiFormManagerPrivate;
    KexiFormManagerPrivate * const d;
};

#endif

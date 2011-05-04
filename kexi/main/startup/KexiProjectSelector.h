/* This file is part of the KDE project
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIPROJECTSELECTOR_H
#define KEXIPROJECTSELECTOR_H

#include "ui_KexiProjectSelector.h"
#include "kexiprojectset.h"

#include <KPageDialog>

#include <QTreeWidget>

/*! Widget that allows to select a kexi project (or database)
*/
class KEXIMAIN_EXPORT KexiProjectSelectorWidget : public QWidget, private Ui_KexiProjectSelector
{
    Q_OBJECT

public:
// enum ConnType { FileBased=1, ServerBased=2 };

    /*! Constructs a project selector widget.
    If \a showProjectNameColumn is true (the default)
    project names' column is visible. If \a showConnectionColumns is true (the default)
    information about database driver and connection columns are added.
    \a prj_set may be NULL - you can assign a set later with setProjectSet().
    */
    KexiProjectSelectorWidget(QWidget* parent = 0,
                              KexiProjectSet* prj_set = 0, bool showProjectNameColumn = true,
                              bool showConnectionColumns = true);

    virtual ~KexiProjectSelectorWidget();

    /*! \return data of selected project. Returns NULL if no selection has been made.
    */
    KexiProjectData* selectedProjectData() const;

    /*! Assigns a new project set \a prj_set. Old project set is not destoyed
    - it is just left unassigned.
    If new project set is in error state (Object::error() == true), nothing is displayed. */
    void setProjectSet(KexiProjectSet* prj_set);

    /*! \return currently assigned project set or NULL if no project set is assigned. */
    inline KexiProjectSet *projectSet() {
        return m_prj_set;
    }

    /*! Sets selectable state on or off. In this state one project item can be selected
    and executed by mouse double clicking or return key pressing.
    The property is true by default. */
    void setSelectable(bool set);

    /*! \return  if a witget has selectable state set. */
    bool isSelectable() const;

    QLabel *label() const;

    QTreeWidget* list() const;
    
signals:
    void projectExecuted(KexiProjectData*);
    void selectionChanged(KexiProjectData*);

protected slots:
    void slotItemExecuted(QTreeWidgetItem*);
    void slotItemExecuted();
    void slotItemSelected();

protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);
    
    KexiProjectSet *m_prj_set;

    class Private;
    Private * const d;

    friend class ProjectDataLVItem;
};

/*! Dialog container for KexiProjectSelectorWidget */
class KexiProjectSelectorDialog : public KPageDialog
{
    Q_OBJECT
public:
    /*! Constructor 1, used for displaying recent projects list
     Label "there are recently opened projects" is displayed automatically
    */
    KexiProjectSelectorDialog(QWidget *parent,
                              KexiProjectSet* prj_set,
                              bool showProjectNameColumn = true, bool showConnectionColumns = true);

    /*! Constructor 2, used for displaying projects list for given connection
     Label "Select one of these existing projects on server" is displayed automatically
     You should test if project set was properly loaded using projectSet()->error().
    */
    KexiProjectSelectorDialog(QWidget *parent,
                              const KexiDB::ConnectionData& cdata,
                              bool showProjectNameColumn = true, bool showConnectionColumns = true);

    virtual ~KexiProjectSelectorDialog();

    /*! \return data of selected project. Returns NULL if no selection has been made.
    */
    KexiProjectData* selectedProjectData() const;

    /*! \return currently assigned project set or NULL if no project set is assigned. */
    inline KexiProjectSet *projectSet() const {
        return m_sel->projectSet();
    }

    virtual void showEvent(QShowEvent * event);

protected slots:
    void slotProjectExecuted(KexiProjectData*);
    void slotProjectSelectionChanged(KexiProjectData*);

protected:
    void init(KexiProjectSet* prj_set, bool showProjectNameColumn, bool showConnectionColumns);

    KexiProjectSelectorWidget* m_sel;
};

#endif


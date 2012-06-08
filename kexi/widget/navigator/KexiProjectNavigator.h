/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2010 Adam Pigg <adam@piggz.co.uk>

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

#ifndef KEXIPROJECTNAVIGATOR_H
#define KEXIPROJECTNAVIGATOR_H

#include <QHash>
#include <QEvent>
#include <QPointer>
#include <KMenu>
#include <kexi_export.h>
#include <kexi.h>

class KIcon;
class KAction;
class KActionMenu;
class KActionCollection;
class KToolBar;
class KexiProjectModelItem;
class KexiSmallToolButton;
class KexiProjectTreeView;
class QTreeView;
class QModelIndex;
class KexiProjectModel;

namespace KexiPart
{
    class Info;
    class Item;
    class Part;
}
class KexiProject;

/*! @internal */
class KexiMenuBase : public KMenu
{
public:
    KexiMenuBase(QWidget *parent, KActionCollection *collection);
    ~KexiMenuBase();

    QAction* addAction(const QString& actionName);

protected:
    QPointer<KActionCollection> m_actionCollection;
};

/*! @internal */
class KexiItemMenu : public KexiMenuBase
{
public:
    KexiItemMenu(QWidget *parent, KActionCollection *collection);
    ~KexiItemMenu();

    //! Rebuilds the menu entirely using information obtained from \a partInfo
    //! and \a partItem.
    void update(KexiPart::Info* partInfo, KexiPart::Item* partItem);
};

/*! @internal */
class KexiGroupMenu : public KexiMenuBase
{
public:
    KexiGroupMenu(QWidget *parent, KActionCollection *collection);
    ~KexiGroupMenu();

    //! Rebuilds the menu entirely using information obtained from \a partInfo.
    void update(KexiPart::Info* partInfo);
};


//! @short The Main Kexi navigator widget
class KEXIEXTWIDGETS_EXPORT KexiProjectNavigator : public QWidget
{
    Q_OBJECT

public:
    enum Feature {
        NoFeatures = 0,
        Writable = 1, //!< the browser supports actions that modify the project (e.g. delete, rename)
        ContextMenus = 2, //!< the browser supports context menu
        Toolbar = 4, //!< the browser displays
        SingleClickOpensItemOptionEnabled = 8, //!< enables "SingleClickOpensItem" option
        DefaultFeatures = Writable | ContextMenus | Toolbar
        | SingleClickOpensItemOptionEnabled //!< the default
    };
    Q_DECLARE_FLAGS(Features, Feature)

    KexiProjectNavigator(QWidget* parent, Features features = DefaultFeatures);
    virtual ~KexiProjectNavigator();

    /*! Sets project \a prj for this browser. If \a partManagerErrorMessages is not NULL
     it will be set to error message if there's a problem with loading any KexiPart.
     If \a itemsPartClass is empty (the default), items of all part classes are displayed,
     items for only one part class are displayed. In the latter case, no group (parent)
     items are displayed.
     Previous items are removed. */
    void setProject(KexiProject* prj, const QString& itemsPartClass = QString(),
                    QString* partManagerErrorMessages = 0, bool addAsSearchableModel = true);

    /*! \return items' part class previously set by setProject. Returns empty string
     if setProject() was not executed yet or itemsPartClass argument of setProject() was
     empty (i.e. all part classes are displayed). */
    QString itemsPartClass() const;

    KexiPart::Item* selectedPartItem() const;

    bool actionEnabled(const QString& actionName) const;

    KexiProjectModel* model() const;

public slots:

    virtual void setFocus();
    void updateItemName(KexiPart::Item& item, bool dirty);
    void selectItem(KexiPart::Item& item);
    void clearSelection();
    void clear();

    //! Sets by main window to disable actions that may try to modify the project.
    //! Does not disable actions like opening objects.
    void setReadOnly(bool set);

    bool isReadOnly() const;

signals:
    void openItem(KexiPart::Item*, Kexi::ViewMode viewMode);

    /*! this signal is emitted when user double clicked (or single -depending on settings)
     or pressed return ky on the part item.
     This signal differs from openItem() signal in that if the object is already opened
     in view mode other than \a viewMode, the mode is not changed. */
    void openOrActivateItem(KexiPart::Item*, Kexi::ViewMode viewMode);

    void newItem(KexiPart::Info*);

    void removeItem(KexiPart::Item*);

    void selectionChanged(KexiPart::Item* item);

    void executeItem(KexiPart::Item*);

    void exportItemToClipboardAsDataTable(KexiPart::Item*);

    void exportItemToFileAsDataTable(KexiPart::Item*);

    void printItem(KexiPart::Item*);

    void pageSetupForItem(KexiPart::Item*);

protected slots:
    //void slotContextMenu(QTreeView*, const QModelIndex &i, const QPoint &point);
    void slotExecuteItem(const QModelIndex &item);
    void slotSelectionChanged(const QModelIndex& i);
    void slotSettingsChanged(int);
    
    void slotNewObject();
    void slotOpenObject();
    void slotDesignObject();
    void slotEditTextObject();
    void slotRemove();
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotRename();
    void slotExecuteObject();
    void slotExportToClipboardAsDataTable();
    void slotExportToFileAsDataTable();
    void slotPrintObject();
    void slotPageSetupForObject();

protected:
    void itemRenameDone();

    KAction* addAction(const QString& name, const KIcon& icon, const QString& text,
                       const QString& toolTip, const QString& whatsThis, const char* slot);

    virtual void contextMenuEvent ( QContextMenuEvent *event );
    Features m_features;
    KexiProjectTreeView *m_list;
    KActionCollection *m_actions;

    KexiItemMenu *m_itemMenu;
    KexiGroupMenu *m_partMenu;
    KAction *m_deleteAction, *m_renameAction,
       *m_newObjectAction, 
    // *m_newObjectToolbarAction,
        *m_openAction, *m_designAction, *m_editTextAction,
        *m_executeAction,
        *m_dataExportToClipboardAction, *m_dataExportToFileAction;
#ifndef KEXI_NO_QUICK_PRINTING
    KAction *m_printAction, *m_pageSetupAction;
#endif

    KActionMenu* m_exportActionMenu;
    QAction *m_itemMenuTitle, *m_partMenuTitle,
    *m_exportActionMenu_sep, *m_pageSetupAction_sep;

    KexiPart::Info *m_prevSelectedPartInfo;
    KToolBar *m_toolbar;
    KexiSmallToolButton /* *m_newObjectToolButton,*/ *m_deleteObjectToolButton;
    
    bool m_singleClick;
    bool m_readOnly;
    KexiProjectModel *m_model;
    QString m_itemsPartClass;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KexiProjectNavigator::Features)


#endif

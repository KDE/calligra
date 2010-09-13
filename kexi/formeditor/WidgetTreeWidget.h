/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2008-2010 Jarosław Staniek <staniek@kde.org>

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

#ifndef KFD_WIDGETTREEWIDGET_H
#define KFD_WIDGETTREEWIDGET_H

#include <QTreeWidget>

#include <kexi_export.h>

class QContextMenuEvent;

namespace KFormDesigner
{

class ObjectTreeItem;
class Form;

//! @short An item in WidgetTreeWidget associated with an ObjectTreeItem.
class KFORMEDITOR_EXPORT WidgetTreeWidgetItem : public QTreeWidgetItem
{
public:
    WidgetTreeWidgetItem(WidgetTreeWidgetItem *parent, ObjectTreeItem *data);
    //! For TabStopDialog
    WidgetTreeWidgetItem(QTreeWidget *tree, ObjectTreeItem *data = 0);
    virtual ~WidgetTreeWidgetItem();

    //! \return the item name, ie the ObjectTreeItem name
    QString name() const;

    //! \return the ObjectTreeItem information associated to this item.
    ObjectTreeItem* data() const {
        return m_data;
    }

    //2.0 virtual void setOpen(bool o);

    //! Added to unhide.
    virtual QVariant data(int column, int role) const { return QTreeWidgetItem::data(column, role); }

    //! Reimplemented to alter sorting for certain widget types, e.g. tab pages.
    virtual bool operator<( const QTreeWidgetItem & other ) const;

    //! Used to alter sorting for certain widget types, e.g. tab pages.
    QString customSortingKey() const { return m_customSortingKey; }

protected:
    //! Reimplemented to draw custom contents (copied from Property Editor)
    //virtual void paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align);

    //! Reimplemented to draw custom contents (copied from Property Editor)
    //virtual void paintBranches(QPainter *p, const QColorGroup &cg, int w, int y, int h);

    //! Reimplemented to draw custom contents (copied from Property Editor)
    //2.0 virtual void setup();

    //! Initializes text, icon, selectable flag, custom serting key
    void init();
    void initTextAndIcon();

private:
    ObjectTreeItem *m_data;
    QString m_customSortingKey;
};

/*! @short A graphical view of Form's ObjectTree.
 This is a tree representin hierarchy of form widgets.
 The actually selected widgets are written bold
 and selected. Clicking on items selects the corresponding widgets on the form.
 */
class KFORMEDITOR_EXPORT WidgetTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    //! Options for the widget's behaviour or look
    enum Option {
        NoOptions = 0,
        DisableSelection = 1,  //!< disables item selection
        DisableContextMenu = 2 //!< disables context menu
    };
    Q_DECLARE_FLAGS(Options, Option)

    WidgetTreeWidget(QWidget *parent, Options options = NoOptions);

    virtual ~WidgetTreeWidget();

    //! @return selected tree item or 0 if there is no selection or more than one item is selected.
    WidgetTreeWidgetItem* selectedItem() const;

    //2.0 virtual QSize sizeHint() const;

    /*! Sets \a form as the current Form in the list. The list will automatically
     be filled with an item for each widget in the Form, and selection will be synced.
     Nothing happens if \a form is already the current Form.
     */
    void setForm(Form *form);

    //! \return the pixmap name for a given class, to be shown next to the widget name.
    QString iconNameForClass(const QByteArray &classname) const;

public slots:
    /*! Sets the widget \a w as selected item, so it will be written bold.
     It is added to current selection if \a add is true. */
    void selectWidget(QWidget *w, bool add = false);

    /*! Adds the ObjectTreeItem \a item in the list, with the appropriate parent. */
    void addItem(ObjectTreeItem *item);

    /*! Removess the ObjectTreeItem \a item from the list. */
    void removeItem(ObjectTreeItem *item);

    /*! Just renames the list item from \a oldname to \a newname. */
    void renameItem(const QByteArray &oldname, const QByteArray &newname);

protected slots:
    ///*! This slot is called when the user right-click a list item.
//     The widget context menu is shown, as inisde the Form. */
//2.0    void displayContextMenu(K3ListView *list, Q3ListViewItem *item, const QPoint &p);

    //2.0 void slotColumnSizeChanged(int);

    /*! The selected list item has changed, so we emit a signal to update the Form. */
    void slotSelectionChanged();

    /*! Called before Form object is destroyed. */
    void slotBeforeFormDestroyed();

protected:
    //! Internal function to fill the list.
    void loadTree(ObjectTreeItem *item, WidgetTreeWidgetItem *parent);

    //! \return The item whose name is \a name.
    WidgetTreeWidgetItem* findItem(const QString &name);

    virtual void contextMenuEvent(QContextMenuEvent* e);

    void handleContextMenuEvent(QContextMenuEvent* e);

    void selectWidgetForItem(QTreeWidgetItem *item);

private:
    Form *m_form;
    //2.0 WidgetTreeWidgetItem *m_topItem;
    Options m_options;

    //! Used to temporarily disable slotSelectionChanged() when reloading contents in setForm().
    bool m_slotSelectionChanged_enabled;

    friend class TabStopDialog;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(WidgetTreeWidget::Options)

}

#endif

/* This file is part of the KDE project
  Copyright (C) 2006 - 2009 Dag Andersen <koffice-devel@kde.org>

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

#ifndef KPTITEMMODELBASE_H
#define KPTITEMMODELBASE_H

#include "kplatomodels_export.h"

#include "kptglobal.h"
#include "kpttreecombobox.h"

#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QMetaEnum>
#include <QEvent>
#include <QComboBox>

#include <KoXmlReaderForward.h>

class KoDocument;
class QDomElement;
class QUndoCommand;
class QPainter;
class QModelIndex;

class QItemSelection;

/// The main namespace
namespace KPlato
{

class Project;

/// Namespace for item delegate specific enums
namespace Delegate
{
    /// For selector delegate
    enum EditorType { EnumEditor, TimeEditor };
    /// Controls action when editor is closed. See QAbstractItemDelegate::EndEditHint.
    enum EndEditHint { 
        NoHint = QAbstractItemDelegate::NoHint,
        EditNextItem = QAbstractItemDelegate::EditNextItem,
        EditPreviousItem = QAbstractItemDelegate::EditPreviousItem,
        SubmitModelCache = QAbstractItemDelegate::SubmitModelCache,
        RevertModelCache = QAbstractItemDelegate::RevertModelCache,
        EditLeftItem = 100,
        EditRightItem = 101,
        EditDownItem = 102,
        EditUpItem = 103
    };
}

/// ItemDelegate implements improved control over closeEditor
class KPLATOMODELS_EXPORT ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /// Constructor
    explicit ItemDelegate(QObject *parent = 0)
    : QStyledItemDelegate( parent ),
    m_lastHint( Delegate::NoHint )
    {}
    
    /// Extend EndEditHint for movement from edited item to next item to edit
    Delegate::EndEditHint endEditHint() const { return m_lastHint; }
    /// Increase the sizehint height a little to give room for editors
    QSize sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

protected:
    /// Implements arrow key navigation
    bool eventFilter(QObject *object, QEvent *event);
    /// Draw custom focus
//    virtual void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect ) const;
    
private:
    Delegate::EndEditHint m_lastHint;
};

class KPLATOMODELS_EXPORT ProgressBarDelegate : public ItemDelegate
{
  Q_OBJECT
public:
    ProgressBarDelegate( QObject *parent = 0 );

    ~ProgressBarDelegate();

    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    void setEditorData( QWidget *editor, const QModelIndex &index ) const;
    void setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;
    void updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

protected:
    void initStyleOptionProgressBar( QStyleOptionProgressBar *option, const QModelIndex &index ) const;

};

class Slider : public QSlider {
    Q_OBJECT
public:
    explicit Slider( QWidget *parent );
private slots:
    void updateTip( int value );
};

class KPLATOMODELS_EXPORT SelectorDelegate : public ItemDelegate
{
    Q_OBJECT
public:
    SelectorDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class KPLATOMODELS_EXPORT EnumDelegate : public ItemDelegate
{
    Q_OBJECT
public:
    EnumDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

//------------------------------------
class KPLATOMODELS_EXPORT RequieredResourceDelegate : public ItemDelegate
{
    Q_OBJECT
public:
    RequieredResourceDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


class KPLATOMODELS_EXPORT DurationSpinBoxDelegate : public ItemDelegate
{
    Q_OBJECT
public:
    DurationSpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class KPLATOMODELS_EXPORT SpinBoxDelegate : public ItemDelegate
{
    Q_OBJECT
public:
    SpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class KPLATOMODELS_EXPORT DoubleSpinBoxDelegate : public ItemDelegate
{
    Q_OBJECT
public:
    DoubleSpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class KPLATOMODELS_EXPORT MoneyDelegate : public ItemDelegate
{
    Q_OBJECT
public:
    MoneyDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class KPLATOMODELS_EXPORT TimeDelegate : public ItemDelegate
{
    Q_OBJECT
public:
    TimeDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class KPLATOMODELS_EXPORT ItemModelBase : public QAbstractItemModel
{
    Q_OBJECT
public:
    // FIXME: Refactor, This is a copy from protected enum in QAbstractItemView
    enum DropIndicatorPosition {
        OnItem, /*QAbstractItemView::OnItem*/  /// The item will be dropped on the index.
        AboveItem, /*QAbstractItemView::AboveItem*/ /// The item will be dropped above the index.
        BelowItem, /*QAbstractItemView::BelowItem*/  /// The item will be dropped below the index.
        OnViewport /*QAbstractItemView::OnViewport*/ /// The item will be dropped onto a region of the viewport with no items if acceptDropsOnView is set.
    };

    explicit ItemModelBase( QObject *parent = 0 );
    virtual ~ItemModelBase();

    virtual const QMetaEnum columnMap() const { return QMetaEnum(); }
    Project *project() const { return m_project; }
    virtual void setProject( Project *project );
    virtual void setReadWrite( bool rw ) { m_readWrite = rw; }
    bool isReadWrite() { return m_readWrite; }
    void setReadOnly( int column, bool ro ) { m_columnROMap[ column ] = ro; }
    /// Returns true if @p column has been set to ReadOnly.
    bool isColumnReadOnly( int column ) const { return m_columnROMap.contains( column ) && m_columnROMap[ column ]; }

    /**
     * Check if the @p data is allowed to be dropped on @p index,
     * @p dropIndicatorPosition indicates position relative @p index.
     *
     * Base implementation checks flags and mimetypes.
     */
    virtual bool dropAllowed( const QModelIndex &index, int dropIndicatorPosition, const QMimeData *data );
    
    /// Create the correct delegate for @p column. @p parent is the delegates parent widget.
    /// If default should be used, return 0.
    virtual QAbstractItemDelegate *createDelegate( int column, QWidget *parent ) const { Q_UNUSED(column); Q_UNUSED(parent); return 0; }

    bool setData( const QModelIndex &index, const QVariant &value, int role );

signals:
    /// Connect to this signal if your model modifies data using undo commands.
    void executeCommand( QUndoCommand* );
    
protected slots:
    virtual void slotLayoutToBeChanged();
    virtual void slotLayoutChanged();

protected:
    Project *m_project;
    bool m_readWrite;
    QMap<int, bool> m_columnROMap;
};


} // namespace KPlato

#endif

/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
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
#ifndef KEXIDBFORM_H
#define KEXIDBFORM_H

#include <qpixmap.h>
#include <QDragMoveEvent>
#include <QEvent>
#include <QDropEvent>

#include <formeditor/form.h>
#include <formeditor/FormWidget.h>
#include "../kexiformdataiteminterface.h"

#ifdef KEXI_USE_GRADIENT_WIDGET
#include <kexigradientwidget.h>
# define KexiDBFormBase KexiGradientWidget
#else
# define KexiDBFormBase QWidget
#endif

class KexiDataAwareObjectInterface;
class KexiFormScrollView;

//! @short A DB-aware form widget, acting as form's toplevel widget
class KEXIFORMUTILS_EXPORT KexiDBForm :
            public KexiDBFormBase,
            public KFormDesigner::FormWidget,
            public KexiFormDataItemInterface
{
    Q_OBJECT
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString dataSourcePartClass READ dataSourcePartClass WRITE setDataSourcePartClass)
    Q_PROPERTY(bool autoTabStops READ autoTabStops WRITE setAutoTabStops)
    //original "size" property is not designable, so here's a custom (not storable) replacement
    Q_PROPERTY(QSize sizeInternal READ sizeInternal WRITE resizeInternal STORED false)
public:
    KexiDBForm(QWidget *parent, KexiDataAwareObjectInterface* dataAwareObject);
    virtual ~KexiDBForm();

    KexiDataAwareObjectInterface* dataAwareObject() const;

    inline QString dataSource() const {
        return KexiFormDataItemInterface::dataSource();
    }
    inline QString dataSourcePartClass() const {
        return KexiFormDataItemInterface::dataSourcePartClass();
    }

    //! no effect
    QVariant value() {
        return QVariant();
    }

    virtual void setInvalidState(const QString& displayText);

    virtual void drawRect(const QRect& r, int type);
    virtual void drawRects(const QList<QRect> &list, int type);
    virtual void initBuffer();
    virtual void clearForm();
    virtual void highlightWidgets(QWidget *from, QWidget *to/*, const QPoint &p*/);

    virtual QSize sizeHint() const;

    bool autoTabStops() const;

    QList<QWidget*>* orderedFocusWidgets() const;

    QList<QWidget*>* orderedDataAwareWidgets() const;

    void updateTabStopsOrder(KFormDesigner::Form* form);

    void updateTabStopsOrder();

    virtual bool eventFilter(QObject * watched, QEvent * e);

    virtual bool valueIsNull();
    virtual bool valueIsEmpty();
    virtual bool isReadOnly() const;
    virtual QWidget* widget();
    virtual bool cursorAtStart();
    virtual bool cursorAtEnd();
    virtual void clear();

    bool preview() const;

    virtual void setCursor(const QCursor & cursor);

public slots:
    void setAutoTabStops(bool set);
    inline void setDataSource(const QString &ds) {
        KexiFormDataItemInterface::setDataSource(ds);
    }
    inline void setDataSourcePartClass(const QString &partClass) {
        KexiFormDataItemInterface::setDataSourcePartClass(partClass);
    }

    //! This implementation just disables read only widget
    virtual void setReadOnly(bool readOnly);

    //! @internal for sizeInternal property
    QSize sizeInternal() const {
        return KexiDBFormBase::size();
    }

    //! @internal for sizeInternal property
    void resizeInternal(const QSize& s) {
        KexiDBFormBase::resize(s);
    }

signals:
    void handleDragMoveEvent(QDragMoveEvent *e);
    void handleDropEvent(QDropEvent *e);

protected:
    //! no effect
    virtual void setValueInternal(const QVariant&, bool) {}

    //! Used to emit handleDragMoveEvent() signal needed to control dragging over the container's surface
    virtual void dragMoveEvent(QDragMoveEvent *e);

    //! Used to emit handleDropEvent() signal needed to control dropping on the container's surface
    virtual void dropEvent(QDropEvent *e);

    //! called from KexiFormScrollView::initDataContents()
    void updateReadOnlyFlags();
//  virtual void paintEvent( QPaintEvent * );

    //! Points to a currently edited data item.
    //! It is cleared when the focus is moved to other
    KexiFormDataItemInterface *editedItem;

    class Private;
    Private * const d;

    friend class KexiFormScrollView;
};

#endif

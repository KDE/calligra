/* This file is part of the KDE project
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIDBLABEL_H
#define KEXIDBLABEL_H

#include <QLabel>
#include <QPixmap>
#include <QPaintEvent>
#include <QShowEvent>
#include <QResizeEvent>

#include "../kexiformdataiteminterface.h"
#include "../kexidbtextwidgetinterface.h"
#include <widget/utils/kexidisplayutils.h>
#include <kexi_global.h>

class QPainter;

//! @short An extended, data-aware, read-only text label.
/*! It's text may have a drop-shadow.

 @author Christian Nitschkowski, Jarosław Staniek
*/
class KEXIFORMUTILS_EXPORT KexiDBLabel : public QLabel, protected KexiDBTextWidgetInterface, public KexiFormDataItemInterface
{
    Q_OBJECT
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString dataSourcePartClass READ dataSourcePartClass WRITE setDataSourcePartClass)
    Q_PROPERTY(bool shadowEnabled READ shadowEnabled WRITE setShadowEnabled)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap DESIGNABLE false)
    Q_PROPERTY(bool scaledContents READ hasScaledContents WRITE setScaledContents DESIGNABLE false)
    Q_PROPERTY(QColor frameColor READ frameColor WRITE setFrameColor)

public:
    KexiDBLabel(QWidget *parent, Qt::WFlags f = 0);
    KexiDBLabel(const QString& text, QWidget *parent, Qt::WFlags f = 0);
    virtual ~KexiDBLabel();

    inline QString dataSource() const {
        return KexiFormDataItemInterface::dataSource();
    }
    inline QString dataSourcePartClass() const {
        return KexiFormDataItemInterface::dataSourcePartClass();
    }

    virtual QVariant value();

    bool shadowEnabled() const;

    virtual void setInvalidState(const QString& displayText);

    virtual bool valueIsNull();

    virtual bool valueIsEmpty();

    //! always true
    virtual bool isReadOnly() const;

    virtual QWidget* widget();

    //! always false
    virtual bool cursorAtStart();

    //! always false
    virtual bool cursorAtEnd();

    virtual void clear();

    //! used to catch setIndent(), etc.
    virtual bool setProperty(const char * name, const QVariant & value);

    virtual const QColor& frameColor() const;

//  const QColor & paletteForegroundColor() const;

    const QPixmap *pixmap() const { return QLabel::pixmap(); }
public slots:
    //! Sets the datasource to \a ds
    inline void setDataSource(const QString &ds) {
        KexiFormDataItemInterface::setDataSource(ds);
    }

    inline void setDataSourcePartClass(const QString &partClass) {
        KexiFormDataItemInterface::setDataSourcePartClass(partClass);
    }

    virtual void setText(const QString& text);

    /*! Enable/Disable the shadow effect.
     KexiDBLabel acts just like a normal QLabel when shadow is disabled. */
    void setShadowEnabled(bool state);

    virtual void setPalette(const QPalette &pal);

    virtual void setFrameColor(const QColor& color);

//  void setPaletteForegroundColor( const QColor& color );

protected slots:
    //! empty
    virtual void setReadOnly(bool readOnly);
//2.0    void updatePixmap();

protected:
    void init();
    virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent* e);

    //! Sets value \a value for a widget.
    virtual void setValueInternal(const QVariant& add, bool removeOld);

    virtual void fontChange(const QFont& font);
    virtual void styleChange(QStyle& style);
    virtual void enabledChange(bool enabled);

    virtual void paletteChange(const QPalette& oldPal);
#ifdef __GNUC__
#warning TODO virtual void frameChanged();
#else
#pragma WARNING( TODO virtual void frameChanged(); )
#endif
    virtual void showEvent(QShowEvent* e);

    //! Reimplemented to paint using real frame color instead of froeground.
    //! Also allows to paint more types of frame.
    virtual void drawFrame(QPainter *);

    void updatePixmapLater();

    class Private;
    Private * const d;
};

#endif

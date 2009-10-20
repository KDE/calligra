/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIDBAUTOFIELD_H
#define KEXIDBAUTOFIELD_H

#include <qwidget.h>
//Added by qt3to4:
#include <QLabel>
#include <kexidb/field.h>
#include <formeditor/container.h>
#include <formeditor/widgetwithsubpropertiesinterface.h>
#include <formeditor/FormWidgetInterface.h>
#include "kexiformdataiteminterface.h"

class QLabel;

//! Universal "Auto Field" widget for Kexi forms
/*! It acts as a container for most data-aware widgets. */
class KEXIFORMUTILS_EXPORT KexiDBAutoField :
            public QWidget,
            public KexiFormDataItemInterface,
            public KFormDesigner::DesignTimeDynamicChildWidgetHandler,
            public KFormDesigner::WidgetWithSubpropertiesInterface,
            public KFormDesigner::FormWidgetInterface
{
    Q_OBJECT
//'caption' is uncovered now Q_PROPERTY(QString labelCaption READ caption WRITE setCaption)
    Q_PROPERTY(QString caption READ caption WRITE setCaption)
//2.0    Q_PROPERTY(QColor paletteForegroundColor READ paletteForegroundColor WRITE setPaletteForegroundColor RESET unsetPalette)
//2.0    Q_PROPERTY(QColor paletteBackgroundColor READ paletteBackgroundColor WRITE setPaletteBackgroundColor RESET unsetPalette)
    Q_PROPERTY(QColor foregroundLabelColor READ foregroundLabelColor WRITE setForegroundLabelColor RESET unsetPalette)
    Q_PROPERTY(QColor backgroundLabelColor READ backgroundLabelColor WRITE setBackgroundLabelColor RESET unsetPalette)
    Q_PROPERTY(bool autoCaption READ hasAutoCaption WRITE setAutoCaption)
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString dataSourcePartClass READ dataSourcePartClass WRITE setDataSourcePartClass)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(LabelPosition labelPosition READ labelPosition WRITE setLabelPosition)
    Q_PROPERTY(WidgetType widgetType READ widgetType WRITE setWidgetType)
    /*internal, for design time only*/
    Q_PROPERTY(int fieldTypeInternal READ fieldTypeInternal WRITE setFieldTypeInternal STORED false)
    Q_PROPERTY(QString fieldCaptionInternal READ fieldCaptionInternal WRITE setFieldCaptionInternal STORED false)
    Q_ENUMS(WidgetType LabelPosition)

public:
    enum WidgetType { Auto = 100, Text, Integer, Double, Boolean, Date, Time, DateTime,
                      MultiLineText, ComboBox, Image
                    };
    enum LabelPosition { Left = 300, Top, NoLabel };

    KexiDBAutoField(const QString &text, WidgetType type, LabelPosition pos,
                    QWidget *parent = 0);
    KexiDBAutoField(QWidget *parent = 0, LabelPosition pos = Left);

    virtual ~KexiDBAutoField();

    inline QString dataSource() const {
        return KexiFormDataItemInterface::dataSource();
    }
    inline QString dataSourcePartClass() const {
        return KexiFormDataItemInterface::dataSourcePartClass();
    }
    virtual void setDataSource(const QString &ds);
    virtual void setDataSourcePartClass(const QString &partClass) {
        KexiFormDataItemInterface::setDataSourcePartClass(partClass);
    }
    virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);

    virtual void setInvalidState(const QString& text);
    virtual bool isReadOnly() const;
    virtual void setReadOnly(bool readOnly);

    virtual QVariant value();
    virtual bool valueIsNull();
    virtual bool valueIsEmpty();
    virtual bool valueIsValid();
    virtual bool valueChanged();
    virtual void clear();

    //! Reimpelmented to also install \a listenter for internal editor
    virtual void installListener(KexiDataItemChangesListener* listener);

    WidgetType widgetType() const;
    void setWidgetType(WidgetType type);

    LabelPosition labelPosition() const;
    virtual void setLabelPosition(LabelPosition position);

    QString caption() const;
    void setCaption(const QString &caption);

    bool hasAutoCaption() const;
    void setAutoCaption(bool autoCaption);

    /*! If \a displayDefaultValue is true, the value set by KexiDataItemInterface::setValue()
     is displayed in a special way. Used by KexiFormDataProvider::fillDataItems().
     \a widget is equal to 'this'.
     Reimplemented after KexiFormDataItemInterface. */
    virtual void setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue);

    QWidget* editor() const;
    QLabel* label() const;

    virtual bool cursorAtStart();
    virtual bool cursorAtEnd();

    static WidgetType widgetTypeForFieldType(KexiDB::Field::Type type);

    /*! On design time it is not possible to pass a reference to KexiDB::Field object
     so we're just providing field type. Only used when widget type is Auto.
     @internal */
    void setFieldTypeInternal(int kexiDBFieldType);

    /*! On design time it is not possible to pass a reference to KexiDB::Field object
     so we're just providing field caption. Only used when widget type is Auto.
     @internal */
    void setFieldCaptionInternal(const QString& text);

    /*! @internal */
    int fieldTypeInternal() const;

    /*! @internal */
    QString fieldCaptionInternal() const;

    virtual QSize sizeHint() const;
    virtual void setFocusPolicy(Qt::FocusPolicy policy);

    //! Reimplemented to return internal editor's color.
    const QColor & paletteForegroundColor() const;

    //! Reimplemented to set internal editor's color.
    void setPaletteForegroundColor(const QColor & color);

    //! Reimplemented to return internal editor's color.
    const QColor & paletteBackgroundColor() const;

    //! Reimplemented to set internal editor's color.
    virtual void setPaletteBackgroundColor(const QColor & color);

    //! \return label's foreground color
    const QColor & foregroundLabelColor() const;

    //! Sets label's foreground color
    virtual void setForegroundLabelColor(const QColor & color);

    //! \return label's background color
    const QColor & backgroundLabelColor() const;

    //! Sets label's background color
    virtual void setBackgroundLabelColor(const QColor & color);

    //! Reimplemented to accept subproperties. @see KFormDesigner::WidgetWithSubpropertiesInterface
    virtual QVariant property(const char * name) const;

    //! Reimplemented to accept subproperties. @see KFormDesigner::WidgetWithSubpropertiesInterface
    virtual bool setProperty(const char * name, const QVariant & value);

    /*! Called by the top-level form on key press event to consume widget-specific shortcuts. */
    virtual bool keyPressed(QKeyEvent *ke);

public slots:
    virtual void unsetPalette();

protected slots:
//  void slotValueChanged();
    virtual void paletteChange(const QPalette& oldPal);

    //! Implemented for KexiDataItemInterface
    virtual void moveCursorToEnd();

    //! Implemented for KexiDataItemInterface
    virtual void moveCursorToStart();

    //! Implemented for KexiDataItemInterface
    virtual void selectAll();

protected:
    virtual void setValueInternal(const QVariant&add, bool removeOld);
    void init(const QString &text, WidgetType type, LabelPosition pos);
    virtual void createEditor();
    void changeText(const QString &text, bool beautify = true);
//  virtual void paintEvent( QPaintEvent* pe );
    void updateInformationAboutUnboundField();

    //! internal editor can be created too late, so certain properties should be copied
    void copyPropertiesToEditor();

    virtual bool eventFilter(QObject *o, QEvent *e);

    //! Used by @ref setLabelPositionInternal(LabelPosition)
    void setLabelPositionInternal(LabelPosition position, bool noLabel);

    //! Used by KexiDBAutoField::setColumnInfo() and KexiDBComboBox::setColumnInfo()
    void setColumnInfoInternal(KexiDB::QueryColumnInfo* cinfo, KexiDB::QueryColumnInfo* visibleColumnInfo);

private:
    class Private;
    Private * const d;
};

#endif

/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KexiDBImageBox_H
#define KexiDBImageBox_H

#include <widget/dataviewcommon/kexiformdataiteminterface.h>
#include "kexiframe.h"
#include "kexidbutils.h"
#include <QContextMenuEvent>
#include <QPixmap>
#include <QPaintEvent>
#include <kexiblobbuffer.h>

#include <widget/utils/kexicontextmenuutils.h>

class KexiDropDownButton;
class KexiImageContextMenu;

//! @short A data-aware, editable image box.
/*! Can also act as a normal static image box.
*/
class KEXIFORMUTILS_EXPORT KexiDBImageBox : public KexiFrame,
                                            public KexiFormDataItemInterface,
                                            public KexiSubwidgetInterface
{
    Q_OBJECT
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString dataSourcePartClass READ dataSourcePartClass WRITE setDataSourcePartClass)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
// Q_PROPERTY( QPixmap pixmap READ pixmap WRITE setPixmap )
// Q_PROPERTY( QByteArray pixmapData READ pixmapData WRITE setPixmapData )
    Q_PROPERTY(uint pixmapId READ pixmapId WRITE setPixmapId STORED false)
    Q_PROPERTY(uint storedPixmapId READ storedPixmapId WRITE setStoredPixmapId DESIGNABLE false STORED true)
    Q_PROPERTY(bool scaledContents READ hasScaledContents WRITE setScaledContents)
    Q_PROPERTY(bool smoothTransformation READ smoothTransformation WRITE setSmoothTransformation)
    Q_PROPERTY(bool keepAspectRatio READ keepAspectRatio WRITE setKeepAspectRatio)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
// Q_PROPERTY( QString originalFileName READ originalFileName WRITE setOriginalFileName DESIGNABLE false )
// Q_PROPERTY( FocusPolicy focusPolicy READ focusPolicy WRITE setFocusPolicy )
    Q_PROPERTY(bool dropDownButtonVisible READ dropDownButtonVisible WRITE setDropDownButtonVisible)
    Q_PROPERTY(Qt::FocusPolicy focusPolicy READ focusPolicyInternal WRITE setFocusPolicy)
    // overriden to update image geometry
    Q_ENUMS(Shape Shadow)
    Q_PROPERTY(Shape frameShape READ frameShape WRITE setFrameShape)
    Q_PROPERTY(Shadow frameShadow READ frameShadow WRITE setFrameShadow)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(int midLineWidth READ midLineWidth WRITE setMidLineWidth)

public:
    KexiDBImageBox(bool designMode, QWidget *parent);
    virtual ~KexiDBImageBox();

    inline QString dataSource() const {
        return KexiFormDataItemInterface::dataSource();
    }
    inline QString dataSourcePartClass() const {
        return KexiFormDataItemInterface::dataSourcePartClass();
    }

    virtual QVariant value(); // { return m_value.data(); }

//  QByteArray pixmapData() const { return m_value.data(); }

    QPixmap pixmap() const;

    uint pixmapId() const;

    uint storedPixmapId() const;
//
    virtual void setInvalidState(const QString& displayText);

    virtual bool valueIsNull();

    virtual bool valueIsEmpty();

    virtual QWidget* widget();

    //! always true
    virtual bool cursorAtStart();

    //! always true
    virtual bool cursorAtEnd();

//  //! used to catch setIndent(), etc.
//  virtual bool setProperty ( const char * name, const QVariant & value );

    virtual bool isReadOnly() const;

    bool hasScaledContents() const;

    bool smoothTransformation() const;

    Qt::Alignment alignment() const {
        return m_alignment;
    }

    bool keepAspectRatio() const {
        return m_keepAspectRatio;
    }

    virtual QSize sizeHint() const;

    KexiImageContextMenu *contextMenu() const;

    /*! \return original file name of image loaded from a file.
     This can be later reused for displaying the image within a collection (to be implemented)
     or on saving the image data back to file. */
//todo  QString originalFileName() const { return m_value.originalFileName(); }

    //! Reimplemented to override behaviour of "paletteBackgroundColor"
    //! and "paletteForegroundColor" properties.
    virtual void setPalette(const QPalette &pal);

    //! Reimplemented to override behaviour of "paletteBackgroundColor" property.
    virtual void setPaletteBackgroundColor(const QColor & color);

    //! \return true id drop down button should be visible (the default).
    bool dropDownButtonVisible() const;

    //! For overridden property
    int lineWidth() const {
        return KexiFrame::lineWidth();
    }

    /*! Overridden to change the policy behaviour a bit:
     NoFocus is returned regardless the real focus flag
     if the data source is empty (see dataSource()). */
#ifdef __GNUC__
#warning TODO focusPolicy() is not virtual!
#else
#pragma WARNING( TODO focusPolicy() is not virtual! )
#endif
    Qt::FocusPolicy focusPolicy() const;

    //! \return the internal focus policy value, i.e. the one unrelated to data source presence.
    Qt::FocusPolicy focusPolicyInternal() const;

    /*! Sets the internal focus policy value.
     "Internal" means that if there is no data source set, real policy becomes NoFocus. */
    virtual void setFocusPolicy(Qt::FocusPolicy policy);

public slots:
    void setPixmapId(uint id);

    void setStoredPixmapId(uint id);

    //! Sets the datasource to \a ds
    virtual void setDataSource(const QString &ds);

    inline void setDataSourcePartClass(const QString &partClass) {
        KexiFormDataItemInterface::setDataSourcePartClass(partClass);
    }

    virtual void setReadOnly(bool set);

    //! Sets \a pixmapData data for this widget. If the widget has data source set,
    //! the pixmap will be also placed inside of the buffer and saved later.
//todo  void setPixmapData(const QByteArray& pixmapData) { m_value.setData(pixmapData); }

    /*! Sets original file name of image loaded from a file.
     @see originalFileName() */
//todo  void setOriginalFileName(const QString& name) { m_value.setOriginalFileName(name); }

    void setScaledContents(bool set);

    void setSmoothTransformation(bool set);

    void setAlignment(Qt::Alignment alignment);

    void setKeepAspectRatio(bool set);

//  void updateActionsAvailability();

    //! @internal
//  void slotToggled( bool on );

    //! \return sets dropDownButtonVisible property. @see dropDownButtonVisible()
    void setDropDownButtonVisible(bool set);

    //! Forces execution of "insert from file" action
    void insertFromFile();

    void setFrameShape(QFrame::Shape s);

    void setFrameShadow(QFrame::Shadow s);

    void setLineWidth(int w);

    void setMidLineWidth(int w);

signals:
    //! Used for db-aware mode. Emitted when value has been changed.
    //! Actual value can be obtained using value().
//  virtual void pixmapChanged();
//  virtual void valueChanged(const QByteArray& data);

    void idChanged(long id);

protected slots:
    void slotUpdateActionsAvailabilityRequested(bool& valueIsNull, bool& valueIsReadOnly);

    void handleInsertFromFileAction(const KUrl& url);
    void handleAboutToSaveAsAction(QString& origFilename, QString& fileExtension, bool& dataIsEmpty);
    void handleSaveAsAction(const QString& fileName);
    void handleCutAction();
    void handleCopyAction();
    void handlePasteAction();
    virtual void clear();
    void handleShowPropertiesAction();

protected:
    //! \return data depending on the current mode (db-aware or static)
    QByteArray data() const;

    virtual void contextMenuEvent(QContextMenuEvent * e);
//  virtual void mousePressEvent( QMouseEvent *e );
    virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent* e);
    virtual bool eventFilter(QObject * watched, QEvent * e);

    //! Sets value \a value for a widget.
    virtual void setValueInternal(const QVariant& add, bool removeOld) {
        setValueInternal(add, removeOld, true /*loadPixmap*/);
    }

    //! @internal, added \a loadPixmap option used by paste().
    void setValueInternal(const QVariant& add, bool removeOld, bool loadPixmap);

    //! Updates i18n'd action strings after datasource change
    void updateActionStrings();
    void updatePixmap();

    //! @internal
    void setData(const KexiBLOBBuffer::Handle& handle);

    bool popupMenuAvailable();

    /*! Called by top-level form on key press event.
     Used for Key_Escape to if the popup is visible,
     so the key press won't be consumed to perform "cancel editing". */
    virtual bool keyPressed(QKeyEvent *ke);

    //! \return real line width, i.e. for Boxed sunken or Boxed raised
    //! frames returns doubled width value.
    int realLineWidth() const;

    //! Implemented for KexiSubwidgetInterface
    virtual bool subwidgetStretchRequired(KexiDBAutoField* autoField) const;

//  virtual void drawContents ( QPainter *p );

//  virtual void fontChange( const QFont& font );
//  virtual void styleChange( QStyle& style );
//  virtual void enabledChange( bool enabled );

//  virtual void paletteChange( const QPalette& pal );
//  virtual void frameChanged();
//  virtual void showEvent( QShowEvent* e );

//  void updatePixmapLater();
//  class ImageLabel;
//  ImageLabel *m_pixmapLabel;
    QPixmap m_pixmap;
    QByteArray m_value; //!< for db-aware mode
    QString m_valueMimeType; //!< for db-aware mode
//  PixmapData m_value;
    KexiBLOBBuffer::Handle m_data;
//  QString m_originalFileName;
    KexiDropDownButton *m_chooser;
    QPointer<KexiImageContextMenu> m_contextMenu;
//moved  KActionCollection m_actionCollection;
//moved  KAction *m_insertFromFileAction, *m_saveAsAction, *m_cutAction, *m_copyAction, *m_pasteAction,
//   *m_deleteAction, *m_propertiesAction;
//  QTimer m_clickTimer;
    Qt::Alignment m_alignment;
    Qt::FocusPolicy m_focusPolicyInternal; //!< Used for focusPolicyInternal()
//2.0 moved to FormWidgetInterface    bool m_designMode : 1;

    QPixmap m_currentScaledPixmap; //!< for caching
    QRect m_currentRect;           //!< for caching
    QPoint m_currentPixmapPos;     //!< for caching

    bool m_readOnly;
    bool m_scaledContents;
    bool m_smoothTransformation;
    bool m_keepAspectRatio;
    bool m_insideSetData;
    bool m_setFocusOnButtonAfterClosingPopup;
//    bool m_lineWidthChanged : 1;
    bool m_paletteBackgroundColorChanged;
    bool m_paintEventEnabled; //!< used to disable paintEvent()
    bool m_dropDownButtonVisible;
    bool m_insideSetPalette;
};

#endif

// -*- c++ -*-

/*
 *   Copyright (C) 2001-2006 by Richard Moore <rich@kde.org>
 *   Copyright (C) 2004-2005 by Sascha Cunz <sascha.cunz@tiscali.de>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef KPASSIVEPOPUP_H
#define KPASSIVEPOPUP_H

#include <kwidgetsaddons_export.h>

#include <QFrame>

class QSystemTrayIcon;

/**
 * @short A dialog-like popup that displays messages without interrupting the user.
 *
 * The simplest uses of KPassivePopup are by using the various message() static
 * methods. The position the popup appears at depends on the type of the parent window:
 *
 * @li Normal Windows: The popup is placed adjacent to the icon of the window.
 * @li System Tray Windows: The popup is placed adjact to the system tray window itself.
 * @li Skip Taskbar Windows: The popup is placed adjact to the window
 *     itself if it is visible, and at the edge of the desktop otherwise.
 *
 * You also have the option of calling show with a QPoint as a parameter that
 * removes the automatic placing of KPassivePopup and shows it in the point you want.
 *
 * The most basic use of KPassivePopup displays a popup containing a piece of text:
 * \code
 *    KPassivePopup::message( "This is the message", this );
 * \endcode
 * We can create popups with titles and icons too, as this example shows:
 * \code
 *    QPixmap px;
 *    px.load( "hi32-app-logtracker.png" );
 *    KPassivePopup::message( "Some title", "This is the main text", px, this );
 * \endcode
 * This screenshot shows a popup with both a caption and a main text which is
 * being displayed next to the toolbar icon of the window that triggered it:
 * \image html kpassivepopup.png "A passive popup"
 *
 * For more control over the popup, you can use the setView(QWidget *) method
 * to create a custom popup.
 * \code
 *    KPassivePopup *pop = new KPassivePopup( parent );
 *
 *    QWidget* content = new QWidget( pop );
 *    QVBoxLayout* vBox = new QVBoxLayout( content );
 *    QLabel* label = new QLabel( "<b>Isn't this great?</b>", content );
 *    vBox->addWidget( label );
 *
 *    QPushButton* btnYes = new QPushButton( "Yes", content );
 *    QPushButton* btnNo = new QPushButton( "No", content );
 *
 *    QHBoxLayout* hBox = new QHBoxLayout( content );
 *    hBox->addWidget( btnYes );
 *    hBox->addWidget( btnNo );
 *
 *    vBox->addLayout( vBox );
 *
 *    pop->setView( content );
 *    pop->show();
 * \endcode
 *
 * @author Richard Moore, rich@kde.org
 * @author Sascha Cunz, sascha.cunz@tiscali.de
 */
class KWIDGETSADDONS_EXPORT KPassivePopup : public QFrame
{
    Q_OBJECT
    Q_PROPERTY (bool autoDelete READ autoDelete WRITE setAutoDelete )
    Q_PROPERTY (int timeout READ timeout WRITE setTimeout )
    Q_PROPERTY (QRect defaultArea READ defaultArea )

public:
    /**
     * Styles that a KPassivePopup can have.
     */
    enum PopupStyle
    {
        Boxed,             ///< Information will appear in a framed box (default)
        Balloon,           ///< Information will appear in a comic-alike balloon
	CustomStyle=128    ///< Ids greater than this are reserved for use by subclasses
    };

    /**
     * Creates a popup for the specified widget.
     */
    explicit KPassivePopup( QWidget *parent=0, Qt::WindowFlags f = 0 );

    /**
     * Creates a popup for the specified window.
     */
    explicit KPassivePopup( WId parent );

#if 0 // These break macos and win32 where the definition of WId makes them ambiguous
    /**
     * Creates a popup for the specified widget.
     * THIS WILL BE REMOVED, USE setPopupStyle().
     */
    explicit KPassivePopup( int popupStyle, QWidget *parent=0, Qt::WindowFlags f=0 ) KDEUI_DEPRECATED;

    /**
     * Creates a popup for the specified window.
     * THIS WILL BE REMOVED, USE setPopupStyle().
     */
    KPassivePopup( int popupStyle, WId parent, Qt::WindowFlags f=0 ) KDEUI_DEPRECATED;
#endif

    /**
     * Cleans up.
     */
    virtual ~KPassivePopup();

    /**
     * Sets the main view to be the specified widget (which must be a child of the popup).
     */
    void setView( QWidget *child );

    /**
     * Creates a standard view then calls setView(QWidget*) .
     */
    void setView( const QString &caption, const QString &text = QString() );

    /**
     * Creates a standard view then calls setView(QWidget*) .
     */
    virtual void setView( const QString &caption, const QString &text, const QPixmap &icon );

    /**
     * Returns a widget that is used as standard view if one of the
     * setView() methods taking the QString arguments is used.
     * You can use the returned widget to customize the passivepopup while
     * keeping the look similar to the "standard" passivepopups.
     *
     * After customizing the widget, pass it to setView( QWidget* )
     *
     * @param caption The window caption (title) on the popup
     * @param text The text for the popup
     * @param icon The icon to use for the popup
     * @param parent The parent widget used for the returned widget. If left 0,
     * then "this", i.e. the passive popup object will be used.
     *
     * @return a QWidget containing the given arguments, looking like the
     * standard passivepopups. The returned widget contains a QVBoxLayout,
     * which is accessible through layout().
     * @see setView( QWidget * )
     * @see setView( const QString&, const QString& )
     * @see setView( const QString&, const QString&, const QPixmap& )
     */
    QWidget * standardView( const QString& caption, const QString& text,
                          const QPixmap& icon, QWidget *parent = 0L );

    /**
     * Returns the main view.
     */
    QWidget *view() const;

    /**
     * Returns the delay before the popup is removed automatically.
     */
    int timeout() const;

    /**
     * Enables / disables auto-deletion of this widget when the timeout
     * occurs.
     * The default is false. If you use the class-methods message(),
     * auto-deletion is turned on by default.
     */
    virtual void setAutoDelete( bool autoDelete );

    /**
     * @returns true if the widget auto-deletes itself when the timeout occurs.
     * @see setAutoDelete
     */
    bool autoDelete() const;

    /**
     * If no relative window (eg taskbar button, system tray window) is
     * available, use this rectangle (pass it to moveNear()).
     * Basically KWindowSystem::workArea() with width and height set to 0
     * so that moveNear uses the upper-left position.
     * @return The QRect to be passed to moveNear() if no other is
     * available.
     */
    QRect defaultArea() const;

    /**
     * Returns the position to which this popup is anchored.
     */
    QPoint anchor() const;

    /**
     * Sets the anchor of this popup. The popup tries automatically to adjust
     * itself somehow around the point.
     */
    void setAnchor( const QPoint& anchor );

    /**
     * Convenience method that displays popup with the specified  message  beside the
     * icon of the specified widget.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( const QString &text, QWidget *parent,
                                   const QPoint& p = QPoint() );

    /**
     * Convenience method that displays popup with the specified  message  beside the
     * icon of the specified QSystemTrayIcon.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( const QString &text, QSystemTrayIcon *parent );

    /**
     * Convenience method that displays popup with the specified caption and message
     * beside the icon of the specified widget.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( const QString &caption, const QString &text,
                                   QWidget *parent, const QPoint& p = QPoint() );

    /**
     * Convenience method that displays popup with the specified caption and message
     * beside the icon of the specified QSystemTrayIcon.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( const QString &caption, const QString &text,
                                   QSystemTrayIcon *parent );

    /**
     * Convenience method that displays popup with the specified icon, caption and
     * message beside the icon of the specified widget.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( const QString &caption, const QString &text,
                                   const QPixmap &icon, QWidget *parent, int timeout = -1,
                                   const QPoint& p = QPoint() );

    /**
     * Convenience method that displays popup with the specified icon, caption and
     * message beside the icon of the specified QSystemTrayIcon.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( const QString &caption, const QString &text,
                   const QPixmap &icon, QSystemTrayIcon *parent, int timeout = -1 );

    /**
     * Convenience method that displays popup with the specified icon, caption and
     * message beside the icon of the specified window.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( const QString &caption, const QString &text,
                                   const QPixmap &icon, WId parent,
                                   int timeout = -1, const QPoint& p = QPoint() );

    /**
     * Convenience method that displays popup with the specified popup-style and message beside the
     * icon of the specified widget.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( int popupStyle, const QString &text, QWidget *parent, const QPoint& p = QPoint() );

    /**
     * Convenience method that displays popup with the specified popup-style and message beside the
     * icon of the specified QSystemTrayIcon.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( int popupStyle, const QString &text, QSystemTrayIcon *parent );

    /**
     * Convenience method that displays popup with the specified popup-style, caption and message
     * beside the icon of the specified QSystemTrayIcon.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text,
                                   QSystemTrayIcon *parent );

    /**
     * Convenience method that displays popup with the specified popup-style, caption and message
     * beside the icon of the specified widget.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text,
                                   QWidget *parent, const QPoint& p = QPoint() );

    /**
     * Convenience method that displays popup with the specified popup-style, icon, caption and
     * message beside the icon of the specified widget.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text,
                                   const QPixmap &icon, QWidget *parent, int timeout = -1,
                                   const QPoint& p = QPoint() );

    /**
     * Convenience method that displays popup with the specified popup-style, icon, caption and
     * message beside the icon of the specified QSystemTrayIcon.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text,
                                   const QPixmap &icon, QSystemTrayIcon *parent, int timeout = -1 );

    /**
     * Convenience method that displays popup with the specified popup-style, icon, caption and
     * message beside the icon of the specified window.
     * Note that the returned object is destroyed when it is hidden.
     * @see setAutoDelete
     */
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text,
                                   const QPixmap &icon, WId parent, int timeout = -1,
                                   const QPoint& p = QPoint() );


public Q_SLOTS:
    /**
     * Sets the delay for the popup is removed automatically. Setting the delay to 0
     * disables the timeout, if you're doing this, you may want to connect the
     * clicked() signal to the hide() slot.
     * Setting the delay to -1 makes it use the default value.
     *
     * @see timeout
     */
    void setTimeout( int delay );

    /**
     * Sets the visual appearance of the popup.
     * @see PopupStyle
     */
    void setPopupStyle( int popupstyle );

    /**
     * Reimplemented to reposition the popup.
     */
    void show();

    /**
     * Shows the popup in the given point
     */
    void show(const QPoint &p);

    virtual void setVisible(bool visible) Q_DECL_OVERRIDE;

Q_SIGNALS:
    /**
     * Emitted when the popup is clicked.
     */
    void clicked();

    /**
     * Emitted when the popup is clicked.
     */
    void clicked( const QPoint &pos );

protected:
    /**
     * This method positions the popup.
     */
    virtual void positionSelf();

    /**
     * Reimplemented to destroy the object when autoDelete() is
     * enabled.
     */
    virtual void hideEvent( QHideEvent * );

    /**
     * Moves the popup to be adjacent to the icon of the specified rectangle.
     */
    void moveNear( const QRect &target );

    /**
     * Calculates the position to place the popup near the specified rectangle.
     */
    QPoint calculateNearbyPoint( const QRect &target);

    /**
     * Reimplemented to detect mouse clicks.
     */
    virtual void mouseReleaseEvent( QMouseEvent *e );

    /**
     * Updates the transparency mask. Unused if PopupStyle == Boxed
     */
    void updateMask();

    /**
     * Overwrite to paint the border when PopupStyle == Balloon.
     * Unused if PopupStyle == Boxed
     */
    virtual void paintEvent( QPaintEvent* pe );

private:
    void init( WId window );

    /* @internal */
    class Private;
    Private *const d;
};

#endif // KPASSIVEPOPUP_H

// Local Variables:
// c-basic-offset: 4
// End:


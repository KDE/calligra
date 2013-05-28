#ifndef FAKE_KPASSIVEPOPUP_H
#define FAKE_KPASSIVEPOPUP_H

#include <QFrame>

class KVBox;
class QSystemTrayIcon;

class KPassivePopup : public QFrame
{
public:
    enum PopupStyle { Boxed, Balloon, CustomStyle=128 };
    KPassivePopup( QWidget *parent=0, Qt::WFlags f = 0 ) : QFrame(parent) {}
#if 0
    explicit KPassivePopup( WId parent ) : QFrame(parent) {}
#endif

    void setView( QWidget *child ) {}
    void setView( const QString &caption, const QString &text = QString() ) {}
    virtual void setView( const QString &caption, const QString &text, const QPixmap &icon ) {}
    KVBox * standardView( const QString& caption, const QString& text, const QPixmap& icon, QWidget *parent = 0L ) { return 0; }
    QWidget *view() const { return 0; }
    int timeout() const { return 0; }
    virtual void setAutoDelete( bool autoDelete ) {}
    bool autoDelete() const  { return true; }
    QRect defaultArea() const  { return QRect(); }
    QPoint anchor() const  { return QPoint(); }
    void setAnchor( const QPoint& anchor ) {}

    static KPassivePopup *message( const QString &text, QWidget *parent ) { return 0; }
    static KPassivePopup *message( const QString &text, QSystemTrayIcon *parent ) { return 0; }
    static KPassivePopup *message( const QString &caption, const QString &text, QWidget *parent ) { return 0; }
    static KPassivePopup *message( const QString &caption, const QString &text, QSystemTrayIcon *parent ) { return 0; }
    static KPassivePopup *message( const QString &caption, const QString &text, const QPixmap &icon, QWidget *parent, int timeout = -1 ) { return 0; }
    static KPassivePopup *message( const QString &caption, const QString &text, const QPixmap &icon, QSystemTrayIcon *parent, int timeout = -1 ) { return 0; }
    static KPassivePopup *message( const QString &caption, const QString &text, const QPixmap &icon, WId parent, int timeout = -1 ) { return 0; }
    static KPassivePopup *message( int popupStyle, const QString &text, QWidget *parent ) { return 0; }
    static KPassivePopup *message( int popupStyle, const QString &text, QSystemTrayIcon *parent ) { return 0; }
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text, QSystemTrayIcon *parent ) { return 0; }
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text, QWidget *parent ) { return 0; }
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text, const QPixmap &icon, QWidget *parent, int timeout = -1 ) { return 0; }
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text, const QPixmap &icon, QSystemTrayIcon *parent, int timeout = -1 ) { return 0; }
    static KPassivePopup *message( int popupStyle, const QString &caption, const QString &text, const QPixmap &icon, WId parent, int timeout = -1 ) { return 0; }

//public Q_SLOTS:
    void setTimeout( int delay ) {}
    void setPopupStyle( int popupstyle ) {}
    void show() {}
    void show(const QPoint &p) {}
#if 0
    virtual void setVisible(bool visible) {}
#endif
};

#endif


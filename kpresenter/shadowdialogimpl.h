#ifndef __shadowdialogimpl_h__
#define __shadowdialogimpl_h__

#include <qframe.h>
#include <qcolor.h>

#include "shadowdialog.h"
#include "global.h"

class ShadowPreview : public QFrame
{
    Q_OBJECT

public:
    ShadowPreview( QWidget* parent, const char* name = 0 );
    ~ShadowPreview() {}

    void setShadowDirection( ShadowDirection sd ) { shadowDirection = sd; repaint( true ); }
    void setShadowDistance( int sd ) { shadowDistance = sd; repaint( true ); }
    void setShadowColor( const QColor &sc ) { shadowColor = sc; repaint( true ); }

protected:
    void drawContents( QPainter* );

    ShadowDirection shadowDirection;
    int shadowDistance;
    QColor shadowColor;

};

class ShadowDialogImpl : public ShadowDialogBase
{
    Q_OBJECT

public:
    ShadowDialogImpl( QWidget *parent, const char* name = 0 );
    ~ShadowDialogImpl();

    void setShadowDirection( ShadowDirection sd );
    void setShadowDistance( int sd );
    void setShadowColor( const QColor &sc );

    ShadowDirection shadowDirection();
    int shadowDistance();
    QColor shadowColor();

signals:
    void apply();

protected slots:
    void colorChanged( const QColor& );
    void directionChanged( int );
    void distanceChanged( int );
    void applyClicked();
    void okClicked();

protected:
    ShadowPreview *_preview;
};


#endif

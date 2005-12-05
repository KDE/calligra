// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
#ifndef __shadowdialogimpl_h__
#define __shadowdialogimpl_h__

#include "shadowdialog.h"
#include "global.h"

class KPrTextPreview;

class KPrShadowDialogImpl : public ShadowDialogBase
{
    Q_OBJECT

public:
    KPrShadowDialogImpl( QWidget *parent, const char* name = 0 );
    ~KPrShadowDialogImpl() {}

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
    KPrTextPreview *_preview;
};


#endif

// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
#ifndef __shadowdialogimpl_h__
#define __shadowdialogimpl_h__

#include "shadowdialog.h"
#include "global.h"

class TextPreview;

class ShadowDialogImpl : public ShadowDialogBase
{
    Q_OBJECT

public:
    ShadowDialogImpl( QWidget *parent, const char* name = 0 );
    ~ShadowDialogImpl() {}

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
    TextPreview *_preview;
};


#endif

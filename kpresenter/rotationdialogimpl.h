#ifndef __rotationdialogimpl_h__
#define __rotationdialogimpl_h__

#include "rotationdialog.h"

class TextPreview;

class RotationDialogImpl : public RotationDialogBase
{
    Q_OBJECT

public:
    RotationDialogImpl( QWidget *parent, const char* name = 0 );
    ~RotationDialogImpl() {}

    void setAngle( double angle );
    double angle();

signals:
    void apply();

protected slots:
    void angleChanged( double );
    void angleMode( int );
    void applyClicked();
    void okClicked();

protected:
    TextPreview *_preview;
};


#endif

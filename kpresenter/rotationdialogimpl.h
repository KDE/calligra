// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
#ifndef __rotationdialogimpl_h__
#define __rotationdialogimpl_h__

#include <kdialogbase.h>

class TextPreview;
class RotationPropertyUI;

class RotationDialogImpl : public KDialogBase
{
    Q_OBJECT

public:
    RotationDialogImpl( QWidget *parent, const char* name = 0 );
    ~RotationDialogImpl() {}

    void setAngle( double angle );
    double angle();

protected slots:
    void angleChanged( double );
    void angleMode( int );
    void slotOk();

protected:
    TextPreview *m_preview;
    RotationPropertyUI *m_dialog;
};


#endif

/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project.
 * Copyright (C) 2001 Martin R. Jones <mjones@kde.org>
 * 
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 */

#ifndef KIMAGEFILEPREVIEW_H
#define KIMAGEFILEPREVIEW_H

#include <qwidget.h>
#include <qimage.h>

#include <kurl.h>

class QCheckBox;
class QPushButton;
class QLabel;
class QTimer;

/**
 * Image preview widget for the file dialog.
 */
class KImageFilePreview : public QWidget
{
    Q_OBJECT
public:
    KImageFilePreview( QWidget *parent );

    virtual QSize sizeHint() const;

public slots:
    void showPreview( const KURL & );

protected slots:
    void showImage();
    void updatePreview();
    void toggleAuto( bool );

protected:
    virtual void resizeEvent( QResizeEvent * );

private:
    bool autoMode;
    KURL currentURL;
    QImage currentImage;
    QTimer *timer;
    QLabel *imageLabel;
    QLabel *infoLabel;
    QCheckBox *autoPreview;
    QPushButton *previewButton;
};


#endif // KIMAGEFILEPREVIEW_H

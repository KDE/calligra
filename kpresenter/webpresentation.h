/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef webpresentation_h
#define webpresentation_h

#include <qwizard.h>
#include <qdialog.h>

class KPresenterDoc;
class KPresenterView;
class Page;
class KURLRequester;
class KColorButton;

class QVBox;
class QHBox;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QListView;
class QListViewItem;
class QCloseEvent;
class KProgress;
class QLabel;

/******************************************************************/
/* Class: KPWebPresentation                                       */
/******************************************************************/

class KPWebPresentation
{
public:
    enum ImageFormat {BMP = 0, PNG, JPEG};

    static QString imageFormat( ImageFormat i ) {
        if ( i == BMP ) return QString( "bmp" );
        if ( i == PNG ) return QString( "png" );
        if ( i == JPEG ) return QString( "jpeg" );
        return QString::null;
    }

    KPWebPresentation( KPresenterDoc *_doc, KPresenterView *_view );
    KPWebPresentation( const QString &_config, KPresenterDoc *_doc, KPresenterView *_view );
    KPWebPresentation( const KPWebPresentation &webPres );

    void setAuthor( const QString &_author )
    { author = _author; }
    void setEMail( const QString &_email )
    { email = _email; }
    void setTitle( const QString &_title )
    { title = _title; }
    void setBackColor( const QColor &_backColor )
    { backColor = _backColor; }
    void setTitleColor( const QColor &_titleColor )
    { titleColor = _titleColor; }
    void setTextColor( const QColor &_textColor )
    { textColor = _textColor; }
    void setImageFormat( ImageFormat _imgFormat )
    { imgFormat = _imgFormat; }
    void setPath( const QString &_path )
    { path = _path; }
    void setZoom( int _zoom )
    { zoom = _zoom; }

    QString getAuthor()
    { return author; }
    QString getEmail()
    { return email; }
    QString getTitle()
    { return title; }
    QColor getBackColor()
    { return backColor; }
    QColor getTitleColor()
    { return titleColor; }
    QColor getTextColor()
    { return textColor; }
    ImageFormat getImageFormat()
    { return imgFormat; }
    QString getPath()
    { return path; }
    int getZoom()
    { return zoom; }

    struct SlideInfo {
        int pageNumber; /* 0-based */
        QString slideTitle;
    };
    // Each entry in this list is a page (number+title).
    // This allows to skip pages.
    QValueList<SlideInfo> getSlideInfos() { return slideInfos; }

    void setSlideTitle( int i, const QString &slideTitle )
    { slideInfos[i].slideTitle = slideTitle; }

    void setConfig( const QString &_config )
    { config = _config; }
    QString getConfig()
    { return config; }

    void loadConfig();
    void saveConfig();

    int initSteps() { return 7; }
    int slides1Steps() { return slideInfos.count(); }
    int slides2Steps() { return slideInfos.count(); }
    int mainSteps() { return 1; }

    void initCreation( KProgress *progressBar );
    void createSlidesPictures( KProgress *progressBar );
    void createSlidesHTML( KProgress *progressBar );
    void createMainPage( KProgress *progressBar );

protected:
    void init();

    KPresenterDoc *doc;
    KPresenterView *view;
    QString config;

    QString author, title, email;
    QValueList<SlideInfo> slideInfos;
    QColor backColor, titleColor, textColor;
    QString path;
    ImageFormat imgFormat;
    int zoom;

};

/******************************************************************/
/* Class: KPWebPresentationWizard                                 */
/******************************************************************/

class KPWebPresentationWizard : public QWizard
{
    Q_OBJECT

public:
    KPWebPresentationWizard( const QString &_config, KPresenterDoc *_doc, KPresenterView *_view );
    ~KPWebPresentationWizard();

    static void createWebPresentation( const QString &_config, KPresenterDoc *_doc, KPresenterView *_view );

protected:
    void setupPage1();
    void setupPage2();
    void setupPage3();

    bool isPathValid();

    void closeEvent( QCloseEvent *e );

    QString config;
    KPresenterDoc *doc;
    KPresenterView *view;
    KPWebPresentation webPres;

    QHBox *page1, *page2, *page3;
    QLineEdit *author, *title, *email;//, *path;
    KColorButton *textColor, *titleColor, *backColor;
    QComboBox *format;
    QSpinBox *zoom;
    KURLRequester *path;
    //QPushButton *choosePath;
    QListView *slideTitles;
    QLineEdit *slideTitle;

protected slots:
    virtual void finish();
    void pageChanged();
    void slotChoosePath(const QString &); 
    void slideTitleChanged( const QString & );
    void slideTitleChanged( QListViewItem * );

};

/******************************************************************/
/* Class: KPWebPresentationCreateDialog                           */
/******************************************************************/

class KPWebPresentationCreateDialog : public QDialog
{
    Q_OBJECT

public:
    KPWebPresentationCreateDialog( KPresenterDoc *_doc, KPresenterView *_view, const KPWebPresentation &_webPres );
    ~KPWebPresentationCreateDialog();

    static void createWebPresentation( KPresenterDoc *_doc, KPresenterView *_view, const KPWebPresentation &_webPres );

    void start();

    void initCreation();
    void createSlidesPictures();
    void createSlidesHTML();
    void createMainPage();

protected:
    void setupGUI();
    void resizeEvent( QResizeEvent *e );

    KPresenterView *view;
    KPresenterDoc *doc;
    KPWebPresentation webPres;

    KProgress *progressBar;
    QLabel *step1, *step2, *step3, *step4;
    QPushButton *bDone, *bSave;
    QVBox *back;

protected slots:
    void saveConfig();

};
#endif

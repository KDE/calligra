// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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

#include <kwizard.h>

#include <qdialog.h>
#include <qvaluelist.h>


class KPresenterDoc;
class KPresenterView;

class KURLRequester;
class KColorButton;
class KListView;
class KIntNumInput;
class KLineEdit;
class KComboBox;

class QVBox;
class QHBox;

class QListViewItem;
class QCloseEvent;
class KProgress;
class QLabel;

class QCheckBox;

class KPWebPresentation
{
public:

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
    void setXML( bool _xml )
        { xml = _xml; }
    void setWriteHeader( bool _writeHeader )
        { m_bWriteHeader = _writeHeader; }
    void setWriteFooter( bool _writeFooter )
        { m_bWriteFooter = _writeFooter; }
    void setLoopSlides( bool _loopSlides )
        { m_bLoopSlides = _loopSlides; }
    void setPath( const QString &_path )
        { path = _path; }
    void setZoom( int _zoom )
        { zoom = _zoom; }
    void setTimeBetweenSlides( int _timeBetweenSlides )
        { timeBetweenSlides = _timeBetweenSlides; } // PAU
    void setEncoding( const QString &_encoding ) { m_encoding = _encoding; }

    QString getAuthor() const { return author; }
    QString getEmail() const { return email; }
    QString getTitle() const { return title; }
    QColor getBackColor() const { return backColor; }
    QColor getTitleColor() const { return titleColor; }
    QColor getTextColor() const { return textColor; }
    bool isXML() const { return xml; }
    bool wantHeader() const { return m_bWriteHeader; }
    bool wantFooter() const { return m_bWriteFooter; }
    bool wantLoopSlides() const { return m_bLoopSlides; }
    QString getPath() const { return path; }
    int getZoom() const { return zoom; }
    int getTimeBetweenSlides() const { return timeBetweenSlides; } // PAU
    QString getEncoding() const { return m_encoding; }

    struct SlideInfo {
        int pageNumber; /* 0-based */
        QString slideTitle;
    };
    // Each entry in this list is a page (number+title).
    // This allows to skip pages.
    QValueList<SlideInfo> getSlideInfos() const { return slideInfos; }

    void setSlideTitle( int i, const QString &slideTitle )
        { slideInfos[i].slideTitle = slideTitle; }

    void setConfig( const QString &_config )
        { config = _config; }
    QString getConfig() const { return config; }

    void loadConfig();
    void saveConfig();

    int initSteps() const { return 7; }
    int slides1Steps() { return slideInfos.count(); }
    int slides2Steps() { return slideInfos.count(); }
    int mainSteps() const { return 1; }

    void initCreation( KProgress *progressBar );
    void createSlidesPictures( KProgress *progressBar );
    void createSlidesHTML( KProgress *progressBar );
    void createMainPage( KProgress *progressBar );

protected:
    void init();
    QString escapeHtmlText( QTextCodec *codec, const QString& strText ) const;
    void writeStartOfHeader(QTextStream& streamOut, QTextCodec *codec,
                            const QString& subtitle, const QString& dest, const QString& next);

    KPresenterDoc *doc;
    KPresenterView *view;
    QString config;
    QString author, title, email;
    QValueList<SlideInfo> slideInfos;
    QColor backColor, titleColor, textColor;
    QString path;    
    bool xml;
    bool m_bWriteHeader, m_bWriteFooter, m_bLoopSlides;
    int timeBetweenSlides;
    int zoom;
    QString m_encoding;
};

class KPWebPresentationWizard : public KWizard
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
    void setupPage4();
    void setupPage5(); // PAU

    void closeEvent( QCloseEvent *e );

    QString config;
    KPresenterDoc *doc;
    KPresenterView *view;
    KPWebPresentation webPres;

    QHBox *page1, *page2, *page3, *page4, *page5; // PAU
    QCheckBox *writeHeader, *writeFooter, *loopSlides; // PAU
    KLineEdit *author, *title, *email;
    KColorButton *textColor, *titleColor, *backColor;
    KComboBox *encoding, *doctype;
    KIntNumInput *zoom, *timeBetweenSlides; // PAU
    KURLRequester *path;
    KListView *slideTitles;
    KLineEdit *slideTitle;

protected slots:
    virtual void finish();
    void pageChanged();
    void slotChoosePath(const QString &);
    void slideTitleChanged( const QString & );
    void slideTitleChanged( QListViewItem * );

};

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
    QLabel *step1, *step2, *step3, *step4, *step5;    
    QPushButton *bDone, *bSave;
    QVBox *back;

protected slots:
    void saveConfig();

};
#endif

// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004 Brad Hards <bradh@frogmouth.net>
   Based heavily on webpresentation.h, which is:
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef mspresentation_h
#define mspresentation_h

#include <k3wizard.h>

#include <qdialog.h>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3VBoxLayout>
#include <QLabel>
#include <QCloseEvent>


class KPrDocument;
class KPrView;

class KUrlRequester;
class KColorButton;
class K3ListView;
class KIntNumInput;
class KLineEdit;
class KComboBox;

class Q3VBox;
class Q3HBox;

class Q3ListViewItem;
class QCloseEvent;
class KProgress;
class QLabel;
class Q3GroupBox;
class Q3VBoxLayout;

class KPrMSPresentation
{
public:

    KPrMSPresentation( KPrDocument *_doc, KPrView *_view );
    KPrMSPresentation( const KPrMSPresentation &msPres );

    void setTitle( const QString &_title )
        { title = _title; }
    void setBackColour( const QColor &_backColour )
        { backColour = _backColour; }
    void setTextColour( const QColor &_textColour )
        { textColour = _textColour; }
    void setPath( const QString &_path )
        { path = _path; }

    QString getTitle() const { return title; }
    QColor getBackColour() const { return backColour; }
    QColor getTextColour() const { return textColour; }
    QString getPath() const { return path; }

    struct SlideInfo {
        int pageNumber; /* 0-based */
    };
    Q3ValueList<SlideInfo> getSlideInfos() const { return slideInfos; }

    int initSteps() const { return 7; }
    int slidesSteps() { return slideInfos.count(); }
    int indexFileSteps() const { return 4 + slideInfos.count(); }

    void initCreation( KProgress *progressBar );
    void createSlidesPictures( KProgress *progressBar );
    void createIndexFile( KProgress *progressBar );

protected:
    void init();

    KPrDocument *doc;
    KPrView *view;

    QString title;
    Q3ValueList<SlideInfo> slideInfos;
    QColor backColour, textColour;
    QString path;
    QString slidePath; // directory relative to path, containing slides 

};

class KPrMSPresentationSetup : public QDialog
{
    Q_OBJECT

public:
    KPrMSPresentationSetup( KPrDocument *_doc, KPrView *_view );
    ~KPrMSPresentationSetup();

    static void createMSPresentation( KPrDocument *_doc, KPrView *_view );

protected:
    KPrDocument *doc;
    KPrView *view;
    KPrMSPresentation msPres;

    KLineEdit *title;
    KColorButton *textColour, *backColour;
    Q3GroupBox *colourGroup;
    Q3VBoxLayout *mainLayout;
    KUrlRequester *path;

protected slots:
    void slotChoosePath(const QString &);
    virtual void finish();
    virtual void helpMe();
    void showColourGroup(bool on);
};

class KPrMSPresentationCreateDialog : public QDialog
{
    Q_OBJECT

public:
    KPrMSPresentationCreateDialog( KPrDocument *_doc, KPrView *_view, const KPrMSPresentation &_msPres );
    ~KPrMSPresentationCreateDialog();

    static void createMSPresentation( KPrDocument *_doc, KPrView *_view, const KPrMSPresentation &_msPres );

    void start();

    void initCreation();
    void createSlidesPictures();
    void createIndexFile();

protected:
    void setupGUI();
    void resizeEvent( QResizeEvent *e );

    KPrView *view;
    KPrDocument *doc;
    KPrMSPresentation msPres;

    KProgress *progressBar;
    QLabel *step1, *step2, *step3;
    QPushButton *bDone;
    Q3VBox *back;

};
#endif


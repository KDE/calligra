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

#ifndef kwtabletemplateselector_h
#define kwtabletemplateselector_h

#include <qwidget.h>
#include <qgroupbox.h>
#include <qptrlist.h>

#include <qlabel.h>
#include <qpushbutton.h>
/*#include <qlayout.h>
#include <qfont.h>
#include <qbrush.h>
*/

class QListBox;
class QCheckBox;
class QButtonGroup;
class QGroupBox;
class QRect;

class KWTableTemplate;
class KWTableTemplateCollection;
class KoRect;
class KoZoomHandler;
class KWTextDocument;
class KWDocument;
class KWTableFrameSet;
class KWTableStyle;

/******************************************************************/
/* Class: KWTableTemplatePreview                                  */
/******************************************************************/

class KWTableTemplatePreview : public QGroupBox
{
    Q_OBJECT

public:
    KWTableTemplatePreview( const QString& title, KWTableStyle *_emptyStyle, QWidget* parent, const char* name=0L );
    ~KWTableTemplatePreview();
    void setTableTemplate(KWTableTemplate *_tableTemplate);
    KWTableTemplate *getTableTemplate();

public slots:
    void cbFirstRowChanged( bool );
    void cbFirstColChanged( bool );
    void cbLastRowChanged( bool );
    void cbLastColChanged( bool );
    void cbBodyChanged( bool );

protected:
    int bottomBorder(const int rows, const int cols, const int rowpos, const int colpos);
    int rightBorder(const int rows, const int cols, const int rowpos, const int colpos);
    void drawCell( QPainter *p, const KWTableStyle *ts, const QRect globalRect, 
                   const int rows, int cols, int rowpos, int colpos, const QString & txt);
    void drawPreviewTable(QPainter *p, int rows, int cols, QRect globalRect);
    void drawContents( QPainter *p );
    void setSpecialCells(KWTableTemplate *);

    KWTableStyle *m_emptyStyle;
    KWTextDocument *m_textdoc;
    KoZoomHandler *m_zoomHandler;
    KWTableTemplate *tableTemplate;
    KWTableTemplate *origTableTemplate;
private:
    QString m_contents[2][5];
    
    void fillContents();
};

/******************************************************************/
/* Class: KWTableTemplateSelector                                    */
/******************************************************************/

class KWTableTemplateSelector : public QWidget
{
    Q_OBJECT

public:
    KWTableTemplateSelector( KWDocument *_doc, QWidget * _parent, const char * _name = 0 );

    KWTableTemplate *getTableTemplate();
    
protected:
    KWDocument *m_doc;

    KWTableTemplatePreview *preview;
    QLabel *lTemplates;
    QListBox *lbTemplates;
    QPushButton *pbCustomize;
    
    QButtonGroup *bgCustomize;
    QCheckBox *cbFirstRow, *cbFirstCol, *cbLastRow, *cbLastCol, *cbBody;

    int selectedTableTemplate;
protected slots:
    void changeTableTemplate();
};

#endif



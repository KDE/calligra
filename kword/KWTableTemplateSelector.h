/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; using
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwtabletemplateselector_h
#define kwtabletemplateselector_h

#include <QWidget>
#include <q3groupbox.h>
#include <q3ptrlist.h>

#include <QLabel>
#include <QPushButton>
/*#include <QLayout>
#include <QFont>
#include <qbrush.h>
*/

class Q3ListBox;
class QCheckBox;
class Q3ButtonGroup;
class Q3GroupBox;
class QRect;

class KWTableTemplate;
class KWTableTemplateCollection;
class KoRect;
class KoTextZoomHandler;
class KWTextDocument;
class KWDocument;
class KWTableFrameSet;
class KWTableStyle;

/******************************************************************/
/* Class: KWTableTemplatePreview                                  */
/******************************************************************/

class KWTableTemplatePreview : public Q3GroupBox
{
    Q_OBJECT

public:
    KWTableTemplatePreview( const QString& title, KWTableStyle *_emptyStyle, QWidget* parent, const char* name=0L );
    ~KWTableTemplatePreview();
    void setTableTemplate(KWTableTemplate *_tableTemplate);
    KWTableTemplate *getTableTemplate()const ;
    void disableRepaint(bool _b) { m_disableRepaint =_b;}
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
    KoTextZoomHandler *m_zoomHandler;
    KWTableTemplate *tableTemplate;
    KWTableTemplate *origTableTemplate;
    //necessary when we init preview
    bool m_disableRepaint;
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
    enum FormatType { FirstRow = 1 , FirstColumn = 2, LastRow = 4, LastCol =8, Body =16 };
    KWTableTemplateSelector( KWDocument *_doc, QWidget * _parent, const QString & _tableTemplate, int _type = 31 , const char * _name = 0 );

    KWTableTemplate *getTableTemplate()const ;
    int getFormatType() const ;
protected:
    KWDocument *m_doc;

    KWTableTemplatePreview *preview;
    QLabel *lTemplates;
    Q3ListBox *lbTemplates;
    QPushButton *pbCustomize;

    Q3ButtonGroup *bgCustomize;
    QCheckBox *cbFirstRow, *cbFirstCol, *cbLastRow, *cbLastCol, *cbBody;

    int selectedTableTemplate;
    void initFormat( int format);
    void initPreview();
protected slots:
    void changeTableTemplate();
};

#endif



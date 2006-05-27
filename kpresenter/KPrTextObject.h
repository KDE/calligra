// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef kptextobject_h
#define kptextobject_h

#include "KPrObject.h"
#include "KoRichText.h"
#include <KoTextView.h>
#include <KoStyleCollection.h> // for KoStyleChangeDefMap
#include <KoStyleStack.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <Q3ValueList>
#include <Q3PtrList>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>

class Q3DragObject;

class KoSavingContext;
class KPrView;
class KPrDocument;
class QDomElement;
class TextCmd;
class KPrGradient;

class KoTextView;
class KoTextObject;
class KPrTextView;
class KCommand;
class KPrCanvas;
class KoPoint;
class KoVariable;
class KPrTextDocument;
class DCOPObject;
class KPrPage;
class KoParagStyle;

class KPrTextObject :  public QObject, public KPr2DObject, public KoTextFlow
{
    Q_OBJECT
public:
    KPrTextObject( KPrDocument *doc );
    virtual ~KPrTextObject();

    virtual DCOPObject* dcopObject();

    virtual void setSize( double _width, double _height );

    virtual void setDrawEditRect( bool b )
        { drawEditRect = b; }
    virtual void setDrawEmpty( bool b )
        { drawEmpty = b; }

    virtual ObjType getType() const
        { return OT_TEXT; }
    virtual QString getTypeString() const
        { return i18n("Text"); }

    virtual bool getDrawEditRect() const
        { return drawEditRect; }
    virtual bool getDrawEmpty() const
        { return drawEmpty; }
    virtual int getSubPresSteps() const;

    virtual QBrush getBrush() const;

    virtual void setShadowParameter(int _distance,ShadowDirection _direction,const QColor &_color);

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);
    virtual void loadOasis(const QDomElement &element, KoOasisContext& context, KPrLoadingInfo *info);
    virtual void saveOasisMarginElement( KoGenStyle &styleobjectauto ) const;

    virtual void paint( QPainter *_painter, KoTextZoomHandler*_zoomHandler,
                        int pageNum, bool drawingShadow, bool drawContour );
    void paint( QPainter *_painter, KoTextZoomHandler*_zoomHandler, bool onlyChanged, KoTextCursor* cursor,
                bool resetChanged, bool drawContour );
    void paintEdited( QPainter *_painter, KoTextZoomHandler*_zoomHandler,
                      bool onlyChanged, KoTextCursor* cursor, bool resetChanged );

    void drawCursor( QPainter *p, KoTextCursor *cursor, bool cursorVisible, KPrCanvas* /*canvas*/ );

    void recalcPageNum( KPrPage *page );
    /// Return the current text cursor position
    QPoint cursorPos(KPrCanvas*, KoTextCursor *) const;


    /** Return the contained text object */
    KoTextObject * textObject() const { return m_textobj; }
    virtual void addTextObjects( Q3PtrList<KoTextObject> &lst ) const {
        if ( !isProtectContent() )
            lst.append( m_textobj );
    }

    KPrTextDocument *textDocument() const;

    KPrDocument* kPresenterDocument() const { return m_doc; }

    KPrTextView * createKPTextView( KPrCanvas *,bool temp=false );
    void applyStyleChange( KoStyleChangeDefMap changed );

    void removeHighlight();
    void highlightPortion( KoTextParag * parag, int index, int length, KPrCanvas *canvas, bool repaint, KDialogBase* dialog = 0 );

    KCommand * pasteOasis( KoTextCursor * cursor, const QByteArray & data, bool removeSelected );

    void saveParagraph( QDomDocument& doc,
                        KoTextParag * parag,
                        QDomElement &parentElem,
                        int from /* default 0 */,
                        int to /* default length()-2 */ );
    KoParagLayout loadParagLayout( QDomElement & parentElem, KPrDocument *doc, bool useRefStyle);

    static KoTextFormat loadFormat( QDomElement &n, KoTextFormat * refFormat, const QFont & defaultFont,
                                    const QString & defaultLanguage, bool hyphen );

    void setEditingTextObj( bool _edit ) { editingTextObj = _edit; }

    KCommand *textContentsToHeight();
    KCommand *textObjectToContents();
    void setProtectContent ( bool _protect ) { textObject()->setProtectContent(_protect);}
    bool isProtectContent() const { return textObject()->protectContent();}
    void loadVariable( Q3ValueList<QDomElement> & listVariable,KoTextParag *lastParag, int offset=0 );

    void layout();

    /** Reimplemented from KoTextFlow */
    int availableHeight() const;

    /** set left margin size */
    void setBLeft( double b ) { bleft = b; }
    /** set right margin size */
    void setBRight( double b ) { bright = b; }
    /** set top margin size */
    void setBTop( double b ) { btop = b; }
    /** set bottom margin size */
    void setBBottom( double b ) { bbottom = b; }

    /** get left margin size */
    double bLeft()const { return bleft; }
    /** get right margin size */
    double bRight()const { return bright; }
    /** get top margin size */
    double bTop()const { return btop; }
    /** get bottom margin size */
    double bBottom()const { return bbottom; }

    void setTextMargins( double _left, double _top, double right, double bottom);

    KoRect innerRect() const;
    double innerWidth() const;
    double innerHeight() const;
    void resizeTextDocument( bool widthChanged = true, bool heightChanged = true );

    VerticalAlignmentType verticalAlignment() const { return m_textVertAlign; }
    void setVerticalAligment( VerticalAlignmentType _type) ;
    double alignmentValue() const {  return alignVertical; }
    virtual KPrTextObject *nextTextObject() { return this;}
    static void saveFormat( QDomElement & element, KoTextFormat*lastFormat );

    QPoint viewToInternal( const QPoint & pos, KPrCanvas* canvas ) const;

signals:
    void repaintChanged( KPrTextObject* );

protected slots:
    void slotFormatChanged(const KoTextFormat &);
    void slotAfterFormatting( int, KoTextParag*, bool* );
    void slotParagraphDeleted(KoTextParag*_parag);
protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;

    virtual QDomElement saveKTextObject( QDomDocument& doc );
    QDomElement saveHelper(const QString &tmpText,KoTextFormat*lastFormat ,QDomDocument &doc);

    virtual void fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const;

    virtual void loadKTextObject( const QDomElement &e );
    void drawText( QPainter* _painter, KoTextZoomHandler* zoomHandler, bool onlyChanged, KoTextCursor* cursor, bool resetChanged );
    void drawParags( QPainter *p, KoTextZoomHandler* zoomHandler, const QColorGroup& cg, int from, int to );
    void saveParagLayout( const KoParagLayout& layout, QDomElement & parentElem );
    void invalidate();
    void recalcVerticalAlignment();
    virtual KoPen defaultPen() const;
protected slots:
    void slotNewCommand( KCommand *cmd );
    void slotAvailableHeightNeeded();
    void slotRepaintChanged();
private:
    KPrTextObject &operator=( const KPrTextObject & );
    void shadowCompatibility();
    static const QString &tagTEXTOBJ, &attrLineSpacing, &attrParagSpacing,
        &attrMargin, &attrBulletType1, &attrBulletType2,
        &attrBulletType3, &attrBulletType4, &attrBulletColor1,
        &attrBulletColor2, &attrBulletColor3, &attrBulletColor4,
        &attrObjType, &tagP, &attrAlign, &attrType,
        &attrDepth, &tagTEXT, &attrFamily, &attrPointSize,
        &attrBold, &attrItalic, & attrUnderline,& attrStrikeOut,
        &attrColor, &attrWhitespace, &attrTextBackColor,
        &attrVertAlign, &attrLinkName, &attrHrefName;

    /** The contained text object */
    KoTextObject *m_textobj;
    KPrDocument *m_doc;
    KoParagLayout m_paragLayout;
    VerticalAlignmentType m_textVertAlign;
    double bleft, bright, btop, bbottom; // margins
    double alignVertical;
    bool drawEditRect, drawEmpty;
    bool editingTextObj;
};


class KPrTextView : public KoTextView
{
    Q_OBJECT
public:
    KPrTextView( KPrTextObject * txtObj, KPrCanvas *_canvas, bool temp=false );
    virtual ~KPrTextView();

    virtual KoTextViewIface* dcopObject();

    KoTextView * textView() { return this; }
    KPrTextObject * kpTextObject() const { return m_kptextobj; }

    void keyPressEvent( QKeyEvent * );
    void keyReleaseEvent( QKeyEvent * );
#warning "kde4 reapply it"	
#if 0
	void imStartEvent( QIMEvent * );
    void imComposeEvent( QIMEvent * );
    void imEndEvent( QIMEvent * );
#endif	
    void mousePressEvent( QMouseEvent *, const QPoint &);
    void mouseMoveEvent( QMouseEvent *, const QPoint & );
    void mouseReleaseEvent( QMouseEvent *, const QPoint & );
    void mouseDoubleClickEvent( QMouseEvent *, const QPoint &);
    void dragEnterEvent( QDragEnterEvent * );
    void dragMoveEvent( QDragMoveEvent *, const QPoint & );
    void dropEvent( QDropEvent * );

    void clearSelection();
    void selectAll(bool select = true);
    virtual void drawCursor( bool b );

    const KoParagLayout & currentParagLayout() const { return m_paragLayout; }
    void showPopup( KPrView *view, const QPoint &point, QList<KAction*> &actionList );
    void insertVariable( int type, int subtype = 0 );
    void insertCustomVariable( const QString &name);
    void insertLink(const QString &_linkName, const QString & hrefName);
    void insertVariable( KoVariable *var, KoTextFormat *format =0,
                         bool refreshCustomMenu =false /*don't refresh variable custom menu all the time */);

    void terminate(bool removeSelection=true);
    void insertComment(const QString &_note);

    /** return true if some text is selected */
    bool hasSelection() const;

    virtual void removeToolTipCompletion();
    bool isLinkVariable(const QPoint &);
    void openLink();
    QPoint viewToInternal( const QPoint & pos ) const;

public slots:
    void cut();
    void copy();
    void paste();
    // Reimplemented from KoTextView
    virtual void updateUI( bool updateFormat, bool force = false );
    virtual void ensureCursorVisible();


protected slots:
    virtual void startDrag();

protected:
    bool canDecode( QMimeSource *e );
    Q3DragObject * newDrag( QWidget * parent );
    // Reimplemented from KoTextView
    virtual void doAutoFormat( KoTextCursor* cursor, KoTextParag *parag, int index, QChar ch );
    virtual bool doIgnoreDoubleSpace(KoTextParag * parag, int index,QChar ch );
    virtual bool doCompletion( KoTextCursor* cursor, KoTextParag *parag, int index );
    virtual bool doToolTipCompletion( KoTextCursor* , KoTextParag *, int, int  );
    virtual void showToolTipBox(KoTextParag *parag, int index, QWidget *widget, const QPoint &pos);
    virtual void textIncreaseIndent();
    virtual bool textDecreaseIndent();
    virtual void showFormat( KoTextFormat *format ) ;

    virtual bool pgUpKeyPressed();
    virtual bool pgDownKeyPressed();

    KPrTextObject *m_kptextobj;
    KPrCanvas *m_canvas;
    KoParagLayout m_paragLayout;
    QList<KAction*> m_actionList; // for the kodatatools
};


#endif

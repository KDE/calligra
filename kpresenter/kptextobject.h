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

#ifndef kptextobject_h
#define kptextobject_h

#include "kpobject.h"
#include "korichtext.h"
#include <kotextview.h>
#include <kostyle.h> // for KoStyleChangeDefMap
#include <koStyleStack.h>

class KoSavingContext;
class KPresenterView;
class KPresenterDoc;
class QDomElement;
class TextCmd;
class KPGradient;

class KoTextView;
class KoTextObject;
class KPTextView;
class KCommand;
class KPrCanvas;
class KoPoint;
class KoVariable;
class KPrTextDrag;
class KPrTextDocument;
class DCOPObject;
class KPrPage;
class KoParagStyle;

class KPTextObject :  public QObject, public KP2DObject
{
    Q_OBJECT
public:
    KPTextObject( KPresenterDoc *doc );
    virtual ~KPTextObject();

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
    virtual void loadOasis(const QDomElement &element, KoOasisContext& context, KPRLoadingInfo *info);
    virtual bool saveOasis( KoXmlWriter &xmlWriter, KoSavingContext& context, int indexObj  ) const;
    virtual void saveOasisMarginElement( KoGenStyle &styleobjectauto ) const;

    virtual void paint( QPainter *_painter, KoZoomHandler*_zoomHandler,
                        int pageNum, bool drawingShadow, bool drawContour );
    void paint( QPainter *_painter, KoZoomHandler*_zoomHandler, bool onlyChanged, KoTextCursor* cursor,
                bool resetChanged, bool drawContour );
    void paintEdited( QPainter *_painter, KoZoomHandler*_zoomHandler,
                      bool onlyChanged, KoTextCursor* cursor, bool resetChanged );

    void drawCursor( QPainter *p, KoTextCursor *cursor, bool cursorVisible, KPrCanvas* /*canvas*/ );

    void recalcPageNum( KPrPage *page );


    /** Return the contained text object */
    KoTextObject * textObject() const { return m_textobj; }
    virtual void addTextObjects( QPtrList<KoTextObject> &lst ) const {
        if ( !isProtectContent() )
            lst.append( m_textobj );
    }

    KPrTextDocument *textDocument() const;

    KPresenterDoc* kPresenterDocument() const { return m_doc; }

    KPTextView * createKPTextView( KPrCanvas * );
    void applyStyleChange( KoStyleChangeDefMap changed );

    void removeHighlight();
    void highlightPortion( KoTextParag * parag, int index, int length, KPrCanvas *canvas, bool repaint, KDialogBase* dialog = 0 );

    KCommand * pasteKPresenter( KoTextCursor * cursor, const QCString & data, bool removeSelected );

    void saveParagraph( QDomDocument& doc,
                        KoTextParag * parag,
                        QDomElement &parentElem,
                        int from /* default 0 */,
                        int to /* default length()-2 */ );
    KoParagLayout loadParagLayout( QDomElement & parentElem, KPresenterDoc *doc, bool useRefStyle);

    static KoTextFormat loadFormat( QDomElement &n, KoTextFormat * refFormat, const QFont & defaultFont,
                                    const QString & defaultLanguage, bool hyphen );

    void setEditingTextObj( bool _edit ) { editingTextObj = _edit; }

    KCommand *textContentsToHeight();
    KCommand *textObjectToContents();
    void setProtectContent ( bool _protect ) { textObject()->setProtectContent(_protect);}
    bool isProtectContent() const { return textObject()->protectContent();}
    void loadVariable( QValueList<QDomElement> & listVariable,KoTextParag *lastParag, int offset=0 );

    void layout();

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
    virtual KPTextObject *nextTextObject() { return this;}
    static void saveFormat( QDomElement & element, KoTextFormat*lastFormat );
signals:
    void repaintChanged( KPTextObject* );

protected slots:
    void slotFormatChanged(const KoTextFormat &);
    void slotAfterFormatting( int, KoTextParag*, bool* );
    void slotParagraphDeleted(KoTextParag*_parag);
protected:
    virtual QDomElement saveKTextObject( QDomDocument& doc );
    QDomElement saveHelper(const QString &tmpText,KoTextFormat*lastFormat ,QDomDocument &doc);

    virtual void loadKTextObject( const QDomElement &e );
    void drawText( QPainter* _painter, KoZoomHandler* zoomHandler, bool onlyChanged, KoTextCursor* cursor, bool resetChanged );
    void drawParags( QPainter *p, KoZoomHandler* zoomHandler, const QColorGroup& cg, int from, int to );
    void saveParagLayout( const KoParagLayout& layout, QDomElement & parentElem );
    void invalidate();
    void recalcVerticalAlignment();
    virtual QPen defaultPen() const;
protected slots:
    void slotNewCommand( KCommand *cmd );
    void slotAvailableHeightNeeded();
    void slotRepaintChanged();
private:
    KPTextObject &operator=( const KPTextObject & );
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
    KPresenterDoc *m_doc;
    KoParagLayout m_paragLayout;
    VerticalAlignmentType m_textVertAlign;
    double bleft, bright, btop, bbottom; // margins
    double alignVertical;
    bool drawEditRect, drawEmpty;
    bool editingTextObj;
};


class KPTextView : public KoTextView
{
    Q_OBJECT
public:
    KPTextView( KPTextObject * txtObj, KPrCanvas *_canvas );
    virtual ~KPTextView();

    virtual KoTextViewIface* dcopObject();

    KoTextView * textView() { return this; }
    KPTextObject * kpTextObject() const { return m_kptextobj; }

    void keyPressEvent( QKeyEvent * );
    void keyReleaseEvent( QKeyEvent * );
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
    void showPopup( KPresenterView *view, const QPoint &point, QPtrList<KAction> &actionList );
    void insertVariable( int type, int subtype = 0 );
    void insertCustomVariable( const QString &name);
    void insertLink(const QString &_linkName, const QString & hrefName);
    void insertVariable( KoVariable *var, KoTextFormat *format =0, bool removeSelectedText=true,
                         bool refreshCustomMenu =false /*don't refresh variable custom menu all the time */);

    void terminate(bool removeSelection=true);
    void insertComment(const QString &_note);

    /** return true if some text is selected */
    bool hasSelection() const;

public slots:
    void cut();
    void copy();
    void paste();
    // Reimplemented from KoTextView
    virtual void updateUI( bool updateFormat, bool force = false );
    virtual void ensureCursorVisible();


protected slots:
    virtual void startDrag();

    KPrTextDrag * newDrag( QWidget * parent );

protected:
    // Reimplemented from KoTextView
    virtual void doAutoFormat( KoTextCursor* cursor, KoTextParag *parag, int index, QChar ch );
    virtual bool doIgnoreDoubleSpace(KoTextParag * parag, int index,QChar ch );

    virtual void doCompletion( KoTextCursor* cursor, KoTextParag *parag, int index );

    virtual void showFormat( KoTextFormat *format ) ;

    virtual bool pgUpKeyPressed();
    virtual bool pgDownKeyPressed();

    QPoint cursorPosition( const QPoint & pos );

    KPTextObject *m_kptextobj;
    KPrCanvas *m_canvas;
    KoParagLayout m_paragLayout;
    QPtrList<KAction> m_actionList; // for the kodatatools
};


#endif

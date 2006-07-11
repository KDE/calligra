#ifndef BORDER_H
#define BORDER_H

#include <QWidget>

class QLabel;

namespace KSpread
{

class Canvas;
class View;

/**
 * HBorder
 */
class HBorder : public QWidget
{
    Q_OBJECT
public:
    HBorder( QWidget *_parent, Canvas *_canvas, View *_view  );
    virtual ~HBorder();

    int markerColumn() const { return  m_iSelectionAnchor; }
    void equalizeColumn( double resize );

    void updateColumns( int from, int to );

    QSize sizeHint() const;

private slots:
    void doAutoScroll();

protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void wheelEvent( QWheelEvent* );
    virtual void focusOutEvent( QFocusEvent* ev );
    virtual void resizeEvent( QResizeEvent * _ev );
    void paintSizeIndicator( int mouseX, bool firstTime );

private:
    Canvas *m_pCanvas;
    View *m_pView;
    QTimer * m_scrollTimer;

    /**
     * Flag that inidicates whether the user wants to mark columns.
     * The user may mark columns by dragging the mouse around in th XBorder widget.
     * If he is doing that right now, this flag is true. Mention that the user may
     * also resize columns by dragging the mouse. This case is not covered by this flag.
     */
    bool m_bSelection;

    /**
     * The column over which the user pressed the mouse button.
     * If the user marks columns in the XBorder widget, then this is the initial
     * column on which he pressed the mouse button.
     */
    int m_iSelectionAnchor;

    /**
     * Flag that indicates whether the user resizes a column
     * The user may resize columns by dragging the mouse around in the HBorder widget.
     * If he is doing that right now, this flag is true.
     */
    bool m_bResize;

    /**
     * The column over which the user pressed the mouse button.
     * The user may resize columns by dragging the mouse around the XBorder widget.
     * This is the column over which he pressed the mouse button. This column is going
     * to be resized.
      */
    int m_iResizedColumn;

    /**
     * Last position of the mouse, when resizing.
     */
    int m_iResizePos;

    /**
     * The label used for showing the current size, when resizing
     */
    QLabel *m_lSize;

    /**
     * True when the mouse button is pressed
     */
    bool m_bMousePressed;

private:
};



/**
 * VBorder
 */
class VBorder : public QWidget
{
    Q_OBJECT
public:
    VBorder( QWidget *_parent, Canvas *_canvas, View *_view );
    virtual ~VBorder();

    int markerRow() const { return  m_iSelectionAnchor; }
    void equalizeRow( double resize );
    void updateRows( int from, int to );

    QSize sizeHint() const;

private slots:
    void doAutoScroll();

protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent( QMouseEvent* _ev );
    virtual void wheelEvent( QWheelEvent* );
    virtual void focusOutEvent( QFocusEvent* ev );
    void paintSizeIndicator( int mouseY, bool firstTime );

private:
    Canvas *m_pCanvas;
    View *m_pView;
    QTimer * m_scrollTimer;

    bool m_bSelection;
    int m_iSelectionAnchor;
    bool m_bResize;
    int m_iResizedRow;
    int m_iResizePos;
    /**
     * The label used for showing the current size, when resizing
     */
    QLabel *m_lSize;

    /**
     * True when the mouse button is pressed
     */
    bool m_bMousePressed;
};

} // namespace KSpread
#endif

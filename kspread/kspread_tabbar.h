#ifndef __kspread_tabbar_h__
#define __kspread_tabbar_h__

#include <qwidget.h>
#include <qpainter.h>
#include <qstrlist.h>

/**
 * This tab bar is used by @ref KSpreadView. It is used to choose between all
 * available tables.
 *
 * @short A bar with tabs and scroll buttons.
 */
class KSpreadTabBar : public QWidget
{
    Q_OBJECT
public:
    KSpreadTabBar( QWidget *_parent );

    /**
     * Adds a tab to the bar and paints it. The tab does not become active.
     * call @ref #setActiveTab to do so.
     */
    void addTab( const char *_text );
    /**
     * Removes the tab from the bar. If the tab was the active one then the one
     * left of it ( or if not available ) the one right of it will become active.
     * It is recommended to call @ref #setActiveTab after a call to this function.
     */
    void removeTab( const char *_text );
    /**
     * Removes all tabs from the bar and repaints the widget.
     */
    void removeAllTabs();
    
    void scrollLeft();
    void scrollRight();    
    void scrollFirst();
    void scrollLast();

    /**
     * Highlights this tab.
     */
    void setActiveTab( const char *_text );
    
signals:
    void tabChanged( const char *_text );
    
protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent ( QMouseEvent* _ev );

    void paintTab( QPainter & painter, int x, char *text, int text_width, int text_y, bool isactive );
    
    /**
     * List with the names of all tabs. The order in this list determines the
     * order of appearance.
     */
    QStrList tabsList;

    /**
     * This is the first visible tab on the left of the bar.
     */
    int leftTab;
    /**
     * The active tab in the range form 1..n.
     * If this value is 0, that means that no tab is active.
     */
    int activeTab;
};

#endif

#ifndef K_MULTI_TAB_BAR_P_H
#define K_MULTI_TAB_BAR_P_H
#include <qscrollview.h>
#include <kmultitabbar.h>

class KMultiTabBarInternal: public QScrollView
{
        Q_OBJECT
public:
        KMultiTabBarInternal(QWidget *parent,KMultiTabBar::KMultiTabBarBasicMode bm);
        int appendTab(const QPixmap &,int=-1,const QString& =QString::null);
        KMultiTabBarTab *getTab(int);
        void removeTab(int);
        void setPosition(enum KMultiTabBar::KMultiTabBarPosition pos);
        void setStyle(enum KMultiTabBar::KMultiTabBarStyle style);
        void showActiveTabTexts(bool show);
        QPtrList<KMultiTabBarTab>* tabs(){return &m_tabs;}
private:
        friend class KMultiTabBar;
        QHBox *box;
        QPtrList<KMultiTabBarTab> m_tabs;
        enum KMultiTabBar::KMultiTabBarPosition position;
        bool m_showActiveTabTexts;
        enum  KMultiTabBar::KMultiTabBarStyle m_style;
protected:
        virtual void drawContents ( QPainter *, int, int, int, int);

        /**
         * [contentsM|m]ousePressEvent are reimplemented from QScrollView
         * in order to ignore all mouseEvents on the viewport, so that the
         * parent can handle them.
         */
        virtual void contentsMousePressEvent(QMouseEvent *);
        virtual void mousePressEvent(QMouseEvent *);
};
#endif


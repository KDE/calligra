#ifndef KIVIO_LAYER_PANEL_H
#define KIVIO_LAYER_PANEL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qlayout.h>
#include <qlistview.h>
#include <qtoolbutton.h>
#include <qwidget.h>

class KivioLayer;
class KivioView;
class KivioCanvas;


class KivioLayerPanel : public QWidget
{
Q_OBJECT;

protected:
    QListView *m_pListView;
    QListViewItem *m_pCurItem;

    QGridLayout *m_pGrid;

    KivioView *m_pView;
    KivioCanvas *m_pCanvas;

    QPixmap *m_pEyePic, *m_pConnPic;
    QPixmap *m_pNullPic;

    QToolButton *m_pEye, *m_pConnector, *m_pProperties, *m_pPlus, *m_pMinus, *m_pUp, *m_pDown;

public:
    KivioLayerPanel( QWidget *, KivioView *, KivioCanvas * );
    virtual ~KivioLayerPanel();

public slots:
    void updateView();
    void selectLayer(QListViewItem *);

    void insertLayer();
    void removeLayer();
    void setLayerName();
    void toggleVisibility();
    void toggleConnectable();
    void layerUp();
    void layerDown();
    void dblClicked( QListViewItem * );
    void rightClick( QListViewItem *, const QPoint &, int );
};

#endif


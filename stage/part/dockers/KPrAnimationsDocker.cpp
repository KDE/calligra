#include "KPrAnimationsDocker.h"
#include "QListWidget"
#include "QVBoxLayout"
#include "KPrView.h"

KPrAnimationsDocker::KPrAnimationsDocker(QWidget* parent, Qt::WindowFlags flags)
: QDockWidget(parent, flags)
, m_view(0)
{
    setWindowTitle( i18n( "Shape Animations" ) );

    QWidget* base = new QWidget( this );
    m_layoutsView = new QListWidget( base );
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_layoutsView );
    base->setLayout( layout );
    setWidget( base );
}

void KPrAnimationsDocker::setView(KPrView* view)
{
    Q_ASSERT( view );
    if (m_view) {
        // don't disconnect the m_view->proxyObject as the object is already deleted
        disconnect(m_layoutsView, 0, this, 0);
    }
    m_view = view;
    //connect( m_view->proxyObject, SIGNAL( activePageChanged() ),
    //         this, SLOT( slotActivePageChanged() ) );

    // remove the layouts from the last view
    //m_layoutsView->clear();


    /*connect( m_layoutsView, SIGNAL( itemPressed( QListWidgetItem * ) ),
             this, SLOT( slotItemPressed( QListWidgetItem * ) ) );
    connect( m_layoutsView, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem * ) ),
             this, SLOT( slotCurrentItemChanged( QListWidgetItem *, QListWidgetItem * ) ) );*/
}

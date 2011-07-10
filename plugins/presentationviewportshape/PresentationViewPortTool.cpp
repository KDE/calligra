#include "PresentationViewPortTool.h"
#include "PresentationViewPortShape.h"

#include <QToolButton>
#include <QGridLayout>
#include <KLocale>
#include <KIconLoader>
#include <KUrl>
#include <KFileDialog>
#include <KIO/Job>

#include <KoCanvasBase.h>
#include <KoImageCollection.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

PresentationViewPortTool::PresentationViewPortTool( KoCanvasBase* canvas )
    : KoToolBase( canvas ),
      m_shape(0)
{
}

void PresentationViewPortTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);

    foreach (KoShape *shape, shapes) {
        m_shape = dynamic_cast<PresentationViewPortShape*>( shape );
        if ( m_shape )
            break;
    }
    if ( !m_shape )
    {
        emit done();
        return;
    }
    useCursor(Qt::ArrowCursor);
}

void PresentationViewPortTool::deactivate()
{
  m_shape = 0;
}

QWidget * PresentationViewPortTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QGridLayout *layout = new QGridLayout(optionWidget);

    QToolButton *button = 0;

    button = new QToolButton(optionWidget);
    button->setIcon(SmallIcon("open"));
    button->setToolTip(i18n( "Open EMF/WMF Shape"));
    layout->addWidget(button, 0, 0);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(changeUrlPressed()));

    return optionWidget;
}

void PresentationViewPortTool::changeUrlPressed()
{
    if (m_shape == 0)
        return;
    KUrl url = KFileDialog::getOpenUrl();
    if (!url.isEmpty()) {
        // TODO move this to an action in the libs, with a nice dialog or something.
        KIO::StoredTransferJob *job = KIO::storedGet(url, KIO::NoReload, 0);
        connect(job, SIGNAL(result(KJob*)), this, SLOT(setImageData(KJob*)));
    }

}

void PresentationViewPortTool::mouseDoubleClickEvent( KoPointerEvent *event )
{
    if(canvas()->shapeManager()->shapeAt(event->point) != m_shape) {
        event->ignore(); // allow the event to be used by another
        return;
    }
    changeUrlPressed();
}

void PresentationViewPortTool::setImageData(KJob *job)
{
    if (m_shape == 0) {
        return;
    }
    KIO::StoredTransferJob *transferJob = qobject_cast<KIO::StoredTransferJob*>(job);
    Q_ASSERT(transferJob);

    QByteArray newData = qCompress(transferJob->data());
//    ChangeVectorDataCommand *cmd = new ChangeVectorDataCommand(m_shape, newData);
  //  canvas()->addCommand(cmd);
}

#include <PresentationViewPortTool.moc>

/* This file is part of the KDE project
 * Copyright (C) 2011 Aakriti Gupta <aakriti.a.gupta@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
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
	//copied from PictureShape. To be changed ofcourse
        connect(job, SIGNAL(result(KJob*)), this, SLOT(setImageData(KJob*)));
    }

}

void PresentationViewPortTool::mousePressEvent(KoPointerEvent* event)
{
        KoSelection *selection = canvas()->shapeManager()->selection();
            selection->select( m_shape );
    
    
    event->ignore();

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
  //TODO
}


#include <PresentationViewPortTool.moc>

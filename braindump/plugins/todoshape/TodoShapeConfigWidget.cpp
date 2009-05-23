/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "TodoShapeConfigWidget.h"

#include <KoCanvasController.h>
#include <KoToolManager.h>

#include <TodoShape.h>

TodoShapeConfigWidget::TodoShapeConfigWidget()
{
  m_widget.setupUi(this);
  connect(m_widget.stateComboBox, SIGNAL(activated(int)), SIGNAL(propertyChanged()));
}

void TodoShapeConfigWidget::blockChildSignals( bool block )
{
  m_widget.stateComboBox->blockSignals(block);
}

void TodoShapeConfigWidget::open(KoShape *shape)
{
  m_shape = dynamic_cast<TodoShape*>( shape );
  if( ! m_shape )
    return;
  blockChildSignals(true);
//   m_widget.urlEdit->setText(m_shape->webPage()->mainFrame()->url().toString());
  blockChildSignals(false);
}

void TodoShapeConfigWidget::save()
{
  if( !m_shape )
    return;

//   QString newUrl = m_widget.urlEdit->text();
  KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
  if ( canvasController ) {
    KoCanvasBase* canvas = canvasController->canvas();
/*    if(newUrl != m_shape->webPage()->mainFrame()->url().toString())
    {
      canvas->addCommand(new ChangeUrl(this, m_shape, newUrl));
    }*/
  }
}

QUndoCommand * TodoShapeConfigWidget::createCommand()
{
    save();

    return 0;
}

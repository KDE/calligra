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

#include "WebShapeConfigWidget.h"

#include <QUndoCommand>
#include <QWebPage>
#include <QWebFrame>

#include "WebShape.h"
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>

class ChangeUrl : public QUndoCommand {
public:
  ChangeUrl( WebShapeConfigWidget* widget, WebShape* shape, const QUrl& newUrl ) : m_widget(widget), m_shape(shape), m_newUrl(newUrl), m_oldUrl(shape->url())
  {
  }
  virtual void undo()
  {
    m_shape->setUrl(m_oldUrl);
    if(m_widget->shape() == m_shape)
      m_widget->open(m_shape);
  }
  virtual void redo()
  {
    m_shape->setUrl(m_newUrl);
    if(m_widget->shape() == m_shape)
      m_widget->open(m_shape);
  }
private:
  WebShapeConfigWidget *m_widget;
  WebShape *m_shape;
  QUrl m_newUrl;
  QUrl m_oldUrl;
};

WebShapeConfigWidget::WebShapeConfigWidget()
{
  m_widget.setupUi(this);
  connect(m_widget.urlEdit, SIGNAL(editingFinished()), SIGNAL(propertyChanged()));
}

void WebShapeConfigWidget::blockChildSignals( bool block )
{
  m_widget.urlEdit->blockSignals(block);
}

void WebShapeConfigWidget::open(KoShape *shape)
{
  m_shape = dynamic_cast<WebShape*>( shape );
  if( !m_shape )
    return;
  blockChildSignals(true);
  m_widget.urlEdit->setText(m_shape->url().url());
  blockChildSignals(false);
}

void WebShapeConfigWidget::save()
{
  if( !m_shape )
    return;

  QString newUrl = m_widget.urlEdit->text();
  KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
  if ( canvasController ) {
    KoCanvasBase* canvas = canvasController->canvas();
    if(newUrl != m_shape->url().url())
    {
      canvas->addCommand(new ChangeUrl(this, m_shape, newUrl));
    }
  }
}

QUndoCommand * WebShapeConfigWidget::createCommand()
{
    save();

    return 0;
}

WebShape* WebShapeConfigWidget::shape()
{
  return m_shape;
}

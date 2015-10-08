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

#include "WebToolWidget.h"

#include <QWebPage>
#include <QWebFrame>

#include <kundo2command.h>

#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>


#include "WebShape.h"
#include "WebTool.h"

class ChangeUrl : public KUndo2Command
{
public:
    ChangeUrl(WebShape* shape, const QUrl& newUrl) : m_shape(shape), m_newUrl(newUrl), m_oldUrl(shape->url()) {
    }
    virtual void undo() {
        m_shape->setUrl(m_oldUrl);
    }
    virtual void redo() {
        m_shape->setUrl(m_newUrl);
    }
private:
    WebShape *m_shape;
    QUrl m_newUrl;
    QUrl m_oldUrl;
};

class ChangeCached : public KUndo2Command
{
public:
    ChangeCached(WebShape* shape) : m_shape(shape) {
        if(shape->isCached()) {
            m_cache = shape->cache();
        }
    }
    virtual void undo() {
        m_shape->setCached(!m_shape->isCached());
        if(m_shape->isCached()) {
            m_shape->setCache(m_cache);
        }
    }
    virtual void redo() {
        m_shape->setCached(!m_shape->isCached());
    }
private:
    WebShape *m_shape;
    QString m_cache;
};

WebToolWidget::WebToolWidget(WebTool* _tool) : m_tool(_tool), m_shape(0)
{
    m_widget.setupUi(this);
    connect(m_widget.urlEdit, SIGNAL(editingFinished()), SLOT(save()));
    connect(m_widget.useCache, SIGNAL(stateChanged(int)), SLOT(save()));
    connect(m_tool, SIGNAL(shapeChanged(WebShape*)), SLOT(open(WebShape*)));
}

void WebToolWidget::blockChildSignals(bool block)
{
    m_widget.urlEdit->blockSignals(block);
    m_widget.useCache->blockSignals(block);
}

void WebToolWidget::open(WebShape *shape)
{
    m_shape = shape;
    if(!m_shape)
        return;
    blockChildSignals(true);
    m_widget.urlEdit->setText(m_shape->url().url());
    m_widget.useCache->setChecked(m_shape->isCached());
    blockChildSignals(false);
}

void WebToolWidget::save()
{
    if(!m_shape)
        return;

    const QUrl newUrl = QUrl::fromUserInput(m_widget.urlEdit->text());
    bool newCached = m_widget.useCache->isChecked();
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    if(canvasController) {
        KoCanvasBase* canvas = canvasController->canvas();
        if(newUrl != m_shape->url()) {
            canvas->addCommand(new ChangeUrl(m_shape, newUrl));
        }
        if(newCached != m_shape->isCached()) {
            canvas->addCommand(new ChangeCached(m_shape));
        }
    }
}

KUndo2Command * WebToolWidget::createCommand()
{
    save();

    return 0;
}

WebShape* WebToolWidget::shape()
{
    return m_shape;
}

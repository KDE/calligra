/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPABackgroundToolWidget.h"

#include <QFileDialog>
#include <QUrl>

#include <KoNetAccess.h>

#include <KoCanvasBase.h>
#include <KoDocumentResourceManager.h>
#include <KoImageCollection.h>
#include <KoPatternBackground.h>
#include <KoShapeBackgroundCommand.h>
#include <KoShapeController.h>

#include "KoPABackgroundTool.h"
#include "commands/KoPADisplayMasterBackgroundCommand.h"
#include "commands/KoPADisplayMasterShapesCommand.h"
#include <KoPADocument.h>
#include <KoPAPage.h>
#include <KoPAViewBase.h>
#include <KoPageApp.h>

KoPABackgroundToolWidget::KoPABackgroundToolWidget(KoPABackgroundTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_tool(tool)
{
    setObjectName("KoPABackgroundToolWidget");
    widget.setupUi(this);

    widget.fillBackground->setView(m_tool->view());

    // adapt texts to type of pages
    const bool isSlideType = (m_tool->view()->kopaDocument()->pageType() == KoPageApp::Slide);
    const QString useMasterBackgroundText = isSlideType ? i18n("Use background of master slide") : i18n("Use background of master page");
    widget.useMasterBackground->setText(useMasterBackgroundText);
    const QString displayMasterShapesText = isSlideType ? i18n("Display shapes of master slide") : i18n("Display shapes of master page");
    widget.displayMasterShapes->setText(displayMasterShapesText);

    connect(widget.useMasterBackground, &QAbstractButton::clicked, this, &KoPABackgroundToolWidget::useMasterBackground);
    connect(widget.backgroundImage, &QAbstractButton::clicked, this, &KoPABackgroundToolWidget::setBackgroundImage);
    connect(widget.displayMasterShapes, &QAbstractButton::clicked, this, &KoPABackgroundToolWidget::displayMasterShapes);

    connect(m_tool->view()->proxyObject, &KoPAViewProxyObject::activePageChanged, this, &KoPABackgroundToolWidget::slotActivePageChanged);

    slotActivePageChanged();
}

KoPABackgroundToolWidget::~KoPABackgroundToolWidget() = default;

void KoPABackgroundToolWidget::slotActivePageChanged()
{
    KoPAPageBase *page = m_tool->view()->activePage();

    KoPAPage *normalPage = dynamic_cast<KoPAPage *>(page);

    widget.useMasterBackground->setEnabled(normalPage);
    widget.displayMasterShapes->setEnabled(normalPage);
    if (normalPage) {
        widget.useMasterBackground->setChecked(normalPage->displayMasterBackground());
        widget.displayMasterShapes->setChecked(normalPage->displayMasterShapes());

        const bool enableBackgroundEditing = (!normalPage->displayMasterBackground());
        widget.backgroundImage->setEnabled(enableBackgroundEditing);
    } else {
        widget.useMasterBackground->setChecked(false);
        widget.displayMasterShapes->setChecked(false);

        widget.backgroundImage->setEnabled(true);
    }
}

void KoPABackgroundToolWidget::setBackgroundImage()
{
    // TODO only make images selectable
    KoImageCollection *collection = m_tool->canvas()->shapeController()->resourceManager()->imageCollection();
    Q_ASSERT(collection);
    KoShape *page = m_tool->canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage);
    Q_ASSERT(page);
    if (!collection || !page) {
        return;
    }

    QUrl url = QFileDialog::getOpenFileUrl();
    if (!url.isEmpty()) {
        QString tmpFile;
        if (KIO::NetAccess::download(url, tmpFile, nullptr)) {
            QImage image(tmpFile);
            if (!image.isNull()) {
                const bool isSlideType = (m_tool->view()->kopaDocument()->pageType() == KoPageApp::Slide);
                const KUndo2MagicString commandTitle = isSlideType ? kundo2_i18n("Change slide background image") : kundo2_i18n("Change page background image");
                KUndo2Command *cmd = new KUndo2Command(commandTitle);
                QSharedPointer<KoPatternBackground> bg(new KoPatternBackground(collection));
                bg->setPattern(image);
                QSizeF imageSize = bg->patternOriginalSize();
                QSizeF pageSize = m_tool->view()->activePage()->size();
                KoPatternBackground::PatternRepeat repeat = KoPatternBackground::Original;
                if (imageSize.width() > pageSize.width() || imageSize.height() > pageSize.height()) {
                    qreal imageRatio = imageSize.width() / imageSize.height();
                    qreal pageRatio = pageSize.width() / pageSize.height();
                    if (qAbs(imageRatio - pageRatio) < 0.1) {
                        repeat = KoPatternBackground::Stretched;
                    } else {
                        qreal zoom = pageSize.width() / imageSize.width();
                        zoom = qMin(zoom, pageSize.height() / imageSize.height());

                        bg->setPatternDisplaySize(imageSize * zoom);
                    }
                }
                bg->setRepeat(repeat);

                new KoShapeBackgroundCommand(page, bg, cmd);
                m_tool->canvas()->addCommand(cmd);
            }
        }
    }
}

void KoPABackgroundToolWidget::useMasterBackground(bool doUse)
{
    KoPAPage *page = dynamic_cast<KoPAPage *>(m_tool->canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage));
    if (page) {
        KoPADisplayMasterBackgroundCommand *cmd = new KoPADisplayMasterBackgroundCommand(page, doUse);
        m_tool->canvas()->addCommand(cmd);
    }

    const bool enableBackgroundEditing = (!doUse);
    widget.backgroundImage->setEnabled(enableBackgroundEditing);
}

void KoPABackgroundToolWidget::displayMasterShapes(bool doDisplay)
{
    KoPAPage *page = dynamic_cast<KoPAPage *>(m_tool->canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage));
    if (page) {
        KoPADisplayMasterShapesCommand *cmd = new KoPADisplayMasterShapesCommand(page, doDisplay);
        m_tool->canvas()->addCommand(cmd);
    }
}

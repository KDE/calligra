/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLECITATIONINDEXWIDGET_H
#define SIMPLECITATIONINDEXWIDGET_H

#include <ui_SimpleCitationBibliographyWidget.h>
#include <KoListStyle.h>
#include "FormattingButton.h"

#include <QWidget>
#include <QTextBlock>

class ReferencesTool;
class KoStyleManager;
class KoBibliographyInfo;
class BibliographyPreview;
class BibliographyTemplate;

class SimpleCitationBibliographyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleCitationBibliographyWidget(ReferencesTool *tool,QWidget *parent = 0);
    ~SimpleCitationBibliographyWidget() override;

public Q_SLOTS:
    void setStyleManager(KoStyleManager *sm);
    void prepareTemplateMenu();
    void pixmapReady(int templateId);

private Q_SLOTS:
    void applyTemplate(int templateId);
    void insertCustomBibliography();

Q_SIGNALS:
    void doneWithFocus();
    
private:
    Ui::SimpleCitationBibliographyWidget widget;
    KoStyleManager *m_styleManager;
    bool m_blockSignals;
    QTextBlock m_currentBlock;
    ReferencesTool *m_referenceTool;
    QList<KoBibliographyInfo *> m_templateList;
    //each template in the template list will have have a previewGenerator that will be deleted after preview is generated
    QList<BibliographyPreview *> m_previewGenerator;
    ItemChooserAction *m_chooser;
    BibliographyTemplate *m_templateGenerator;
};

#endif

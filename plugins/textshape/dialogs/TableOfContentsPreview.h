#ifndef TABLEOFCONTENTSPREVIEW_H
#define TABLEOFCONTENTSPREVIEW_H
/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <KoZoomHandler.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextRangeManager.h>

#include <QFrame>
#include <QPixmap>

class TextShape;
class KoTableOfContentsGeneratorInfo;
class KoStyleManager;

class TableOfContentsPreview : public QFrame
{
    Q_OBJECT
public:
    explicit TableOfContentsPreview(QWidget *parent = 0);
    ~TableOfContentsPreview() override;
    void setStyleManager(KoStyleManager *styleManager);
    /// sets the size of the generated preview pixmap if not set then it takes the widget's size
    void setPreviewSize(const QSize &size);
    QPixmap previewPixmap();

protected:
    void paintEvent(QPaintEvent *event) override;

Q_SIGNALS:
    void pixmapGenerated();
public Q_SLOTS:
    void updatePreview(KoTableOfContentsGeneratorInfo *info);

private Q_SLOTS:
    void finishedPreviewLayout();

private:
    TextShape *m_textShape;
    QPixmap *m_pm;
    KoZoomHandler m_zoomHandler;
    KoStyleManager *m_styleManager;
    KoInlineTextObjectManager m_itom;
    KoTextRangeManager m_tlm;
    QSize m_previewPixSize;

    void deleteTextShape();

};

#endif // TABLEOFCONTENTSPREVIEW_H

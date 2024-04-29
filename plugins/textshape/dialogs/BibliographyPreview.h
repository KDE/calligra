#ifndef BIBLIOGRAPHYPREVIEW_H
#define BIBLIOGRAPHYPREVIEW_H
/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <KoInlineTextObjectManager.h>
#include <KoTextRangeManager.h>
#include <KoZoomHandler.h>

#include <QFrame>
#include <QPixmap>

class TextShape;
class KoBibliographyInfo;
class KoStyleManager;

class BibliographyPreview : public QFrame
{
    Q_OBJECT
public:
    explicit BibliographyPreview(QWidget *parent = nullptr);
    ~BibliographyPreview() override;
    void setStyleManager(KoStyleManager *styleManager);
    /// sets the size of the generated preview pixmap if not set then it takes the widget's size
    void setPreviewSize(const QSize &size);
    QPixmap previewPixmap();

protected:
    void paintEvent(QPaintEvent *event) override;

Q_SIGNALS:
    void pixmapGenerated();
public Q_SLOTS:
    void updatePreview(KoBibliographyInfo *info);

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

#endif // BIBLIOGRAPHYPREVIEW_H

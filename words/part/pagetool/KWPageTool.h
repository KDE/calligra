/*
 *  SPDX-FileCopyrightText: 2011 Jignesh Kakadiya <jigneshhk1992@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KWPAGETOOL_H
#define KWPAGETOOL_H

#include <KoToolBase.h>

class KWCanvas;
class KWDocument;

class KWPageTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KWPageTool(KoCanvasBase *canvas);
    ~KWPageTool() override;

public:
    void paint(QPainter &painter, const KoViewConverter &converter) override;

public Q_SLOTS:
    void activate(KoToolBase::ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;

public: // Events
    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    //  virtual void mouseDoubleClickEvent(KoPointerEvent *event);

    //  virtual void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
    /// Force the remaining content on the page to next page.
    void insertPageBreak();

private:
    KWCanvas *getCanvas() const;
    KWDocument *getDocument() const;

protected:
    QList<QPointer<QWidget>> createOptionWidgets() override;

private:
    KWCanvas *m_canvas;
    KWDocument *m_document;
};

#endif

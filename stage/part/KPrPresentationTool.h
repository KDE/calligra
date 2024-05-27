/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2009 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Jim Courtiau <jeremy.courtiau@gmail.com>
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * SPDX-FileCopyrightText: 2009 Johann Hingue <yoan1703@hotmail.fr>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KPRPRESENTATIONTOOL_H
#define KPRPRESENTATIONTOOL_H

#include <KoToolBase.h>

#include <QSet>

class QFrame;
class KoEventAction;
class KPrViewModePresentation;
class KPrPresentationToolWidget;
class KPrPresentationStrategyBase;

#ifdef WITH_QTDBUS
class KPrPresentationToolAdaptor;
#endif

/// The tool used for presentations
class KPrPresentationTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KPrPresentationTool(KPrViewModePresentation &viewMode);
    ~KPrPresentationTool() override;

    bool wantsAutoScroll() const override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(KoPointerEvent *event) override;

    KPrPresentationStrategyBase *strategy();
    KPrViewModePresentation &viewModePresentation();

public Q_SLOTS:
    void activate(KoToolBase::ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;
    void highlightPresentation();
    void drawOnPresentation();
    void blackPresentation();
    void normalPresentation();

private:
    void finishEventActions();
    void switchStrategy(KPrPresentationStrategyBase *strategy);
    bool eventFilter(QObject *obj, QEvent *event) override;

    /**
     * Returns true if shape is a TextShape and event->point is over hyperlink
     * @param event the mouse event
     * @param shape the shape fhich is searched for hyperlink
     * @param hyperLink the string which is filled with hyperlink url
     */
    bool checkHyperlink(KoPointerEvent *event, KoShape *shape, QString &hyperLink);

    /**
     * Runs url string defined inside hyperlink
     * @param hyperLink the hyperlink string
     */
    void runHyperlink(const QString &hyperLink);

    KPrViewModePresentation &m_viewMode;
    QSet<KoEventAction *> m_eventActions;

    KPrPresentationToolWidget *m_presentationToolWidget;
    QFrame *m_frame;
    KPrPresentationStrategyBase *m_strategy;

#ifdef WITH_QTDBUS
    KPrPresentationToolAdaptor *m_bus;
#endif

    friend class KPrPresentationStrategyBase;
};

#endif /* KPRPRESENTATIONTOOL_H */

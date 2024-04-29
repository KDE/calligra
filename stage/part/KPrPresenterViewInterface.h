/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPRESENTERVIEWINTERFACE
#define KPRPRESENTERVIEWINTERFACE

#include "KPrPresenterViewBaseInterface.h"

#include <QFrame>

class QLabel;
class QTextEdit;

class KoPACanvas;
class KoPAPageBase;

/**
 * KPrPresenterViewInterface
 * This widget is the main interface, this widget shows current slide, next slide
 * and the presenter's notes
 */
class KPrPresenterViewInterface : public KPrPresenterViewBaseInterface
{
    Q_OBJECT
public:
    KPrPresenterViewInterface(const QList<KoPAPageBase *> &pages, KoPACanvas *canvas, QWidget *parent = nullptr);

    void setPreviewSize(const QSize &size);

public Q_SLOTS:
    /// reimplemented
    void setActivePage(int pageIndex) override;

private:
    KoPACanvas *m_canvas;
    QLabel *m_currentSlideLabel;
    QLabel *m_nextSlideLabel;
    QLabel *m_nextSlidePreview;
    QTextEdit *m_notesTextEdit;
    QSize m_previewSize;
};

#endif

/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>
  SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef EMFVIEWER_H
#define EMFVIEWER_H

#include "../EmfParser.h"

#include <QLabel>
#include <QMainWindow>

using namespace Libemf;

class EmfViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit EmfViewer(QSize &size);
    ~EmfViewer();

    void loadFile(const QString &fileName);

private Q_SLOTS:
    void slotOpenFile();

private:
    // Actions
    QAction *m_fileOpenAction;
    QAction *m_fileQuitAction;

    // The central widget
    QLabel *m_label;

    QSize m_size;
};

#endif

/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MCTWIDGET_H
#define MCTWIDGET_H

#include <QWidget>
#include "MctMain.h"

class KWDocument;

namespace Ui {
class MctWidget;
}

class MctWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MctWidget(QWidget *parent, KWDocument *document, QString fileUrl);
    ~MctWidget();

    bool status();
    bool hasMct();
    //void setBackgroundColor();
    QString getFileUrl() const;

private:
    void enableUi(bool enable);
    bool checkExtension();
    KWDocument *getKoDoc();
    void setFileUrl(QString url);

public slots:
    void updateFileStatus(bool enabled);
    void startChangeTracking();
    void createMctChange(KoShape &selection, MctChangeTypes changeType, const KUndo2MagicString title, QString fileUrl, ChangeAction action);
    void createRevisionOnSave();

private slots:
    void on_buttonStart_clicked();
    void on_buttonStop_clicked();
    void on_buttonCreate_clicked();
    void on_buttonRedo_clicked();
    void on_buttonMergeManager_clicked();
    void on_buttonRevisionManager_clicked();
    void on_buttonClose_clicked();

    void mergeManager();
    void revisionManager();
    void adjustListOfRevisions();
    void stopChangTracking();

    void redoRevision(QString revisionIndex);
    void removeSelectedRevision(QString target);
    void clearRevisionHistory();

    void normalizeBuffer();

    void updateMctStatus(bool enable);

signals:
    void start();
    void sendMctStatus(bool status);
    void createRevision(QString author, QString comment);
    void restoreRevision();
    void openRevisionManager();
    void openMergeManager();
    void save();
    void saveAs(bool);

    void clearComboBox();
    void refreshRevisionManager();

private:
    Ui::MctWidget *ui;

    KWDocument *document;
    QString fileUrl;
    MctMain *mct;

    bool isFileSaved;
    bool isEnabled;
};

#endif // MCTWIDGET_H

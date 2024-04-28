/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef PARAGRAPHSTYLESMODEL_H
#define PARAGRAPHSTYLESMODEL_H

#include <QModelIndex>

class ParagraphStylesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QObject *textEditor READ textEditor WRITE setTextEditor NOTIFY textEditorChanged)
    Q_PROPERTY(QFont cursorFont READ cursorFont NOTIFY cursorFontChanged)
    Q_PROPERTY(int currentStyle READ currentStyle NOTIFY cursorFontChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)

public:
    enum ParagraphStyleRoles { Name = Qt::UserRole + 1, Current, Font, FontFamily, FontPointSize, FontWeight, FontItalic, FontUnderline };
    ParagraphStylesModel();
    ~ParagraphStylesModel() override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;

    QObject *document() const;
    void setDocument(QObject *newDocument);

    QObject *textEditor() const;
    void setTextEditor(QObject *newEditor);

    Q_SLOT void cursorPositionChanged();

    Q_INVOKABLE void activate(int index);

    QFont cursorFont() const;
    int currentStyle() const;

    qreal zoomLevel() const;
    void setZoomLevel(const qreal &newZoom);
Q_SIGNALS:
    void documentChanged();
    void textEditorChanged();
    void cursorFontChanged();
    void zoomLevelChanged();

private:
    class Private;
    Private *d;
};

#endif // PARAGRAPHSTYLESMODEL_H

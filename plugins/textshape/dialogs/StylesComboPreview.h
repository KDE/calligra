/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STYLESCOMBOPREVIEW_H
#define STYLESCOMBOPREVIEW_H

#include <QLineEdit>

class QImage;
class QPushButton;
class QSize;
class QString;

/** This is an internal class, used for the preview of styles in the main area of the @class StylesCombo. */

class StylesComboPreview : public QLineEdit
{
    Q_OBJECT

    Q_PROPERTY(bool showAddButton READ isAddButtonShown WRITE setAddButtonShown)

public:
    explicit StylesComboPreview(QWidget *parent = nullptr);
    ~StylesComboPreview() override;

    QSize availableSize() const;
    void setAddButtonShown(bool show);
    bool isAddButtonShown() const;

    void setPreview(const QImage &image);

Q_SIGNALS:
    void resized();
    void newStyleRequested(const QString &name);
    void clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private Q_SLOTS:
    void addNewStyle();

private:
    void init();
    void updateAddButton();

    bool m_renamingNewStyle;
    bool m_shouldAddNewStyle;

    QImage m_stylePreview;

    QPushButton *m_addButton;
};

#endif // STYLESCOMBOPREVIEW_H

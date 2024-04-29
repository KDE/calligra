/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Pierre Stirnweiss <pstirnweiss@googlemail.com>
   SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FORMATTINGPREVIEW_H
#define FORMATTINGPREVIEW_H

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>

#include <QFont>
#include <QFrame>
#include <QWidget>

class QString;
class KoStyleThumbnailer;

class FormattingPreview : public QFrame
{
    Q_OBJECT

public:
    explicit FormattingPreview(QWidget *parent = nullptr);
    ~FormattingPreview() override;

public Q_SLOTS:
    /// Character properties
    void setCharacterStyle(const KoCharacterStyle *style);
    void setParagraphStyle(const KoParagraphStyle *style);

    void setText(const QString &sampleText);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_sampleText;

    KoCharacterStyle *m_characterStyle;
    KoParagraphStyle *m_paragraphStyle;
    KoStyleThumbnailer *m_thumbnailer;
    bool m_previewLayoutRequired;
};

#endif // FORMATTINGPREVIEW_H

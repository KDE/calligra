#ifndef STYLESMODELNEW_H
#define STYLESMODELNEW_H

#include <QAbstractListModel>
#include <QList>

class KoCharacterStyle;
class KoStyleThumbnailer;

class StylesModelNew : public QAbstractListModel
{
public:
    enum Roles {
        StylePointer = Qt::UserRole + 1,
    };

    StylesModelNew(QObject *parent = 0);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void setStyleThumbnailer(KoStyleThumbnailer *thumbnailer);
    void setStyles(const QList<KoCharacterStyle *> styles);
    void addStyle(KoCharacterStyle *style);
    void removeStyle(KoCharacterStyle *style);
    void replaceStyle(KoCharacterStyle *oldStyle, KoCharacterStyle *newStyle);
    void updateStyle(KoCharacterStyle *style);

    QModelIndex styleIndex(KoCharacterStyle *style);

private:
    QList<KoCharacterStyle *> m_styles;
    KoStyleThumbnailer *m_styleThumbnailer;
};

#endif /* STYLESMODELNEW_H */

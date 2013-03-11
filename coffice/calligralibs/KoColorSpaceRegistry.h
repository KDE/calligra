#ifndef CALLIGRADOCUMENTS_KCOLORSPACEREGISTRY_H
#define CALLIGRADOCUMENTS_KCOLORSPACEREGISTRY_H

#include <QColor>
#include <KoColorSpace.h>
//class KoColorProfile;
//class KoColorSpace;

class KoColorSpaceRegistry
{
public:
    static KoColorSpaceRegistry* instance()
    {
        static KoColorSpaceRegistry *s_instance = 0;
        if (!s_instance)
            s_instance = new KoColorSpaceRegistry();
        return s_instance;
    }

    enum ColorSpaceListVisibility { OnlyUserVisible = 1, AllColorSpaces = 4 };
    enum ColorSpaceListProfilesSelection { OnlyDefaultProfile = 1, AllProfiles = 4 };

    void add(KoColorSpaceFactory* item) {}
    void remove(KoColorSpaceFactory* item) {}
    void addProfileToMap(KoColorProfile *p) {}
    void addProfile(KoColorProfile* profile);
    void addProfile(const KoColorProfile* profile) {}
    void removeProfile(KoColorProfile* profile) {}
    void addProfileAlias(const QString& name, const QString& to) {}
    QString profileAlias(const QString& name) const { return QString(); }
    const KoColorProfile* createColorProfile(const QString & colorModelId, const QString & colorDepthId, const QByteArray& rawData) { return 0; }
    const KoColorProfile *  profileByName(const QString & name) const { return 0; }
    QList<const KoColorProfile *>  profilesFor(const KoColorSpaceFactory * factory) const { return QList<const KoColorProfile *>(); }
    //QList<const KoColorProfile *>  profilesFor(const KoID& id) const { return QList<const KoColorProfile *>(); }
    QList<const KoColorSpaceFactory*> colorSpacesFor(const KoColorProfile* _profile) const { return QList<const KoColorSpaceFactory*>(); }
    QList<const KoColorProfile *>  profilesFor(const QString& id) const { return QList<const KoColorProfile *>(); }
    const KoColorSpaceFactory* colorSpaceFactory(const QString &colorSpaceId) const {}
    const KoColorSpace * colorSpace(const QString & colorModelId, const QString & colorDepthId, const KoColorProfile *profile) { return 0; }
    const KoColorSpace * colorSpace(const QString & colorModelId, const QString & colorDepthId, const QString &profileName) { return 0; }
    QString colorSpaceId(const QString & colorModelId, const QString & colorDepthId) const { return QString(); }
    //QString colorSpaceId(const KoID& colorModelId, const KoID& colorDepthId) const { return QString(); }
    //KoID colorSpaceColorModelId(const QString & _colorSpaceId) const {}
    //KoID colorSpaceColorDepthId(const QString & _colorSpaceId) const {}
    const KoColorSpace * alpha8() { return 0; }
    const KoColorSpace * rgb8(const QString &profileName = QString()) { return 0; }
    const KoColorSpace * rgb8(const KoColorProfile * profile) { return 0; }
    const KoColorSpace * rgb16(const QString &profileName = QString()) { return 0; }
    const KoColorSpace * rgb16(const KoColorProfile * profile) { return 0; }
    const KoColorSpace * lab16(const QString &profileName = QString()) { return 0; }
    const KoColorSpace * lab16(const KoColorProfile * profile) { return 0; }
    //QList<KoID> colorModelsList(ColorSpaceListVisibility option) const;
    //QList<KoID> colorDepthList(const KoID& colorModelId, ColorSpaceListVisibility option) const;
    //QList<KoID> colorDepthList(const QString & colorModelId, ColorSpaceListVisibility option) const;
    //const KoColorConversionSystem* colorConversionSystem() const;
    //KoColorConversionCache* colorConversionCache() const;
    //const KoColorSpace* permanentColorspace(const KoColorSpace* _colorSpace);
    //QList<KoID> listKeys() const;
    //QList<const KoColorSpace*> allColorSpaces(ColorSpaceListVisibility visibility, ColorSpaceListProfilesSelection pSelection);

};

#endif
 

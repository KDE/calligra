#ifndef FAKE_KUSER_H
#define FAKE_KUSER_H

class KUser
{
public:
    enum UIDMode { UseEffectiveUID, UseRealUserID };
    enum UserProperty { FullName, RoomNumber, WorkPhone, HomePhone };
    KUser(UIDMode = UseEffectiveUID) {}
    QVariant property(UserProperty) const { return QVariant(); }
};
    
#endif

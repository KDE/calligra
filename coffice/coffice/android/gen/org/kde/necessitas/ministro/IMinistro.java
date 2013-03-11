/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /home/snoopy/src/calligra/documents/calligradocuments/android/src/org/kde/necessitas/ministro/IMinistro.aidl
 */
package org.kde.necessitas.ministro;
public interface IMinistro extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements org.kde.necessitas.ministro.IMinistro
{
private static final java.lang.String DESCRIPTOR = "org.kde.necessitas.ministro.IMinistro";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an org.kde.necessitas.ministro.IMinistro interface,
 * generating a proxy if needed.
 */
public static org.kde.necessitas.ministro.IMinistro asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = (android.os.IInterface)obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof org.kde.necessitas.ministro.IMinistro))) {
return ((org.kde.necessitas.ministro.IMinistro)iin);
}
return new org.kde.necessitas.ministro.IMinistro.Stub.Proxy(obj);
}
public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_requestLoader:
{
data.enforceInterface(DESCRIPTOR);
org.kde.necessitas.ministro.IMinistroCallback _arg0;
_arg0 = org.kde.necessitas.ministro.IMinistroCallback.Stub.asInterface(data.readStrongBinder());
android.os.Bundle _arg1;
if ((0!=data.readInt())) {
_arg1 = android.os.Bundle.CREATOR.createFromParcel(data);
}
else {
_arg1 = null;
}
this.requestLoader(_arg0, _arg1);
reply.writeNoException();
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements org.kde.necessitas.ministro.IMinistro
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
/**
* Check/download required libs to run the application
*
* param callback  - interface used by Minsitro service to notify the client when the loader is ready
* param parameters
*            parameters fields:
*                 * Key Name                   Key type         Explanations
*                   "required.modules"         StringArray      Required modules by your application
*                   "application.title"        String           Application name, used to show more informations to user
*                   "qt.provider"              String           Qt libs provider, currently only "necessitas" is supported.
*                   "minimum.ministro.api"     Integer          Minimum Ministro API level, used to check if Ministro service compatible with your application. Current API Level is 1 !
*                   "minimum.qt.version"       Integer          Minimim Qt version (e.g. 0x040800, which means Qt 4.8.0, check http://doc.trolltech.com/4.8/qtglobal.html#QT_VERSION)!
*                   "3rd.party.repositories"   StringArray      3rd party repositories, which should be downloaded by ministro, needs minimum.ministro.api >= 2
*                   Check http://community.kde.org/Necessitas/Ministro for more details.
*/
public void requestLoader(org.kde.necessitas.ministro.IMinistroCallback callback, android.os.Bundle parameters) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeStrongBinder((((callback!=null))?(callback.asBinder()):(null)));
if ((parameters!=null)) {
_data.writeInt(1);
parameters.writeToParcel(_data, 0);
}
else {
_data.writeInt(0);
}
mRemote.transact(Stub.TRANSACTION_requestLoader, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
}
static final int TRANSACTION_requestLoader = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
}
/**
* Check/download required libs to run the application
*
* param callback  - interface used by Minsitro service to notify the client when the loader is ready
* param parameters
*            parameters fields:
*                 * Key Name                   Key type         Explanations
*                   "required.modules"         StringArray      Required modules by your application
*                   "application.title"        String           Application name, used to show more informations to user
*                   "qt.provider"              String           Qt libs provider, currently only "necessitas" is supported.
*                   "minimum.ministro.api"     Integer          Minimum Ministro API level, used to check if Ministro service compatible with your application. Current API Level is 1 !
*                   "minimum.qt.version"       Integer          Minimim Qt version (e.g. 0x040800, which means Qt 4.8.0, check http://doc.trolltech.com/4.8/qtglobal.html#QT_VERSION)!
*                   "3rd.party.repositories"   StringArray      3rd party repositories, which should be downloaded by ministro, needs minimum.ministro.api >= 2
*                   Check http://community.kde.org/Necessitas/Ministro for more details.
*/
public void requestLoader(org.kde.necessitas.ministro.IMinistroCallback callback, android.os.Bundle parameters) throws android.os.RemoteException;
}

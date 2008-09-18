%define name kivio
%define version 0.9.1
%define release 2tkc
%define prefix /usr/local

Name: %{name}
Version: %{version}
Release: %{release}
Summary: Kivio - Powerful Flowchart Program for KDE 2
Prefix: %{prefix}
Group: KDE/Office
Copyright: GPL
Vendor: theKompany.com
Packager: Catalin Climov <xxl@thekompany.com>
#Distribution: theKompany.com
Source: %{name}-%{version}.tar.gz
URL: http://www.thekompany.com/projects/kivio/
Buildroot: /var/tmp/%{name}-%{version}-buildroot

%description
Kivio on the surface is your everyday flowcharting program.
Underneath this skin, however, lies much more. Kivio will offer basic
 flowcharting abilities, but with a twist. Objects are scriptable, and
 a backend plugin system will offer the ability to make objects do just
 about anything.

%prep
rm -rf $RPM_BUILD_ROOT

%setup -n %{name}-%{version}

%build
make -f Makefile.cvs
./configure --with-install-root=$RPM_BUILD_ROOT
make

%install
make prefix=$RPM_BUILD_ROOT%{prefix} install

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.%{name}
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.%{name}
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.%{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files -f ../file.list.%{name}

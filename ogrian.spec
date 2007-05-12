Name        : ogrian
Summary     : Ogrian Carpet is a first-person 'shooter' inspired by Bullfrog's Magic Carpet and a remake of a remake of Arabian Flights
Version     : 0.9.2
Release     : 1
License     : GPL
Group       : Amusements/Games/Action
Provides    : ogrian
Source      : %{name}-%{version}.tar.gz
BuildRoot   : %{_tmppath}/build-root-%{name}
Packager    : Jeremiah D. Powell <waveclaw@hotmail.com>
Vendor      : Waveclaw.net
Distribution: SuSE
Url         : http://orgian.sourceforge.net
Requires    : ogre3d >= 1.4, RakNet >= 2.4.5.2, fMod >= 4.04.14


%description
You have been charged by your god Meriach to bring order to chaos. 

There are innumerable islands in this world that are inhabited by the minions of the chaos god Marduk. You will be sent to each island in turn to destroy the chaos minions and construct a holy fortress.

To aid you in this quest, Meriach has provied you with an artifact of great power, an enchanted flying carpet imbued with great magical powers of both creation and destruction.

The power of this Ogrian Carpet will ensure your triumph over the legions of chaos. There are other gods who have sent avatars to claim the chaos islands for themselves. Defeat them, raze their heretical altars to the ground and add to your glory!

%prep -q
rm -rf $RPM_BUILD_ROOT 
mkdir $RPM_BUILD_ROOT

%setup -q

%build
autoconf
./configure
make

%install
mkdir -p $RPM_BUILD_ROOT%{_prefix}/games/ogrian
make install DESTDIR=$RPM_BUILD_ROOT%{_prefix}/ bindir=games/ogrian 
cp -p *.cfg *.txt *.ico $RPM_BUILD_ROOT%{_prefix}/games/ogrian/
cp -pR Media $RPM_BUILD_ROOT%{_prefix}/games/ogrian/

%clean
rm -rf $RPM_BUILD_ROOT

%files 
%{_prefix}/games/ogrian
%{_prefix}/games/ogrian/ogrian
%{_prefix}/games/ogrian/ogrian.cfg  
%{_prefix}/games/ogrian/plugins.cfg  
%{_prefix}/games/ogrian/constants.cfg  
%{_prefix}/games/ogrian/config.cfg  
%{_prefix}/games/ogrian/ogrian.ico
%doc AUTHORS README INSTALL TODO ChangeLog

%package media
Summary: Default content for Ogrian Carpet
Group: Amusements/Games/Action

%description media
Contains the developer's content (resources) for Ogrian Carpet.  Can you say 'programmer art?'

%files media
%{_prefix}/games/ogrian/resources.cfg
%{_prefix}/games/ogrian/castleskins.txt  
%{_prefix}/games/ogrian/maps.txt  
%{_prefix}/games/ogrian/shrineskins.txt  
%{_prefix}/games/ogrian/strings.txt  
%{_prefix}/games/ogrian/wizardskins.txt
%{_prefix}/games/ogrian/Media

%changelog -n ogrian
* Fri May 11 2007 - waveclaw@hotmail.com
- new version 0.9.2

dnl Autoconf script written in M4 to write the Configure script for writing Makefiles for Ogrian Carpet.
AC_INIT([Ogrian Carpet],
        [0.9.2],
        [ogrian@ogrian.sourceforge.net],
        [ogrian])
dnl AC_DEFINE([datadir],[Ogrian],[Shipped default content.])
dnl AC_CHECK_DECLS([HAVE_WOMAN], AC_WARNING([Hands off scum!]),AC_WARNING([tehehe!]))
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER([include/config.h])
AM_INIT_AUTOMAKE([dist-zip])

AC_LANG_CPLUSPLUS

dnl Portability tests from autoscan
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_CHECK_FUNCS([gettimeofday]) 
AC_CHECK_FUNCS([sqrt])
AC_CHECK_HEADERS([sys/time.h])
AC_C_CONST
AC_C_INLINE
AC_FUNC_FORK
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_PROG_RANLIB
AC_TYPE_PID_T
AC_TYPE_SIZE_T


dnl Manual process to check for fmod
dnl AC _CHECK_LIB([fmodexp64],FMOD_System_Create,[AC _DEFINE([FMOD_LIBS],[-Lfmodexp64],[fMod Sound API library.])],
dnl	     AC _MSG_ERROR([Please install the fMod Sound API library from http://fmod.org.]),)
dnl AC _CHECK_HEADER([fmodex/fmod.h],[AC _DEFINE([FMOD_CFLAGS],[-Ifmodex/fmod.h],[fMod development headers.])],
dnl		[AC _MSG_ERROR([Please install the fMod development headers from http://fmod.org.])],)

dnl These depend upon packages installed with pkg-config files
PKG_CHECK_MODULES([fMod], [fMod >= 4.04],,
		  AC_MSG_ERROR([Please install the fMod Sound API library from http://fmod.org.]))
PKG_CHECK_MODULES([OGRE], [OGRE >= 1.4],,
		  AC_MSG_ERROR([Ogrian Carpet requires Ogre 3D at least of version 1.4.]))
PKG_CHECK_MODULES([RakNet], [RakNet >= 2.4.5.2],,
		  [AC_MSG_ERROR([Raknet networking support not found, please visit http://rakarsoft.net.])])

PKG_CHECK_MODULES([CEGUI], [CEGUI >= 0.5],,
		  [AC_MSG_ERROR([Crazy-Eddies GUI not found, please visit http://www.cegui.org.uk/])])

PKG_CHECK_MODULES([OIS], [OIS >= 1.0],,
		  [AC_MSG_ERROR([Object-Oriented Input System not found, please visit http://sourceforge.net/projects/wgois/])])

AM_CXXFLAGS="$AM_CXXFLAGS $OGRE_CFLAGS $RakNet_CFLAGS $fMod_CFLAGS $CEGUI_CFLAGS $IOS_CFLAGS -Wall -Wimplicit -Wunused -g -pg"
AM_LDFLAGS="$AM_LDFLAGS $OGRE_LIBS $RakNet_LIBS $fMod_LIBS $CEGUI_LIBS $OIS_LIBS"

AC_SUBST(AM_CXXFLAGS, "$AM_CXXFLAGS")
AC_SUBST(AM_LDFLAGS, "$AM_LDFLAGS")
AC_SUBST(PKGDATADIR, "${datadir}/${PACKAGE}")

AC_CONFIG_FILES([Makefile include/Makefile src/Makefile Tests/Makefile])

AC_OUTPUT

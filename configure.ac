AC_INIT([dbus-signal], [0.0.0])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign 1.13 subdir-objects -Wall -Werror])
AM_SILENT_RULES([yes])

SYSTEMD_REQUIRED=225

AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
	AC_MSG_ERROR([You need pkg-config to continue])
])
AC_PROG_CC

PKG_CHECK_MODULES(libsystemd, [
	libsystemd >= $SYSTEMD_REQUIRED
], [
	AC_SUBST([libsystemd_CFLAGS])
	AC_SUBST([libsystemd_LIBS])
], [
	AC_MSG_ERROR([
		You need to install libsystemd to continue
	])
])

AC_CONFIG_SRCDIR([src/emitter.c])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

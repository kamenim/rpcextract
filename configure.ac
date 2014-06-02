# Simple configuration script for rpcextract

AC_PREREQ(2.57)
AC_INIT(rpcextract, 1.0, [jmedegan@openchange.org])
AC_CONFIG_HEADER([config.h])
AC_DEFINE(_GNU_SOURCE, 1, [Use GNU extensions])

AC_PROG_CC

dnl ***********************
dnl Check for PERL
dnl ***********************

AC_PATH_PROG(PERL, perl)

if test -z "$PERL"
then
	AC_MSG_ERROR(Please install perl)
fi

dnl ***********************
dnl Check for Pidl
dnl ***********************

AC_PATH_PROG(PIDL, pidl)

if test -z "$PIDL"
then
	AC_MSG_ERROR(Please install pidl)
fi

dnl ***********************
dnl Check for libpcap
dnl ***********************

AC_ETHEREAL_PCAP_CHECK

dnl ***********************
dnl Makefiles 
dnl ***********************

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

dnl ***********************
dnl Output 
dnl ***********************


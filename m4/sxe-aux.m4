dnl sxe-aux.m4 -- just some auxiliary macros

dnl -------------------------------------------------------------------------
dnl Local macros
dnl -------------------------------------------------------------------------

AC_DEFUN([SXE_USAGE_ERROR], [dnl
(echo "$progbasename: Usage error:"
echo " " $1
echo "  Use \`$progname --help' to show usage.") >&2 && exit 1])

dnl SXE_PRINT_VAR(var var ...)  prints values of shell variables
AC_DEFUN([SXE_PRINT_VAR],[for var in patsubst([$1],[[
]+],[ ]); do eval "echo \"$var = '\$$var'\""; done])

dnl SXE_ADD_OBJS(foo.o ...)
AC_DEFUN([SXE_ADD_OBJS], [
	extra_objs="$extra_objs [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. uncertain) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_OBJS

dnl SXE_ADD_RUNPATH_DIR(directory)
AC_DEFUN([SXE_ADD_RUNPATH_DIR], [dnl
{
	xe_runpath_dir=$1
	if test "$xe_runpath_dir" != "/lib" -a \
		"$xe_runpath_dir" != "/usr/lib" -a \
		-n "`ls ${xe_runpath_dir}/*.s[[ol]] 2>/dev/null`"; then
		eval "$xe_add_unique_runpath_dir"
	fi
}
])dnl SXE_ADD_RUNPATH_DIR


AC_DEFUN([SXE_CHECK_USER_SPECIFIED_P], [dnl
	## arg1 var-name to check for
	## generates a variable `arg1'_uspecified_p
	pushdef([VARNAME], [$1])
	pushdef([VARNAME_USPECIFIED_P], [$1_uspecified_p])
	pushdef([USER_VARNAME], [USER_$1])

	AC_MSG_CHECKING([for user specified ]VARNAME)
	if test "${[]VARNAME[]-unset}" != "unset"; then
		VARNAME_USPECIFIED_P[]=yes;
		USER_VARNAME=${[]VARNAME[]}
	else
		VARNAME_USPECIFIED_P[]=no;
		USER_VARNAME=
	fi
	AC_MSG_RESULT([$]VARNAME_USPECIFIED_P)
	popdef([VARNAME])
	popdef([VARNAME_USPECIFIED_P])
	popdef([USER_VARNAME])
])dnl SXE_CHECK_USER_SPECIFIED_P

AC_DEFUN([SXE_CHECK_USER_VARS], [dnl
	## If we don't set CFLAGS here, AC_PROG_CC will set it.
	## But we know better what's good for us, so we do our own
	## computation of real CFLAGS later.
	SXE_CHECK_USER_SPECIFIED_P([CFLAGS])
	SXE_CHECK_USER_SPECIFIED_P([CPPFLAGS])
	SXE_CHECK_USER_SPECIFIED_P([LDFLAGS])
	SXE_CHECK_USER_SPECIFIED_P([XFLAGS])
	SXE_CHECK_USER_SPECIFIED_P([LIBS])
	SXE_CHECK_USER_SPECIFIED_P([CC])
	SXE_CHECK_USER_SPECIFIED_P([CPP])
	SXE_CHECK_USER_SPECIFIED_P([AR])
	SXE_CHECK_USER_SPECIFIED_P([YACC])
	SXE_CHECK_USER_SPECIFIED_P([YFLAGS])
	SXE_CHECK_USER_SPECIFIED_P([XMKMF])
])dnl SXE_CHECK_USER_VARS


dnl SXE_ADD_EVENTS_OBJS(foo.o ...)
AC_DEFUN([SXE_ADD_EVENTS_OBJS], [
	libsxeevents_objs="$libsxeevents_objs [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. EVENTS) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_EVENTS_OBJS
AC_DEFUN([SXE_SUBST_EVENTS_OBJS], [AC_SUBST(libsxeevents_objs)])

dnl SXE_ADD_CORE_OBJS(foo.o ...)
AC_DEFUN([SXE_ADD_CORE_OBJS], [
	libsxecore_objs="$libsxecore_objs [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. CORE) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_CORE_OBJS
AC_DEFUN([SXE_SUBST_CORE_OBJS], [AC_SUBST(libsxecore_objs)])

dnl SXE_ADD_ENT_OBJS(foo.o ...)
AC_DEFUN([SXE_ADD_ENT_OBJS], [
	libent_objs="$libent_objs [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. ENT) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_ENT_OBJS
AC_DEFUN([SXE_SUBST_ENT_OBJS], [AC_SUBST(libent_objs)])

dnl SXE_ADD_MM_OBJS(foo.o ...)
AC_DEFUN([SXE_ADD_MM_OBJS], [
	libmm_objs="$libmm_objs [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. MM) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_MM_OBJS
AC_DEFUN([SXE_SUBST_MM_OBJS], [AC_SUBST(libmm_objs)])

dnl SXE_ADD_MULE_OBJS(foo.o ...)
AC_DEFUN([SXE_ADD_MULE_OBJS], [
	libmule_objs="$libmule_objs [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. MULE) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_MULE_OBJS
AC_DEFUN([SXE_SUBST_MULE_OBJS], [AC_SUBST(libmule_objs)])

dnl SXE_ADD_DB_OBJS(foo.o ...)
AC_DEFUN([SXE_ADD_DB_OBJS], [
	libdb_objs="$libdb_objs [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. DB) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_DB_OBJS
AC_DEFUN([SXE_SUBST_DB_OBJS], [
	AC_SUBST([libdb_objs])
	AC_SUBST([DB_CPPFLAGS])
	AC_SUBST([DB_LDFLAGS])
	AC_SUBST([DB_LIBS])
])dnl SXE_SUBST_DB_OBJS

dnl SXE_ADD_MEMALLOC_OBJS(foo.o ...)
AC_DEFUN([SXE_ADD_MEMALLOC_OBJS], [
	libmemalloc_objs="$libmemalloc_objs [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. MEMALLOC) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_MEMALLOC_OBJS
AC_DEFUN([SXE_SUBST_MEMALLOC_OBJS], [AC_SUBST(libmemalloc_objs)])

dnl SXE_ADD_SXEUI_OBJS(foo.o ...)
AC_DEFUN([SXE_ADD_SXEUI_OBJS], [
	libsxeui_objs="$libsxeui_objs [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. SXE UI) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_SXEUI_OBJS
AC_DEFUN([SXE_SUBST_SXEUI_OBJS], [AC_SUBST(libsxeui_objs)])

dnl SXE_ADD_STATMOD_A(foo.o ...)
AC_DEFUN([SXE_ADD_STATMOD_A], [
	statmod_archives="$statmod_archives [$1]" && \
	if test "$extra_verbose" = "yes"; then
		echo "    sxemacs (cat. static module) will be linked with \"[$1]\""
	fi
])dnl SXE_ADD_STATMOD_A
AC_DEFUN([SXE_SUBST_STATMOD_A], [AC_SUBST(statmod_archives)])

dnl SXE_APPEND(value, varname)
AC_DEFUN([SXE_APPEND],
[[$2]="$[$2] [$1]" && dnl
 if test "$extra_verbose" = "yes"; then echo "    Appending \"[$1]\" to \$[$2]"; fi])

dnl SXE_PREPEND(value, varname)
AC_DEFUN([SXE_PREPEND],
[[$2]="[$1] $[$2]" && dnl
 if test "$extra_verbose" = "yes"; then echo "    Prepending \"[$1]\" to \$[$2]"; fi])

dnl SXE_DIE(message)
AC_DEFUN([SXE_DIE], [{ echo "Error:" $1 >&2; exit 1; }])


dnl SXE_CHECK_FEATURE_DEPENDENCY(feature1, feature2)
AC_DEFUN([SXE_CHECK_FEATURE_DEPENDENCY],
[if test "$with_$1 $with_$2" = "yes no"; then
	SXE_USAGE_ERROR("--with-$1 requires --with-$2")
elif test "$with_$2" = "no" ; then with_$1=no
elif test "$with_$1" = "yes"; then with_$2=yes
fi
])

dnl SXE_STRIP_4TH_COMPONENT(var)
dnl Changes i986-pc-linux-gnu to i986-pc-linux, as God (not RMS) intended.
AC_DEFUN([SXE_STRIP_4TH_COMPONENT],
[$1=`echo "$$1" | sed '[s/^\([^-][^-]*-[^-][^-]*-[^-][^-]*\)-.*$/\1/]'`])

dnl AC_DEFUN([SXE_DEBUG_AC], [AS_MESSAGE($@)])
AC_DEFUN([SXE_DEBUG_AC], [])

dnl Do our best to deal with automounter brokenness
dnl SXE_CANONICALISE_PATH(varname)
AC_DEFUN([SXE_CANONICALISE_PATH], [dnl
	## arg #1 is the file/path to canonicalise
	pushdef([tmpp], [$1])

	SXE_DEBUG_AC([canonicalising \$]tmpp[: "]$[]tmpp["])

	if test -d "/net"; then
		if test -d "/tmp_mnt/net"; then
			tdir="tmp_mnt/net";
		else
			tdir="tmp_mnt";
		fi
		tmpp[]=$(echo "[$]tmpp[]" | \
			sed -e "s|^${tdir}/|/net/|" \
			    -e "s|^/a/|/net/|" \
			    -e "s|^/amd/|/net/|")
	fi

dnl 	if test -d "[$]tmpp"; then
dnl 		## the easy way ...
dnl 		## just let the filesystem implementation do the hard work
dnl 		tmpp[]=$(cd [$]tmpp[] && echo $(pwd))
dnl 	elif test -f "[$]tmpp"; then
dnl 		## the easy way ...
dnl 		## just let the filesystem implementation do the hard work
dnl 		tmpp[]=$(cd $(dirname [$]tmpp[]) && \
dnl 			echo $(pwd)/$(basename [$]tmpp[]))
dnl 	else
		## simple magic
		canonicalised="no"
		while test "$canonicalised" = "no"; do
			_SXE_CANONICALISE_STEP(tmpp,
				[canonicalised="yes"], [canonicalised="no"])
		done
dnl	fi

	SXE_DEBUG_AC([canonicalised \$]tmpp[: "]$[]tmpp["])
	popdef([tmpp])
])dnl SXE_CANONICALISE_PATH

AC_DEFUN([_SXE_CANONICALISE_STEP], [
	## _SXE_CANONICALISE_STEP( VAR, DO-ON-SUCCESS, DO-ON-FAILURE)
	## arg #1 is the varname to canonicalise
	## arg #2 is the code to execute on success
	## arg #3 is the code to execute on failure
	pushdef([tmpvar], [$1])
	pushdef([dosucc], [$2])
	pushdef([dofail], [$3])

	tmp2=[$]tmpvar
	tmp3=
	tmp4=$(basename "$tmp2")
	## assume we had success
	dosucc
	while test "$tmp4" != "/" -a "$tmp4" != "."; do
		if test "$tmp4" = ".." -a \
			$(basename $(dirname "$tmp2")) = "."; then
			## we prepend and we know the path
			## is still dirty but wont do anything
			## alternative would be to prepend $(pwd) now
			dosucc
			tmp3="$(basename $tmp2)/$tmp3"
		elif test "$tmp4" = ".." -a \
			$(basename $(dirname "$tmp2")) != ".."; then
			tmp2=$(dirname "$tmp2")
		elif test "$tmp4" = ".."; then
			## we prepend, but now we know the path
			## is still dirty
			dofail
			tmp3="$(basename $tmp2)/$tmp3"
		elif test -n "$tmp3"; then
			## ordinary component, just prepend
			tmp3="$(basename $tmp2)/$tmp3"
		else
			## just set
			tmp3="$(basename $tmp2)"
		fi
		tmp2=$(dirname "$tmp2")
		tmp4=$(basename "$tmp2")
	done

	tmp3="$tmp4/$tmp3"
	tmpvar[]=$(echo "$tmp3" | sed -e "s@//@/@g")

	popdef([tmpvar])
	popdef([dosucc])
	popdef([dofail])
])dnl _SXE_CANONICALISE_STEP


dnl SXE_PROTECT_LINKER_FLAGS(shell_var)
AC_DEFUN([SXE_PROTECT_LINKER_FLAGS], [
if test "$GCC" = "yes"; then
  set x $[$1]; shift; [$1]=""
  while test -n "[$]1"; do
    case [$]1 in
      -L  | -l  | -u               ) [$1]="$[$1] [$]1 [$]2"; shift ;;
      -L* | -l* | -u* | -Wl* | -pg ) [$1]="$[$1] [$]1" ;;
      -Xlinker* ) ;;
      * ) [$1]="$[$1] -Xlinker [$]1" ;;
    esac
    shift
  done
fi])dnl


dnl Allow use of either ":" or spaces for lists of directories
AC_DEFUN([SXE_COLON_TO_SPACE],
  [case "$[$1]" in *:* [)] [$1]="`echo '' $[$1] | sed -e 's/^ //' -e 's/:/ /g'`";; esac])dnl


dnl The construct foo=`echo $w1 $w2 $w3` fails on some systems if $w1 = -e or -n
dnl So we use the following instead.
dnl SXE_SPACE(var, words)
AC_DEFUN([SXE_SPACE],[
T=""
for W in $2; do if test -z "$T"; then T="$W"; else T="$T $W"; fi; done
$1="$T"
])dnl SXE_SPACE



AC_DEFUN([SXE_DUMP_LIBS], [
	save_c_switch_site="$c_switch_site"
	save_LIBS="$LIBS"
	save_ac_c_werror_flag="$ac_c_werror_flag"
	save_ld_switch_site="$ld_switch_site"
	save_ld_switch_machine="$ld_switch_machine"
	save_CC="$CC"
	save_CFLAGS="$CFLAGS"
	save_CPPFLAGS="$CPPFLAGS"
	save_LDFLAGS="$LDFLAGS"
])dnl SXE_DUMP_LIBS

AC_DEFUN([SXE_RESTORE_LIBS], [
	LIBS="$save_LIBS"
	c_switch_site="$save_c_switch_site"
	ac_c_werror_flag="$save_ac_c_werror_flag"
	ld_switch_site="$save_ld_switch_site"
	ld_switch_machine="$save_ld_switch_machine"
	CC="$save_CC"
	CFLAGS="$save_CFLAGS"
	CPPFLAGS="$save_CPPFLAGS"
	LDFLAGS="$save_LDFLAGS"
])dnl SXE_RESTORE_LIBS

AC_DEFUN([SXE_SEARCH_CONFIG_PROG], [
	## arg #1 program to search
	pushdef([config_prog], [$1])
	pushdef([have_config_prog], [have_]translit([$1], [-], [_]))
	pushdef([CONFIG_PROG], translit([$1], [-a-z], [_A-Z]))
	AC_CHECK_PROG(have_config_prog, config_prog, [yes], [no])
	AC_PATH_PROG(CONFIG_PROG, config_prog, [echo])
	popdef([config_prog])
	popdef([have_config_prog])
	popdef([CONFIG_PROG])
])dnl SXE_SEARCH_CONFIG_PROG


AC_DEFUN([_SXE_CHECK_pkgconfig_based], [
	## assumes $PKG_CONFIG is defined
	## arg #1: MM param name
	## arg #2: lib to check
	## arg #3: version of that lib

	pushdef([IO_LIB], [$1])

	AC_MSG_CHECKING([for ]IO_LIB[ support])
	AC_MSG_RESULT([])

	if test "$have_pkg_config" = "no" -o -z "$PKG_CONFIG"; then
		AS_MESSAGE([*** pkg-config not found. See http://pkgconfig.sourceforge.net])
		AS_MESSAGE([*** Cannot check for ]IO_LIB[.])
		have_pkg_config=no
		PKG_CONFIG=
	fi

	pushdef([IO_MOD], [$2])
	pushdef([IO_MOD_REQUIRED_VERSION], [$3])
	AC_MSG_CHECKING([whether ]IO_MOD[ is at least ]IO_MOD_REQUIRED_VERSION)
	if test -n "$PKG_CONFIG" && \
	     $PKG_CONFIG --atleast-version IO_MOD_REQUIRED_VERSION IO_MOD; then
		IO_LIB[]_version=`$PKG_CONFIG --modversion []IO_MOD[]`
		AC_MSG_RESULT([yes ($]IO_LIB[_version)])
		have_[]IO_LIB[]_pkg="yes"
		## define some useful variables
		IO_LIB[]_libs=`$PKG_CONFIG --libs []IO_MOD[]`
		IO_LIB[]_libs_L=`$PKG_CONFIG --libs-only-L []IO_MOD[]`
		IO_LIB[]_libs_l=`$PKG_CONFIG --libs-only-l []IO_MOD[]`
		IO_LIB[]_libs_other=`$PKG_CONFIG --libs-only-other []IO_MOD[]`
		IO_LIB[]_cflags=`$PKG_CONFIG --cflags []IO_MOD[]`
		IO_LIB[]_cflags_I=`$PKG_CONFIG --cflags-only-I []IO_MOD[]`
		IO_LIB[]_cflags_other=`$PKG_CONFIG --cflags-only-other []IO_MOD[]`
	elif test -n "$PKG_CONFIG"; then
		IO_LIB[]_version=`$PKG_CONFIG --modversion []IO_MOD[]`
		AC_MSG_RESULT([no ($]IO_LIB[_version)])
		have_[]IO_LIB[]_pkg="no"
	else
		have_[]IO_LIB[]_pkg="uncertain"
		AC_MSG_RESULT([$have_]IO_LIB[])
	fi
	popdef([IO_LIB])
	popdef([IO_MOD_REQUIRED_VERSION])
	popdef([IO_MOD])
])dnl _SXE_CHECK_pkgconfig_based

AC_DEFUN([_SXE_MM_CHECK_pkgconfig_based], [
	## assumes $PKG_CONFIG is defined
	## arg #1: MM param name
	## arg #2: lib to check
	## arg #3: version of that lib
	## arg #4: funs to check
	## arg #5: headers to check
	## arg #6: success action
	## arg #7: failure action

	pushdef([MM_LIB], [$1])
	pushdef([MM_SUCC], [$6])
	pushdef([MM_FAIL], [$7])

	AC_MSG_CHECKING([for ]MM_LIB[ support])
	AC_MSG_RESULT([])

	if test "$have_pkg_config" = "no" -o -z "$PKG_CONFIG"; then
		AS_MESSAGE([*** pkg-config not found. See http://pkgconfig.sourceforge.net])
		AS_MESSAGE([*** Cannot check for ]MM_LIB[.])
		have_pkg_config=no
		PKG_CONFIG=
		MM_FAIL
	fi

	pushdef([MM_MOD], [$2])
	pushdef([MM_MOD_REQUIRED_VERSION], [$3])
	pushdef([MM_MOD_FUNS], [$4])
	pushdef([MM_MOD_HDRS], [$5])
	AC_MSG_CHECKING([whether ]MM_MOD[ is at least ]MM_MOD_REQUIRED_VERSION)
	if test -n "$PKG_CONFIG" && \
	     $PKG_CONFIG --atleast-version MM_MOD_REQUIRED_VERSION MM_MOD; then
		actual_version=`$PKG_CONFIG --modversion []MM_MOD[]`
		AC_MSG_RESULT([yes ($actual_version)])
		AC_MSG_CHECKING([for ]mm_lib[ libraries])
		MM_MOD_LIBS_L=`$PKG_CONFIG --libs-only-l []MM_MOD[]`
		MM_MOD_LIBS=`echo $MM_MOD_LIBS_L | sed -e "s/-l//g"`
		MM_MOD_LIB=`echo $MM_MOD_LIBS | sed -e "s/ .*$//"`
		MM_MOD_OTHER_LIBS=`echo $MM_MOD_LIBS_L | sed -e "s/^[^ ]*//"`
		AC_MSG_RESULT([$MM_MOD_LIBS])

		## backup our site flags
		SXE_DUMP_LIBS
		MM_MOD_LIBS_SITE=`$PKG_CONFIG --libs-only-L []MM_MOD[]`
		MM_MOD_HDRS_SITE=`$PKG_CONFIG --cflags-only-I []MM_MOD[]`
		SXE_PREPEND([$MM_MOD_HDRS_SITE], [CPPFLAGS])
		SXE_PREPEND([$MM_MOD_LIBS_SITE], [LDFLAGS])

		MM_SUCC
		SXE_CHECK_HEADERS([]MM_MOD_HDRS[], [:], [MM_FAIL])
		for i in MM_MOD_FUNS; do
			AC_CHECK_LIB([$MM_MOD_LIB], [$i], [:], [MM_FAIL],
				[$MM_MOD_LIBS_L])
		done

		## restore old values
		SXE_RESTORE_LIBS
	elif test -n "$PKG_CONFIG"; then
		actual_version=`$PKG_CONFIG --modversion []MM_MOD[]`
		AC_MSG_RESULT([no ($actual_version)])
	else
		AC_MSG_RESULT([uncertain])
	fi
	popdef([MM_LIB])
	popdef([MM_MOD_FUNS])
	popdef([MM_MOD_HDRS])
	popdef([MM_MOD_REQUIRED_VERSION])
	popdef([MM_MOD])
	popdef([MM_SUCC])
	popdef([MM_FAIL])
])dnl _SXE_MM_CHECK_pkgconfig_based


AC_DEFUN([SXE_CHECK_SUFFICIENCY], [dnl
	## arg #1 is the option to check sufficiency on
	## arg #2 is a description
	## arg #3 are additional actions to perform when support is sufficient
	## arg #4 are additional actions to perform when support is insufficient
	pushdef([og_group], translit([$1], [-], [_]))
	pushdef([og_descr], [$2])
	pushdef([OG_GROUP], translit([$1], [-a-z], [_A-Z]))
	pushdef([IF_YES_DO], [$3])
	pushdef([IF_NO_DO], [$4])

	tmp=${[have_]og_group[]-uncertain}
	if test "$with_[]og_group" != "no"; then
		AC_MSG_CHECKING([for sufficient ]og_descr[ support])
		AC_MSG_RESULT([$tmp])
		if test "$tmp" = "yes"; then
			AC_DEFINE([HAVE_]OG_GROUP, [1],
				[Whether ]og_descr[ is doable])
			:
			IF_YES_DO
		else
			:
			IF_NO_DO
		fi
	else
		:
		IF_NO_DO
	fi
	AM_CONDITIONAL([HAVE_]OG_GROUP,
		[test "$have_[]og_group" = "yes" -a "$with_[]og_group" != "no"])
	popdef([og_group])
	popdef([og_descr])
	popdef([OG_GROUP])
	popdef([IF_YES_DO])
	popdef([IF_NO_DO])
])dnl SXE_CHECK_SUFFICIENCY


AC_DEFUN([SXE_CHECK_HEADERS], [dnl
	dnl One day, this macro will only check for headers if
	dnl their presence is yet unknown
	AC_CHECK_HEADERS($1,$2,$3,$4)
])dnl SXE_CHECK_HEADERS

AC_DEFUN([SXE_CHECK_LIB_FUNCS], [dnl
	pushdef([SXE_LIB], [$1])
	pushdef([SXE_FUNCS], [$2])

	for i in SXE_LIB; do
		for j in SXE_FUNCS; do
			AC_CHECK_LIB([$i], [$j], [:], [:])
		done
	done

	popdef([SXE_LIB])
	popdef([SXE_FUNCS])
])dnl SXE_CHECK_LIB_FUNCS


AC_DEFUN([_SXE_CHECK_PURE_OPT], [dnl
	unq_opt=`echo $1 | tr -d "'\""`
	AC_MSG_CHECKING([if option $1 is known])

	case "$unq_opt" in
	--enable-* | --disable-* )
		strp_opt=`echo $unq_opt | \
			sed -e "s/^--enable-//g" -e "s/^--disable-//g"`
		OG_CHECK_OPTION([enable], [$strp_opt],
			AC_MSG_RESULT([yes]),
			AC_MSG_RESULT([no])
			[bogus_cmd_opts="$bogus_cmd_opts $unq_opt"])
		;;
	--with-* | --without-* )
		strp_opt=`echo $unq_opt | \
			sed -e "s/^--with-//g" -e "s/^--without-//g"`
		OG_CHECK_OPTION([with], [$strp_opt],
			AC_MSG_RESULT([yes]),
			AC_MSG_RESULT([no])
			[bogus_cmd_opts="$bogus_cmd_opts $unq_opt"])
		;;
	* )
		## whatever that is
		AC_MSG_RESULT([yes])
		;;
	esac
])dnl _SXE_CHECK_PURE_OPT

AC_DEFUN([SXE_CHECK_COMMAND_LINE], [dnl
	dnl Check for bogus options
	sxe_with_options="$sxe_with_options x"
	cmd_line_args="$ac_configure_args"

	for opt in $cmd_line_args; do
		pure_opt=`echo $opt | sed -e "s/=.*$//g"`
		_SXE_CHECK_PURE_OPT([$pure_opt])
	done

	dnl Final command line argument checks.
	dnl --with-quantify or --with-purify imply --with-system-malloc
	if test "$with_purify" = "yes" -o "$with_quantify" = "yes"; then
		if test "$with_system_malloc" = "default"; then
			with_system_malloc="yes"
		fi
	fi
])dnl SXE_CHECK_COMMAND_LINE



dnl sxe-aux.m4 ends here

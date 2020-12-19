#!/usr/bin/bash

# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: CC0-1.0

cd "$(dirname $0)"
BASEDIR="$(pwd)"
SRCDIR="$(dirname $BASEDIR)"

function xgettext_wrapper {
    xgettext --copyright-holder="This file is copyright:" \
        --package-name=plasma5-wallpapers-dynamic \
        --msgid-bugs-address=https://github.com/zzag/plasma5-wallpapers-dynamic/issues \
        --from-code=UTF-8 \
        -C --kde \
        -ci18n \
        -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 \
        -ki18nd:2 -ki18ndc:2c,3 -ki18ndp:2,3 -ki18ndcp:2c,3,4 \
        -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
        -kki18nd:2 -kki18ndc:2c,3 -kki18ndp:2,3 -kki18ndcp:2c,3,4 \
        -kxi18n:1 -kxi18nc:1c,2 -kxi18np:1,2 -kxi18ncp:1c,2,3 \
        -kxi18nd:2 -kxi18ndc:2c,3 -kxi18ndp:2,3 -kxi18ndcp:2c,3,4 \
        -kkxi18n:1 -kkxi18nc:1c,2 -kkxi18np:1,2 -kkxi18ncp:1c,2,3 \
        -kkxi18nd:2 -kkxi18ndc:2c,3 -kkxi18ndp:2,3 -kkxi18ndcp:2c,3,4 \
        -kI18N_NOOP:1 -kI18NC_NOOP:1c,2 \
        -kI18N_NOOP2:1c,2 -kI18N_NOOP2_NOSTRIP:1c,2 \
        -ktr2i18n:1 -ktr2xi18n:1 \
        "$@"
}
export -f xgettext_wrapper

podir=$BASEDIR/po
files=`find $SRCDIR -name Messages.sh`
dirs=`for i in $files; do echo \`dirname $i\`; done | sort -u`

for subdir in $dirs; do
    cd $subdir

    if test -f Messages.sh; then
        podir=$podir srcdir=. XGETTEXT="xgettext_wrapper" bash Messages.sh
    fi
done

pofiles=`find $podir -name \*.po`
for po in $pofiles; do
    msgmerge -o $po.new $po $podir/plasma_wallpaper_com.github.zzag.dynamic.pot
    mv $po.new $po
done

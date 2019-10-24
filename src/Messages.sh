#! /usr/bin/env bash

$XGETTEXT `find . -name \*.qml -o -name \*.cc` -o $podir/template.pot

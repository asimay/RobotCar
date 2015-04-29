#!/bin/sh

if [ $# -gt 0 ] ; then
    base=`basename $1`
    echo "compiling $base"
    gcc -ggdb `pkg-config opencv --cflags --libs` $base.c -o $base
fi

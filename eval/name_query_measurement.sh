#!/usr/bin/env bash

LIST=`node -e " var hoge='hoge'; for (var i=0; i<=100; i++) { console.log(hoge+i); } "`

for i in ${LIST}
do
    ../test/name_pkt ${i}
done

#!/usr/bin/env bash

LIST=`node -e " var hoge='hoge'; for (var i=0; i<=10000; i++) { console.log(hoge+i+'.mydns'); } "`

for i in ${LIST}
do
    ../test/name_tfo_pkt ${i} 192.168.0.1
done

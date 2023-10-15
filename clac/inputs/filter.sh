#!/bin/bash

i=1
while read -r line;do
  if ! [[ $(../execution/clac.exe "$line") =~ ^(nan|-nan|inf|-inf)$ ]];then
    echo s $i \""$line"\"
    ((i++))
  fi
done

#!/bin/bash

i=1
while read -r line;do
  result=$(../execution/quich.exe "$line")
  code=$?
  if ! [[ $result =~ ^(nan|-nan|inf|-inf)$ ]];then
    if [[ $code -gt 0 ]];then continue;fi
    echo s $i \""$line"\"
    ((i++))
  fi
done

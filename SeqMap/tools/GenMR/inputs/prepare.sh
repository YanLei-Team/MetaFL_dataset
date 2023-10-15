#!/bin/bash

cp -r ../../../inputs/{p,T,TestPool_all} .
shuf -n 10 TestPool_all > TestPool_sample10

#!/bin/bash

# Peş peşe komutları test et
echo "cat << t1 << t2 > samet.txt | cd . | cat << t3 > samet2.txt"
echo "cat << t1 << t2 > samet.txt | echo selam | cat << t3 > samet2.txt"
echo "exit" 
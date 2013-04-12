#! /bin/bash

for archivo in *.decaf
do
    nombre="${archivo%%.*}"
    ../dcc < "$archivo" >& "$nombre.outerr"
done

for archivo in *.out
do
    nombre="${archivo%%.*}"
    diff -w "$archivo" "$nombre.outerr" > "$nombre.resultado"
    if [ -s "$nombre.resultado" ]; then gedit "$nombre.resultado"; fi
done

echo "Listo!"

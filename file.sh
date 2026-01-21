#!/bin/sh
ls

a=3

sh -c 'echo $a'

export a
exit
sh -c 'echo $a'

export b
sh -c 'echo $a'
sh -c 'echo $b'

export c=5
sh -c 'echo $a'
sh -c 'echo $b'
sh -c 'echo $c'

echo $c

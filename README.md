* semtool something POST
* semtool something WAIT
* timeout 10 semtool something WAIT
* timeout=1.5 semtool something WAIT
* semtool something UNLINK
* semtool something POST
* semtool something WAIT
* semtool something UNLINK
* initial=1 semtool something WAIT
* semtool something UNLINK
* initial=2 semtool something WAIT
* semtool something WAIT
* timeout=1 semtool something WAIT
* semtool something UNLINK
* initial=1 semtool something POST
* semtool something WAIT
* semtool something WAIT
* semtool something UNLINK
* name=something op=WAIT initial=1 semtool
* name=something semtool UNLINK

...and that’s about it.

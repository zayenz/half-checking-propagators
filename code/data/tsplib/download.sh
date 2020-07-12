wget http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/index.html
for f in $(egrep -o "[A-Za-z0-9]+\.tsp" index.html | sort | uniq) ; do wget http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/$f ; done
for f in $(egrep -o "[A-Za-z0-9.]+\.tour" index.html | sort | uniq) ; do wget http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/$f ; done

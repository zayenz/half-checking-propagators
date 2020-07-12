#!/bin/bash

# shellcheck disable=SC2086

for time in "-time 1000" "-time 5000" "-time 10000" "-time 60000"  "-time 120000"
#for time in "-time 1000"
do
    for solutions in "-solutions 1 -assets 1 -threads 1" "-print-last true -solutions 0 -threads 2"
    do
    #	for case in "-tsp-grid 6" "-tsp-grid 10" "-file ../data/euc2d_tsplib/berlin52.tsp" "-file ../data/euc2d_tsplib/berlin52.tsp"  "-file ../data/euc2d_tsplib/eil51.tsp" "-file ../data/euc2d_tsplib/eil76.tsp" "-file ../data/euc2d_tsplib/eil101.tsp"  "-file ../data/euc2d_tsplib/lin105.tsp"
      for case in "-tsp-grid 6" "-tsp-grid 10"
	    do
        for config in "-domination-propagation false"  "-domination-propagation true" "-warnsdorff-domination-2-propagation true"  "-christofides-propagation true"  "-one-tree-propagation true"
        do
            for nogoods in "-use-all-nogoods false"  "-use-all-nogoods true"
            do
              echo
              echo "Solutions ${solutions} case ${case} time ${time} config ${config} nogoods ${nogoods}"
              echo ./src/programs/tsp-main  -branching-val min-length $time $solutions $case $config $nogoods
              ./src/programs/tsp-main -branching-val min-length $time $solutions $case $config $nogoods
            done
        done
	    done
	done
done

for time in "-time 1000" "-time 5000" "-time 10000" "-time 60000"  "-time 120000"
#for time in "-time 1000"
do
    for solutions in "-solutions 1 -assets 1 -threads 1" "-print-last true -solutions 0 -threads 2"
    do
      for inst in berlin52.tsp eil52.tsp eil76.tsp eil101.tsp lin105 pr76.tsp pr107.tsp pr124.tsp pr136.tsp pr144.tsp pr152.tsp
	    do
        for config in "-domination-propagation false"  "-domination-propagation true" "-warnsdorff-domination-2-propagation true"  "-christofides-propagation true"  "-one-tree-propagation true"
        do
            for nogoods in "-use-all-nogoods false"  "-use-all-nogoods true"
            do
                echo
                echo "Solutions ${solutions} case -file ../data/euc2d_tsplib/${inst} time ${time} config ${config} nogoods ${nogoods}"
                echo ./src/programs/tsp-main  -branching-val min-length $time $solutions -file ../data/euc2d_tsplib/$inst $config $nogoods
                ./src/programs/tsp-main -branching-val min-length $time $solutions -file ../data/euc2d_tsplib/$inst $config $nogoods
            done
        done
	    done
	done
done

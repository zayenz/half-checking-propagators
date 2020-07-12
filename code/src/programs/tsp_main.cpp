#include <sstream>
#include <vector>
#include <iostream>
#include <chrono>

#include <gecode/driver.hh>

#include "config.h"
#include "utilities/runner.h"
#include "models/tsp.h"

int main(int argc, char **argv) {
    // Clock function used.
    auto now = [] { return std::chrono::steady_clock::now(); };

    const auto model_start = now();

    hc::InspectorTSPModelOptions opt;
    opt.parse(argc, argv);
    hc::run<hc::TSPModel,
            Gecode::BAB,
            hc::InspectorTSPModelOptions>(opt);

    // Report results
    //
    const auto model_end = now();
    const std::chrono::duration<double, std::milli> model_duration =
            model_end - model_start;

    std::cout << "Ran model in " << model_duration.count()
              << std::endl;

    return EXIT_SUCCESS;
}


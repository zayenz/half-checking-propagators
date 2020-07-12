#include <sstream>
#include <vector>
#include <iostream>
#include <chrono>

#include <gecode/driver.hh>

#include "config.h"
#include "utilities/runner.h"
#include "models/tsp.h"
#include "propagators/propagators.h"

using namespace std;
using namespace hc;

const bool first_for_branching = false;

void run_instance(const InspectorTSPModelOptions &opt);

int main(int argc, char **argv) {
    // Clock function used.
    auto now = [] { return chrono::steady_clock::now(); };

    cout << "File; Nodes; CLTime; ";
    cout << "Start; SumSucc; Next; MinCost; MaxCost; ";
    cout << "NCLSumSucc; NCLSuccPercent; NCLNext; NCLNextPercent; NCLMinCost; NCLMinCostPercent;  NCLMaxCost; NCLMaxCostPercent; ";
    cout << "WNCLSumSucc; WNCLSuccPercent; WNCLNext; WNCLNextPercent; WNCLMinCost; WNCLMinCostPercent;  WNCLMaxCost; WNCLMaxCostPercent; ";
    cout << "CBPSumSucc; CBPSuccPercent; CBPNext; CBPNextPercent; CBPMinCost; CBPMinCostPercent;  CBPMaxCost; CBPMaxCostPercent; ";
    cout << "HK1SumSucc; HK1SuccPercent; HK1Next; HK1NextPercent; HK1MinCost; HK1MinCostPercent;  HK1MaxCost; HK1MaxCostPercent; ";
    cout << "ALLSumSucc; ALLSuccPercent; ALLNext; ALLNextPercent; ALLMinCost; ALLMinCostPercent;  ALLMaxCost; ALLMaxCostPercent; ";
    cout << "Steps; 10Percent; ";
    cout << "SumSucc; Next; MinCost; MaxCost; ";
    cout << "NCLSumSucc; NCLSuccPercent; NCLNext; NCLNextPercent; NCLMinCost; NCLMinCostPercent;  NCLMaxCost; NCLMaxCostPercent; ";
    cout << "WNCLSumSucc; WNCLSuccPercent; WNCLNext; WNCLNextPercent; WNCLMinCost; WNCLMinCostPercent;  WNCLMaxCost; WNCLMaxCostPercent; ";
    cout << "CBPSumSucc; CBPSuccPercent; CBPNext; CBPNextPercent; CBPMinCost; CBPMinCostPercent;  CBPMaxCost; CBPMaxCostPercent; ";
    cout << "HK1SumSucc; HK1SuccPercent; HK1Next; HK1NextPercent; HK1MinCost; HK1MinCostPercent;  HK1MaxCost; HK1MaxCostPercent; ";
    cout << "ALLSumSucc; ALLSuccPercent; ALLNext; ALLNextPercent; ALLMinCost; ALLMinCostPercent;  ALLMaxCost; ALLMaxCostPercent; ";
    cout << "Steps; 50Percent; ";
    cout << "SumSucc; Next; MinCost; MaxCost; ";
    cout << "NCLSumSucc; NCLSuccPercent; NCLNext; NCLNextPercent; NCLMinCost; NCLMinCostPercent;  NCLMaxCost; NCLMaxCostPercent; ";
    cout << "WNCLSumSucc; WNCLSuccPercent; WNCLNext; WNCLNextPercent; WNCLMinCost; WNCLMinCostPercent;  WNCLMaxCost; WNCLMaxCostPercent; ";
    cout << "CBPSumSucc; CBPSuccPercent; CBPNext; CBPNextPercent; CBPMinCost; CBPMinCostPercent;  CBPMaxCost; CBPMaxCostPercent; ";
    cout << "HK1SumSucc; HK1SuccPercent; HK1Next; HK1NextPercent; HK1MinCost; HK1MinCostPercent;  HK1MaxCost; HK1MaxCostPercent; ";
    cout << "ALLSumSucc; ALLSuccPercent; ALLNext; ALLNextPercent; ALLMinCost; ALLMinCostPercent;  ALLMaxCost; ALLMaxCostPercent; ";
    cout << endl;

    const auto model_start = now();

    for (const auto& file : {
        "berlin52.tsp", "st70.tsp",
        "eil51.tsp",  "eil76.tsp",
        "eil101.tsp",
        "lin105.tsp", "lin318.tsp",
        "pr76.tsp",  "pr107.tsp",  "pr124.tsp",  "pr136.tsp",  "pr144.tsp",  "pr152.tsp"
    }) {
        InspectorTSPModelOptions opt;
        opt.instance(make_shared<const TSPInstance>(TSPInstance::read_instance(std::string("../data/euc2d_tsplib/") + file).unwrap()));

        const auto de_start = now();
        opt.instance()->compute_dominated_edges();
        const auto de_end = now();
        const chrono::duration<double, milli> de_duration =
                de_end - de_start;

        cout
                << setw(14) << right << file
                << "; " << setw(4) << opt.instance()->locations()
                << "; " << fixed << setw(12) << right << de_duration.count() << "; ";
        run_instance(opt);

        cout << endl;
    }



    // Report results
    //
    const auto model_end = now();
    const chrono::duration<double, milli> model_duration =
            model_end - model_start;

    cout << "Ran model in; " << model_duration.count()
              << endl;

    return EXIT_SUCCESS;
}

unsigned int domsize(const Gecode::IntVarArray& x) {
    unsigned int result = 0;
    for (int i = 0; i < x.size(); ++i) {
        result += x[i].size();
    }
    return result;
}

void report(const TSPModel* standard, const TSPModel* variant) {
    if (variant->failed()) {
        cout << "    X;      X;   X;      X;        X;      X;          X;      X; ";
    } else {
        int next = standard->warnsdorff_next();
        cout << ""   << setw(5) << domsize(variant->succ())
             << "; " << setw(6) << fixed << setprecision(4) << (((double) domsize(variant->succ())) / ((double)domsize(standard->succ())))
             << "; " << setw(4) << variant->succ()[next].size()
             << "; " << setw(6) << fixed << (variant->succ()[next].size() / (double)standard->succ()[next].size())
             << "; " << setw(8) << variant->tour_cost().min()
                << "; " << setw(6) << fixed << setprecision(4) << ((double)variant->tour_cost().min() / standard->tour_cost().min())
             << "; " << setw(10) << variant->tour_cost().max()
             << "; " << setw(6) << fixed << setprecision(4) << ((double)variant->tour_cost().max() / standard->tour_cost().max())
             << "; ";
    }
}

void advance(Gecode::CommitStatistics &commit_statistics, vector<TSPModel*>& variants, int steps) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
//    const int brpos = first_for_branching ? 0 : variants.size() - 1;
    const int brpos = first_for_branching ? 0 : 1;
#pragma clang diagnostic pop
    for (int step = 0; step < steps; ++step) {
        variants[brpos]->status();
        const Gecode::Choice *choice = variants[brpos]->choice();
        for (auto & variant : variants) {
            variant->commit(*choice, 0, commit_statistics);
        }
        delete choice;
    }
    for (auto & variant : variants) {
        variant->status();
    }
}

void report_all(vector<TSPModel*>& variants) {
    cout
            << ""  << setw(5) << domsize(variants[0]->succ())
            << "; " << setw(4) << variants[0]->succ()[variants[0]->warnsdorff_next()].size()
            << "; " << setw(8) << variants[0]->tour_cost().min()
            << "; " << setw(10) << variants[0]->tour_cost().max()
            << "; ";
    for (int i = 1; i < variants.size(); ++i) {
        report(variants[0], variants[i]);
    }
}


void run_instance(const InspectorTSPModelOptions &opt) {
    Gecode::StatusStatistics status_statistics;
    Gecode::CloneStatistics clone_statistics;
    Gecode::CommitStatistics commit_statistics;

    auto *root = new TSPModel(opt);
    root->status(status_statistics);
    root->configure_branching();
    vector<TSPModel*> variants;
    variants.push_back(root);

    {
        auto *ncl = dynamic_cast<TSPModel *>(root->clone(clone_statistics));
        no_dominated_edge_pairs(*ncl, opt.instance(), ncl->succ());
        ncl->status(status_statistics);
        variants.push_back(ncl);
    }

    {
        auto *wncl = dynamic_cast<TSPModel *>(root->clone(clone_statistics));
        no_warnsdorff_dominated_edges2(*wncl, opt.instance(), wncl->warnsdorff_start(), wncl->succ());
        wncl->status(status_statistics);
        variants.push_back(wncl);
    }

    {
        auto *cbp = dynamic_cast<TSPModel *>(root->clone(clone_statistics));
        christofides(*cbp, opt.instance(), cbp->succ(), cbp->tour_cost());
        cbp->status(status_statistics);
        variants.push_back(cbp);
    }

    {
        auto *hk1 = dynamic_cast<TSPModel *>(root->clone(clone_statistics));
        hk_1tree(*hk1, opt.instance(), hk1->succ(), hk1->prev(), hk1->tour_cost());
        hk1->status(status_statistics);
        variants.push_back(hk1);
    }

    {
        auto *all = dynamic_cast<TSPModel *>(root->clone(clone_statistics));
        no_warnsdorff_dominated_edges2(*all, opt.instance(), all->warnsdorff_start(), all->succ());
        christofides(*all, opt.instance(), all->succ(), all->tour_cost());
        hk_1tree(*all, opt.instance(), all->succ(), all->prev(), all->tour_cost());
        all->status(status_statistics);
        variants.push_back(all);
    }

    cout << "Start; ";
    report_all(variants);

    int steps1 = static_cast<int>(opt.instance()->locations() * 0.1);
    advance(commit_statistics, variants, steps1);

    cout << "Steps; " << setw(3) << steps1 << "; ";
    report_all(variants);

    const double second_fraction = first_for_branching ? 0.25 : 0.25;
    int steps2 = static_cast<int>(opt.instance()->locations() * second_fraction);
    advance(commit_statistics, variants, steps2 - steps1);

    cout << "Steps; " << setw(3) << steps2 << "; ";
    report_all(variants);

    for (auto & variant : variants) {
        delete variant;
    }
}



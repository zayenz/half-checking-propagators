//
// This file contains an updated version of the  Gecode Driver runner code, that supports portfolios
// with restart-based search.
//

#ifndef HC_RUNNER_H
#define HC_RUNNER_H

#include <cstring>
#include <cstdlib>
#include <memory>
#include <optional>

#include <gecode/driver.hh>
#include <gecode/int.hh>

namespace hc {
    std::ostream& select_ostream(const char* sn, std::ofstream& ofs) {
        if (strcmp(sn, "stdout") == 0) {
            return std::cout;
        } else if (strcmp(sn, "stdlog") == 0) {
            return std::clog;
        } else if (strcmp(sn, "stderr") == 0) {
            return std::cerr;
        } else {
            ofs.open(sn);
            return ofs;
        }
    }


    template<class Script, template<class> class Engine, class Options,
            template<class, template<class> class> class Meta>
    void runMetaSEB(const Options &o, Script *s, Gecode::SEBs &sebs) {
        using namespace std;

        ofstream sol_file, log_file;

        ostream& s_out = select_ostream(o.out_file(), sol_file);
        ostream& l_out = select_ostream(o.log_file(), log_file);

        Gecode::Search::Options so;

        try {
            switch (o.mode()) {
                case Gecode::SM_GIST:
#ifdef GECODE_HAS_GIST
                {
                    Gecode::Gist::Print<Script> pi(o.name());
                    Gecode::Gist::VarComparator<Script> vc(o.name());
                    Gecode::Gist::Options opt;
                    opt.inspect.click(&pi);
                    opt.inspect.compare(&vc);
                    opt.clone = false;
                    opt.c_d   = o.c_d();
                    opt.a_d   = o.a_d();
                    for (unsigned int i=0; o.inspect.click(i) != NULL; i++)
                        opt.inspect.click(o.inspect.click(i));
                    for (unsigned int i=0; o.inspect.solution(i) != NULL; i++)
                        opt.inspect.solution(o.inspect.solution(i));
                    for (unsigned int i=0; o.inspect.move(i) != NULL; i++)
                        opt.inspect.move(o.inspect.move(i));
                    for (unsigned int i=0; o.inspect.compare(i) != NULL; i++)
                        opt.inspect.compare(o.inspect.compare(i));
                    if (s == NULL)
                        s = new Script(o);
                    (void) Gecode::Driver::GistEngine<Engine<Script> >::explore(s, opt);
                }
                    break;
                    // If Gist is not available, goto solution
#else
                    goto solution;
#endif
                case Gecode::SM_CPPROFILER:
#ifdef GECODE_HAS_CPPROFILER
                    {
          CPProfilerSearchTracer::GetInfo* getInfo = nullptr;
          if (o.profiler_info())
            getInfo = new ScriptGetInfo<BaseSpace>;
          so.tracer = new CPProfilerSearchTracer
            (o.profiler_id(), o.name(), o.profiler_port(), getInfo);
        }
        /* FALL THROUGH */
#endif
                case Gecode::SM_SOLUTION:
#ifndef GECODE_HAS_GIST
                    solution:
#endif
                {
                    l_out << o.name() << endl;
                    Gecode::Support::Timer t{};
                    int i = static_cast<int>(o.solutions());
                    t.start();
                    if (s == NULL)
                        s = new Script(o);
                    unsigned int n_p = Gecode::PropagatorGroup::all.size(*s);
                    unsigned int n_b = Gecode::BrancherGroup::all.size(*s);
                    so.threads = o.threads();
                    so.c_d     = o.c_d();
                    so.a_d     = o.a_d();
                    so.d_l     = o.d_l();
                    so.assets  = o.assets();
                    so.slice   = o.slice();
                    so.stop    = Gecode::Driver::CombinedStop::create(o.node(),o.fail(), o.time(),
                                                                      o.interrupt());
                    so.cutoff  = Gecode::Driver::createCutoff(o);
                    so.clone   = false;
                    so.nogoods_limit = o.nogoods() ? o.nogoods_limit() : 0U;
                    if (o.interrupt())
                        Gecode::Driver::CombinedStop::installCtrlHandler(true);
                    {
                        Meta<Script,Engine> e(s, sebs, so);
                        if (o.print_last()) {
                            Script* px = NULL;
                            do {
                                Script* ex = e.next();
                                if (ex == NULL) {
                                    if (px != NULL) {
                                        px->print(s_out);
                                        delete px;
                                    }
                                    break;
                                } else {
                                    delete px;
                                    px = ex;
                                }
                            } while (--i != 0);
                        } else {
                            do {
                                Script* ex = e.next();
                                if (ex == NULL)
                                    break;
                                ex->print(s_out);
                                delete ex;
                            } while (--i != 0);
                        }
                        if (o.interrupt())
                            Gecode::Driver::CombinedStop::installCtrlHandler(false);
                        Gecode::Search::Statistics stat = e.statistics();
                        s_out << endl;
                        if (e.stopped()) {
                            l_out << "Search engine stopped..." << endl
                                  << "\treason: ";
                            int r = static_cast<Gecode::Driver::CombinedStop*>(so.stop)->reason(stat,so);
                            if (r & Gecode::Driver::CombinedStop::SR_INT)
                                l_out << "user interrupt " << endl;
                            else {
                                if (r & Gecode::Driver::CombinedStop::SR_NODE)
                                    l_out << "node ";
                                if (r & Gecode::Driver::CombinedStop::SR_FAIL)
                                    l_out << "fail ";
                                if (r & Gecode::Driver::CombinedStop::SR_TIME)
                                    l_out << "time ";
                                l_out << "limit reached" << endl << endl;
                            }
                        }
                        l_out << "Initial" << endl
                              << "\tpropagators: " << n_p << endl
                              << "\tbranchers:   " << n_b << endl
                              << endl
                              << "Summary" << endl
                              << "\truntime:      ";
                        Gecode::Driver::stop(t, l_out);
                        l_out << endl
                              << "\tsolutions:    "
                              << ::abs(static_cast<int>(o.solutions()) - i) << endl
                              << "\tpropagations: " << stat.propagate << endl
                              << "\tnodes:        " << stat.node << endl
                              << "\tfailures:     " << stat.fail << endl
                              << "\trestarts:     " << stat.restart << endl
                              << "\tno-goods:     " << stat.nogood << endl
                              << "\tpeak depth:   " << stat.depth << endl
                              #ifdef GECODE_PEAKHEAP
                              << "\tpeak memory:  "
                  << static_cast<int>((heap.peak()+1023) / 1024) << " KB"
                  << endl
                              #endif
                              << endl;
                    }
                    delete so.stop;
                    delete so.tracer;
                }
                    break;
                case Gecode::SM_STAT:
                {
                    l_out << o.name() << endl;
                    Gecode::Support::Timer t{};
                    int i = static_cast<int>(o.solutions());
                    t.start();
                    if (s == NULL)
                        s = new Script(o);
                    unsigned int n_p = Gecode::PropagatorGroup::all.size(*s);
                    unsigned int n_b = Gecode::BrancherGroup::all.size(*s);

                    so.clone   = false;
                    so.threads = o.threads();
                    so.assets  = o.assets();
                    so.slice   = o.slice();
                    so.c_d     = o.c_d();
                    so.a_d     = o.a_d();
                    so.d_l     = o.d_l();
                    so.stop    = Gecode::Driver::CombinedStop::create(o.node(),o.fail(), o.time(),
                                                                      o.interrupt());
                    so.cutoff  = Gecode::Driver::createCutoff(o);
                    so.nogoods_limit = o.nogoods() ? o.nogoods_limit() : 0U;
                    if (o.interrupt())
                        Gecode::Driver::CombinedStop::installCtrlHandler(true);
                    {
                        Meta<Script,Engine> e(s, sebs, so);
                        do {
                            Script* ex = e.next();
                            if (ex == NULL)
                                break;
                            delete ex;
                        } while (--i != 0);
                        if (o.interrupt())
                            Gecode::Driver::CombinedStop::installCtrlHandler(false);
                        Gecode::Search::Statistics stat = e.statistics();
                        l_out << endl
                              << "\tpropagators:  " << n_p << endl
                              << "\tbranchers:    " << n_b << endl
                              << "\truntime:      ";
                        Gecode::Driver::stop(t, l_out);
                        l_out << endl
                              << "\tsolutions:    "
                              << ::abs(static_cast<int>(o.solutions()) - i) << endl
                              << "\tpropagations: " << stat.propagate << endl
                              << "\tnodes:        " << stat.node << endl
                              << "\tfailures:     " << stat.fail << endl
                              << "\trestarts:     " << stat.restart << endl
                              << "\tno-goods:     " << stat.nogood << endl
                              << "\tpeak depth:   " << stat.depth << endl
                              #ifdef GECODE_PEAKHEAP
                              << "\tpeak memory:  "
                  << static_cast<int>((heap.peak()+1023) / 1024) << " KB"
                  << endl
                              #endif
                              << endl;
                    }
                    delete so.stop;
                }
                    break;
                case Gecode::SM_TIME:
                {
                    l_out << o.name() << endl;
                    Gecode::Support::Timer t{};
                    auto* ts = new double[o.samples()];
                    bool stopped = false;
                    for (unsigned int ns = o.samples(); !stopped && ns--; ) {
                        t.start();
                        for (unsigned int k = o.iterations(); !stopped && k--; ) {
                            unsigned int i = o.solutions();
                            auto* s1 = new Script(o);
                            Gecode::Search::Options sok;
                            sok.clone   = false;
                            sok.threads = o.threads();
                            sok.assets  = o.assets();
                            sok.slice   = o.slice();
                            sok.c_d     = o.c_d();
                            sok.a_d     = o.a_d();
                            sok.d_l     = o.d_l();
                            sok.stop    = Gecode::Driver::CombinedStop::create(o.node(),o.fail(), o.time(),
                                                                               false);
                            sok.cutoff  = Gecode::Driver::createCutoff(o);
                            sok.nogoods_limit = o.nogoods() ? o.nogoods_limit() : 0U;
                            {
                                Meta<Script,Engine> e(s1,sok);
                                do {
                                    Script* ex = e.next();
                                    if (ex == NULL)
                                        break;
                                    delete ex;
                                } while (--i != 0);
                                if (e.stopped())
                                    stopped = true;
                            }
                            delete sok.stop;
                        }
                        ts[ns] = t.stop() / o.iterations();
                    }
                    if (stopped) {
                        l_out << "\tSTOPPED" << endl;
                    } else {
                        double m = Gecode::Driver::am(ts,o.samples());
                        double d = Gecode::Driver::dev(ts,o.samples()) * 100.0;
                        l_out << "\truntime: "
                              << setw(20) << right
                              << showpoint << fixed
                              << setprecision(6) << m << "ms"
                              << setprecision(2) << " (" << d << "% deviation)"
                              << endl;
                    }
                    delete [] ts;
                }
                    break;
            }
        } catch (Gecode::Exception& e) {
            cerr << "Exception: " << e.what() << "." << endl
                 << "Stopping..." << endl;
            if (sol_file.is_open())
                sol_file.close();
            if (log_file.is_open())
                log_file.close();
            exit(EXIT_FAILURE);
        }
        if (sol_file.is_open())
            sol_file.close();
        if (log_file.is_open())
            log_file.close();
    }

    template<class Script, template<class> class Engine, class Options,
            template<class, template<class> class> class Meta>
    void runMeta(const Options &o, Script *s) {
        using namespace std;

        ofstream sol_file, log_file;

        ostream& s_out = select_ostream(o.out_file(), sol_file);
        ostream& l_out = select_ostream(o.log_file(), log_file);

        Gecode::Search::Options so;

        try {
            switch (o.mode()) {
                case Gecode::SM_GIST:
#ifdef GECODE_HAS_GIST
                {
                    Gecode::Gist::Print<Script> pi(o.name());
                    Gecode::Gist::VarComparator<Script> vc(o.name());
                    Gecode::Gist::Options opt;
                    opt.inspect.click(&pi);
                    opt.inspect.compare(&vc);
                    opt.clone = false;
                    opt.c_d   = o.c_d();
                    opt.a_d   = o.a_d();
                    for (unsigned int i=0; o.inspect.click(i) != NULL; i++)
                        opt.inspect.click(o.inspect.click(i));
                    for (unsigned int i=0; o.inspect.solution(i) != NULL; i++)
                        opt.inspect.solution(o.inspect.solution(i));
                    for (unsigned int i=0; o.inspect.move(i) != NULL; i++)
                        opt.inspect.move(o.inspect.move(i));
                    for (unsigned int i=0; o.inspect.compare(i) != NULL; i++)
                        opt.inspect.compare(o.inspect.compare(i));
                    if (s == NULL)
                        s = new Script(o);
                    (void) Gecode::Driver::GistEngine<Engine<Script> >::explore(s, opt);
                }
                    break;
                    // If Gist is not available, goto solution
#else
                    goto solution;
#endif
                case Gecode::SM_CPPROFILER:
#ifdef GECODE_HAS_CPPROFILER
                    {
          CPProfilerSearchTracer::GetInfo* getInfo = nullptr;
          if (o.profiler_info())
            getInfo = new ScriptGetInfo<BaseSpace>;
          so.tracer = new CPProfilerSearchTracer
            (o.profiler_id(), o.name(), o.profiler_port(), getInfo);
        }
        /* FALL THROUGH */
#endif
                case Gecode::SM_SOLUTION:
#ifndef GECODE_HAS_GIST
                    solution:
#endif
                {
                    l_out << o.name() << endl;
                    Gecode::Support::Timer t{};
                    int i = static_cast<int>(o.solutions());
                    t.start();
                    if (s == NULL)
                        s = new Script(o);
                    unsigned int n_p = Gecode::PropagatorGroup::all.size(*s);
                    unsigned int n_b = Gecode::BrancherGroup::all.size(*s);
                    so.threads = o.threads();
                    so.c_d     = o.c_d();
                    so.a_d     = o.a_d();
                    so.d_l     = o.d_l();
                    so.assets  = o.assets();
                    so.slice   = o.slice();
                    so.stop    = Gecode::Driver::CombinedStop::create(o.node(),o.fail(), o.time(),
                                                                      o.interrupt());
                    so.cutoff  = Gecode::Driver::createCutoff(o);
                    so.clone   = false;
                    so.nogoods_limit = o.nogoods() ? o.nogoods_limit() : 0U;
                    if (o.interrupt())
                        Gecode::Driver::CombinedStop::installCtrlHandler(true);
                    {
                        Meta<Script,Engine> e(s, so);
                        if (o.print_last()) {
                            Script* px = NULL;
                            do {
                                Script* ex = e.next();
                                if (ex == NULL) {
                                    if (px != NULL) {
                                        px->print(s_out);
                                        delete px;
                                    }
                                    break;
                                } else {
                                    delete px;
                                    px = ex;
                                }
                            } while (--i != 0);
                        } else {
                            do {
                                Script* ex = e.next();
                                if (ex == NULL)
                                    break;
                                ex->print(s_out);
                                delete ex;
                            } while (--i != 0);
                        }
                        if (o.interrupt())
                            Gecode::Driver::CombinedStop::installCtrlHandler(false);
                        Gecode::Search::Statistics stat = e.statistics();
                        s_out << endl;
                        if (e.stopped()) {
                            l_out << "Search engine stopped..." << endl
                                  << "\treason: ";
                            int r = static_cast<Gecode::Driver::CombinedStop*>(so.stop)->reason(stat,so);
                            if (r & Gecode::Driver::CombinedStop::SR_INT)
                                l_out << "user interrupt " << endl;
                            else {
                                if (r & Gecode::Driver::CombinedStop::SR_NODE)
                                    l_out << "node ";
                                if (r & Gecode::Driver::CombinedStop::SR_FAIL)
                                    l_out << "fail ";
                                if (r & Gecode::Driver::CombinedStop::SR_TIME)
                                    l_out << "time ";
                                l_out << "limit reached" << endl << endl;
                            }
                        }
                        l_out << "Initial" << endl
                              << "\tpropagators: " << n_p << endl
                              << "\tbranchers:   " << n_b << endl
                              << endl
                              << "Summary" << endl
                              << "\truntime:      ";
                        Gecode::Driver::stop(t, l_out);
                        l_out << endl
                              << "\tsolutions:    "
                              << ::abs(static_cast<int>(o.solutions()) - i) << endl
                              << "\tpropagations: " << stat.propagate << endl
                              << "\tnodes:        " << stat.node << endl
                              << "\tfailures:     " << stat.fail << endl
                              << "\trestarts:     " << stat.restart << endl
                              << "\tno-goods:     " << stat.nogood << endl
                              << "\tpeak depth:   " << stat.depth << endl
                              #ifdef GECODE_PEAKHEAP
                              << "\tpeak memory:  "
                  << static_cast<int>((heap.peak()+1023) / 1024) << " KB"
                  << endl
                              #endif
                              << endl;
                    }
                    delete so.stop;
                    delete so.tracer;
                }
                    break;
                case Gecode::SM_STAT:
                {
                    l_out << o.name() << endl;
                    Gecode::Support::Timer t{};
                    int i = static_cast<int>(o.solutions());
                    t.start();
                    if (s == NULL)
                        s = new Script(o);
                    unsigned int n_p = Gecode::PropagatorGroup::all.size(*s);
                    unsigned int n_b = Gecode::BrancherGroup::all.size(*s);

                    so.clone   = false;
                    so.threads = o.threads();
                    so.assets  = o.assets();
                    so.slice   = o.slice();
                    so.c_d     = o.c_d();
                    so.a_d     = o.a_d();
                    so.d_l     = o.d_l();
                    so.stop    = Gecode::Driver::CombinedStop::create(o.node(),o.fail(), o.time(),
                                                                      o.interrupt());
                    so.cutoff  = Gecode::Driver::createCutoff(o);
                    so.nogoods_limit = o.nogoods() ? o.nogoods_limit() : 0U;
                    if (o.interrupt())
                        Gecode::Driver::CombinedStop::installCtrlHandler(true);
                    {
                        Meta<Script,Engine> e(s, so);
                        do {
                            Script* ex = e.next();
                            if (ex == NULL)
                                break;
                            delete ex;
                        } while (--i != 0);
                        if (o.interrupt())
                            Gecode::Driver::CombinedStop::installCtrlHandler(false);
                        Gecode::Search::Statistics stat = e.statistics();
                        l_out << endl
                              << "\tpropagators:  " << n_p << endl
                              << "\tbranchers:    " << n_b << endl
                              << "\truntime:      ";
                        Gecode::Driver::stop(t, l_out);
                        l_out << endl
                              << "\tsolutions:    "
                              << ::abs(static_cast<int>(o.solutions()) - i) << endl
                              << "\tpropagations: " << stat.propagate << endl
                              << "\tnodes:        " << stat.node << endl
                              << "\tfailures:     " << stat.fail << endl
                              << "\trestarts:     " << stat.restart << endl
                              << "\tno-goods:     " << stat.nogood << endl
                              << "\tpeak depth:   " << stat.depth << endl
                              #ifdef GECODE_PEAKHEAP
                              << "\tpeak memory:  "
                  << static_cast<int>((heap.peak()+1023) / 1024) << " KB"
                  << endl
                              #endif
                              << endl;
                    }
                    delete so.stop;
                }
                    break;
                case Gecode::SM_TIME:
                {
                    l_out << o.name() << endl;
                    Gecode::Support::Timer t{};
                    auto* ts = new double[o.samples()];
                    bool stopped = false;
                    for (unsigned int ns = o.samples(); !stopped && ns--; ) {
                        t.start();
                        for (unsigned int k = o.iterations(); !stopped && k--; ) {
                            unsigned int i = o.solutions();
                            auto* s1 = new Script(o);
                            Gecode::Search::Options sok;
                            sok.clone   = false;
                            sok.threads = o.threads();
                            sok.assets  = o.assets();
                            sok.slice   = o.slice();
                            sok.c_d     = o.c_d();
                            sok.a_d     = o.a_d();
                            sok.d_l     = o.d_l();
                            sok.stop    = Gecode::Driver::CombinedStop::create(o.node(),o.fail(), o.time(),
                                                                               false);
                            sok.cutoff  = Gecode::Driver::createCutoff(o);
                            sok.nogoods_limit = o.nogoods() ? o.nogoods_limit() : 0U;
                            {
                                Meta<Script,Engine> e(s1,sok);
                                do {
                                    Script* ex = e.next();
                                    if (ex == NULL)
                                        break;
                                    delete ex;
                                } while (--i != 0);
                                if (e.stopped())
                                    stopped = true;
                            }
                            delete sok.stop;
                        }
                        ts[ns] = t.stop() / o.iterations();
                    }
                    if (stopped) {
                        l_out << "\tSTOPPED" << endl;
                    } else {
                        double m = Gecode::Driver::am(ts,o.samples());
                        double d = Gecode::Driver::dev(ts,o.samples()) * 100.0;
                        l_out << "\truntime: "
                              << setw(20) << right
                              << showpoint << fixed
                              << setprecision(6) << m << "ms"
                              << setprecision(2) << " (" << d << "% deviation)"
                              << endl;
                    }
                    delete [] ts;
                }
                    break;
            }
        } catch (Gecode::Exception& e) {
            cerr << "Exception: " << e.what() << "." << endl
                 << "Stopping..." << endl;
            if (sol_file.is_open())
                sol_file.close();
            if (log_file.is_open())
                log_file.close();
            exit(EXIT_FAILURE);
        }
        if (sol_file.is_open())
            sol_file.close();
        if (log_file.is_open())
            log_file.close();
    }

    template<class Script, template<class> class Engine, class Options>
    void run(Options &o, Script *s = nullptr) {
        int assets = o.assets();
        if ((o.restart() != Gecode::RM_NONE) && (assets > 0)) {
            int extra_threads;
            int asset_threads;
            {
                int expanded_total_threads;
                {
                    Gecode::Search::Options threads_base;
                    threads_base.threads = o.threads();
                    const Gecode::Search::Options &threads_expanded = threads_base.expand();
                    expanded_total_threads = static_cast<int>(threads_expanded.threads);
                }
                asset_threads = std::min(expanded_total_threads, assets);
                extra_threads = std::max(expanded_total_threads - assets, 0);
            }
            
            std::vector<Gecode::SEB> sebs_vec;
            sebs_vec.reserve(assets);
            for (int i = 0; i < assets; ++i) {
                const int asset_extra_threads = static_cast<int>(ceil(static_cast<double>(extra_threads) / (assets - i)));
                extra_threads -= asset_extra_threads;

                Gecode::Search::Options asset_options;
                asset_options.clone   = true;
                asset_options.threads = 1 + extra_threads;
                asset_options.assets  = assets;
                asset_options.slice   = o.slice();
                asset_options.c_d     = o.c_d();
                asset_options.a_d     = o.a_d();
                asset_options.d_l     = o.d_l();
                asset_options.stop    = Gecode::Driver::CombinedStop::create(o.node(),
                                                                             o.fail(),
                                                                             o.time(),
                                                                             o.interrupt());
                asset_options.cutoff  = Gecode::Driver::createCutoff(o);
                asset_options.nogoods_limit = o.nogoods() ? o.nogoods_limit() : 0U;

                sebs_vec.emplace_back(Gecode::rbs<Script, Engine>(asset_options));
            }
            o.threads(asset_threads);
            Gecode::SEBs sebs(sebs_vec);
            runMetaSEB<Script,Engine,Options,Gecode::PBS>(o, s, sebs);
        } else if (o.restart() != Gecode::RM_NONE) {
            runMeta<Script,Engine,Options,Gecode::RBS>(o, s);
        } else if (assets > 0) {
            runMeta<Script,Engine,Options,Gecode::PBS>(o, s);
        } else {
            runMeta<Script,Engine,Options,Gecode::Driver::EngineToMeta>(o, s);
        }
    }

}
#endif //HC_RUNNER_H

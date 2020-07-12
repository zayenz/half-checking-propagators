#ifndef HC_VALUE_SELECTION_H
#define HC_VALUE_SELECTION_H

#include <utility>
#include <algorithm>

#include <gecode/driver.hh>
#include <gecode/int.hh>

namespace hc {

    /**
     * This function can be used to run a merit-based value selection, using biased selection.
     *
     * Values are based on merit returned by \a evaluator, with smaller values being better. Choosing is done using
     * simple biased selection, 50% for the first value, 25% for the second value, 12.5%& for the third value,
     * and so on.
     *
     * @return The chosen value
     */
    template <typename T>
    int choose_value_biased(Gecode::Rnd& rnd, const Gecode::IntVar& var, const std::function<T(int)>& evaluator) {
        std::vector<std::pair<T, int>> values;
        values.reserve(var.size());
        Gecode::Int::ViewValues<Gecode::Int::IntView> iv(var);
        while (iv()) {
            const int val = iv.val();
            const T evaluation = evaluator(val);
            values.emplace_back(std::make_pair(evaluation, val));
            ++iv;
        }
        std::stable_sort(values.begin(), values.end());

        size_t pos = 0;
        while (rnd(2) != 0) {
            pos = (pos+1) % values.size();
        }
        return values[pos].second;
    }

}

#endif //HC_VALUE_SELECTION_H

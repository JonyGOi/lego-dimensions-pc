#include "hle/hle_perf.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include "hle/hle_log.h"

namespace hle::perf {

namespace {
std::atomic<Counter*> g_head{nullptr};
}

Counter::Counter(const char* name_) : name(name_) {
    next = g_head.load();
    while (!g_head.compare_exchange_weak(next, this)) {
    }
}

void PrintWindow(double seconds, u64 frames) {
    struct Row {
        const char* name;
        u64 ns, calls, max_ns;
    };
    std::vector<Row> rows;
    for (Counter* c = g_head.load(); c; c = c->next) {
        const Row row{c->name, c->ns.exchange(0), c->calls.exchange(0), c->max_ns.exchange(0)};
        if (row.calls) {
            rows.push_back(row);
        }
    }
    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) { return a.ns > b.ns; });
    const double frame_ms = frames ? seconds * 1000.0 / frames : 0.0;
    std::string line;
    char item[160];
    for (const Row& row : rows) {
        std::snprintf(item, sizeof(item), "%s%s %.2f ms/frame (%.0f chamadas/frame, pior %.1f ms)", line.empty() ? "" : " | ",
                      row.name, frames ? row.ns / 1e6 / frames : 0.0, frames ? static_cast<double>(row.calls) / frames : 0.0,
                      row.max_ns / 1e6);
        line += item;
    }
    log::Tagged("PERF", "%.1f quadros/s (%.1f ms por frame) | %s", seconds > 0 ? frames / seconds : 0.0, frame_ms, line.c_str());
    log::Tagged("PERF", "HLE mais chamados por frame: %s", log::TopCallDeltas(12, frames).c_str());
}

} // namespace hle::perf

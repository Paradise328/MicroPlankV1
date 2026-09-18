#ifndef FORCEHISTORY_H
#define FORCEHISTORY_H

#include <algorithm>
#include <cmath>
#include <deque>
#include <vector>

// Storage is independent of the display resolution. Only the grip view expires data.
class ForceHistory {
public:
    struct Sample { double time, first, second; bool valid; };
    struct Point { double time, value; bool valid; };
    void reset(bool endurance) { m_samples.clear(); m_endurance = endurance; }
    void append(Sample sample) {
        if (!std::isfinite(sample.time) || sample.time < 0
            || (!m_samples.empty() && sample.time <= m_samples.back().time)) { return; }
        sample.valid = sample.valid && std::isfinite(sample.first) && std::isfinite(sample.second);
        m_samples.push_back(sample);
        if (!m_endurance) {
            while (!m_samples.empty() && m_samples.front().time < sample.time - 300.0) {
                m_samples.pop_front();
            }
        }
    }
    std::size_t size() const { return m_samples.size(); }
    double endTime() const { return m_samples.empty() ? 0.0 : m_samples.back().time; }
    std::vector<Point> plot(int channel, double start, double end, int columns) const {
        std::vector<Point> result;
        if (channel < 0 || channel > 2 || !std::isfinite(start) || !std::isfinite(end)
            || end <= start || m_samples.empty()) { return result; }
        columns = std::max(1, std::min(columns, 1600));
        auto cursor = std::lower_bound(m_samples.begin(), m_samples.end(), start,
            [](const Sample& s, double t) { return s.time < t; });
        const auto value = [channel](const Sample& s) {
            return channel == 0 ? s.first + s.second : (channel == 1 ? s.first : s.second);
        };
        for (int column = 0; column < columns && cursor != m_samples.end(); ++column) {
            const double boundary = start + (end - start) * (column + 1) / columns;
            std::vector<Point> extrema;
            bool gap = false;
            Point first{}, last{}, low{}, high{};
            int count = 0;
            while (cursor != m_samples.end() && cursor->time <= boundary) {
                Point p{cursor->time, value(*cursor), cursor->valid};
                if (count++ == 0) { first = low = high = p; }
                last = p;
                gap = gap || !p.valid;
                if (p.value < low.value) { low = p; }
                if (p.value > high.value) { high = p; }
                ++cursor;
            }
            if (!count) { continue; }
            if (gap) {
                // A pixel interval containing missing data must not bridge that gap.
                result.push_back({first.time, 0.0, false});
                result.push_back({last.time, 0.0, false});
            } else {
                extrema = {first, low, high, last};
                std::sort(extrema.begin(), extrema.end(),
                    [](const Point& a, const Point& b) { return a.time < b.time; });
                for (const auto& p : extrema) {
                    if (result.empty() || result.back().time != p.time) { result.push_back(p); }
                }
            }
        }
        return result;
    }
private:
    bool m_endurance = false;
    std::deque<Sample> m_samples;
};
#endif

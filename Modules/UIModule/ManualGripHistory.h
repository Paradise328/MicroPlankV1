#ifndef MANUALGRIPHISTORY_H
#define MANUALGRIPHISTORY_H

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <deque>
#include <vector>

// Independent of Qt and display resolution. Indices: angles 0..1, torques 2..5,
// actual encoder positions 6..11. No invalid sample may connect two valid runs.
class ManualGripHistory {
public:
    static constexpr std::size_t SeriesCount = 12;
    struct Sample {
        double time = 0.0;
        std::array<double, SeriesCount> values{};
        std::uint16_t validMask = 0;
    };
    struct Point { double time, value; bool valid; };

    void reset() { m_samples.clear(); }
    std::size_t size() const { return m_samples.size(); }
    double endTime() const { return m_samples.empty() ? 0.0 : m_samples.back().time; }

    bool append(Sample sample) {
        if (!std::isfinite(sample.time) || sample.time < 0.0
            || (!m_samples.empty() && sample.time <= endTime())) { return false; }
        for (std::size_t i = 0; i < SeriesCount; ++i) {
            if (!std::isfinite(sample.values[i])) {
                sample.values[i] = 0.0;
                sample.validMask &= ~(1U << i);
            }
        }
        // The producer runs at 10 Hz. A missing batch must not be interpolated.
        if (!m_samples.empty() && sample.time - endTime() > 0.25) {
            Sample gap;
            gap.time = endTime() + 0.001;
            m_samples.push_back(gap);
        }
        m_samples.push_back(sample);
        while (!m_samples.empty() && (m_samples.size() > 3001
               || m_samples.front().time < sample.time - 300.0)) {
            m_samples.pop_front();
        }
        return true;
    }

    std::vector<Point> plot(int series, double start, double end, int columns) const {
        std::vector<Point> result;
        if (series < 0 || series >= static_cast<int>(SeriesCount)
            || !std::isfinite(start) || !std::isfinite(end) || end <= start) { return result; }
        columns = std::max(1, std::min(columns, 1600));
        auto cursor = std::lower_bound(m_samples.begin(), m_samples.end(), start,
            [](const Sample& sample, double time) { return sample.time < time; });
        for (int column = 0; column < columns && cursor != m_samples.end(); ++column) {
            const double boundary = start + (end - start) * (column + 1) / columns;
            Point first{}, last{}, low{}, high{};
            bool hasRun = false;
            const auto flushRun = [&]() {
                if (!hasRun) { return; }
                std::array<Point, 4> extrema{{first, low, high, last}};
                std::sort(extrema.begin(), extrema.end(),
                    [](const Point& a, const Point& b) { return a.time < b.time; });
                for (const Point& point : extrema) {
                    if (result.empty() || result.back().time != point.time) { result.push_back(point); }
                }
                hasRun = false;
            };
            while (cursor != m_samples.end() && cursor->time <= boundary) {
                const Point point{cursor->time, cursor->values[series],
                    (cursor->validMask & (1U << series)) != 0};
                ++cursor;
                if (!point.valid) {
                    flushRun();
                    if (result.empty() || result.back().valid) { result.push_back(point); }
                    continue;
                }
                if (!hasRun) { first = low = high = point; hasRun = true; }
                last = point;
                if (point.value < low.value) { low = point; }
                if (point.value > high.value) { high = point; }
            }
            flushRun();
        }
        return result;
    }

private:
    std::deque<Sample> m_samples;
};
#endif

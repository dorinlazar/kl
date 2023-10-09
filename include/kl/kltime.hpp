#pragma once
#include "kl.hpp"
#include "kltext.hpp"
#include "klexcept.hpp"

#include <array>
#include <compare>
#include <cstdint>
#include <sys/time.h>

namespace kl {

struct TimeOfDay {
  uint32_t hour;
  uint32_t min;
  uint32_t sec;
  uint32_t nanos;
};

struct Date {
  uint32_t year;
  uint32_t month;
  uint32_t day;
};

struct TimeLimits {
  static constexpr int64_t MinTicks = 0LL;                   // 0001-01-01 00:00:00 UTC (if that makes any sense)
  static constexpr int64_t MaxTicks = 3155378975999999999LL; // 9999-12-31 23:59:59 UTC

  static constexpr int32_t MaxYear = 9999;
  static constexpr int32_t MinYear = 1;

  static constexpr int32_t RegularYearsOffset = 400;
  static constexpr int32_t DaysInRegularInterval = 365 * 400 + 97;
  static constexpr int32_t MonthsPerYear = 12;
  static constexpr int32_t MinutesPerHour = 60;
  static constexpr int32_t SecondsPerMinute = 60;
  static constexpr int32_t HoursPerDay = 24;
  static constexpr int32_t SecondsPerHour = SecondsPerMinute * MinutesPerHour;
  static constexpr int64_t TicksPerSecond = 10'000'000LL;
  static constexpr int64_t TicksPerMillisecond = 10'000LL;
  static constexpr int64_t TicksPerMicrosecond = 10LL;
  static constexpr int32_t MillisecondsPerSecond = 1000;
  static constexpr int64_t NanosecondsPerTick = 100LL;
  static constexpr int64_t NanosecondsPerSecond = 1'000'000'000LL;
  static constexpr int64_t NanosecondsPerMillisecond = 1'000'000LL;
  static constexpr int64_t TicksPerMinute = TicksPerSecond * SecondsPerMinute;
  static constexpr int64_t TicksPerHour = TicksPerSecond * SecondsPerHour;
  static constexpr int64_t TicksPerDay = TicksPerHour * HoursPerDay;
};

// Interface inspired after the C# Timespan
struct TimeSpan {
  int64_t ticks;

  static constexpr TimeSpan from_hours(int64_t h) {
    return {.ticks = h * TimeLimits::SecondsPerHour * TimeLimits::TicksPerSecond};
  }
  static constexpr TimeSpan from_minutes(int64_t m) { return {.ticks = m * TimeLimits::TicksPerMinute}; }
  static constexpr TimeSpan from_seconds(int64_t s) { return {.ticks = s * TimeLimits::TicksPerSecond}; }
  static constexpr TimeSpan from_days(int64_t d) { return {.ticks = d * TimeLimits::TicksPerDay}; }
  static constexpr TimeSpan from_nanos(int64_t d) { return {.ticks = d / TimeLimits::NanosecondsPerTick}; }
  static constexpr TimeSpan from_timeval(struct timeval tv) {
    return {.ticks = static_cast<int64_t>(tv.tv_sec) * TimeLimits::TicksPerSecond +
                     static_cast<int64_t>(tv.tv_usec) * TimeLimits::TicksPerMicrosecond};
  }

  [[nodiscard]] constexpr int64_t total_hours() const { return ticks / TimeLimits::TicksPerHour; }
  [[nodiscard]] constexpr int64_t total_minutes() const { return ticks / (TimeLimits::TicksPerMinute); }
  [[nodiscard]] constexpr int64_t total_seconds() const { return ticks / TimeLimits::TicksPerSecond; }
  [[nodiscard]] constexpr int64_t total_milliseconds() const { return ticks / (TimeLimits::TicksPerMillisecond); }
  [[nodiscard]] constexpr int64_t hours() const {
    return (ticks / (TimeLimits::TicksPerHour)) % TimeLimits::HoursPerDay;
  }
  [[nodiscard]] constexpr int64_t minutes() const {
    return (ticks / (TimeLimits::TicksPerMinute)) % TimeLimits::MinutesPerHour;
  }
  [[nodiscard]] constexpr int64_t seconds() const {
    return (ticks / TimeLimits::TicksPerSecond) % TimeLimits::SecondsPerMinute;
  }
  [[nodiscard]] constexpr int64_t days() const { return ticks / TimeLimits::TicksPerDay; }
  [[nodiscard]] constexpr int64_t milliseconds() const {
    return (ticks / TimeLimits::TicksPerMillisecond) % TimeLimits::MillisecondsPerSecond;
  }
  [[nodiscard]] constexpr struct timeval timeval() const {
    return {.tv_sec = total_seconds(),
            .tv_usec = (ticks % TimeLimits::TicksPerSecond) / TimeLimits::TicksPerMicrosecond};
  }

  auto operator<=>(const TimeSpan&) const = default;
  constexpr TimeSpan operator-(const TimeSpan& ts) const { return {.ticks = ticks - ts.ticks}; }
  constexpr TimeSpan operator+(const TimeSpan& ts) const { return {.ticks = ticks + ts.ticks}; }
};

// Interface inspired after the C# Datetime
class DateTime {
  // One Tick represents 100 nanoseconds.
  int64_t m_ticks = TimeLimits::MinTicks;

public:
  [[nodiscard]] int64_t ticks() const;
  [[nodiscard]] int32_t days() const;
  [[nodiscard]] TimeOfDay time_of_day() const;
  [[nodiscard]] Date date() const;
  static DateTime from_ticks(int64_t ticks);
  static DateTime parse(const kl::Text& src);

public:
  DateTime() = default;
  explicit DateTime(time_t seconds, int32_t nsec = 0);
  DateTime(int32_t year, int32_t month, int32_t day, int32_t hour = 0, int32_t minute = 0, int32_t sec = 0,
           int32_t nsec = 0);
  DateTime(const DateTime&) = default;
  DateTime(DateTime&&) = default;
  DateTime& operator=(DateTime&& d) = default;
  DateTime& operator=(const DateTime& d) = default;
  ~DateTime() = default;

public:
  TimeSpan operator-(DateTime d) const;
  DateTime operator-(TimeSpan ts) const;
  DateTime operator+(TimeSpan ts) const;
  friend std::strong_ordering operator<=>(const kl::DateTime& x, const kl::DateTime& y) = default;
  friend bool operator==(const kl::DateTime& x, const kl::DateTime& y) = default;
  friend bool operator!=(const kl::DateTime& x, const kl::DateTime& y) = default;

  static const DateTime UnixEpoch;
  static const DateTime Max;
  static const DateTime Min;
  static DateTime now();
};

} // namespace kl

template <>
struct std::formatter<kl::TimeSpan> {
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it != end && *it != '}') {
      throw std::format_error("invalid format");
    }
    return it;
  }
  template <typename FormatContext>
  auto format(const kl::TimeSpan& ts, FormatContext& ctx) const -> decltype(ctx.out()) {
    std::string s;
    kl::TimeSpan t = ts;
    if (t.ticks < 0) {
      s = "-";
      t.ticks = -t.ticks;
    }
    uint32_t days = t.days();
    uint32_t hours = t.hours();
    uint32_t minutes = t.minutes();
    uint32_t seconds = t.seconds();
    uint32_t millis = t.milliseconds();

    if (days > 0) {
      return std::format_to(ctx.out(), "{}{}d {:0>2}:{:0>2}:{:0>2}.{:0>3}", s, days, hours, minutes, seconds, millis);
    }

    if (hours > 0) {
      return std::format_to(ctx.out(), "{}{:0>2}:{:0>2}:{:0>2}.{:0>3}", s, hours, minutes, seconds, millis);
    }

    if (minutes > 0) {
      return std::format_to(ctx.out(), "{}{:0>2}:{:0>2}.{:0>3}", s, minutes, seconds, millis);
    }

    return std::format_to(ctx.out(), "{}{:0>2}.{:0>3}", s, seconds, millis);
  }
};

template <>
struct std::formatter<kl::DateTime> {
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it != end && *it != '}') {
      throw std::format_error("invalid format");
    }
    return it;
  }
  template <typename FormatContext>
  auto format(const kl::DateTime& dt, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto d = dt.date();
    auto t = dt.time_of_day();
    return std::format_to(ctx.out(), "{:0>4}.{:0>2}.{:0>2}T{:0>2}:{:0>2}:{:0>2}.{:0>3}Z", d.year, d.month, d.day,
                          t.hour, t.min, t.sec, t.nanos / kl::TimeLimits::NanosecondsPerMillisecond);
  }
};

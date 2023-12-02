#include <kl/kl.hpp>
#include <kl/kltime.hpp>
#include <gtest/gtest.h>

TEST(kldate, sanity_checks) {
  EXPECT_EQ((25 * kl::TimeLimits::DaysInRegularInterval - 366) * kl::TimeLimits::TicksPerDay,
            kl::TimeLimits::MaxTicks + 1);
}

TEST(kldate, test_ticks) {
  EXPECT_EQ(kl::DateTime::from_ticks(kl::TimeLimits::MinTicks - 100).ticks(), kl::TimeLimits::MinTicks);
  EXPECT_EQ(kl::DateTime::from_ticks(kl::TimeLimits::MinTicks).ticks(), kl::TimeLimits::MinTicks);
  EXPECT_EQ(kl::DateTime::from_ticks(kl::TimeLimits::MaxTicks).ticks(), kl::TimeLimits::MaxTicks);
  EXPECT_EQ(kl::DateTime::from_ticks(kl::TimeLimits::MaxTicks + 1000).ticks(), kl::TimeLimits::MaxTicks);
}

void check_year(uint32_t year) {
  uint32_t delta_leap = ((year & 0x03) == 0 && (year % 100 != 0 || year % 400 == 0)) ? 1 : 0;
  uint32_t months[] = {31, 28 + delta_leap, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  for (uint32_t month = 0; month < 12; month++) {
    for (uint32_t day = 0; day < months[month]; day++) {
      auto dt = kl::DateTime(year, month + 1, day + 1);
      auto d = dt.date();
      auto t = dt.time_of_day();
      EXPECT_EQ(d.year, year);
      EXPECT_EQ(d.month, month + 1);
      EXPECT_EQ(d.day, day + 1);
      EXPECT_EQ(t.hour, 0);
      EXPECT_EQ(t.min, 0);
      EXPECT_EQ(t.sec, 0);
      EXPECT_EQ(t.nanos, 0);

      dt = kl::DateTime(year, month + 1, day + 1, 23, 59, 59, 999'999'999);
      d = dt.date();
      t = dt.time_of_day();
      EXPECT_EQ(d.year, year);
      EXPECT_EQ(d.month, month + 1);
      EXPECT_EQ(d.day, day + 1);
      EXPECT_EQ(t.hour, 23);
      EXPECT_EQ(t.min, 59);
      EXPECT_EQ(t.sec, 59);
      EXPECT_EQ(t.nanos, 999'999'900);

      dt = kl::DateTime(year, month + 1, day + 1, 14, 37, 21, 123'456'789);
      d = dt.date();
      t = dt.time_of_day();
      EXPECT_EQ(d.year, year);
      EXPECT_EQ(d.month, month + 1);
      EXPECT_EQ(d.day, day + 1);
      EXPECT_EQ(t.hour, 14);
      EXPECT_EQ(t.min, 37);
      EXPECT_EQ(t.sec, 21);
      EXPECT_EQ(t.nanos, 123'456'700);
    }
  }
}

TEST(kldate, test_date_time) {
  auto dt = kl::DateTime(9999, 12, 31, 23, 59, 59, 999999999);
  EXPECT_EQ(dt.ticks(), kl::TimeLimits::MaxTicks);
  EXPECT_EQ(dt.days(), 3652058);
  auto d = dt.date();
  EXPECT_EQ(d.year, 9999);
  EXPECT_EQ(d.month, 12);
  EXPECT_EQ(d.day, 31);

  check_year(2021);
  check_year(2020);
  check_year(2000);
  check_year(9021);
  check_year(9020);
  check_year(9000);
}

TEST(kldate, test_parsing) {
  auto dt = kl::DateTime::parse("2019-03-03T10:21Z");
  EXPECT_EQ(dt, kl::DateTime(2019, 03, 03, 10, 21));
  dt = kl::DateTime::parse("2019-03-03T10:21");
  EXPECT_EQ(dt, kl::DateTime(2019, 03, 03, 10, 21));
  dt = kl::DateTime::parse("2020-03-03 10:21");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 10, 21));
  dt = kl::DateTime::parse("2020-03-03 10:21:14");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 10, 21, 14));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781321");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 10, 21, 14, 781'321'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 10, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781Z");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 10, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781+02:00");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 8, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781+03:21");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 7, 00, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781+03:00");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 7, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781-02:00");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 12, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781-03:21");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 13, 42, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781-03:00");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 13, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14-03:00");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 13, 21, 14));
  dt = kl::DateTime::parse("2020-03-03 10:21:14+03:00");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 7, 21, 14));
  dt = kl::DateTime::parse("2020-03-03 10:21-03:00");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 13, 21));
  dt = kl::DateTime::parse("2020-03-03 10:21+03:00");
  EXPECT_EQ(dt, kl::DateTime(2020, 03, 03, 7, 21));
}

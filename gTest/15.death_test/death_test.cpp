#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>

void abort_func() {
    abort();
}

void crash_func(int a) {
    if (a == 0) {
        /* std::cout << "branch a == 0\n"; */
        std::cerr << "branch a == 0\n";
        int *p = nullptr;
        *p = 0;
    } else {
        /* std::cout << "branch a != 0\n"; */
        std::cerr << "branch a != 0\n";
        exit(50);
    }
}

TEST(DeathTest, Crash) {
    ASSERT_DEATH(crash_func(0), "branch.*");
}

TEST(DeathTest, Abort) {
    EXPECT_EXIT(abort_func(), ::testing::KilledBySignal(SIGABRT), ".*");
}

TEST(DeathTest, CrashCode) {
    EXPECT_EXIT(crash_func(0), ::testing::KilledBySignal(SIGSEGV), ".*");
}

TEST(DeathTest, Normal) {
    EXPECT_EXIT(crash_func(1), ::testing::ExitedWithCode(50), "br.*");
}

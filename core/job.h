#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <map>
#include <memory>
#include <sys/types.h>

struct Job {
    std::string id;
    std::string cmd;

    std::vector<std::string> needs;
    std::vector<std::string> semaphores;
    std::vector<std::string> children;

    std::atomic<int> remaining_deps{0};
    std::atomic<bool> started{false};
    std::atomic<bool> finished{false};

    int exit_code{-1};
    pid_t pid{0};
};

using JobPtr = std::shared_ptr<Job>;
extern std::map<std::string, JobPtr> jobs;

extern std::atomic<bool> abort_all;
extern std::atomic<int> running_children;

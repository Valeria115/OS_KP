#include "job.h"
#include <atomic>

std::map<std::string, JobPtr> jobs;
std::atomic<bool> abort_all{false};
std::atomic<int> running_children{0};

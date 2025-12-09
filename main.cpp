#include <iostream>
#include "config/parser.h"
#include "core/validator.h"
#include "core/scheduler.h"
#include "core/semaphoreManager.h"
#include "core/job.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: dag_executor <config.yaml>\n";
        return 1;
    }

    std::string cfg = argv[1];
    int max_concurrent = 0;

    if (!Parser::load_config(cfg, max_concurrent)) {
        std::cerr << "Config parsing failed\n";
        return 2;
    }

    if (!Validator::validate_graph()) {
        std::cerr << "Graph validation failed\n";
        return 3;
    }

    SemaphoreManager::init_global(max_concurrent);

    std::cout << "Starting scheduler with max_concurrent=" << max_concurrent << "\n";

    Scheduler::run();

    SemaphoreManager::destroy();

    return abort_all.load() ? 1 : 0;
}

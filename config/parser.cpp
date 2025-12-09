#include "parser.h"
#include <memory>
#include <string>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include "../core/job.h"
#include "../core/semaphoreManager.h"

bool Parser::load_config(const std::string &filename, int &max_concurrent) {
    YAML::Node cfg = YAML::LoadFile(filename);

    if (!cfg["max_concurrent"]) {
        std::cerr << "No max_concurrent in config\n";
        return false;
    }
    max_concurrent = cfg["max_concurrent"].as<int>();

    if (cfg["semaphores"]) {
        for (auto s : cfg["semaphores"]) {
            SemaphoreManager::define_semaphore(s["name"].as<std::string>(), s["value"].as<int>());
        }
    }

    if (!cfg["jobs"]) {
        std::cerr << "No jobs in config\n";
        return false;
    }

    for (auto j : cfg["jobs"]) {
        JobPtr job = std::make_shared<Job>();
        job->id = j["id"].as<std::string>();
        job->cmd = j["cmd"].as<std::string>();

        if (j["needs"]) {
            for (auto n : j["needs"])
                job->needs.push_back(n.as<std::string>());
        }

        if (j["semaphores"]) {
            for (auto s : j["semaphores"])
                job->semaphores.push_back(s.as<std::string>());
        }

        jobs[job->id] = job;
    }

    // children
    for (auto &kv : jobs) {
        for (auto &dep : kv.second->needs) {
            if (jobs.count(dep) == 0) {
                std::cerr << "Unknown dependency: " << dep << "\n";
                return false;
            }
            jobs[dep]->children.push_back(kv.first);
        }
    }

    for (auto &kv : jobs) {
        kv.second->remaining_deps.store(kv.second->needs.size());
    }

    return true;
}
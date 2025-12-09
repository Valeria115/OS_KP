#include "semaphoreManager.h"
#include <algorithm>
#include <semaphore.h>
#include <vector>

std::map<std::string, SemaphoreDef> SemaphoreManager::s_map;
sem_t SemaphoreManager::global_sem;

void SemaphoreManager::define_semaphore(const std::string &name, int value) {
    SemaphoreDef d;
    d.name = name;
    d.value = value;
    sem_init(&d.sem, 0, value);
    s_map[name] = d;
}

void SemaphoreManager::init_global(int max_concurrent) {
    sem_init(&global_sem, 0, max_concurrent);
}

sem_t* SemaphoreManager::global() { return &global_sem; }

bool SemaphoreManager::acquire_all(const std::vector<std::string>& vec) {
    std::vector<std::string> sems = vec;
    std::sort(sems.begin(), sems.end());

    for (auto &s : sems) {
        if (sem_wait(&s_map[s].sem) != 0) {
            return false;
        }
    }
    return true;
}

void SemaphoreManager::release_all(const std::vector<std::string>& vec) {
    for (auto &s : vec) {
        sem_post(&s_map[s].sem);
    }
}

void SemaphoreManager::destroy() {
    for (auto &kv : s_map) {
        sem_destroy(&kv.second.sem);
    }

    sem_destroy(&global_sem);
}

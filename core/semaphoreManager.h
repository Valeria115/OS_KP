#pragma once
#include <string>
#include <map>
#include <semaphore.h>
#include <vector>

struct SemaphoreDef {
    std::string name;
    int value;
    sem_t sem;
};

class SemaphoreManager {
public:
    static void define_semaphore(const std::string& name, int value);
    static bool acquire_all(const std::vector<std::string>& sems);
    static void release_all(const std::vector<std::string>& sems);

    static void init_global(int max_concurrent);
    static void destroy();

    static sem_t* global();

private:
    static std::map<std::string, SemaphoreDef> s_map;
    static sem_t global_sem;
};

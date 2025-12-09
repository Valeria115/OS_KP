#include "scheduler.h"
#include "job.h"
#include "semaphoreManager.h"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include <thread>


using namespace std::chrono_literals;

static void kill_all_children() {
    for (auto &kv : jobs) {
        pid_t p = kv.second->pid;
        if (p > 0 && kill(p, 0) == 0)
            kill(p, SIGTERM);
    }
    std::this_thread::sleep_for(200ms);
    for (auto &kv : jobs) {
        pid_t p = kv.second->pid;
        if (p > 0 && kill(p, 0) == 0)
            kill(p, SIGKILL);
    }
}

static void start_job(JobPtr job) {
    bool expected = false;
    if (!job->started.compare_exchange_strong(expected, true)) return;

    sem_wait(SemaphoreManager::global());

    if (!SemaphoreManager::acquire_all(job->semaphores)) {
        sem_post(SemaphoreManager::global());
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        SemaphoreManager::release_all(job->semaphores);
        sem_post(SemaphoreManager::global());
        return;
    }
    if (pid == 0) {
        setpgid(0, 0);
        execl("/bin/sh", "sh", "-c", job->cmd.c_str(), (char*)NULL);
        perror("exec");
        _exit(127);
    }

    job->pid = pid;
    running_children++;

    std::thread([job]() {
        int st = 0;
        waitpid(job->pid, &st, 0);

        if (WIFEXITED(st))
            job->exit_code = WEXITSTATUS(st);
        else if (WIFSIGNALED(st))
            job->exit_code = 128 + WTERMSIG(st);
        else
            job->exit_code = -1;

        job->finished = true;
        running_children--;

        SemaphoreManager::release_all(job->semaphores);
        sem_post(SemaphoreManager::global());

        if (job->exit_code != 0) {
            abort_all = true;
            kill_all_children();
        }

        for (auto &cid : job->children)
            jobs[cid]->remaining_deps--;
    }).detach();
}

void Scheduler::run() {
    while (true) {
        if (abort_all) break;

        bool all_done = true;

        for (auto &kv : jobs) {
            auto job = kv.second;
            if (!job->finished) {
                all_done = false;

                if (!job->started && job->remaining_deps.load() == 0) {
                    start_job(job);
                }
            }
        }

        if (all_done) break;

        std::this_thread::sleep_for(50ms);
    }

    while (running_children > 0) {
        std::this_thread::sleep_for(50ms);
    }

    std::cout << "Execution completed";
    for (auto &kv : jobs) {
        std::cout << kv.first << "exit=" << kv.second->exit_code << " started="
                << kv.second->started << "finished=" << kv.second->finished << "\n";
    }
}

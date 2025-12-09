#include "validator.h"
#include "job.h"
#include <iostream>
#include <set>
#include <map>
#include <vector>

static bool dfs_cycle(const JobPtr& j, std::map<std::string, int>& st) {
    st[j->id] = 1;
    for (auto &c : j->children) {
        if (st[c] == 1) return true;
        if (st[c] == 0 && dfs_cycle(jobs[c], st)) return true;
    }
    st[j->id] = 2;
    return false;
}

bool Validator::validate_graph() {
    if (jobs.empty()) {
        std::cerr << "Empty job list\n";
        return false;
    }

    std::map<std::string,int> st;
    for (auto &kv : jobs) st[kv.first] = 0;
    for (auto &kv : jobs) {
        if (st[kv.first] == 0) {
            if (dfs_cycle(kv.second, st)) {
                std::cerr << "Cycle detected\n";
                return false;
            }
        }
    }

    std::set<std::string> vis;
    std::vector<std::string> st2;
    st2.push_back(jobs.begin()->first);

    while (!st2.empty()) {
        auto cur = st2.back(); st2.pop_back();
        if (!vis.insert(cur).second) continue;

        for (auto &n : jobs[cur]->needs) {
            if (!vis.count(n)) st2.push_back((n));
        }

        for (auto &c : jobs[cur]->children) {
            if (!vis.count(c)) st2.push_back(c);
        }
    }

    if (vis.size() != jobs.size()) {
        std::cerr << "Graph not connected\n";
        return false;
    }

    int starts = 0, ends = 0;
    for (auto &kv : jobs) {
        if (kv.second->needs.empty()) starts++;
        if (kv.second->children.empty()) ends++;
    }
    if (!starts || !ends) {
        std::cerr << "No start ot no end jobs\n";
        return false;
    }

    return true;
}

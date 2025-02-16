#pragma once
#include <map>
#include <mutex>
#include <string>

class SessionManager {
    std::map<std::string, std::map<std::string, double>> sessions_;
    mutable std::mutex mtx_;

public:
    std::map<std::string, double>& get_session(const std::string& user) {
        std::lock_guard<std::mutex> lock(mtx_);
        return sessions_[user];
    }

    void clear_session(const std::string& user) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = sessions_.find(user);
        if (it != sessions_.end()) it->second.clear();
    }
};
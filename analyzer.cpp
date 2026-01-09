#include "analyzer.h"
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

class TripAnalyzer::Impl {
private:
    map<string, int> zones;
    map<string, map<int, int>> slots;

public:
    void ingestFile(const string& csvPath) {
         zones.clear();
         slots.clear();

        ifstream file(csvPath);
        if (!file.is_open()) return;
        
        string line;
        
        // Skip header
        getline(file, line);
        
        while (getline(file, line)) {
            if (line.empty()) continue;
            
            // Handle Windows line endings
            if (line.back() == '\r') line.pop_back();
            
            // Find two commas
            size_t c1 = line.find(',');
            if (c1 == string::npos) continue;
            
            size_t c2 = line.find(',', c1 + 1);
            if (c2 == string::npos) continue;
            
            // Get zone (between commas)
            string zone = line.substr(c1 + 1, c2 - c1 - 1);
            
            // Get the rest for datetime
            string rest = line.substr(c2 + 1);
            
            // Find hour in format HH:MM
            int hour = -1;
            for (size_t i = 0; i + 4 < rest.length(); i++) {
                if (isdigit(rest[i]) && isdigit(rest[i+1]) &&
                    rest[i+2] == ':' &&
                    isdigit(rest[i+3]) && isdigit(rest[i+4])) {
                    
                    hour = (rest[i] - '0') * 10 + (rest[i+1] - '0');
                    if (hour >= 0 && hour <= 23) {
                        break;
                    }
                }
            }
            
            if (hour == -1) continue;
            
            zones[zone]++;
            slots[zone][hour]++;
        }
    }
    
    vector<ZoneCount> topZones(int k = 10) const {
        vector<pair<string, int>> sorted;
        for (const auto& p : zones) {
            sorted.push_back(p);
        }
        
        // FIXED: Changed b.count to b.second
        sort(sorted.begin(), sorted.end(),
            [](const pair<string, int>& a, const pair<string, int>& b) {
                if (a.second != b.second) return a.second > b.second;
                return a.first < b.first;
            });
        
        k = min(k, (int)sorted.size());
        vector<ZoneCount> result;
        for (int i = 0; i < k; i++) {
            result.push_back({sorted[i].first, sorted[i].second});
        }
        return result;
    }
    
    vector<SlotCount> topBusySlots(int k = 10) const {
        vector<SlotCount> allSlots;
        
        for (const auto& zonePair : slots) {
            for (const auto& hourPair : zonePair.second) {
                allSlots.push_back({zonePair.first, hourPair.first, hourPair.second});
            }
        }
        
        sort(allSlots.begin(), allSlots.end(),
            [](const SlotCount& a, const SlotCount& b) {
                if (a.count != b.count) return a.count > b.count;
                if (a.zone != b.zone) return a.zone < b.zone;
                return a.hour < b.hour;
            });
        
        k = min(k, (int)allSlots.size());
        return vector<SlotCount>(allSlots.begin(), allSlots.begin() + k);
    }
};

TripAnalyzer::TripAnalyzer() : impl(new Impl()) {}
TripAnalyzer::~TripAnalyzer() { delete impl; }

void TripAnalyzer::ingestFile(const string& csvPath) {
    impl->ingestFile(csvPath);
}

vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    return impl->topZones(k);
}

vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    return impl->topBusySlots(k);
}
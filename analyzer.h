#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>
#include <vector>

struct ZoneCount {
    std::string zone;
    int count;
};

struct SlotCount {
    std::string zone;
    int hour;
    int count;
};

class TripAnalyzer {
public:
    TripAnalyzer();
    ~TripAnalyzer();
    
    void ingestFile(const std::string& csvPath);
    std::vector<ZoneCount> topZones(int k = 10) const;
    std::vector<SlotCount> topBusySlots(int k = 10) const;

private:
    class Impl;
    Impl* impl;
};

#endif
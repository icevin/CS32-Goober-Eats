#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

unsigned int hasher(const GeoCoord& g) {
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl {
  public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
  private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> m_map;
    void addSegment(StreetSegment s);
};

StreetMapImpl::StreetMapImpl() {
}

StreetMapImpl::~StreetMapImpl() {
}

bool StreetMapImpl::load(string mapFile) {
    ifstream infile(mapFile);
    if (!infile)
        return false;
    string street_name;
    while (getline(infile, street_name)) {
        int nSegments;
        infile >> nSegments;
        infile.ignore(10000, '\n');
        for (; nSegments > 0; nSegments--) {
            string startLat = "", startLong = "", endLat = "", endLong = "";
            infile >> startLat >> startLong >> endLat >> endLong;
            infile.ignore(10000, '\n');
            GeoCoord start(startLat, startLong);
            GeoCoord end(endLat, endLong);
            addSegment(StreetSegment(start, end, street_name));
            addSegment(StreetSegment(end, start, street_name));
        }
    }
    // cout << m_map.size() << "<- size" << endl;

    return true;
}

void StreetMapImpl::addSegment(StreetSegment s) {
    vector<StreetSegment>* segments = m_map.find(s.start);
    if(segments != nullptr) {
        segments->push_back(s);
        return;
    }
    vector<StreetSegment> new_segments;
    new_segments.push_back(s);
    m_map.associate(s.start, new_segments);
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const {
    const vector<StreetSegment>* temp = m_map.find(gc);
    if(temp == nullptr)
        return false;
    segs = *temp;
    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap() {
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap() {
    delete m_impl;
}

bool StreetMap::load(string mapFile) {
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const {
    return m_impl->getSegmentsThatStartWith(gc, segs);
}

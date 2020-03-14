#include <list>
#include <queue>
#include <set>
#include <unordered_map>
#include "provided.h"
using namespace std;

class PointToPointRouterImpl {
  public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;

  private:
    const StreetMap* m_map;

    // double euclideanDist(const GeoCoord a, const GeoCoord b) const {
    //     return sqrt(
    //         pow(a.latitude - b.latitude, 2) + pow(a.longitude - b.longitude, 2));
    // }
    struct StreetSegmentNode {
        StreetSegmentNode(StreetSegment s, double p) {
            seg = s;
            priority = p;
        }
        StreetSegment seg;
        double priority;
        bool operator==(const StreetSegmentNode& other) const {
            return this->seg.start.latitude == other.seg.start.latitude 
            && this->seg.start.longitude == other.seg.start.longitude
            && this->seg.end.latitude == other.seg.end.latitude 
            && this->seg.end.longitude == other.seg.end.longitude
            && this->seg.name == other.seg.name;
        }
    };
    struct GeoCoordHash {
        unsigned int operator()(const GeoCoord& p) const {
            return std::hash<string>()(p.latitudeText + p.longitudeText);
        }
    };
    
    struct StreetSegmentHash {
        unsigned int operator()(const StreetSegment& p) const {
            return std::hash<string>()(p.start.latitudeText + p.start.longitudeText + p.end.latitudeText + p.end.longitudeText);
        }
    };

    struct StreetSegmentCompareCost {
        bool operator()(StreetSegmentNode const& p1, StreetSegmentNode const& p2) {
            return p1.priority > p2.priority;
        }
    };
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm) {
    m_map = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl() {
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
    const GeoCoord& start,
    const GeoCoord& end,
    list<StreetSegment>& route,
    double& totalDistanceTravelled) const {

    unordered_map<GeoCoord, double, GeoCoordHash> distance_so_far;
    unordered_map<StreetSegment, StreetSegment, StreetSegmentHash> parent_of;

    priority_queue<StreetSegmentNode, vector<StreetSegmentNode>, StreetSegmentCompareCost> frontier;

    distance_so_far[start] = 0;
    totalDistanceTravelled = 0;

    vector<StreetSegment> start_segments;
    if(!m_map->getSegmentsThatStartWith(start, start_segments))
        return BAD_COORD;

    StreetSegment a(start, start, "__start__");
    StreetSegmentNode current(a, 0);
    frontier.push(current);

    while (!frontier.empty()) {
        // cout << current.seg.name << endl;
        // cout << current.seg.start.latitudeText << " " << current.seg.start.longitudeText << "|" << current.seg.end.latitudeText << " " << current.seg.end.longitudeText << "|  " << current.seg.name << endl;
        current = frontier.top();
        frontier.pop();
        if (current.seg.end == end)
            break;

        vector<StreetSegment> neighbors;
        if(m_map->getSegmentsThatStartWith(current.seg.end, neighbors)) {
            for (StreetSegment a : neighbors) {
                double new_distance = distance_so_far[current.seg.end] + distanceEarthMiles(a.start, a.end);
                if(distance_so_far.find(a.end) == distance_so_far.end() || new_distance < distance_so_far[a.end]) {
                    distance_so_far[a.end] = new_distance;
                    StreetSegmentNode next(a, new_distance + distanceEarthMiles(a.end, end));
                    frontier.push(next);
                    parent_of[next.seg] = current.seg;
                }
            }
        }
    }

    if(current.seg.end != end)
        return NO_ROUTE;

    StreetSegment endSegment = current.seg;

    while(endSegment.start != start) {
        route.push_front(endSegment);
        endSegment = parent_of[endSegment];
        totalDistanceTravelled += distanceEarthMiles(endSegment.start, endSegment.end);
    }
    route.push_front(endSegment);
    totalDistanceTravelled += distanceEarthMiles(endSegment.start, endSegment.end);
    return DELIVERY_SUCCESS;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm) {
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter() {
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
    const GeoCoord& start,
    const GeoCoord& end,
    list<StreetSegment>& route,
    double& totalDistanceTravelled) const {
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}

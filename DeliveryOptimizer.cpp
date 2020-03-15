#include <algorithm>
#include <cmath>
#include <list>
#include <random>
#include <vector>
#include "provided.h"
using namespace std;

class DeliveryOptimizerImpl {
  public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;

  private:
    StreetMap* sm;
    PointToPointRouter p;
    double distanceOfDeliveries(GeoCoord depot, vector<DeliveryRequest> list) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
    : p(sm) {
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl() {
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const {
    if (deliveries.size() == 0)
        return;
    oldCrowDistance = distanceOfDeliveries(depot, deliveries);
    newCrowDistance = oldCrowDistance;

    // Simulated Annealing;
    double temperature = 10000;
    double coolingRate = 0.99;

    vector<DeliveryRequest> best(deliveries);
    vector<DeliveryRequest> current(deliveries);
    random_shuffle(current.begin(), current.end());

    while (temperature > 1) {
        vector<DeliveryRequest> newD(current);

        random_shuffle(newD.begin(), newD.end());

        double currentDistance = distanceOfDeliveries(depot, current);
        double newDistance     = distanceOfDeliveries(depot, newD);
        double bestDistance    = distanceOfDeliveries(depot, best);

        if (newDistance < currentDistance || ((exp((bestDistance - currentDistance) / temperature) > ((rand() % 1000) / 1000.0))))
            current = newD;
        if (newDistance < bestDistance)
            best = current;
        temperature *= coolingRate;
        newCrowDistance = bestDistance;
    }
    deliveries.swap(best);
}

double DeliveryOptimizerImpl::distanceOfDeliveries(GeoCoord depot, vector<DeliveryRequest> listR) const {
    list<StreetSegment> trash;
    double distance = 0;
    //p.generatePointToPointRoute(depot, listR.begin()->location, trash, distance);
    distance += distanceEarthMiles(depot, listR.begin()->location);
    for (auto i = listR.begin(); next(i) != listR.end(); i++) {
        // double x;
        // p.generatePointToPointRoute(depot, listR.begin()->location, trash, x);
        // distance += x;
        distance += distanceEarthMiles(i->location, next(i)->location);
    }
    // double y;
    // p.generatePointToPointRoute(prev(listR.end())->location, depot, trash, y);
    // distance += y;
    distance += distanceEarthMiles(prev(listR.end())->location, depot);
    return distance;
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm) {
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer() {
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const {
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}

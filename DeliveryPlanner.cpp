#include <vector>
#include "provided.h"
using namespace std;

class DeliveryPlannerImpl {
  public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;

  private:
    PointToPointRouter m_router;
    const DeliveryOptimizer m_optimizer;
    string angleToDir(double angle) const {
        if ((angle >= 0 && angle < 22.5) || angle >= 337.5)
            return "east";
        else if (angle >= 22.5 && angle < 67.5)
            return "northeast";
        else if (angle >= 67.5 && angle < 112.5)
            return "north";
        else if (angle >= 112.5 && angle < 157.5)
            return "northwest";
        else if (angle >= 157.5 && angle < 202.5)
            return "west";
        else if (angle >= 202.5 && angle < 247.5)
            return "southwest";
        else if (angle >= 247.5 && angle < 292.5)
            return "south";
        else if (angle >= 292.5 && angle < 337.5)
            return "southeast";
        return "";
    }
    DeliveryResult generateOrders(list<StreetSegment> deliveryRt, vector<DeliveryCommand>& cmds, GeoCoord& start, GeoCoord& end, double& total_distance) const;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
    : m_router(sm), m_optimizer(sm) {
}

DeliveryPlannerImpl::~DeliveryPlannerImpl() {
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const {
    if (deliveries.size() == 0)
        return DELIVERY_SUCCESS;

    double oldDist = 0;
    double newDist = 0;
    vector<DeliveryRequest> copy_deliveries(deliveries);
    m_optimizer.optimizeDeliveryOrder(depot, copy_deliveries, oldDist, newDist);

    GeoCoord start_coord = depot;
    GeoCoord end_coord;

    totalDistanceTravelled = 0;

    for (DeliveryRequest r : copy_deliveries) {
        end_coord = r.location;
        list<StreetSegment> delivery_route;
        double delivery_distance       = 0;
        DeliveryResult generate_result = generateOrders(delivery_route, commands, start_coord, end_coord, delivery_distance);
        if (generate_result != DELIVERY_SUCCESS)
            return generate_result;
        totalDistanceTravelled += delivery_distance;
        DeliveryCommand deliver;
        deliver.initAsDeliverCommand(r.item);
        commands.push_back(deliver);
        start_coord = r.location;
    }

    end_coord = depot;
    list<StreetSegment> home_route;
    double home_distance       = 0;
    DeliveryResult home_result = generateOrders(home_route, commands, start_coord, end_coord, home_distance);
    if (home_result != DELIVERY_SUCCESS)
        return home_result;
    totalDistanceTravelled += home_distance;

    return DELIVERY_SUCCESS;
}

DeliveryResult DeliveryPlannerImpl::generateOrders(list<StreetSegment> deliveryRt, vector<DeliveryCommand>& cmds, GeoCoord& start, GeoCoord& end, double& total_distance) const {
    DeliveryResult temp_result = m_router.generatePointToPointRoute(start, end, deliveryRt, total_distance);

    if (temp_result != DELIVERY_SUCCESS)
        return temp_result;

    if (deliveryRt.size() == 0 || deliveryRt.size() == 1) {
        if (deliveryRt.size() == 1) {
            DeliveryCommand proceed;
            proceed.initAsProceedCommand(
                angleToDir(angleOfLine(*deliveryRt.begin())),
                deliveryRt.begin()->name,
                distanceEarthMiles(deliveryRt.begin()->start, deliveryRt.begin()->end));
            cmds.push_back(proceed);
            total_distance += distanceEarthMiles(deliveryRt.begin()->start, deliveryRt.begin()->end);
        }
        return DELIVERY_SUCCESS;
    }

    total_distance = 0;
    auto startSegI = deliveryRt.begin();  // Segment representing the start of our current command
    auto nextSegI  = next(deliveryRt.begin());
    while (nextSegI != deliveryRt.end()) {
        double segment_distance = 0;
        segment_distance += distanceEarthMiles(startSegI->start, startSegI->end);
        double street_angle = angleOfLine(*startSegI);

        if (nextSegI->name == startSegI->name) {
            while (nextSegI->name == startSegI->name) {
                segment_distance += distanceEarthMiles(nextSegI->start, nextSegI->end);
                if (next(nextSegI) == deliveryRt.end()) {
                    break;
                } else {
                    nextSegI++;
                }
            }
        }
        DeliveryCommand current_command;
        current_command.initAsProceedCommand(
            angleToDir(street_angle),
            startSegI->name,
            segment_distance);
        cmds.push_back(current_command);

        if (next(nextSegI) != deliveryRt.end()) {
        DeliveryCommand streetChange;
        double turn_angle = angleBetween2Lines(*startSegI, *nextSegI);
        if (turn_angle < 1.0 || turn_angle > 359.0) {
        } else if (turn_angle >= 1.0 && turn_angle <= 180) {
            streetChange.initAsTurnCommand("left", nextSegI->name);
            cmds.push_back(streetChange);
        } else if (turn_angle >= 180.0 && turn_angle <= 359.0) {
            streetChange.initAsTurnCommand("right", nextSegI->name);
            cmds.push_back(streetChange);
        }
        }

        total_distance += segment_distance;
        startSegI = nextSegI;
        nextSegI++;
    }
    return DELIVERY_SUCCESS;
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm) {
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner() {
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const {
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}

#include "decision.hpp"

Action decide(double distance_cm, const Params& p){
    if (distance_cm <= p.stop_cm){
        return Action ::Stop;
    }

    else if (distance_cm <= p.slow_cm){
        return Action::Slow;
    }

    else{
        return Action::Move;
    }
    
}
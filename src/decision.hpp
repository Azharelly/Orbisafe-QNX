#pragma once
enum class Action {Move, Slow, Stop};
struct Params {
    double stop_cm = 30.0;
    double slow_cm = 70.0;
};

Action decide (double distance_cm, const Params& p=Params{});

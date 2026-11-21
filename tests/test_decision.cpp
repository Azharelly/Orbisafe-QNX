#include <iostream>
#include <cassert>
#include "decision.hpp"

int main(){
    assert(decide(100.0) == Action::Move);
    assert(decide(71.0) == Action::Move);
    assert(decide(50.0) == Action::Slow);
    assert(decide(31.0) == Action::Slow);
    assert(decide(70.0) == Action::Slow);
    assert(decide(20.0) == Action::Stop);
    assert(decide(30.0) == Action::Stop);
    assert(decide(5.0) == Action::Stop);

    std::cout << "All tests passed\n";
    return 0;
}
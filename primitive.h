#pragma once
#include <vector>
#include <array>
#include "model.h"

class Primitive {
public:
    static Model createPlane();
    static Model createSphere(int segments = 16);
    static Model createCylinder(int segments = 16);
};
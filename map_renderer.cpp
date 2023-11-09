#include "map_renderer.h"
bool renderer::IsZero(double value) {
    return std::abs(value) < EPSILON;
}
double renderer::SphereProjector::GetZoom() {
    return zoom_coeff_;
}
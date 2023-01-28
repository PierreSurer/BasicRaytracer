#pragma once

#include <glm.hpp>
#include <memory>

class Material;
class Object;

class Ray
{
public:
    glm::dvec3 O;
    glm::dvec3 D;
    double time;

    Ray(const glm::dvec3 &from, const glm::dvec3 &dir, double delay = 0.0)
        : O(from), D(dir), time(delay)
    { }

    glm::dvec3 at(double t) const
    { return O + t * D; }

};

struct HitParams
{
    const Object* obj = nullptr;
    glm::dvec3 normal = glm::dvec3(0.0);
    glm::dvec2 tex_coords = glm::dvec3(0.0);
};

class BaseHit
{
public:
    double t;
    bool no_hit;
    
    BaseHit(double t, bool no_hit = false)
        : t(t), no_hit(no_hit)
    { }

    virtual ~BaseHit() = default;

    virtual HitParams params() const = 0;
};

class Hit : public BaseHit
{
public:
    Hit(double t, HitParams params, bool no_hit = false)
        : BaseHit(t, no_hit), _params(params)
    { }

    static const std::unique_ptr<BaseHit> NO_HIT() { 
        constexpr double NaN = std::numeric_limits<double>::infinity();
        static Hit no_hit(NaN, HitParams{}, true);
        return std::make_unique<Hit>(no_hit);
    }

    HitParams params() const override {
        return _params;
    }

private:
    HitParams _params;
};

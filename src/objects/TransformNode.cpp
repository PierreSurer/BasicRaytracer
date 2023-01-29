#include "TransformNode.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_access.hpp>

using namespace glm;

TransformNode::TransformNode(std::unique_ptr<Object> obj, dmat4 model)
    : obj(std::move(obj))
{
    setModel(model);
}

AABB TransformNode::computeAABB() const
{
    return obj->computeAABB();
}


class NodeHit : public BaseHit
{
public:
    NodeHit(const TransformNode* node, std::unique_ptr<BaseHit> hit, double time);

    HitParams params() const override;

private:
    const TransformNode* node;
    std::unique_ptr<BaseHit> hit;
    double time;
};


NodeHit::NodeHit(const TransformNode* node, std::unique_ptr<BaseHit> hit, double time)
    // CAREFUL hit->t is used before hit is moved, so this is fine but changing
    // the code might lead to undefined behavior.
    : BaseHit(hit->t), node(node), hit(std::move(hit)), time(time)
{ }

HitParams NodeHit::params() const
{
    HitParams params = hit->params();
    dmat3 M = node->computeModelInv(time);
    params.normal = normalize(params.normal * M);
    return params;
}

std::unique_ptr<BaseHit> TransformNode::intersect(const Ray& globalRay) const
{
    Ray localRay = computeLocalRay(globalRay);
    auto hit = obj->intersect(localRay);
    if (hit->no_hit) {
        return hit;
    }
    else {
        // spatial dilatation is uniform in a given direction (matrix transform)
        dmat3 M = computeModel(globalRay.time);
        hit->t *= length(M * localRay.D);
        return std::make_unique<NodeHit>(this, std::move(hit), globalRay.time);
    }
}

Ray TransformNode::computeLocalRay(const Ray& globalRay) const
{
    dmat3 M = computeModelInv(globalRay.time);
    dvec3 O = M * (globalRay.O - position - velocity * globalRay.time);
    dvec3 D = normalize(M * dvec4(globalRay.D, 0.0));

    return Ray(O, D, globalRay.time);
}

void TransformNode::setModel(dmat4 M)
{
    model = M;
    modelInv = inverse(model);
    position = column(M, 3);
}

glm::dmat3 TransformNode::computeModel(double time) const
{
    auto angle = angularVelocity * time;
    return dmat3(eulerAngleYXZ(angle.x, angle.y, angle.z)) * model;
}
glm::dmat3 TransformNode::computeModelInv(double time) const
{
    auto angle = -angularVelocity * time;
    return modelInv * dmat3(eulerAngleYXZ(angle.x, angle.y, angle.z));
}

void TransformNode::setLinearVelocity(glm::dvec3 vel)
{
    velocity = vel;
}
void TransformNode::setAngularVelocity(glm::dvec3 vel)
{
    angularVelocity = vel;
}

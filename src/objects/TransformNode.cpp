#include "TransformNode.hpp"

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
    NodeHit(const TransformNode* node, std::unique_ptr<BaseHit> hit);

    HitParams params() const override;

private:
    const TransformNode* node;
    std::unique_ptr<BaseHit> hit;
};


NodeHit::NodeHit(const TransformNode* node, std::unique_ptr<BaseHit> hit)
    // CAREFUL hit->t is used before hit is moved, so this is fine but changing
    // the code might lead to undefined behavior.
    : BaseHit(hit->t), node(node), hit(std::move(hit))
{ }

HitParams NodeHit::params() const
{
    HitParams params = hit->params();
    params.normal = normalize(node->model_inv_transp * params.normal);
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
        hit->t *= length(model * localRay.D);
        return std::make_unique<NodeHit>(this, std::move(hit));
    }
}

Ray TransformNode::computeLocalRay(const Ray& globalRay) const
{
    dvec3 O = model_inv * (globalRay.O - position - velocity * globalRay.time);
    dvec3 D = normalize(model_inv * globalRay.D);
    return Ray(O, D, globalRay.time);
}

void TransformNode::setModel(dmat4 M)
{
    model = M;
    model_inv = inverse(model);
    model_inv_transp = transpose(model_inv);
    position = column(M, 3);
}


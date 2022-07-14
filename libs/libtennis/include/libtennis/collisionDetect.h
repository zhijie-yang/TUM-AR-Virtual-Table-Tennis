//
// Created by Ryou on 2022/6/23.
//

#ifndef MODERN_CMAKE_TEMPLATE_COLLISIONDETECT_H
#define MODERN_CMAKE_TEMPLATE_COLLISIONDETECT_H

#endif //MODERN_CMAKE_TEMPLATE_COLLISIONDETECT_H

// header file:
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

// the return structure, with these values, you should be able to calculate the impulse
// the depth shouldn't be used in your impulse calculation, it is a redundant value
// if the normalWorld == XMVectorZero(), no collision
struct CollisionInfo{
    bool isValid;                          // whether there is a collision point, true for yes
    glm::vec3 collisionPointWorld; // the position of the collision point in world space
    glm::vec3 normalWorld;         // the direction of the impulse to A, negative of the collision face of A
    float depth;                           // the distance of the collision point to the surface, not necessary.
};

// tool data structures/functions called by the collision detection method, you can ignore the details here
namespace libtennis{
    struct Projection{
        float min, max;
    };

    /*
     * Math impl
     */
    inline glm::vec4 vector3TransformNormal(glm::vec4 vec, glm::mat4 mat) {
        glm::vec4 z = glm::vec4(vec[2], vec[2], vec[2], vec[2]);
        glm::vec4 y = glm::vec4(vec[1], vec[1], vec[1], vec[1]);
        glm::vec4 x = glm::vec4(vec[0], vec[0], vec[0], vec[0]);

        glm::vec4 result = z * glm::vec4(row(mat, 2));
        result = y * glm::vec4(row(mat, 1)) + result;
        result = x * glm::vec4(row(mat, 0)) + result;

        return result;
    }

    inline glm::vec4 vector3Transform(glm::vec4 vec, glm::mat4 mat) {
        glm::vec4 z = glm::vec4(vec[2], vec[2], vec[2], vec[2]);
        glm::vec4 y = glm::vec4(vec[1], vec[1], vec[1], vec[1]);
        glm::vec4 x = glm::vec4(vec[0], vec[0], vec[0], vec[0]);

        glm::vec4 result = z * glm::vec4(row(mat, 2)) + glm::vec4(row(mat, 3));
        result = y * glm::vec4(row(mat, 1)) + result;
        result = y * glm::vec4(row(mat, 0)) + result;

        return result;
    }

    inline glm::vec4 vector3LengthSq(glm::vec4 vec) {
        float dot = glm::dot(vec, vec);
        return glm::vec4(dot, dot, dot, dot);
    }

    inline glm::vec4 vector3Length(glm::vec4 vec) {
        glm::vec4 result = vector3LengthSq(vec);
        result[0] = sqrt(result[0]);
        result[1] = sqrt(result[1]);
        result[2] = sqrt(result[2]);
        result[3] = sqrt(result[3]);
        return result;
    }

    inline glm::vec4 vector3Normalize(glm::vec4 vec) {
        float len = glm::length(vec);

        if (len > 0) len = 1.0f / len;
        return (vec * len);
    }

    /*
     * Math impl end
    */


    inline glm::vec4 getVectorConnnectingCenters(const glm::mat4& obj2World_A, const glm::mat4& obj2World_B)
    {
        const glm::vec4 centerWorld_A = vector3Transform(glm::vec4(0, 0, 0, 0), obj2World_A);
        const glm::vec4 centerWorld_B = vector3Transform(glm::vec4(0, 0, 0, 0), obj2World_B);
        return centerWorld_B - centerWorld_A;

    }

    // Get Corners
    inline std::vector<glm::vec4> getCorners(const glm::mat4& obj2World)
    {
        const glm::vec4 centerWorld = vector3Transform(glm::vec4(0, 0, 0, 0), obj2World);
        glm::vec4 edges[3];
        for (size_t i = 0; i < 3; ++i) {
            glm::vec4 vec = glm::vec4(0, 0, 0, 0);
            vec[i] = 0.5f;
            edges[i] = vector3TransformNormal(vec, obj2World);
        }
        std::vector<glm::vec4> results;
        results.push_back(centerWorld - edges[0] - edges[1] - edges[2]);
        results.push_back(centerWorld + edges[0] - edges[1] - edges[2]);
        results.push_back(centerWorld - edges[0] + edges[1] - edges[2]);
        results.push_back(centerWorld + edges[0] + edges[1] - edges[2]); // this +,+,-
        results.push_back(centerWorld - edges[0] - edges[1] + edges[2]);
        results.push_back(centerWorld + edges[0] - edges[1] + edges[2]); //this +,-,+
        results.push_back(centerWorld - edges[0] + edges[1] + edges[2]); //this -,+,+
        results.push_back(centerWorld + edges[0] + edges[1] + edges[2]);//this +,+,+
        return results;
    }

    // Get Rigid Box Size
    inline glm::vec4 getBoxSize(const glm::mat4& obj2World)
    {
        glm::vec4 size(0, 0, 0, 0);
        glm::vec4 edges[3];
        for (size_t i = 0; i < 3; ++i){
            glm::vec4 vec = glm::vec4(0, 0, 0, 0);
            vec[i] = 0.5f;
            edges[i] = vector3TransformNormal(vec, obj2World);
            size[i] = 2.0f * glm::length(edges[i]);
        }
        return size;
    }

    // Get the Normal to the faces
    inline std::vector<glm::vec4> getAxisNormalToFaces(const glm::mat4& obj2World)
    {
        std::vector<glm::vec4> edges;
        glm::vec4 xaxis = glm::vec4(1, 0, 0, 1);
        glm::vec4 yaxis = glm::vec4(0, 1, 0, 1);
        glm::vec4 zaxis = glm::vec4(0, 0, 1, 1);
        glm::vec4 edge1 = vector3Normalize(vector3TransformNormal(xaxis, obj2World));
        glm::vec4 edge2 = vector3Normalize(vector3TransformNormal(yaxis, obj2World));
        glm::vec4 edge3 = vector3Normalize(vector3TransformNormal(zaxis, obj2World));
        edges.push_back(edge1);
        edges.push_back(edge2);
        edges.push_back(edge3);
        return edges;
    }


    // Get the pair of edges
    inline std::vector<glm::vec4> getPairOfEdges(const glm::mat4& obj2World_A, const glm::mat4& obj2World_B)
    {
        std::vector<glm::vec4> edges1 = getAxisNormalToFaces(obj2World_A);
        std::vector<glm::vec4> edges2 = getAxisNormalToFaces(obj2World_B);

        std::vector<glm::vec4> results;
        for (int i = 0; i < edges1.size(); i++)
        {
            for (int j = 0; j<edges2.size(); j++)
            {
                glm::vec4 vector = glm::vec4(glm::cross(glm::vec3(edges1[i]), glm::vec3(edges2[j])), 0.0f);

                if (glm::length(vector) > 0)
                    results.push_back(vector3Normalize(vector));
            }
        }
        return results;
    }

    // project a shape on an axis
    inline Projection project(const glm::mat4& obj2World, glm::vec4 axis)
    {
        // Get corners
        std::vector<glm::vec4> cornersWorld = getCorners(obj2World);
        float min = glm::dot(cornersWorld[0], axis);
        float max = min;
        for (int i = 1; i < cornersWorld.size(); i++)
        {
            float p = glm::dot(cornersWorld[i], axis);
            if (p < min) {
                min = p;
            }
            else if (p > max) {
                max = p;
            }
        }
        Projection projection;
        projection.max = max;
        projection.min = min;
        return projection;
    }

    inline bool overlap(Projection p1, Projection p2)
    {
        return !((p1.max > p2.max && p1.min > p2.max) || (p2.max > p1.max && p2.min > p1.max));
    }

    inline float getOverlap(Projection p1, Projection p2)
    {
        return std::min(p1.max, p2.max) - std::max(p1.min, p2.min);
    }

    static inline glm::vec3 contactPoint(
            const glm::vec4 &pOne,
            const glm::vec4 &dOne,
            float oneSize,
            const glm::vec4 &pTwo,
            const glm::vec4 &dTwo,
            float twoSize,

            // If this is true, and the contact point is outside
            // the edge (in the case of an edge-face contact) then
            // we use one's midpoint, otherwise we use two's.
            bool useOne)
    {
        glm::vec4 toSt, cOne, cTwo;
        float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
        float denom, mua, mub;

        smOne = vector3LengthSq(dOne)[0];
        smTwo = vector3LengthSq(dTwo)[0];
        dpOneTwo = glm::dot(dTwo, dOne);

        toSt = pOne - pTwo;
        dpStaOne = glm::dot(dOne, toSt);
        dpStaTwo = glm::dot(dTwo, toSt);

        denom = smOne * smTwo - dpOneTwo * dpOneTwo;

        // Zero denominator indicates parrallel lines
        if (abs(denom) < 0.0001f) {
            return useOne ? pOne : pTwo;
        }

        mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
        mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

        // If either of the edges has the nearest point out
        // of bounds, then the edges aren't crossed, we have
        // an edge-face contact. Our point is on the edge, which
        // we know from the useOne parameter.
        if (mua > oneSize ||
            mua < -oneSize ||
            mub > twoSize ||
            mub < -twoSize)
        {
            return useOne ? pOne : pTwo;
        }
        else
        {
            cOne = pOne + dOne * mua;
            cTwo = pTwo + dTwo * mub;

            return cOne * (0.5f) + cTwo * (0.5f);
        }
    }

    inline glm::vec3 handleVertexToface(const glm::mat4& obj2World, const glm::vec4& toCenter)
    {
        std::vector<glm::vec4> corners = getCorners(obj2World);
        float min = 1000;
        glm::vec4 vertex;
        for (int i = 0; i < corners.size(); i++)
        {
            float value = glm::dot(corners[i], toCenter);
            if (value < min)
            {
                vertex = corners[i];
                min = value;
            }
        }

        return vertex;
    }


    inline CollisionInfo checkCollisionSATHelper(const glm::mat4& obj2World_A, const glm::mat4& obj2World_B, glm::vec3 size_A, glm::vec3 size_B)
    {
        CollisionInfo info;
        info.isValid = false;
        glm::vec3 collisionPoint(0, 0, 0);
        float smallOverlap = 10000.0f;
        glm::vec4 axis;
        int index;
        int fromWhere = -1;
        bool bestSingleAxis = false;
        glm::vec4 toCenter = getVectorConnnectingCenters(obj2World_A, obj2World_B);
        std::vector<glm::vec4> axes1 = getAxisNormalToFaces(obj2World_A);
        std::vector<glm::vec4> axes2 = getAxisNormalToFaces(obj2World_B);
        std::vector<glm::vec4> axes3 = getPairOfEdges(obj2World_A, obj2World_B);
        // loop over the axes1
        for (int i = 0; i < axes1.size(); i++) {
            // project both shapes onto the axis
            Projection p1 = project(obj2World_A, axes1[i]);
            Projection p2 = project(obj2World_B, axes1[i]);
            // do the projections overlap?
            if (!overlap(p1, p2)) {
                // then we can guarantee that the shapes do not overlap
                return info;
            }
            else{
                // get the overlap
                float o = getOverlap(p1, p2);
                // check for minimum
                if (o < smallOverlap) {
                    // then set this one as the smallest
                    smallOverlap = o;
                    axis = axes1[i];
                    index = i;
                    fromWhere = 0;
                }
            }
        }
        // loop over the axes2
        for (int i = 0; i < axes2.size(); i++) {
            // project both shapes onto the axis
            Projection p1 = project(obj2World_A, axes2[i]);
            Projection p2 = project(obj2World_B, axes2[i]);
            // do the projections overlap?
            if (!overlap(p1, p2)) {
                // then we can guarantee that the shapes do not overlap
                return  info;
            }
            else{
                // get the overlap
                float o = getOverlap(p1, p2);
                // check for minimum
                if (o < smallOverlap) {
                    // then set this one as the smallest
                    smallOverlap = o;
                    axis = axes2[i];
                    index = i;
                    fromWhere = 1;
                    bestSingleAxis = true;
                }
            }
        }
        int whichEdges = 0;
        // loop over the axes3
        for (int i = 0; i < axes3.size(); i++) {
            // project both shapes onto the axis
            Projection p1 = project(obj2World_A, axes3[i]);
            Projection p2 = project(obj2World_B, axes3[i]);
            // do the projections overlap?
            if (!overlap(p1, p2)) {
                // then we can guarantee that the shapes do not overlap
                return info;
            }
            else{
                // get the overlap
                float o = getOverlap(p1, p2);
                // check for minimum
                if (o < smallOverlap) {
                    // then set this one as the smallest
                    smallOverlap = o;
                    axis = axes3[i];
                    index = i;
                    whichEdges = i;
                    fromWhere = 2;
                }
            }
        }
        // if we get here then we know that every axis had overlap on it
        // so we can guarantee an intersection
        glm::vec4 normal;
        switch (fromWhere){
            case 0:{
                normal = axis;
                if (glm::dot(axis, toCenter) <= 0)
                {
                    normal = normal * -1.0f;
                }
                collisionPoint = handleVertexToface(obj2World_B, toCenter);
            }break;
            case 1:{
                normal = axis;
                if (glm::dot(axis, toCenter) <= 0)
                {
                    normal = normal * -1.0f;
                }
                collisionPoint = handleVertexToface(obj2World_A, toCenter * (-1.0f));
            }break;
            case 2:{
                glm::vec4 axis = vector3Normalize(glm::vec4(glm::cross(glm::vec3(axes1[whichEdges / 3]), glm::vec3(axes2[whichEdges % 3])), 0.0f));
                normal = axis;
                if (glm::dot(axis, toCenter) <= 0)
                {
                    normal = normal * -1.0f;
                }
                glm::vec4 ptOnOneEdge(0.5, 0.5, 0.5, 1);
                glm::vec4 ptOnTwoEdge(0.5, 0.5, 0.5, 1);

                for (int i = 0; i < 3; i++)
                {
                    if (i == whichEdges / 3) {
                        ptOnOneEdge[i] = 0;
                    }
                    else if (glm::dot(axes1[i], normal) < 0) {
                        ptOnOneEdge[i] = -ptOnOneEdge[i];
                    }

                    if (i == whichEdges % 3) {
                        ptOnTwoEdge[i] = 0;
                    }
                    else if (glm::dot(axes2[i], normal) > 0) {
                        ptOnTwoEdge[i] = -ptOnTwoEdge[i];
                    }
                }
                ptOnOneEdge = vector3Transform(ptOnOneEdge, obj2World_A);
                ptOnTwoEdge = vector3Transform(ptOnTwoEdge, obj2World_B);
                collisionPoint = contactPoint(ptOnOneEdge,
                                              axes1[whichEdges / 3],
                                              size_A[whichEdges / 3],
                                              ptOnTwoEdge,
                                              axes2[whichEdges % 3],
                                              size_B[whichEdges % 3],
                                              bestSingleAxis);
            }
            break;
        }

        if (fromWhere != -1) {
            info.isValid = true;
            info.collisionPointWorld = collisionPoint;
            info.depth = smallOverlap;
            info.normalWorld = normal * (-1.0f);
        }
        return info;
    }
}

/* params:
obj2World_A, the transfer matrix from object space of A to the world space
obj2World_B, the transfer matrix from object space of B to the world space
*/
inline CollisionInfo checkCollisionSAT(glm::mat4 obj2World_A, glm::mat4 obj2World_B) {
    using namespace libtennis;
    glm::vec4 calSizeA = getBoxSize(obj2World_A);
    glm::vec4 calSizeB = getBoxSize(obj2World_B);

    return checkCollisionSATHelper(obj2World_A, obj2World_B, calSizeA, calSizeB);
}


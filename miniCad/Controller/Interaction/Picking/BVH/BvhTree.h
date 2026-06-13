//
// Created by ZQD on 2026/6/13.
//

#pragma once

#include "Controller/Interaction/Picking/PickTypes.h"
#include "GeomCalculator.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <optional>
#include <vector>

namespace Picking {
    template<typename Item, typename Result>
    class BvhTree {
    public:
        using ResultBuilder = std::function<std::optional<Result>(const Item &, uint32_t)>;
        using BoundsTolerance = std::function<double(const PickSettings &)>;

        BvhTree(ResultBuilder resultBuilder, BoundsTolerance boundsTolerance)
            : m_ResultBuilder(std::move(resultBuilder)),
              m_BoundsTolerance(std::move(boundsTolerance)) {
        }

        void Build(const std::vector<Item> &items) {
            m_Items = items;
            m_ItemIndices.resize(m_Items.size());
            std::iota(m_ItemIndices.begin(), m_ItemIndices.end(), 0);
            m_Nodes.clear();
            m_Root = InvalidNode;

            if (m_Items.empty()) {
                return;
            }

            m_Root = BuildRecursive(0, static_cast<uint32_t>(m_ItemIndices.size()));
        }

        [[nodiscard]] std::vector<Result> Query(const PickQuery &query) const {
            std::vector<Result> result;
            if (m_Root == InvalidNode) {
                return result;
            }

            QueryRecursive(m_Root, query, result);
            return result;
        }

        [[nodiscard]] bool Empty() const {
            return m_Root == InvalidNode;
        }

    private:
        static constexpr uint32_t MaxLeafItemCount = 8;
        static constexpr int InvalidNode = -1;

        struct Node {
            Bnd_Box bounds;
            int left{InvalidNode};
            int right{InvalidNode};
            uint32_t firstItem{0};
            uint32_t itemCount{0};

            [[nodiscard]] bool IsLeaf() const {
                return itemCount > 0;
            }
        };

        enum class Axis {
            X,
            Y,
            Z
        };

        [[nodiscard]] static gp_XYZ BoxCenter(const Bnd_Box &box) {
            Standard_Real xmin = 0.0;
            Standard_Real ymin = 0.0;
            Standard_Real zmin = 0.0;
            Standard_Real xmax = 0.0;
            Standard_Real ymax = 0.0;
            Standard_Real zmax = 0.0;
            box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
            return {(xmin + xmax) * 0.5, (ymin + ymax) * 0.5, (zmin + zmax) * 0.5};
        }

        [[nodiscard]] static Axis LongestAxis(const Bnd_Box &box) {
            Standard_Real xmin = 0.0;
            Standard_Real ymin = 0.0;
            Standard_Real zmin = 0.0;
            Standard_Real xmax = 0.0;
            Standard_Real ymax = 0.0;
            Standard_Real zmax = 0.0;
            box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

            const double dx = xmax - xmin;
            const double dy = ymax - ymin;
            const double dz = zmax - zmin;
            if (dx >= dy && dx >= dz) {
                return Axis::X;
            }
            if (dy >= dz) {
                return Axis::Y;
            }
            return Axis::Z;
        }

        [[nodiscard]] static double AxisValue(const gp_XYZ &point, Axis axis) {
            switch (axis) {
                case Axis::X:
                    return point.X();
                case Axis::Y:
                    return point.Y();
                case Axis::Z:
                default:
                    return point.Z();
            }
        }

        int BuildRecursive(uint32_t begin, uint32_t end) {
            const uint32_t count = end - begin;
            Node node;
            node.bounds = ComputeBounds(begin, end);

            const int nodeIndex = static_cast<int>(m_Nodes.size());
            m_Nodes.push_back(node);

            if (count <= MaxLeafItemCount) {
                m_Nodes[nodeIndex].firstItem = begin;
                m_Nodes[nodeIndex].itemCount = count;
                return nodeIndex;
            }

            const Axis axis = LongestAxis(node.bounds);
            const uint32_t mid = begin + count / 2;
            std::nth_element(
                m_ItemIndices.begin() + begin,
                m_ItemIndices.begin() + mid,
                m_ItemIndices.begin() + end,
                [this, axis](uint32_t lhs, uint32_t rhs) {
                    return AxisValue(BoxCenter(m_Items[lhs].bounds), axis) <
                           AxisValue(BoxCenter(m_Items[rhs].bounds), axis);
                });

            m_Nodes[nodeIndex].left = BuildRecursive(begin, mid);
            m_Nodes[nodeIndex].right = BuildRecursive(mid, end);
            return nodeIndex;
        }

        void QueryRecursive(int nodeIndex, const PickQuery &query, std::vector<Result> &out) const {
            const auto &node = m_Nodes[nodeIndex];

            Bnd_Box bounds = node.bounds;
            bounds.Enlarge(m_BoundsTolerance(query.settings));

            double tmin = 0.0;
            double tmax = 0.0;
            if (!GeomCalculator::RayIntersectBox(bounds, query.ray, tmin, tmax)) {
                return;
            }

            if (node.IsLeaf()) {
                for (uint32_t index = 0; index < node.itemCount; ++index) {
                    const auto itemIndex = m_ItemIndices[node.firstItem + index];
                    if (auto result = m_ResultBuilder(m_Items[itemIndex], itemIndex)) {
                        out.push_back(*result);
                    }
                }
                return;
            }

            if (node.left != InvalidNode) {
                QueryRecursive(node.left, query, out);
            }
            if (node.right != InvalidNode) {
                QueryRecursive(node.right, query, out);
            }
        }

        [[nodiscard]] Bnd_Box ComputeBounds(uint32_t begin, uint32_t end) const {
            Bnd_Box bounds;
            for (uint32_t index = begin; index < end; ++index) {
                bounds.Add(m_Items[m_ItemIndices[index]].bounds);
            }
            return bounds;
        }

        std::vector<Item> m_Items;
        std::vector<uint32_t> m_ItemIndices;
        std::vector<Node> m_Nodes;
        int m_Root{InvalidNode};
        ResultBuilder m_ResultBuilder;
        BoundsTolerance m_BoundsTolerance;
    };
}

#pragma once

#include <tuple>
#include <utility>
#include <functional>
#include <unordered_map>

namespace halfMesh {
    //
    //  Key types for faces, undirected edges, and directed half-edges
    //
    using FaceKey = std::tuple<unsigned, unsigned, unsigned>;
    using EdgeKey = std::pair<unsigned, unsigned>; // undirected
    using HalfEdgeKey = std::pair<unsigned, unsigned>; // directed

    //
    //  Hash-combine helper (32-bit golden ratio)
    //
    inline void hash_combine(std::size_t &seed, const std::size_t v) {
        seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    //
    //  FaceKey: sort triple then combine hashes
    //
    struct FaceKeyHash {
        std::size_t operator()(FaceKey const &tup) const noexcept {
            auto a = std::get<0>(tup),
                    b = std::get<1>(tup),
                    c = std::get<2>(tup);
            // sort
            if (a > b) std::swap(a, b);
            if (b > c) std::swap(b, c);
            if (a > b) std::swap(a, b);
            // combine
            std::size_t h = 0;
            hash_combine(h, std::hash<unsigned>{}(a));
            hash_combine(h, std::hash<unsigned>{}(b));
            hash_combine(h, std::hash<unsigned>{}(c));
            return h;
        }
    };

    struct FaceKeyEqual {
        bool operator()(FaceKey const &x, FaceKey const &y) const noexcept {
            unsigned ax = std::get<0>(x), bx = std::get<1>(x), cx = std::get<2>(x);
            unsigned ay = std::get<0>(y), by = std::get<1>(y), cy = std::get<2>(y);
            // sort both
            if (ax > bx) std::swap(ax, bx);
            if (bx > cx) std::swap(bx, cx);
            if (ax > bx) std::swap(ax, bx);
            if (ay > by) std::swap(ay, by);
            if (by > cy) std::swap(by, cy);
            if (ay > by) std::swap(ay, by);
            return ax == ay && bx == by && cx == cy;
        }
    };

    //
    //  EdgeKey: undirected pair, so canonicalize by sorting
    //
    struct EdgeKeyHash {
        std::size_t operator()(EdgeKey const &pr) const noexcept {
            unsigned i = pr.first, j = pr.second;
            if (i > j) std::swap(i, j);
            std::size_t h = std::hash<unsigned>{}(i);
            hash_combine(h, std::hash<unsigned>{}(j));
            return h;
        }
    };

    struct EdgeKeyEqual {
        bool operator()(EdgeKey const &a, EdgeKey const &b) const noexcept {
            return (a.first == b.first && a.second == b.second)
                   || (a.first == b.second && a.second == b.first);
        }
    };

    //
    //  HalfEdgeKey: directed pair, no canonicalization
    //
    struct HalfEdgeKeyHash {
        std::size_t operator()(HalfEdgeKey const &k) const noexcept {
            // combine first and second with golden-ratio mix
            std::size_t h = std::hash<unsigned>{}(k.first);
            hash_combine(h, std::hash<unsigned>{}(k.second));
            return h;
        }
    };

    struct HalfEdgeKeyEqual {
        bool operator()(HalfEdgeKey const &a, HalfEdgeKey const &b) const noexcept {
            return a.first == b.first && a.second == b.second;
        }
    };

    //
    //  Handle maps for fast lookup / de-duplication
    //
    using FaceHandleMap = std::unordered_map<FaceKey, unsigned, FaceKeyHash, FaceKeyEqual>;
    using EdgeHandleMap = std::unordered_map<EdgeKey, unsigned, EdgeKeyHash, EdgeKeyEqual>;
}

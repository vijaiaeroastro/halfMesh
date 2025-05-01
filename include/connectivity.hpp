#pragma once

#include <tuple>
#include <utility>
#include <functional>
#include <unordered_map>

namespace HalfMesh {

  using FaceKey = std::tuple<unsigned, unsigned, unsigned>;
  using EdgeKey = std::pair<unsigned, unsigned>;

  // A tiny hash_combine you can sprinkle anywhere:
  inline void hash_combine(std::size_t& seed, std::size_t v) {
      // 0x9e3779b9 is the 32-bit fractional part of the golden ratio
      seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  struct FaceKeyHash {
    std::size_t operator()(FaceKey const& tup) const noexcept {
      // 1) sort the triple so that (1,2,3),(2,3,1),… all canonicalize to (1,2,3)
      unsigned a = std::get<0>(tup),
               b = std::get<1>(tup),
               c = std::get<2>(tup);
      if (a > b) std::swap(a,b);
      if (b > c) std::swap(b,c);
      if (a > b) std::swap(a,b);

      // 2) mix them together
      std::size_t h = 0;
      hash_combine(h, std::hash<unsigned>{}(a));
      hash_combine(h, std::hash<unsigned>{}(b));
      hash_combine(h, std::hash<unsigned>{}(c));
      return h;
    }
  };
  struct FaceKeyEqual {
    bool operator()(FaceKey const& x, FaceKey const& y) const noexcept {
      // must compare *after* the same sort logic
      auto sx = x, sy = y;
      unsigned ax = std::get<0>(sx), bx = std::get<1>(sx), cx = std::get<2>(sx);
      unsigned ay = std::get<0>(sy), by = std::get<1>(sy), cy = std::get<2>(sy);
      if (ax>bx) std::swap(ax,bx);
      if (bx>cx) std::swap(bx,cx);
      if (ax>bx) std::swap(ax,bx);
      if (ay>by) std::swap(ay,by);
      if (by>cy) std::swap(by,cy);
      if (ay>by) std::swap(ay,by);
      return ax==ay && bx==by && cx==cy;
    }
  };

  struct EdgeKeyHash {
    std::size_t operator()(EdgeKey const& pr) const noexcept {
      // sort pair so (i,j) and (j,i) collide
      unsigned i = pr.first, j = pr.second;
      if (i > j) std::swap(i,j);

      std::size_t h = std::hash<unsigned>{}(i);
      hash_combine(h, std::hash<unsigned>{}(j));
      return h;
    }
  };
  struct EdgeKeyEqual {
    bool operator()(EdgeKey const& a, EdgeKey const& b) const noexcept {
      return (a.first==b.first && a.second==b.second)
          || (a.first==b.second && a.second==b.first);
    }
  };

  using FaceHandleMap = std::unordered_map<FaceKey, unsigned, FaceKeyHash, FaceKeyEqual>;
  using EdgeHandleMap = std::unordered_map<EdgeKey, unsigned, EdgeKeyHash, EdgeKeyEqual>;

} // namespace HalfMesh
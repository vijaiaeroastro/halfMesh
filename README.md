# halfMesh

A simple and flexible half‐edge data structure for triangular meshes, with built‐in support for per‐entity properties and multiple I/O formats.

## Features

- **Half‐Edge Representation** for efficient adjacency and traversal
- **JSON/BSON‐Backed Property Store** for vertices, edges, and faces
- **Custom `.bm` Format** (BSON) for full mesh+property serialization
- **STL Support**: Read & write both ASCII and binary STL
- **Export**: OBJ, GMSH (v2), VTK

## Input / Output Capabilities

| Format                        | Read | Write | Notes                                                      |
|-------------------------------|:----:|:-----:|------------------------------------------------------------|
| **OBJ** (Triangles only)      |  No  |  Yes  | Export only; OBJ reading has been removed                  |
| **GMSH** (v2, Triangles only) |  Yes |  Yes  | Version 2, triangular elements only                       |
| **STL** (ASCII & Binary)      |  Yes |  Yes  | Supports both ASCII and binary STL                         |
| **VTK** (Triangles only)      |  No  |  Yes  | Export only, triangular faces                              |
| **BM** (BSON)                 |  Yes |  Yes  | Custom mesh+properties format                             |

## Dependencies

- C++17 (or later)
- [nlohmann/json](https://github.com/nlohmann/json) for JSON/BSON serialization and property storage

## Usage

```cpp
#include "mesh.hpp"

int main() {
    HalfMesh::Mesh mesh;

    // Add vertices
    auto v1 = mesh.add_vertex(0.0, 0.0, 0.0);
    auto v2 = mesh.add_vertex(1.0, 0.0, 0.0);
    auto v3 = mesh.add_vertex(0.0, 1.0, 0.0);

    // Build one triangle and finalize connectivity
    mesh.add_face(v1, v2, v3);
    mesh.complete_mesh();

    // Export to various formats
    mesh.save("mesh.stl");

    return 0;
}
```

# Dependencies
C++17

# License
MIT

# Notes
* Designed to be lightweight and STL‐only; future enhancements may add optional dependencies.
* Contributions welcome! Feel free to open issues or pull requests.e than happy to add it here in future. Just contact over email.
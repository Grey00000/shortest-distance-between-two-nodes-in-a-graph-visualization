# graphviz — Graph Visualizer with Dijkstra Shortest Path

A terminal program that draws a weighted graph, lets you pick two nodes, and
highlights the shortest path between them using Dijkstra's algorithm — with an
optional step-by-step animation of the search.

```
Start node: A
End node:   D
Animate? [y/N] n

Shortest path: A -> B -> D   cost 6
```

At rest the graph is drawn low-contrast (edges and costs in faint grey), so once
you run a query the shortest path is the only bright thing on screen.

## Build & run

No external dependencies — just a C++17 compiler and `make`.

```bash
make            # builds ./graphviz
make test       # builds and runs the test suite
make clean      # removes binaries and object files

./graphviz                      # loads the bundled graphs/sample.txt
./graphviz mygraph.txt          # loads your own graph file
./graphviz --no-color           # plain ASCII, no ANSI escapes
./graphviz --no-animate         # skip the per-query "Animate?" prompt
```

Compiler flags are `-std=c++17 -Wall -Wextra -Werror -O2` (warnings are errors).

## How to use it

The program draws the graph once, then loops:

1. Enter a **start node** label (e.g. `A`).
2. Enter an **end node** label (e.g. `D`).
3. If animation is enabled, answer **`Animate?`** — `y` steps through Dijkstra's
   search frame by frame; anything else jumps straight to the result.

It then redraws the graph with the shortest path highlighted and prints the
caption. Press **Ctrl-D** at any prompt to exit cleanly.

Query mistakes never crash the loop: a bad label re-prompts (`No node named 'Z'.
Available: A, B, C, D`), the same node twice is a valid cost-0 query, and an
unreachable target is reported in the caption rather than as an error.

## Graph file format

One directive per line. `#` begins a comment; blank lines are ignored.

```
# optional node declaration (sets label and draw order)
N A
N B

# edges: from  to  weight
E A B 4
E A C 8
E B D 2
E C D 3
```

- **`N` lines are optional.** Nodes are created on first mention in an `E` line,
  in encounter order. Use `N` to declare an isolated node or to pin the draw
  order (which sets each node's angle on the circle).
- **Edges are undirected.** `E A B 4` allows travel both ways at cost 4.
- **Weights are non-negative integers.** Negative weights are rejected at parse
  time; zero is allowed.
- **Duplicate edge** between a pair keeps the smaller weight and warns.
  **Self-loop** (`E A A`) is rejected.

Startup errors (bad file) print a line-numbered message and exit non-zero:

```
mygraph.txt:7: negative weight -3 (Dijkstra requires non-negative weights)
```

## Colour / glyph key

With colour (the default), one function maps each drawing style to an ANSI code:

| Meaning | Colour | When |
|---|---|---|
| Edges & cost labels at rest | faint grey | always, until highlighted |
| Node circles | plain | always |
| Shortest path & its costs | bright yellow | after a query |
| Settled node | cyan | during animation |
| Frontier node (in the queue) | magenta | during animation |

With `--no-color`, styles are shown by glyph instead: path edges draw as `#`
against `.` for other edges, and the two endpoints keep `[A]` brackets against
`(A)` for other nodes. (Animation colours aren't distinguishable in this mode —
it exists for tests and pipes, not for watching.)

## Architecture

Seven modules plus `main`, in a strict one-way dependency chain. Nothing points
backwards — the algorithm never knows a terminal exists. Arrows read "is used by":

```
graph_io ─┐
          ├─► graph ─┬─► dijkstra ─┐
          │          └─► layout ───┼─► scene ─► renderer ─► main
bresenham ┴──────────────────────────────────┘
```

| Module | Header | Responsibility | Depends on |
|---|---|---|---|
| `graph` | `graph.h` | `Graph` type: labels, weighted adjacency list, label↔index lookup | — |
| `graph_io` | `graph_io.h` | Parse edge-list file into a `Graph`; line-numbered errors | `graph` |
| `dijkstra` | `dijkstra.h` | Shortest path plus a recorded step trace (for animation) | `graph` |
| `layout` | `layout.h` | Assign each node an (x, y) canvas cell (circular layout) | `graph` |
| `scene` | `scene.h` | Plain data: positioned nodes/edges tagged with a `Style` | `graph` |
| `bresenham` | `bresenham.h` | Rasterize a line between two cells | — |
| `renderer` | `renderer.h` | Abstract sink; `TerminalRenderer` emits ANSI | `scene`, `bresenham` |
| `main` | `main.cpp` | Wiring, CLI parsing, and the interactive query loop | all |

**The pivot is `Scene`** — a behaviourless struct. Everything above it is pure
computation and unit-testable without a terminal; everything below is pure
drawing. `main` computes a `Scene` (nodes/edges tagged with styles) and hands it
to the renderer. Adding an SFML renderer later means writing one new file that
consumes `Scene`, with no changes to `graph`, `graph_io`, `dijkstra`, or `layout`.

## Layout on disk

```
Makefile
src/      graph.{h,cpp}  graph_io.{h,cpp}  dijkstra.{h,cpp}  layout.{h,cpp}
          scene.h  bresenham.{h,cpp}  renderer.{h,cpp}  main.cpp
tests/    test_main.cpp  test_graph_io.cpp  test_dijkstra.cpp
          test_layout.cpp  test_bresenham.cpp  test_renderer.cpp  test_harness.h
graphs/   sample.txt
docs/     superpowers/specs/…-design.md   (full design rationale)
          superpowers/plans/…-plan.md     (task-by-task build plan)
```


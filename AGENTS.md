# AGENTS.md

Operational guide for LLM coding tools working in this repository. Read this first.

If a rule conflicts with a task, stop and surface it. Do not silently work around the rules.

## Acknowledgment

To confirm that you have read necessary files and will respect discovered rules for the duration of the task, print the following in the beggining of each session:

> 🤝 Following project rules.

If you cannot honor a rule for any reason, surface that explicitly instead of printing the acknowledgment.

---

## Doc map

Read these before touching code in the corresponding area.

| If you're working on…                                          | Read first                                        |
| -------------------------------------------------------------- | ------------------------------------------------- |
| Anything                                                       | `README.md`                                       |

---

## Coding rules

Authoritative coding rules for current project. Apply to all contributors — human and LLM.

1. C++20 baseline
2. **Naming.** `lowerCamelCase` for variables and functions, `PascalCase` for types, `SCREAMING_SNAKE_CASE` for constants, `PascalCase` for filenames matching the primary type

---

## Implementation approach

- Treat each task as an incremental change to the whole system, not as an isolated patch. Inspect the surrounding code, documented architecture, ownership model and likely extension points before choosing an interface or class design.
- Prefer the smallest coherent design over the smallest diff. Keep the implementation limited to the task, but do not sacrifice clean boundaries, accurate abstractions or future compatibility to finish it quickly.
- Prevent duplication. Search for existing implementations and shared concepts before adding code. When functionality is inaccessible or poorly placed, make a focused refactoring that creates an appropriate shared abstraction instead of copying it.
- Follow modern C++ best practices: use RAII, explicit ownership, value semantics, strong types and standard-library facilities. Avoid owning raw pointers and manual resource cleanup.
- Encapsulate C libraries and other low-level APIs behind narrow C++ abstractions. Resource ownership alone is not sufficient: the abstraction should also enforce valid lifecycle transitions, preserve invariants and keep implementation details from leaking into callers.
- Match documented interfaces and responsibilities by behavior, not merely by class name or file location. Do not introduce a type whose API conflicts with the architectural role its name implies.
- Define error and partial-success semantics deliberately. Avoid ad hoc combinations of status flags, strings and side-channel error collections when a clearer result model is possible.
- Before implementation, identify existing reusable code, resource ownership, object lifecycle and error propagation. After implementation, review the change for duplication, unnecessary repeated work and architectural drift.
- Do not over-engineer speculative features. Broader design awareness should improve the current change without introducing unrelated frameworks or abstractions.

---

## Formatting (end of task)

After all code changes are complete and verified (build/tests pass), run **clang-format** on every C/C++ source and header file you modified in the task. Do this as the **last** step before declaring the work done — not at the start, and not in the middle of edits you still intend to change.

- Use the repo config: `.clang-format` at the repository root.
- Format only files you touched (e.g. `clang-format -i path/to/file.cpp`). Do not reformat unrelated files.
- If `clang-format` is not installed, say so explicitly instead of skipping silently.

---

## When in doubt

- Ask before inventing. Don't guess function signatures, file paths, library APIs, or build invocations. If something isn't in the docs or the code, it doesn't exist yet.
- If two docs disagree, surface the conflict instead of guessing which one is right.
- If a rule conflicts with the task, stop. Don't silently work around the rule.



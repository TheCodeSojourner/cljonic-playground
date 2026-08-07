🎯 `doc/` = committed source; `docs/` = committed generated HTML for GitHub Pages.

- `doc/Doxyfile` — `INPUT = ../src`, `OUTPUT_DIRECTORY = ../docs`, `HTML_OUTPUT = .`
- `doc/doc-logo.png` — project logo
- `make docs` runs: `rm -rf docs/ && cd doc && doxygen Doxyfile`; clears first to prevent stale pages
- `docs/index.html` is the GitHub Pages entry point when serving from the `docs/` folder
- Doxygen target is standalone; NOT wired into `upsert-gate-strict` (too slow for tight loop)
- To switch to `gh-pages` branch CI: change `HTML_OUTPUT = html` and gitignore `docs/`

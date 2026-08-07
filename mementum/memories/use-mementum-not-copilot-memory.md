🎯 All gybis-relevant memories and knowledge belong in the mementum system (`mementum/memories/` and `mementum/knowledge/`), not the Copilot memory system (`/memories/repo/`).

- mementum memories: `mementum/memories/{slug}.md` | <200 words | one insight per file | git-committed
- mementum knowledge: `mementum/knowledge/{topic}.md` | frontmatter required | for larger/structured content
- Copilot `/memories/repo/` is NOT the gybis memory store; do not use it for repo-specific insights
- General (non-repo-specific) knowledge goes in Copilot user memory (`/memories/`)
- After storing, always `git add mementum/` and commit with the appropriate symbol prefix

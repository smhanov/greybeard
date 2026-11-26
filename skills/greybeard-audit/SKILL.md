---
name: greybeard-audit
description: >
  Audit a whole repo for cost: rank the O(n^2) loops, the hot-path allocations,
  the N+1 queries, the unbounded caches, and the unindexed lookups, most
  expensive first, with the fix and the number. Use for "audit this for
  performance", "/greybeard-audit", or "why is this slow".
argument-hint: "[path|module]"
license: MIT
---

# Greybeard audit

The whole repo, ranked. Fix this first.

## Map before you rank

1. **Entry points.** Where does work enter? HTTP handlers, CLI commands, cron
   jobs, message consumers, the main loop.
2. **Data volume.** For each entry point, what `n` arrives — rows, bytes,
   requests per second. Read the schema, the fixtures, the config. If the repo
   cannot tell you, say the number is unknown and rank by worst case.
3. **The hot path.** Follow one request end to end and note every loop, query,
   allocation, and copy on it. That trace is the audit's spine.

## Rank, do not list

Findings go in one table, ordered by expected cost at production `n`:

| # | Where | What | Cost at n | Fix | Verdict |
|---|-------|------|-----------|-----|---------|
| 1 | `route.py:88` | SQL query inside loop over 5k rows | 5k round trips, ~40 s | one `IN` query | must fix |
| 2 | `load.py:31` | whole file into memory to read a header | 2 GB RSS | seek + read 512 B | must fix |

Verdicts: **must fix** (wrong complexity or unbounded), **should fix** (constant
factor that matters), **nice to have** (worth it only if touched anyway).

## Rules

- One finding per line. No prose padding, no "consider also".
- Every finding carries the number: at what `n`, at what cost.
- Prefer structural fixes. A cache in front of an O(n^2) loop is a slower O(n^2).
- Check the tests for the real sizes the code meets. A "slow" function called
  once at startup is not a finding.
- End with the single first thing to fix, in one sentence.

## Do not

- Do not rewrite the repo. The audit is a report.
- Do not report style, naming, or architecture unless it changes the cost.
- Do not claim a fix is faster without measuring it or stating it is an estimate.

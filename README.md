<img src="assets/logo.svg" alt="Greybeard" width="660">

# Greybeard

**He doesn't optimize. He measures.**

An agent skill that makes coding agents write efficient code the way a 1990s
systems programmer would: the right algorithm, the right representation, and a
number to prove it.

`MIT` · `no dependencies` · `any agent that reads a skill` · `works on Python, C, SQL, anything`

---

## The problem

You ask an agent for a feature. It gives you:

- a nested loop over the same collection, `O(n²)` on 5,000 rows
- a query per row, 5,000 round trips
- a 400 KB dependency to pad a string
- an unbounded cache keyed by user input
- and then a "performance optimization" pass that micro-tunes a function called
  once at startup

Greybeard is the opposite reflex. It will not write a line until it knows `n`.
It will fix the complexity before the constant factor, and the representation
before the loop. And it ends every reply with the number.

## The ladder

Bentley's *Programming Pearls* is fifteen columns, and only one of them is about
clever tricks — the ninth, which opens by telling you not to. Greybeard is built
on that ordering:

| Rung | Question | Where the wins are |
|---|---|---|
| **0** | Is it already fast enough? | Ten million rows in 20 ms is not a problem, and "fast" is not a goal |
| **1** | Is the *requirement* the cost? | Ten million numbers sorted in 1 MB is a bitmap, not a faster sort |
| **2** | Is the algorithm right? | All the 100x. Hash, binary search, heap, DP, prefix sums |
| **3** | Is the representation right? | Bits over objects, structure-of-arrays, sorted once, implicit indices |
| **4** | Is there an invariant to exploit? | Monotonic → binary search. Sorted → merge. One spare slot → a sentinel |
| **5** | Only now: tune the hot constants | After a profile says it's hot, never before |
| **6** | Never tune by feel | Unmeasured micro-optimization is a readability cost with an unknown return |

## The numbers

Every claim in this repo is measured by a program in [`bench/`](bench), on this
machine, checking that the fast version and the slow version produce the same
answer. Full output: [`bench/results/`](bench/results).

| Benchmark | Naive | Tuned | Ratio |
|---|---|---|---|
| `prefix.c` — max sum subarray | 414 ms | 0.04 ms | **11,382x** |
| `rotate.c` — rotate by k | 72 ms | 0.08 ms | **960x** |
| `bitsort.c` — sort 10M ints | 1,507 ms, 38 MB | 31 ms, **1.19 MB** | **48x** |
| `bits.c` — popcount | 283 ms | 21 ms | **13x** |
| `layout.c` — sum one field | 12.7 ms, 153 MB touched | 3.5 ms, 38 MB | **3.6x** |
| `sentinel.c` — search, `-O0` | 216 ms | 156 ms | **1.4x** |
| `sentinel.c` — search, `-O1` | 49 ms | 96 ms | **0.5x** |
| `bits.c` — Kernighan `x & (x-1)` | 283 ms | 277 ms | **1.0x** |

The last three rows are the honest ones, and they are why this project exists
rather than a list of tricks:

- The `x & (x-1)` trick from every bit-twiddling page since 1983 is a **wash**
  on a modern CPU. The loop is latency-bound and the trick has a longer
  dependency chain; meanwhile `popcnt` is one instruction.
- The sentinel trick pays **1.4x at `-O0`**, costs **2x at `-O1`**, and does
  **nothing at `-O2`**, where the compiler hoists the bound test for you.

A 1990s constant-factor trick is not portable across compilers or
microarchitectures. Bentley measured on a VAX, which charged per instruction.
Your laptop doesn't, and your optimizer has opinions. So:

> Spend the cleverness at rung 2 and rung 3. At rung 5, measure — the answer is
> often "no change", and occasionally "twice as slow".

## The receipt

Every reply ends with three lines. No receipt, no reply.

```
n=10^7  budget 200ms
cost:    O(n), 1 pass, 1.25 MB bitset, 48 ms (was 940 ms qsort)
skipped: no SIMD, no arenas - 4x headroom already
risk:    ids must be < 10^9; falls back to merge above that
```

What it costs, what you deliberately did **not** do, and the limit a future
reader has to know.

## Two rules that keep it shippable

**Cleverness must be legible.** A trick that needs decoding is a landmine, and
landmines are paged at 3 a.m. Every non-obvious line carries one comment naming
the invariant:

```c
x & (x - 1)                  // clears the lowest set bit
mid = lo + (hi - lo) / 2     // no overflow; invariant lo <= mid < hi
rev(rev(head) + rev(tail))   // rotation in O(1) space: three flips
```

**1990s efficiency, not 1990s bugs.** Restore the discipline, not the defects.
Bounds checks at trust boundaries, integer overflow, UTF-8 correctness (one byte
is not one character), endianness, error paths, and the test — all of it stays.
The C of that era is where a generation of CVEs was born.

## Install

```bash
git clone https://github.com/smhanov/greybeard.git
```

Then, depending on your agent — see [INSTALL.md](INSTALL.md) for the details:

| Agent | Do this |
|---|---|
| **Hermes Agent** | `cp -r greybeard/skills/* ~/.hermes/skills/` |
| **Claude Code** | `cp -r greybeard/skills/* ~/.claude/skills/` |
| **Codex / Cursor / Gemini / anything with a rules file** | paste the block from [INSTALL.md](INSTALL.md) into `AGENTS.md` |

## Turning it on

Say any of these, and it stays on for the session:

```
greybeard          make it fast       1990 mode
tighten this       back of the envelope     profile it
```

Levels, when your agent supports arguments (`/greybeard lite|full|ultra|off`),
or just say them:

| Level | Behavior |
|---|---|
| **lite** | Measure and report. Don't rewrite working code. |
| **full** | Budget → ladder → receipt. Default. |
| **ultra** | Also: no dependencies, no allocation in hot paths, fixed buffers, hand-rolled loops. Question any requirement whose cost isn't justified. |

Turn off with **"stop greybeard"** or **"normal mode"**.

Three skills ship:

| Skill | Use |
|---|---|
| [`skills/greybeard`](skills/greybeard/SKILL.md) | The mode: write it efficient, with a receipt |
| [`skills/greybeard-review`](skills/greybeard-review/SKILL.md) | Review a diff for O(n²), N+1, unbounded growth, hot-path allocation |
| [`skills/greybeard-audit`](skills/greybeard-audit/SKILL.md) | Rank a whole repo's costs, most expensive first |

## Why "Greybeard"

Ponytail is the laziest senior dev in the room. Greybeard is the other one: the
fastidious old systems programmer with the cardigan, the copy of K&R with the
spine gone, and a bignum library he wrote in a weekend in 1989 that still runs,
unmodified, on the machine in the basement.

You show him your 4,000-line React app. He asks how many rows. You say forty.
He closes the laptop.

## Provenance

After **Jon Bentley, _Programming Pearls_** (1986; 2nd ed. 2000) — the bitmap
sort, the rotation by reversal, the back of the envelope, the sentinel, the
code-tuning chapter, and the discipline of measuring before believing. The
benchmarks in `bench/` are re-implementations of those problems, re-measured on
hardware Bentley never saw. Several of them now disagree with the book. That is
the book's own method working.

Not affiliated with or endorsed by Bentley, Addison-Wesley, or anyone's beard.

## License

MIT.

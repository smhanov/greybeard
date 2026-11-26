# Install

Greybeard is three markdown skills and a benchmark suite. No dependencies, no
install script, nothing to compile unless you want to run the benchmarks.

```bash
git clone https://github.com/smhanov/greybeard.git
cd greybeard
```

---

## Hermes Agent

```bash
cp -r skills/* ~/.hermes/skills/
```

The skills land as `~/.hermes/skills/greybeard`, `greybeard-review`, and
`greybeard-audit`. Verify:

```bash
ls ~/.hermes/skills/greybeard/SKILL.md
```

They are picked up on the next message — no restart. Trigger with "greybeard"
or `/greybeard`.

## Claude Code

```bash
cp -r skills/* ~/.claude/skills/          # user-level, all projects
# or
cp -r skills/* .claude/skills/            # project-level, this repo only
```

Then `/greybeard`, `/greybeard-review`, `/greybeard-audit`.

## Codex, Cursor, Gemini CLI, Aider, anything with a rules file

These agents read a project-level instruction file instead of installing
skills. Paste the block below into your `AGENTS.md` (`CLAUDE.md`, `.cursor/rules`,
or equivalent). It is the same ladder, compressed to fit.

````markdown
## Efficiency mode: Greybeard

You are a fastidious old systems programmer. You measured before
microprocessors were cheap, and you never stopped.

Before writing code, state the budget in one line: n, bytes, deadline.
No number, no code.

Then stop at the first rung that holds:

0. Is it already fast enough? If it fits with 10x headroom, write the boring
   code and say what you skipped. Being slow is a bug; being fast is not a goal.
1. Is the requirement the cost? Change the problem's shape before its code.
2. Right algorithm. Asymptotics. All the 100x wins live here.
3. Right representation. Bits over objects, struct-of-arrays for scans,
   sorted once and searched many times, implicit indices.
4. Exploit the invariant. Sorted gets binary search. One spare slot gets a
   sentinel. Same prefix every iteration gets rolled forward.
5. Measure, then tune constants. Only the measured hot path.
6. Never tune by feel.

Every non-obvious line carries one comment naming the invariant it uses.
If you cannot state the invariant in one line, you have not earned the trick.

Never cut: bounds checks at trust boundaries, integer overflow, UTF-8
correctness, endianness, error paths, the test. 1990s efficiency, not 1990s bugs.

Verify the awkward inputs: 0, 1, all-equal, max, wrap, empty, unaligned.
If you claim a speedup, measure it before and after. An unmeasured speedup
claim is a lie with a decimal point.

End every reply with the receipt:

    n=<size>  budget <deadline>
    cost:    <complexity>, <passes>, <memory>, <time> (was <time>)
    skipped: <what you deliberately did not do>
    risk:    <the limit a future reader must know>

Turn off with "stop greybeard" or "normal mode".
````

## Per-project, per-session

Rules files apply to a whole repo. If you only want Greybeard on one task,
don't install anything — paste the block into the conversation, or just say:

> Greybeard mode: state the budget, use the ladder, give me the receipt.

The skill content is the whole mechanism. There is no state to configure.

---

## Run the benchmarks

```bash
cd bench
make            # builds with cc -O2
make bench      # runs all six and verifies each answer
make clean
```

Needs `cc` and `make`. Nothing else. Total runtime is about 4 seconds, mostly
the ten-million-integer qsort.

To compare against your own machine, publish a results file:

```bash
cd bench && make bench > results/$(date +%F)-$(uname -m).md
```

Open a PR with it. A results file that disagrees with ours is welcome — it is
the point of the project.

## Uninstall

```bash
rm -rf ~/.hermes/skills/greybeard ~/.hermes/skills/greybeard-review ~/.hermes/skills/greybeard-audit
```

(or delete the same three directories from `~/.claude/skills/`), and remove the
block from your rules file.

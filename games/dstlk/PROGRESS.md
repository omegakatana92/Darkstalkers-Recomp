# Darkstalkers: progress

What is known of *Darkstalkers: The Night Warriors* (Euro 940705)'s memory, and what has been built on
it. Every address here was confirmed by experiment on the running board with the toolkit's own tools
(`docs/FEATURES.md`); several were first reached for from FinalBurn Neo's cheat collection for this same
ROM set, which is said where it is so, and then each was checked on the board itself. Nothing here is a
dump of the game's data, and no game data is written down.

## The ROM set

The set is MAME's **`dstlk`** (Euro 940705), the parent of `dstlku`, `dstlka`, `dstlkh`, `dstlkj` and the
rest. It was told apart from them by the board's own boot warning, which names the revision on the first
screen, and by the set's files against MAME's ROM definition for `dstlk` (Capcom, 1994, board `cps2_2p6b`,
2 players, 6 buttons):

- the program is eight 512 KiB ROMs — `vame.03a`, `vame.04a` … `vame.10a` — 4 MiB in all, so
  `code_end = 400000`;
- the graphics are four 4 MiB ROMs — `vam.13m`, `vam.15m`, `vam.17m`, `vam.19m` — and four 1 MiB ones —
  `vam.14m`, `vam.16m`, `vam.18m`, `vam.20m`;
- the sound program is `vam.01`, `vam.02`; the QSound samples are `vam.11m` and `vam.12m`;
- the key is `dstlk.key`.

Every file is checked by CRC-32 (`games/dstlk/game.cfg`), so only this revision is accepted.

## How the game's own memory is found

The tools used, in the order they are reached for (all in `docs/FEATURES.md`):

- `--movie FILE` replays a hand-written input movie, and `--poke ADDR=BYTE` holds a byte at a guess; the
  two together are how the game is driven to a known place and a thought tried.
- `--dump-ram FRAME:FILE` writes the 64 KiB of work RAM after a frame; `--trace LOW-HIGH:FILE` appends a
  stretch of work RAM after every frame; `--shot-every N:PFX` writes pictures of the board.
- `cps2recomp/tools/ramfind.py` lists the addresses that held a known value in each of several dumps;
  `cps2recomp/tools/tracechanges.py` prints a trace where a value changes.

**Getting to a match.** The board boots into an attract show and then a demo fight, and the demo keeps no
player records at all — its fight is not the game's own one — so it cannot be used to check a cheat. A
research movie instead holds two credits (`FF81D0`) and presses Start on the title and again in the
character select; a real, player-controlled match then starts at about frame **2426**, with the clock
ticking a step every 37 frames. A second movie punches from inside that match, which is what the health
cheats are checked over.

**The clock.** A `--trace` of a stretch of work RAM over a match was searched for a byte counting down a
step at a time; `FF9409` answered, a BCD byte (`0x99` is "99"). Holding it at `0x99` with `--poke` stood
the digits still, so the round never ran out; the digits then read **98**, because the game takes its own
step and draws the result in the same frame, and the write lands after both. Holding it one higher,
`0x9A`, makes the digits read 99, but the game treats anything above `0x99` as the round's time being up
and ends the round within a few frames (a trace of the health showed it at 0 at frame 2806 with `0x9A`
held). `0x99` is therefore what the feature holds. The byte before it, `FF9408`, reads `0x01` through a
round; it is not the clock's.

**The player records.** Dumping work RAM mid-round and outside a round showed a 0x400-byte record at
`FF8380` (player 1) and `FF8780` (player 2) whose bytes fall as a player is hit and read 0 outside a
round. FinalBurn Neo puts this game's health at `FF83CB`/`FF87CB`, which is `+0x4B` in each record: a
`--trace` of those bytes over the punch movie showed player 1's falling 0x90 → 0x7A → 0x63 as hits landed,
and a `--poke` of player 1's to 0 emptied his bar at once.

**The health the bar is drawn from** is the same value twice more: at `+0x4D` (`FF83CD`/`FF87CD`), which
changes exactly one frame after `+0x4B` with the same value, and at `+0x1CB` (`FF854B`/`FF894B`), which
drains a step at a time over the frames after a hit — the bar being drawn down. Both are recorded because a
cheat that puts only `+0x4B` back is not enough: this game takes the damage off the health as the hit lands
and starts the round's end in the same frame if none is left, so the cheats restore the health **before**
each frame, not after it (see `src/cheats.cpp`).

**The Special gauge** is the byte at `+0x1DF` (`FF855F`/`FF895F`): `0x8E` with the bar full, as FinalBurn
Neo's collection has it, and holding it there with `--poke` keeps both bars lit.

**A player's place across the stage** is the word at `+0x18` (`FF8398`/`FF8798`): over a movie that walks
the character right it ramps up, and larger is further right.

**Which way a player faces** is taken from those two places rather than from a flag byte of its own: the
player whose place is the larger faces left. Outside a round both records hold 0, so neither is taken to
face anywhere.

**The credits** are a byte at `FF81D0`: holding it at 2 with `--poke` changes the title to "CREDITS 2" and
"PRESS UP TO 2 START BUTTONS", and a movie that presses Start with **no** coin at all reaches a match once
free play holds it. FinalBurn Neo names the same address.

**The character** is the byte at `+0x3A9` (`FF8729`), 1–12 in the select screen's order; it is recorded
for a readout rather than used by a feature yet.

## The memory so far

| Address | Size | What | How it was found |
| --- | --- | --- | --- |
| `FF81D0` | byte | **Credits**: what the title reads ("CREDITS n") before it offers "PRESS UP TO 2 START BUTTONS" | Held at 2 with `--poke`: the title changed and a movie with no coin reached a match. Also in FinalBurn Neo's collection. |
| `FF9409` | byte (BCD) | **The round's clock**: `0x99` down to `0x00`, a step every 37 frames; 0 outside a round | `--trace`d work RAM over a match for a byte counting down; holding it at `0x99` froze the digits, holding it at `0x9A` ended the round. |
| `FF8380` (+`FF8780`) | record | **Player 1 (player 2)'s record**, the two 0x400 apart | Dumps mid-round and outside one: a 0x400-byte record whose bytes fall on a hit and read 0 outside a round. |
| `+0x18` (`FF8398`/`FF8798`) | word | **Player 1 (player 2)'s place across the stage**: larger is further right | `--trace`d a walk-right movie (the word ramps with the walk). |
| `+0x4B` (`FF83CB`/`FF87CB`) | byte | **Player 1 (player 2) health**: `0x90` at a round's start, steps down as hits land, 0 once beaten and outside a round | `--trace`d the punch movie; a `--poke` of it to 0 emptied the bar at once. Also in FinalBurn Neo's collection. |
| `+0x4D` (`FF83CD`/`FF87CD`) | byte | The same health again, one frame later: what the bar's label is set from | Compared two traces of a match: it changes exactly one frame after `+0x4B`, with the same value. |
| `+0x1CB` (`FF854B`/`FF894B`) | byte | The health the **bar** is drawn from, draining a step at a time | Traced beside `+0x4B`; it changes over the frames after each hit, where `+0x4B` changes at once. |
| `+0x1DF` (`FF855F`/`FF895F`) | byte | **The Special gauge**: `0x8E` with the bar full | Held at `0x8E` with `--poke`: both bars stayed full. Also in FinalBurn Neo's collection. |
| `+0x3A9` (`FF8729`) | byte | **The character**, 1–12 in the select screen's order | Read from dumps at the select screen and at a match, against the characters chosen. |

The health bytes are 0 outside a round (and in the attract show) and pass through `0xFE`/`0xFF` on the last
frames of a beaten player, which is what the cheats use to tell a round being fought from everything else:
a round is on while **both** healths are 1–`0x90`.

## Features built on it (`src/`)

| Feature | Category | What it does |
| --- | --- | --- |
| `free_play` | Game (Enhanced) | Holds two credits, so Start works without a coin and a second player can always join. |
| `no_damage` | Cheats | Puts player 1's health back to the round's full value before every frame, so no hit can take the last of it. |
| `no_damage_2` | Cheats | The same for player 2 (the opponent's, in a one-player game). |
| `infinite_time` | Cheats | Holds the round's clock at `0x99` every frame, so it never runs out (the digits read 98). |
| `one_hit` | Cheats | Leaves the opponent (player 2) a single point of health, so any hit ends the round. |
| `full_special` | Cheats | Holds both players' Special gauges full (`+0x1DF`), so a Special is always ready. |
| `input_display` | Training | What each player holds, and for how long, at their side of the picture: the toolkit's `fight::InputHistory`, fed from the port's own panel bits and the game's facing (taken from the two players' places), so the notation is turned to the player's side. |

Each was checked with no window against a run of the board, with `--trace` of the relevant bytes and
`--shot-every` pictures:

- `free_play`: a movie with **no** coin press reaches character select (frame 2000) and a match, where
  without the feature it stays on the title. Its state hashes match, frame for frame over 2400 frames of
  the same movie, a run that holds the credits with `--poke` instead.
- `no_damage`: over the walk movie player 1's health goes `0x90` → `0x7A` → `0x90` → … sixty times and
  never reaches 0; the same movie without it drains the health to 0 at frame 4174.
- `no_damage_2`: over the punch movie (with `no_damage` on, so player 1 can punch without being beaten
  first) player 2's health falls `0x90` → `0x86` → `0x7C` → … → `0x4A` over nine hits; with the feature on
  every hit is undone on the next frame (`0x90` → `0x86` → `0x90`).
- `infinite_time`: the clock byte stands at `0x99` from the round's start to frame 9000, where the round
  would have timed out at about 6100; with `no_damage` on too, the trace shows the round still being
  fought at 9000.
- `one_hit`: player 2's health is held at 1 from the round's start onwards.
- `full_special`: player 1's Special byte stays `0x8E` all round.
- `input_display`: the picture carries each player's holds and how long they were held, at their side of
  the picture, turned to the way they face.

## Next

- **The rest of the player record.** The state word (what a player is doing: standing, walking, crouching,
  in the air, attacking, reeling) is not yet told apart; the toolkit's training aids want exactly that
  (`fight::StepView`) for the frame data, the combo readout, the dummy and the hit boxes, and the second
  thing they want is the attack boxes themselves.
- **The widescreen picture**: the picture is drawn from the board's own 384 columns; a wider picture needs
  the camera and the stage scrollers found and a `widescreen.cpp` written (the toolkit's `Video::wide`).
- A **rewrite of the damage rule** (`docs/READABLE.md`) for a cleaner `one_hit` than holding the health at
  one point.

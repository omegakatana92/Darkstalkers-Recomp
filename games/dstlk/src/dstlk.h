// SPDX-License-Identifier: GPL-3.0-only
//
// Darkstalkers: The Night Warriors (Euro 940705): names for what is known of the game's memory. Every
// address here was found by experiment on the running board with the toolkit's own tools (--trace,
// --dump-ram, --poke, --shot-every) and, where said, first reached for from FinalBurn Neo's cheat
// collection for this same ROM set (cheats/dstlk.ini), then checked on the board. No game data is written
// down here, only where the game keeps it. PROGRESS.md tells how each was found.
#pragma once

#include "game.h"

#include <cstdint>

namespace dstlk {

using namespace cps2recomp;

// The credits the game is holding, a byte: the title shows it as "CREDITS n" and waits for one before it
// offers "PRESS UP TO 2 START BUTTONS". Found by poking 2 into 0xFF81D0 on the title screen and watching
// the line change (PROGRESS.md).
constexpr uint32_t kCredits = 0xFF81D0;
constexpr uint8_t kCreditsFree = 2; // (enough for both players to start)

// A player's record: the two are 0x400 apart, player 1's first.
//
//   +0x018  the place across the stage, a word (found by walking a player right and left and watching it
//           run 0x11C up to 0x17B and back)
//   +0x04B  the health, a byte (0x90 at the round's start; watched falling 0x90, 0x7A, 0x63, ... as hits
//           landed)
//   +0x1DF  the "Special" gauge, a byte (0x8E with the bar full)
//   +0x3A9  the character, a byte (1-12 in the select screen's order)
constexpr uint32_t kPlayer0 = 0xFF8380;
constexpr uint32_t kPlayerStride = 0x400;
constexpr uint32_t kPlace = 0x018;
constexpr uint32_t kHealth = 0x04B;
constexpr uint32_t kSpecial = 0x1DF;
constexpr uint32_t kCharacter = 0x3A9;

constexpr uint8_t kHealthFull = 0x90; // (every character's, as the board plays it)
constexpr uint8_t kSpecialFull = 0x8E; // (the whole bar, three levels and the last one's tail)

// The round's clock: packed decimal in a byte, 0x99 down to 0x00, a step every 37 frames. The byte before
// it is not the clock's: the toolkit's own watch of the match shows 0x01 there through a round.
constexpr uint32_t kClock = 0xFF9409;
constexpr uint8_t kClockStart = 0x99;
// Held at 0x99 every frame, the game's own step takes it to 0x98 and the clock is put back before the next
// step, so it never counts down to 0x00 and the round never times out. (0x9A would display "99" - the step
// takes it to 0x99, which is drawn - but the game treats anything above 0x99 as the round's time being up
// and ends it at once, so 0x99 is the one that can be held.)
constexpr uint8_t kClockHeld = 0x99;

// A player's record (n: 0 or 1)
constexpr uint32_t Player(int n) {
    return kPlayer0 + kPlayerStride * n;
}
inline uint8_t Health(int n) {
    return Ram8(Player(n) + kHealth);
}
inline uint8_t Special(int n) {
    return Ram8(Player(n) + kSpecial);
}
inline uint16_t Place(int n) {
    return Ram16(Player(n) + kPlace);
}
// The character: 1-12 (the select screen's order), 0 when no record is up
inline uint8_t Character(int n) {
    return Ram8(Player(n) + kCharacter);
}

// Both players are up and neither has been beaten: healths 1-0x90. This is what a round in progress looks
// like, and what the cheats leave alone (the attract mode's own fight keeps no records at all: both
// players' memory is zero there, so nothing here touches it).
inline bool InRound() {
    const uint8_t a = Health(0);
    const uint8_t b = Health(1);
    return a >= 1 && a <= kHealthFull && b >= 1 && b <= kHealthFull;
}

// Which way a player faces: towards the other one, as the board has it. Only meaningful while a round is
// fought (outside one both records hold 0), so a player outside one is taken to face right.
inline bool FacingLeft(int n) {
    if (!InRound()) {
        return false;
    }
    return Place(n) > Place(1 - n);
}

} // namespace dstlk

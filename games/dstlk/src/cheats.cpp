// SPDX-License-Identifier: GPL-3.0-only
//
// Darkstalkers: the cheats, as features the player switches. With every one off the port runs exactly as the
// board does. Each was checked on the board with the toolkit's --poke first and then as a feature
// (PROGRESS.md says how).
//
// The health cheats work *before* a frame, not after it: this game takes a hit's damage off the health as the
// hit lands and, if that leaves none, starts the round's end there and then. Putting the health back after the
// frame would be too late - the round would already be over - while putting it back at the start of every
// frame means a hit can never take the last of it. (The cheats that need only be seen by the next frame -
// credits, one hit, the clock - are done after it, as the board leaves them.)
#include "dstlk.h"

#include "state.h"

namespace cps2recomp {

namespace {
    using namespace dstlk;

    // What the last round in progress started with. On this board every character starts at 0x90, but it is
    // read from the round rather than written down, so a round whose start was missed is still put back to
    // what it began with.
    uint8_t healthAtStart[2] = {kHealthFull, kHealthFull};
    bool wasInRound = false;

    void NoteRound() {
        const bool inRound = InRound();
        if (inRound && !wasInRound) {
            healthAtStart[0] = Health(0);
            healthAtStart[1] = Health(1);
        }
        wasInRound = inRound;
    }
} // namespace

void CheatsBeforeFrame(Machine &machine) {
    (void)machine;
    NoteRound();
    if (!wasInRound) {
        // (the attract mode's own fight keeps no records at all: both players' memory is zero there, so there
        // is nothing to hold)
        return;
    }
    if (FeatureOn("no_damage")) {
        SetRam8(Player(0) + kHealth, healthAtStart[0]);
    }
    if (FeatureOn("no_damage_2")) {
        SetRam8(Player(1) + kHealth, healthAtStart[1]);
    }
    if (FeatureOn("full_special")) {
        SetRam8(Player(0) + kSpecial, kSpecialFull);
        SetRam8(Player(1) + kSpecial, kSpecialFull);
    }
}

void CheatsFrame(Machine &machine) {
    (void)machine;
    NoteRound();

    if (FeatureOn("free_play") && Ram8(kCredits) < kCreditsFree) {
        SetRam8(kCredits, kCreditsFree);
    }
    if (!wasInRound) {
        return;
    }
    if (FeatureOn("one_hit")) {
        SetRam8(Player(1) + kHealth, 1);
    }
    if (FeatureOn("infinite_time")) {
        SetRam8(kClock, kClockHeld);
    }
}

void CheatsState(Archive &ar) {
    ar.Pod(healthAtStart[0]);
    ar.Pod(healthAtStart[1]);
    ar.Pod(wasInRound);
}

void RegisterCheats() {
    Game &game = TheGame();
    game.features.push_back({"free_play", "Game", "Free play", "Start works without a coin.", kFeatureEnhanced});
    game.features.push_back({"no_damage", "Cheats", "No damage (1P)", "Player 1 takes no damage.", 0});
    game.features.push_back({"no_damage_2", "Cheats", "No damage (2P)", "Player 2 takes no damage.", 0});
    game.features.push_back({"one_hit", "Cheats", "One hit (2P)", "A single hit beats player 2.", 0});
    game.features.push_back({"infinite_time", "Cheats", "Infinite time", "The round's clock stands still.", 0});
    game.features.push_back({"full_special", "Cheats", "Full Special gauge", "Both Special gauges stay full.", 0});
}

} // namespace cps2recomp

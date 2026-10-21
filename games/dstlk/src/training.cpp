// SPDX-License-Identifier: GPL-3.0-only
//
// Darkstalkers: the training aid. The input display needs only what the port read off the panel and which
// way each player faces, both found by experiment (PROGRESS.md). The others (frame data, the combo readout,
// a dummy, the hit boxes) wait on more of the game's own memory being known: the players' records are found
// (dstlk.h), with health, place and the Special gauge picked out, but what a player is doing is not told
// apart yet (ROADMAP.md).
#include "dstlk.h"

#include "fight.h"
#include "machine.h"
#include "state.h"

#include <vector>

namespace cps2recomp {

namespace {
    using namespace dstlk;

    fight::InputHistory g_inputs;

    void ReadInputs(Machine &machine) {
        for (int n = 0; n < 2; n++) {
            g_inputs.Push(n, machine.players[n], FacingLeft(n));
        }
    }
} // namespace

void TrainingBeforeFrame(Machine &machine) {
    ReadInputs(machine);
}

void TrainingFrame(Machine &) {
    std::vector<HudLine> lines;
    if (FeatureOn("input_display")) {
        g_inputs.Hud(0, lines);
        g_inputs.Hud(1, lines);
    }
    SetHud(lines);
}

void TrainingState(Archive &ar) {
    // (fight::InputHistory's own frame counts are not part of the save state: a load starts the display over)
    if (!ar.Saving()) {
        g_inputs.Clear();
    }
}

void RegisterTraining() {
    Game &game = TheGame();
    game.features.push_back({"input_display", "Training", "Input display",
                             "What each player holds, and for how long, at their side of the picture."});
    TrainingAids({"input_display"});
}

} // namespace cps2recomp

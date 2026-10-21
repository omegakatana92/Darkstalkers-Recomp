// SPDX-License-Identifier: GPL-3.0-only
//
// Darkstalkers: what the game's own sources add to the port (see PROGRESS.md). The features are put together
// here from the game's own memory (dstlk.h) and the toolkit's fight/ parts (fight/fight.h).
#include "game.h"

namespace cps2recomp {

// (the game's own parts; each registers its features and state and is called from around a frame)
void RegisterCheats(); // cheats.cpp
void CheatsBeforeFrame(Machine &machine);
void CheatsFrame(Machine &machine);
void CheatsState(Archive &ar);
void RegisterTraining(); // training.cpp
void TrainingBeforeFrame(Machine &machine);
void TrainingFrame(Machine &machine);
void TrainingState(Archive &ar);

namespace {
    // (the cheats that hold health hold it before the frame, or a hit would end the round first; the ones
    // that need only be seen by the next frame do it after)
    void BeforeFrame(Machine &machine) {
        CheatsBeforeFrame(machine);
        TrainingBeforeFrame(machine);
    }

    // (the cheats see what a frame did, after it; the training aids show what was just read)
    void Frame(Machine &machine) {
        CheatsFrame(machine);
        TrainingFrame(machine);
    }

    void State(Archive &ar) {
        CheatsState(ar);
        TrainingState(ar);
    }
} // namespace

void RegisterGame() {
    RegisterCheats();
    RegisterTraining();
    TheGame().beforeFrame = BeforeFrame;
    TheGame().frame = Frame;
    TheGame().state = State;
}

} // namespace cps2recomp

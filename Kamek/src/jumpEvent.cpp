#include <common.h>
#include <game.h>

class daEnJumpEvent_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

    daPlBase_c *players[4];
	Remocon *control[4];

	int time = 10;
	bool reset = true;
	int driver;
	u8 event;

	static daEnJumpEvent_c* build();
};


daEnJumpEvent_c* daEnJumpEvent_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnJumpEvent_c));
	return new(buffer) daEnJumpEvent_c;
}


int daEnJumpEvent_c::onCreate() {
    this->event = (this->settings >> 28 & 0xF) - 1; // Nybble 5

	OSReport("Spawned!\n");

	this->onExecute();

	return true;
}

int daEnJumpEvent_c::onExecute() {
	int i;

	if (time < 20) { time++; }

    for (i = 0; i < 4; i++) {
        control[i] = GetRemoconMng()->controllers[i];
        players[i] = GetPlayerOrYoshi(i);

        if ((control[i]) && (players[i])) {
			bool start = ((control[i]->nowPressed & WPAD_TWO) || (control[i]->isShaking));

            if ((start) && (reset) && (time == 20) && (!collMgr.isOnTopOfTile())) {
                if (dFlagMgr_c::instance->active(event)) {
                    dFlagMgr_c::instance->set(event, 0, false, false, false);
                } else {
                    dFlagMgr_c::instance->set(event, 0, true, false, false);
                }

                reset = false;

				time = 0;
            }

            reset = players[i]->collMgr.isOnTopOfTile();
        }
    }

	return true;
}

int daEnJumpEvent_c::onDelete() {
	return true;
}

int daEnJumpEvent_c::onDraw() {
	return true;
}
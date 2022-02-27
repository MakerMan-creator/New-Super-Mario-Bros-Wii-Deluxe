#include <common.h>
#include <game.h>
#include <sfx.h>
#include "boss.h"
#include "bowserjr.h"

class daBossItemEditor : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();

	bool affectItem();

	int itemSettings, ItemConnectID, other;

    // Boss item class variables ([class] a) go from here
	daCustomShell daShell;
	// to here
	
	static daBossItemEditor* build();
};


daBossItemEditor* daBossItemEditor::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBossItemEditor));
	return new(buffer) daBossItemEditor;
}


int daBossItemEditor::onCreate() {
    this->ItemConnectID = (this->settings & 0x000F0000) >> 16;
    this->itemSettings = this->settings >> 28 & 0xF;
    this->other = this->settings >> 12 & 0xF;

    if (this->itemSettings == 0) {
    	if (this->other > 1) { this->other = 1; }
    }

	this->onExecute();

	return true;
}

int daBossItemEditor::onExecute() {
    bool done = affectItem();

    if (done) { this->Delete(true); }

	return true;
}

int daBossItemEditor::onDelete() {
	return true;
}

bool daBossItemEditor::affectItem() {
	if (itemSettings == 0) {
		if (this->daShell.BossItemID == this->ItemConnectID) {
            this->daShell.direction = this->other;
            return true;
		}
		return false;
	}
	return false;
}
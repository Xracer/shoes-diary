/*
 * Copyright 2010-2014 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_SOLDIERINFOSTATE_H
#define OPENXCOM_SOLDIERINFOSTATE_H

#include "../Engine/State.h"
#include <vector>

namespace OpenXcom
{

class Base;
class Surface;
class TextButton;
class Text;
class TextEdit;
class Bar;
class Soldier;
class SavedBattleGame;
class BattlescapeState;
class BattleUnit;

/**
 * Soldier Info screen that shows all the
 * info of a specific soldier.
 */
class SoldierInfoState : public State
{
private:
	Base *_base;
	size_t _soldierId;
	Soldier *_soldier;
	std::vector<Soldier*> *_list;

	Surface *_bg, *_rank;
	Surface *_selAmmo, *_statsbg, *_commendationsbg, *_recordsbg;	
	TextButton *_btnOk, *_btnPrev, *_btnNext, *_btnArmor, *_btnSack;
	TextButton *_btnUnload, *_btnGround, *_btnStats, *_btnCommendations, *_btnRecords;
	Text *_txtRank, *_txtMissions, *_txtKills, *_txtCraft, *_txtRecovery, *_txtPsionic;
	TextEdit *_edtSoldier;
	Text *_txtItem, *_txtAmmo;	

	Text *_txtTimeUnits, *_txtStamina, *_txtHealth, *_txtBravery, *_txtReactions, *_txtFiring, *_txtThrowing, *_txtMelee, *_txtStrength, *_txtPsiStrength, *_txtPsiSkill;
	Text *_txtFrontArmor, *_txtLeftArmor, *_txtRightArmor, *_txtRearArmor, *_txtUnderArmor;	
	Text *_numTimeUnits, *_numStamina, *_numHealth, *_numBravery, *_numReactions, *_numFiring, *_numThrowing, *_numMelee, *_numStrength, *_numPsiStrength, *_numPsiSkill;
	Text *_numFrontArmor, *_numLeftArmor, *_numRightArmor, *_numRearArmor, *_numUnderArmor;
	Bar *_barTimeUnits, *_barStamina, *_barHealth, *_barBravery, *_barReactions, *_barFiring, *_barThrowing, *_barMelee, *_barStrength, *_barPsiStrength, *_barPsiSkill;
	Bar *_barFrontArmor, *_barLeftArmor, *_barRightArmor, *_barRearArmor, *_barUnderArmor;
	bool _statsOn, _commOn, _recOn;
	std::vector<Surface*> _statScreenElements, _commScreenElements, _recScreenElements;
public:
	/// Creates the Soldier Info state.
	SoldierInfoState(Base *base, size_t soldierId);
	/// Cleans up the Soldier Info state.
	~SoldierInfoState();
	/// Updates the soldier info.
	void init();
	/// Handler for pressing on the Name edit.
	void edtSoldierPress(Action *action);
	/// Handler for changing text on the Name edit.
	void edtSoldierChange(Action *action);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
	/// Handler for clicking the Armor button.
	void btnArmorClick(Action *action);
	/// Handler for clicking the Sack button.
	void btnSackClick(Action *action);
	/// Handler for clicking the unload button.
	void btnUnloadClick(Action *action);
	/// Handler for clicking on the Ground -> button.
	void btnGroundClick(Action *action);
	/// Handler for clicking the Inventory button.
	void invClick(Action *action);
	/// Handler for clicking the stats button.
	void btnStatsClick(Action *action);
	/// Handler for clicking the commendation button.
	void btnCommendationsClick(Action *action);
	/// Handler for clicking the service record button.
	void btnRecordsClick(Action *action);	
	/// Saves the soldiers' equipment-layout.
	void saveEquipmentLayout();
	void switchTabs();	
};

}

#endif

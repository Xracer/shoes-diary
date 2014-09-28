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
#ifndef OPENXCOM_INVENTORYSTATE_H
#define OPENXCOM_INVENTORYSTATE_H

#include "../Engine/State.h"
#include "../Interface/TextButton.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/EquipmentLayoutItem.h"
#include "UnitInfoState.h"

namespace OpenXcom
{

class Surface;
class Text;
class InteractiveSurface;
class Inventory;
class SavedBattleGame;
class BattlescapeState;
class BattleUnit;
class Bar;
class TextButton;

/**
 * Screen which displays soldier's inventory.
 */
class InventoryState : public State
{
private:
	Surface *_bg, *_soldier, *_rank;
	Text *_txtName, *_txtItem, *_txtAmmo, *_txtWeight, *_txtTus, *_txtFAcc, *_txtReact, *_txtPSkill, *_txtPStr;
	Text *_txtRank, *_txtRankName, *_txtCraft, *_txtCraftName, *_txtMissions, *_txtnumMissions, *_txtKills;
	Text *_txtnumKills, *_txtStatus, *_txtStatusType, *_txtArmor, *_txtArmorType;
	InteractiveSurface *_btnOk, *_btnPrev, *_btnNext, *_btnUnload, *_btnGround, *_btnRank;
	InteractiveSurface *_btnCreateTemplate, *_btnApplyTemplate;
	//InteractiveSurface *_btnStats, *_btnAwards;
	Surface *_selAmmo;
	Inventory *_inv;
	std::vector<EquipmentLayoutItem*> _curInventoryTemplate;
	SavedBattleGame *_battleGame;
	const bool _tu;
	bool _fromInventory, _mindProbe;

	BattleUnit *_unit;
	BattlescapeState *_parent;
	std::string _currentTooltip;
	
	Text *_txtTimeUnits, *_txtEnergy, *_txtHealth, *_txtFatalWounds, *_txtBravery, *_txtMorale, *_txtReactions, *_txtFiring, *_txtThrowing, *_txtMelee, *_txtStrength;
	Text *_txtPsiStrength, *_txtPsiSkill;
	Text *_numTimeUnits, *_numEnergy, *_numHealth, *_numFatalWounds, *_numBravery, *_numMorale, *_numReactions, *_numFiring, *_numThrowing, *_numMelee, *_numStrength;
	Text *_numPsiStrength, *_numPsiSkill;
	Bar *_barTimeUnits, *_barEnergy, *_barHealth, *_barFatalWounds, *_barBravery, *_barMorale, *_barReactions, *_barFiring, *_barThrowing, *_barMelee, *_barStrength;
	Bar *_barPsiStrength, *_barPsiSkill;

	Text *_txtFrontArmor, *_txtLeftArmor, *_txtRightArmor, *_txtRearArmor, *_txtUnderArmor;
	Text *_numFrontArmor, *_numLeftArmor, *_numRightArmor, *_numRearArmor, *_numUnderArmor;
	Bar *_barFrontArmor, *_barLeftArmor, *_barRightArmor, *_barRearArmor, *_barUnderArmor;

public:
	/// Creates the Inventory state.
	InventoryState(bool tu, BattlescapeState *parent);
	/// Cleans up the Inventory state.
	~InventoryState();
	/// Updates all soldier info.
	void init();
	/// Updates the soldier info (Weight, TU).
	void updateStats();
	/// Saves the soldiers' equipment-layout.
	void saveEquipmentLayout();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
	/// Handler for clicking the Unload button.
	void btnUnloadClick(Action *action);
	/// Handler for clicking on the Ground -> button.
	void btnGroundClick(Action *action);
	/// Handler for clicking the Rank button.
	void btnRankClick(Action *action);
	/// Handler for clicking on the Create Template button.
	void btnCreateTemplateClick(Action *action);
	/// Handler for clicking the Apply Template button.
	void btnApplyTemplateClick(Action *action);
	/// Handler for hitting the Clear Inventory hotkey.
	void onClearInventory(Action *action);
	/// Handler for clicking on the inventory.
	void invClick(Action *action);
	/// Handler for showing item info.
	void invMouseOver(Action *action);
	/// Handler for hiding item info.
	void invMouseOut(Action *action);
	/// Handles keypresses.
	void handle(Action *action);
	/// Handler for showing tooltip.
	void txtTooltipIn(Action *action);
	/// Handler for hiding tooltip.
	void txtTooltipOut(Action *action);

private:
	/// Update the visibility and icons for the template buttons
	void _updateTemplateButtons(bool isVisible);
	/// Refresh the hover status of the mouse
	void _refreshMouse();
};

}

#endif

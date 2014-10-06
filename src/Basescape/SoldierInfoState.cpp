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
#include "SoldierInfoState.h"
#include "SoldierDiaryOverviewState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Engine/InteractiveSurface.h"
#include "../Interface/Bar.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Engine/Surface.h"
#include "../Interface/Window.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/Base.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/EquipmentLayoutItem.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDiary.h"
#include "../Savegame/ItemContainer.h"
#include "../Engine/SurfaceSet.h"
#include "../Battlescape/Inventory.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/RuleCraft.h"
#include "../Menu/ErrorMessageState.h"
#include "SellState.h"
#include "SoldierArmorState.h"
#include "SackSoldierState.h"
#include "CraftEquipmentState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from. NULL to use the dead soldiers list.
 * @param soldierId ID of the selected soldier.
 */
SoldierInfoState::SoldierInfoState(Base *base, size_t soldierId) : _base(base), _soldierId(soldierId)
{
	if (_base == 0)
	{
		_list = _game->getSavedGame()->getDeadSoldiers();
		if (_soldierId >= _list->size())
		{
			_soldierId = 0;
		}
		else
		{
			_soldierId = _list->size() - (1 + _soldierId);
		}
	}
	else
	{
		_list = _base->getSoldiers();
	}

	// Create objects
	_bg = new Surface(960, 600, 0, 0);
	_rank = new Surface(26, 23, 4, 4);
	_btnPrev = new TextButton(28, 16, 859, 8);
	_btnOk = new TextButton(36, 16, 887, 8);
	_btnNext = new TextButton(28, 16, 923, 8);
	_btnArmor = new TextButton(100, 14, 425, 38);	// (60, 14, 130, 33);
	_edtSoldier = new TextEdit(this, 200, 16, 40, 9);
	_btnSack = new TextButton(60, 14, 600, 38); 	// (60, 14, 248, 10);
    _btnDiary = new TextButton(60, 14, 260, 48);
	_txtRank = new Text(100, 9, 5, 38);
	_txtMissions = new Text(100, 9, 215, 38);
	_txtKills = new Text(100, 9, 320, 38);
	_txtCraft = new Text(100, 9, 110, 38);
	_txtRecovery = new Text(180, 9, 130, 56);
	_txtPsionic = new Text(150, 9, 0, 66);
	
	//Variables for the Inventory screen
	_txtItem = new Text(160, 9, 8, 310);
	_txtAmmo = new Text(80, 24, 8, 432);
	_btnUnload = new TextButton(32, 25, 8, 280);
	_btnGround = new TextButton(32, 15, 924, 442);
	_selAmmo = new Surface(RuleInventory::HAND_W * RuleInventory::SLOT_W, RuleInventory::HAND_H * RuleInventory::SLOT_H, 8, 325);	

	//Variables for the selection of subscreens
	_btnStats = new TextButton(60, 15, 540, 70); // add control for stats screen
	_btnCommendations = new TextButton(60, 15, 601, 70); // add control for Awards screen
	_btnRecords = new TextButton(60, 15, 662, 70); // add control for stats screen	
	
	//variables for stats window
	_statsbg = new Surface(338, 295, 541, 89);	
	
	int yPos = 100;
	int xPos = 540;
	int step = 13;

	_txtTimeUnits = new Text(120, 9, xPos + 6, yPos);
	_numTimeUnits = new Text(18, 9, xPos + 131, yPos);
	_barTimeUnits = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtStamina = new Text(120, 9, xPos + 6, yPos);
	_numStamina = new Text(18, 9, xPos + 131, yPos);
	_barStamina = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtHealth = new Text(120, 9, xPos + 6, yPos);
	_numHealth = new Text(18, 9, xPos + 131, yPos);
	_barHealth = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtBravery = new Text(120, 9, xPos + 6, yPos);
	_numBravery = new Text(18, 9, xPos + 131, yPos);
	_barBravery = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtReactions = new Text(120, 9, xPos + 6, yPos);
	_numReactions = new Text(18, 9, xPos + 131, yPos);
	_barReactions = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtFiring = new Text(120, 9, xPos + 6, yPos);
	_numFiring = new Text(18, 9, xPos + 131, yPos);
	_barFiring = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtThrowing = new Text(120, 9, xPos + 6, yPos);
	_numThrowing = new Text(18, 9, xPos + 131, yPos);
	_barThrowing = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtMelee = new Text(120, 9, xPos + 6, yPos);
	_numMelee = new Text(18, 9, xPos + 131, yPos);
	_barMelee = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtStrength = new Text(120, 9, xPos + 6, yPos);
	_numStrength = new Text(18, 9, xPos + 131, yPos);
	_barStrength = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtPsiStrength = new Text(120, 9, xPos + 6, yPos);
	_numPsiStrength = new Text(18, 9, xPos + 131, yPos);
	_barPsiStrength = new Bar(200, 7, xPos + 150, yPos);
	yPos += step;

	_txtPsiSkill = new Text(120, 9, xPos + 6, yPos);
	_numPsiSkill = new Text(18, 9, xPos + 131, yPos);
	_barPsiSkill = new Bar(200, 7, xPos + 150, yPos);
	
	//variables for commendations window
	_commendationsbg = new Surface(381, 287, 541, 89);
	
	//variables for records window
	_recordsbg = new Surface(300, 200, 541, 89);	

	// Set palette
	setPalette("PAL_BASESCAPE");

	//add items to the screen	
	add(_bg);
	add(_rank);
	add(_btnOk);
	add(_btnPrev);
	add(_btnNext);
	add(_btnArmor);
	add(_edtSoldier);
	add(_btnSack);
	add(_btnDiary);
	add(_txtRank);
	add(_txtMissions);
	add(_txtKills);
	add(_txtCraft);
	add(_txtRecovery);
	add(_txtPsionic);

	//add interface items for Inventory control
	add(_txtItem);
	add(_txtAmmo);
	add(_btnUnload);
	add(_btnGround);

	//add interface items for multiwindow control
	add(_btnStats);
	add(_btnCommendations);
	add(_btnRecords);

	add(_selAmmo);
	//add(_inv);
	
	//add text, num and bar for the stats screen
	add(_txtTimeUnits);
	add(_numTimeUnits);
	add(_barTimeUnits);

	add(_txtStamina);
	add(_numStamina);
	add(_barStamina);

	add(_txtHealth);
	add(_numHealth);
	add(_barHealth);

	add(_txtBravery);
	add(_numBravery);
	add(_barBravery);

	add(_txtReactions);
	add(_numReactions);
	add(_barReactions);

	add(_txtFiring);
	add(_numFiring);
	add(_barFiring);

	add(_txtThrowing);
	add(_numThrowing);
	add(_barThrowing);

	add(_txtMelee);
	add(_numMelee);
	add(_barMelee);

	add(_txtStrength);
	add(_numStrength);
	add(_barStrength);

	add(_txtPsiStrength);
	add(_numPsiStrength);
	add(_barPsiStrength);

	add(_txtPsiSkill);
	add(_numPsiSkill);
	add(_barPsiSkill);

	// push all the elements of the "stats tab" into this vector
	// do not put the buttons to switch tabs in these vectors
	_statScreenElements.push_back(_statsbg);
	_statScreenElements.push_back(_txtTimeUnits);
	_statScreenElements.push_back(_numTimeUnits);
	_statScreenElements.push_back(_barTimeUnits);

	_statScreenElements.push_back(_txtStamina);
	_statScreenElements.push_back(_numStamina);
	_statScreenElements.push_back(_barStamina);

	_statScreenElements.push_back(_txtHealth);
	_statScreenElements.push_back(_numHealth);
	_statScreenElements.push_back(_barHealth);

	_statScreenElements.push_back(_txtBravery);
	_statScreenElements.push_back(_numBravery);
	_statScreenElements.push_back(_barBravery);

	_statScreenElements.push_back(_txtReactions);
	_statScreenElements.push_back(_numReactions);
	_statScreenElements.push_back(_barReactions);

	_statScreenElements.push_back(_txtFiring);
	_statScreenElements.push_back(_numFiring);
	_statScreenElements.push_back(_barFiring);

	_statScreenElements.push_back(_txtThrowing);
	_statScreenElements.push_back(_numThrowing);
	_statScreenElements.push_back(_barThrowing);

	_statScreenElements.push_back(_txtStrength);
	_statScreenElements.push_back(_numStrength);
	_statScreenElements.push_back(_barStrength);

	_statScreenElements.push_back(_txtPsiStrength);
	_statScreenElements.push_back(_numPsiStrength);
	_statScreenElements.push_back(_barPsiStrength);

	_statScreenElements.push_back(_txtPsiSkill);
	_statScreenElements.push_back(_numPsiSkill);
	_statScreenElements.push_back(_barPsiSkill);

	// push all the elements of the "commendations tab" into this vector
	//_commScreenElements.push_back();
	_commScreenElements.push_back(_commendationsbg);

	// push all the elements of the "records tab" into this vector
	//_recScreenElements.push_back();
	_recScreenElements.push_back(_recordsbg);

	// centerAllSurfaces();   Xracer removed for higher res

	// Set up objects
	_game->getResourcePack()->getSurface("HDBACK06.PNG")->blit(_bg);
	_game->getResourcePack()->getSurface("STATSBG.PNG")->blit(_statsbg);
	_game->getResourcePack()->getSurface("COMMENDATIONSBG.PNG")->blit(_commendationsbg);
	_game->getResourcePack()->getSurface("RECORDSBG.PNG")->blit(_recordsbg);

	//_statsWindow;
	//_statsbg;

	_btnOk->setColor(Palette::blockOffset(15)+6);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierInfoState::btnOkClick, Options::keyCancel);

	_btnPrev->setColor(Palette::blockOffset(15)+6);
	_btnPrev->setText(L"<<");
	if (_base == 0)
	{
		_btnPrev->onMouseClick((ActionHandler)&SoldierInfoState::btnNextClick);
		_btnPrev->onKeyboardPress((ActionHandler)&SoldierInfoState::btnNextClick, Options::keyBattlePrevUnit);
	}
	else
	{
		_btnPrev->onMouseClick((ActionHandler)&SoldierInfoState::btnPrevClick);
		_btnPrev->onKeyboardPress((ActionHandler)&SoldierInfoState::btnPrevClick, Options::keyBattlePrevUnit);
	}

	_btnNext->setColor(Palette::blockOffset(15)+6);
	_btnNext->setText(L">>");
	if (_base == 0)
	{
		_btnNext->onMouseClick((ActionHandler)&SoldierInfoState::btnPrevClick);
		_btnNext->onKeyboardPress((ActionHandler)&SoldierInfoState::btnPrevClick, Options::keyBattleNextUnit);
	}
	else
	{
		_btnNext->onMouseClick((ActionHandler)&SoldierInfoState::btnNextClick);
		_btnNext->onKeyboardPress((ActionHandler)&SoldierInfoState::btnNextClick, Options::keyBattleNextUnit);
	}

	_btnArmor->setColor(Palette::blockOffset(15)+6);
	_btnArmor->setText(tr("STR_ARMOR"));
	_btnArmor->onMouseClick((ActionHandler)&SoldierInfoState::btnArmorClick);

	_edtSoldier->setColor(Palette::blockOffset(13)+10);
	_edtSoldier->setBig();
	_edtSoldier->onChange((ActionHandler)&SoldierInfoState::edtSoldierChange);
	_edtSoldier->onMousePress((ActionHandler)&SoldierInfoState::edtSoldierPress);

	_btnSack->setColor(Palette::blockOffset(15)+6);
	_btnSack->setText(tr("STR_SACK"));
	_btnSack->onMouseClick((ActionHandler)&SoldierInfoState::btnSackClick);

	_btnDiary->setColor(Palette::blockOffset(15)+6);
	_btnDiary->setText(tr("STR_DIARY"));
	_btnDiary->onMouseClick((ActionHandler)&SoldierInfoState::btnDiaryClick);

	_txtRank->setColor(Palette::blockOffset(13)+10);
	_txtRank->setSecondaryColor(Palette::blockOffset(13));

	_txtMissions->setColor(Palette::blockOffset(13)+10);
	_txtMissions->setSecondaryColor(Palette::blockOffset(13));

	_txtKills->setColor(Palette::blockOffset(13)+10);
	_txtKills->setSecondaryColor(Palette::blockOffset(13));

	_txtCraft->setColor(Palette::blockOffset(13)+10);
	_txtCraft->setSecondaryColor(Palette::blockOffset(13));

	_txtRecovery->setColor(Palette::blockOffset(13)+10);
	_txtRecovery->setSecondaryColor(Palette::blockOffset(13));

	_txtPsionic->setColor(Palette::blockOffset(15)+1);
	_txtPsionic->setText(tr("STR_IN_PSIONIC_TRAINING"));

	_txtTimeUnits->setColor(Palette::blockOffset(15)+1);
	_txtTimeUnits->setText(tr("STR_TIME_UNITS"));

	_numTimeUnits->setColor(Palette::blockOffset(13));

	_barTimeUnits->setColor(Palette::blockOffset(3));
	_barTimeUnits->setColor2(Palette::blockOffset(3)+4);
	_barTimeUnits->setScale(1.0);
	_barTimeUnits->setInvert(true);

	_txtStamina->setColor(Palette::blockOffset(15)+1);
	_txtStamina->setText(tr("STR_STAMINA"));

	_numStamina->setColor(Palette::blockOffset(13));

	_barStamina->setColor(Palette::blockOffset(9));
	_barStamina->setColor2(Palette::blockOffset(9)+4);
	_barStamina->setScale(1.0);
	_barStamina->setInvert(true);

	_txtHealth->setColor(Palette::blockOffset(15)+1);
	_txtHealth->setText(tr("STR_HEALTH"));

	_numHealth->setColor(Palette::blockOffset(13));

	_barHealth->setColor(Palette::blockOffset(2));
	_barHealth->setColor2(Palette::blockOffset(2)+4);
	_barHealth->setScale(1.0);
	_barHealth->setInvert(true);

	_txtBravery->setColor(Palette::blockOffset(15)+1);
	_txtBravery->setText(tr("STR_BRAVERY"));

	_numBravery->setColor(Palette::blockOffset(13));

	_barBravery->setColor(Palette::blockOffset(4));
	_barBravery->setColor2(Palette::blockOffset(4)+4);
	_barBravery->setScale(1.0);
	_barBravery->setInvert(true);

	_txtReactions->setColor(Palette::blockOffset(15)+1);
	_txtReactions->setText(tr("STR_REACTIONS"));

	_numReactions->setColor(Palette::blockOffset(13));

	_barReactions->setColor(Palette::blockOffset(1));
	_barReactions->setColor2(Palette::blockOffset(1)+4);
	_barReactions->setScale(1.0);
	_barReactions->setInvert(true);

	_txtFiring->setColor(Palette::blockOffset(15)+1);
	_txtFiring->setText(tr("STR_FIRING_ACCURACY"));

	_numFiring->setColor(Palette::blockOffset(13));

	_barFiring->setColor(Palette::blockOffset(8));
	_barFiring->setColor2(Palette::blockOffset(8)+4);
	_barFiring->setScale(1.0);
	_barFiring->setInvert(true);

	_txtThrowing->setColor(Palette::blockOffset(15)+1);
	_txtThrowing->setText(tr("STR_THROWING_ACCURACY"));

	_numThrowing->setColor(Palette::blockOffset(13));

	_barThrowing->setColor(Palette::blockOffset(10));
	_barThrowing->setColor2(Palette::blockOffset(10)+4);
	_barThrowing->setScale(1.0);
	_barThrowing->setInvert(true);

	_txtMelee->setColor(Palette::blockOffset(15)+1);
	_txtMelee->setText(tr("STR_MELEE_ACCURACY"));

	_numMelee->setColor(Palette::blockOffset(13));

	_barMelee->setColor(Palette::blockOffset(4));
	_barMelee->setColor2(Palette::blockOffset(4)+4);
	_barMelee->setScale(1.0);
	_barMelee->setInvert(true);

	_txtStrength->setColor(Palette::blockOffset(15)+1);
	_txtStrength->setText(tr("STR_STRENGTH"));

	_numStrength->setColor(Palette::blockOffset(13));

	_barStrength->setColor(Palette::blockOffset(5));
	_barStrength->setColor2(Palette::blockOffset(5)+4);
	_barStrength->setScale(1.0);
	_barStrength->setInvert(true);

	_txtPsiStrength->setColor(Palette::blockOffset(15)+1);
	_txtPsiStrength->setText(tr("STR_PSIONIC_STRENGTH"));

	_numPsiStrength->setColor(Palette::blockOffset(13));

	_barPsiStrength->setColor(Palette::blockOffset(11));
	_barPsiStrength->setColor2(Palette::blockOffset(11)+4);
	_barPsiStrength->setScale(1.0);
	_barPsiStrength->setInvert(true);

	_txtPsiSkill->setColor(Palette::blockOffset(15)+1);
	_txtPsiSkill->setText(tr("STR_PSIONIC_SKILL"));

	_numPsiSkill->setColor(Palette::blockOffset(13));

	_barPsiSkill->setColor(Palette::blockOffset(11));
	_barPsiSkill->setColor2(Palette::blockOffset(11)+4);
	_barPsiSkill->setScale(1.0);
	_barPsiSkill->setInvert(true);
}

/**
 *
 */
SoldierInfoState::~SoldierInfoState()
{

}

/**
 * Updates soldier stats when
 * the soldier changes.
 */
void SoldierInfoState::init()
{
	State::init();
	
/* Need to create if statement for selecting option
	 * Need to move all stats into UnitStats()
	 * Need to setup independent bg for UnitStats() to display stats grid
	 * Need to setup independent bg for UnitAwards() to display awards grid
	if 
		award button is clicked then go to UnitAwards()
		else
		display UnitStats()
	
	
	*/

/*
BattleUnit *unit = _battleGame->getSelectedUnit();

	// no selected unit, close inventory
	if (unit == 0)
	{
		btnOkClick(0);
		return;
	}
	// skip to the first unit with inventory
	if (!unit->hasInventory())
	{
		if (_parent)
		{
			_parent->selectNextPlayerUnit(false, false, true);
		}
		else
		{
			_battleGame->selectNextPlayerUnit(false, false, true);
		}
		// no available unit, close inventory
		if (_battleGame->getSelectedUnit() == 0 || !_battleGame->getSelectedUnit()->hasInventory())
		{
			// starting a mission with just vehicles
			btnOkClick(0);
			return;
		}
		else
		{
			unit = _battleGame->getSelectedUnit();
		}
	}

	if (_parent)
		_parent->getMap()->getCamera()->centerOnPosition(unit->getPosition(), false);

	unit->setCache(0);
	_soldier->clear();

	_inv->setSelectedUnit(unit);
	Soldier *s = _game->getSavedGame()->getSoldier(unit->getId());
	if (s)
	{
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
		texture->getFrame(s->getRankSprite())->setX(0);
		texture->getFrame(s->getRankSprite())->setY(0);
		texture->getFrame(s->getRankSprite())->blit(_rank);

		std::string look = s->getArmor()->getSpriteInventory();
		if (s->getGender() == GENDER_MALE)
			look += "M";
		else
			look += "F";
		if (s->getLook() == LOOK_BLONDE)
			look += "0";
		if (s->getLook() == LOOK_BROWNHAIR)
			look += "1";
		if (s->getLook() == LOOK_ORIENTAL)
			look += "2";
		if (s->getLook() == LOOK_AFRICAN)
			look += "3";
		look += ".SPK";
		if (!CrossPlatform::fileExists(CrossPlatform::getDataFile("UFOGRAPH/" + look)) && !_game->getResourcePack()->getSurface(look))
		{
			look = s->getArmor()->getSpriteInventory() + ".SPK";
		}
		_game->getResourcePack()->getSurface(look)->blit(_soldier);
	}
	else
	{
		Surface *armorSurface = _game->getResourcePack()->getSurface(unit->getArmor()->getSpriteInventory());
		if (armorSurface)
		{
			armorSurface->blit(_soldier);
		}
	}
	*/
	
	if (_list->empty())
	{
		_game->popState();
		return;
	}
	if (_soldierId >= _list->size())
	{
		_soldierId = 0;
	}
	_soldier = _list->at(_soldierId);
	_edtSoldier->setBig();
	_edtSoldier->setText(_soldier->getName());
	UnitStats *initial = _soldier->getInitStats();
	UnitStats *current = _soldier->getCurrentStats();

	UnitStats withArmor(*current);
	withArmor += *(_soldier->getArmor()->getStats());

	SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
	texture->getFrame(_soldier->getRankSprite())->setX(0);
	texture->getFrame(_soldier->getRankSprite())->setY(0);
	texture->getFrame(_soldier->getRankSprite())->blit(_rank);

	std::wostringstream ss;
	ss << withArmor.tu;
	_numTimeUnits->setText(ss.str());
	_barTimeUnits->setMax(current->tu);
	_barTimeUnits->setValue(withArmor.tu);
	_barTimeUnits->setValue2(std::min(withArmor.tu, initial->tu));

	std::wostringstream ss2;
	ss2 << withArmor.stamina;
	_numStamina->setText(ss2.str());
	_barStamina->setMax(current->stamina);
	_barStamina->setValue(withArmor.stamina);
	_barStamina->setValue2(std::min(withArmor.stamina, initial->stamina));

	std::wostringstream ss3;
	ss3 << withArmor.health;
	_numHealth->setText(ss3.str());
	_barHealth->setMax(current->health);
	_barHealth->setValue(withArmor.health);
	_barHealth->setValue2(std::min(withArmor.health, initial->health));

	std::wostringstream ss4;
	ss4 << withArmor.bravery;
	_numBravery->setText(ss4.str());
	_barBravery->setMax(current->bravery);
	_barBravery->setValue(withArmor.bravery);
	_barBravery->setValue2(std::min(withArmor.bravery, initial->bravery));

	std::wostringstream ss5;
	ss5 << withArmor.reactions;
	_numReactions->setText(ss5.str());
	_barReactions->setMax(current->reactions);
	_barReactions->setValue(withArmor.reactions);
	_barReactions->setValue2(std::min(withArmor.reactions, initial->reactions));

	std::wostringstream ss6;
	ss6 << withArmor.firing;
	_numFiring->setText(ss6.str());
	_barFiring->setMax(current->firing);
	_barFiring->setValue(withArmor.firing);
	_barFiring->setValue2(std::min(withArmor.firing, initial->firing));

	std::wostringstream ss7;
	ss7 << withArmor.throwing;
	_numThrowing->setText(ss7.str());
	_barThrowing->setMax(current->throwing);
	_barThrowing->setValue(withArmor.throwing);
	_barThrowing->setValue2(std::min(withArmor.throwing, initial->throwing));

	std::wostringstream ss8;
	ss8 << withArmor.melee;
	_numMelee->setText(ss8.str());
	_barMelee->setMax(current->melee);
	_barMelee->setValue(withArmor.melee);
	_barMelee->setValue2(std::min(withArmor.melee, initial->melee));

	std::wostringstream ss9;
	ss9 << withArmor.strength;
	_numStrength->setText(ss9.str());
	_barStrength->setMax(current->strength);
	_barStrength->setValue(withArmor.strength);
	_barStrength->setValue2(std::min(withArmor.strength, initial->strength));

	std::wstring wsArmor;
	std::string armorType = _soldier->getArmor()->getType();
	if (armorType == "STR_NONE_UC")
	{
		wsArmor= tr("STR_ARMOR_").arg(tr(armorType));
	}
	else
	{
		wsArmor = tr(armorType);
	}

	_btnArmor->setText(wsArmor);

	_btnSack->setVisible(!(_soldier->getCraft() && _soldier->getCraft()->getStatus() == "STR_OUT"));

	_txtRank->setText(tr("STR_RANK_").arg(tr(_soldier->getRankString())));

	_txtMissions->setText(tr("STR_MISSIONS").arg(_soldier->getMissions()));

	_txtKills->setText(tr("STR_KILLS").arg(_soldier->getKills()));

	std::wstring craft;
	if (_soldier->getCraft() == 0)
	{
		craft = tr("STR_NONE_UC");
	}
	else
	{
		craft = _soldier->getCraft()->getName(_game->getLanguage());
	}
	_txtCraft->setText(tr("STR_CRAFT_").arg(craft));

	if (_soldier->getWoundRecovery() > 0)
	{
		_txtRecovery->setText(tr("STR_WOUND_RECOVERY").arg(tr("STR_DAY", _soldier->getWoundRecovery())));
	}
	else
	{
		_txtRecovery->setText(L"");
	}

	_txtPsionic->setVisible(_soldier->isInPsiTraining());

	if (current->psiSkill > 0 || (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getRuleset()->getPsiRequirements())))
	{
		std::wstringstream ss14;
		ss14 << withArmor.psiStrength;
		_numPsiStrength->setText(ss14.str());
		_barPsiStrength->setMax(current->psiStrength);
		_barPsiStrength->setValue(withArmor.psiStrength);
		_barPsiStrength->setValue2(std::min(withArmor.psiStrength, initial->psiStrength));

		_txtPsiStrength->setVisible(true);
		_numPsiStrength->setVisible(true);
		_barPsiStrength->setVisible(true);
	}
	else
	{
		_txtPsiStrength->setVisible(false);
		_numPsiStrength->setVisible(false);
		_barPsiStrength->setVisible(false);
	}

	if (current->psiSkill > 0)
	{
		std::wstringstream ss15;
		ss15 << withArmor.psiSkill;
		_numPsiSkill->setText(ss15.str());
		_barPsiSkill->setMax(current->psiSkill);
		_barPsiSkill->setValue(withArmor.psiSkill);
		_barPsiSkill->setValue2(std::min(withArmor.psiSkill, initial->psiSkill));

		_txtPsiSkill->setVisible(true);
		_numPsiSkill->setVisible(true);
		_barPsiSkill->setVisible(true);
	}
	else
	{
		_txtPsiSkill->setVisible(false);
		_numPsiSkill->setVisible(false);
		_barPsiSkill->setVisible(false);
	}

	// Dead can't talk
	if (_base == 0)
	{
		_btnArmor->setVisible(false);
		_btnSack->setVisible(false);
		_txtCraft->setVisible(false);
	}
	else
	{
		_btnSack->setVisible(_game->getSavedGame()->getMonthsPassed() > -1);
	}
	
	//Needs to call the two new functions in case 
	//btnStats();
	//btnAwardsSelect();
}

/**
 * Disables the soldier input.
 * @param action Pointer to an action.
 */
void SoldierInfoState::edtSoldierPress(Action *action)
{
	if (_base == 0)
	{
		_edtSoldier->setFocus(false);
	}
}

/**
 * Set the soldier Id.
 */
void SoldierInfoState::setSoldierId(size_t soldier)
{
	_soldierId = soldier;
}

/**
 * Changes the soldier's name.
 * @param action Pointer to an action.
 */
void SoldierInfoState::edtSoldierChange(Action *action)
{
	_soldier->setName(_edtSoldier->getText());
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnOkClick(Action *)
{
    
	_game->popState();
	if (_game->getSavedGame()->getMonthsPassed() > -1 && Options::storageLimitsEnforced && _base != 0 && _base->storesOverfull())
	{
		_game->pushState(new SellState(_base));
		_game->pushState(new ErrorMessageState(tr("STR_STORAGE_EXCEEDED").arg(_base->getName()).c_str(), _palette, Palette::blockOffset(15)+1, "BACK01.SCR", 0));
	}
}

/**
 * Goes to the previous soldier.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnPrevClick(Action *)
{
	if (_soldierId == 0)
		_soldierId = _list->size() - 1;
	else
		_soldierId--;
	init();
}

/**
 * Goes to the next soldier.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnNextClick(Action *)
{
	_soldierId++;
	if (_soldierId >= _list->size())
		_soldierId = 0;
	init();
}

/**
 * Shows the Select Armor window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnArmorClick(Action *)
{
	if (!_soldier->getCraft() || (_soldier->getCraft() && _soldier->getCraft()->getStatus() != "STR_OUT"))
	{
		_game->pushState(new SoldierArmorState(_base, _soldierId));
	}
}

/**
 * Shows the Sack Soldier window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnSackClick(Action *)
{
	_game->pushState(new SackSoldierState(_base, _soldierId));
}

/**
* Shows the Soldier Stats window within the main window.
* @param action Pointer to an action.
 */
void SoldierInfoState::btnStatsClick(Action *)
{
	_statsOn = true;
	_commOn = false;
	_recOn = false;
	switchTabs();
}

/**
 * Shows the Soldier Award window within the main window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnCommendationsClick(Action *)
{
	_statsOn = false;
	_commOn = true;
	_recOn = false;
	switchTabs();
}

/**
 * Shows the Soldier Records window within the main window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnRecordsClick(Action *)
{
	_statsOn = false;
	_commOn = false;
	_recOn = true;
	switchTabs();
}

void SoldierInfoState::switchTabs()
{
	// switch visiblity of all elements of each "tab" based on the three bools (_statsOn, _commOn, _recOn)
	for (std::vector<Surface*>::iterator i = _statScreenElements.begin(); i != _statScreenElements.end(); ++i)
	{
		(*i)->setVisible(_statsOn);
	}
	for (std::vector<Surface*>::iterator i = _commScreenElements.begin(); i != _commScreenElements.end(); ++i)
	{
		(*i)->setVisible(_commOn);
	}
	for (std::vector<Surface*>::iterator i = _recScreenElements.begin(); i != _recScreenElements.end(); ++i)
	{
		(*i)->setVisible(_recOn);
	}
}


/**
 * Shows the Diary Soldier window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnDiaryClick(Action *)
{
	_game->pushState(new SoldierDiaryOverviewState(_base, _soldierId, this));
}

/**
 * Unloads the selected weapon.
 * @param action Pointer to an action.
 
void SoldierInfoState::btnUnloadClick(Action *action)
{
	if (_inv->getSelectedItem() != 0 && _inv->getSelectedItem()->getAmmoItem() != 0 && _inv->getSelectedItem()->needsAmmo())
	{
		_inv->unload();
		_txtItem->setText(L"");
		_txtAmmo->setText(L"");
		_selAmmo->clear();
		btnStatsClick(action);
	}
}

/**
 * Shows more ground items / rearranges them.
 * @param action Pointer to an action.
 
void SoldierInfoState::btnGroundClick(Action *)
{
	_inv->arrangeGround();
}
*/

}

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
#include "InventoryState.h"
#include "Inventory.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/CrossPlatform.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Screen.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Text.h"
#include "../Engine/Action.h"
#include "../Engine/InteractiveSurface.h"
#include "../Engine/Sound.h"
#include "../Engine/SurfaceSet.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Tile.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Options.h"
#include "UnitInfoState.h"
#include "BattlescapeState.h"
#include "TileEngine.h"
#include "Map.h"
#include "Camera.h"
#include "Pathfinding.h"
#include "../Interface/Bar.h"

namespace OpenXcom
{

static const int _templateBtnX = 924;
static const int _createTemplateBtnY = 392;
static const int _applyTemplateBtnY  = 417;

/**
 * Initializes all the elements in the Inventory screen.
 * @param game Pointer to the core game.
 * @param tu Does Inventory use up Time Units?
 * @param parent Pointer to parent Battlescape.
 */
InventoryState::InventoryState(bool tu, BattlescapeState *parent) : _tu(tu), _parent(parent)
{
	_battleGame = _game->getSavedGame()->getSavedBattle();

	if (Options::maximizeInfoScreens)
	{
		Options::baseXResolution = Screen::ORIGINAL_WIDTH;
		Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
		_game->getScreen()->resetDisplay(false);
	}
	else if (!_battleGame->getTileEngine())
	{
		Screen::updateScale(Options::battlescapeScale, Options::battlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, true);
		_game->getScreen()->resetDisplay(false);
	}

	// Create objects
	_bg = new Surface(960, 600, 0, 0);
	_soldier = new Surface(121, 215, 109, 85);
	_rank = new Surface(26, 23, 5, 5); //added for the rank icon
	_txtName = new Text(400, 17, 40, 10);
	_txtWeight = new Text(70, 9, 400, 52);
	_txtFAcc = new Text(40, 9, 400, 60);
	_txtReact = new Text(40, 9, 400, 68);
	_txtPSkill = new Text(40, 9, 400, 76);
	_txtPStr = new Text(40, 9, 400, 84);
	_txtItem = new Text(160, 9, 8, 310);
	_txtAmmo = new Text(80, 24, 8, 432);
	_btnOk = new InteractiveSurface(35, 22, 890, 5);
	_btnPrev = new InteractiveSurface(23, 22, 866, 5);
	_btnNext = new InteractiveSurface(23, 22, 926, 5);
	_btnUnload = new InteractiveSurface(32, 25, 8, 280);
	_btnGround = new InteractiveSurface(32, 15, 924, 442);
//	_btnRank = new InteractiveSurface(26, 23, 0, 0);
	_btnCreateTemplate = new InteractiveSurface(32, 22, _templateBtnX, _createTemplateBtnY);
	_btnApplyTemplate = new InteractiveSurface(32, 22, _templateBtnX, _applyTemplateBtnY);
	_selAmmo = new Surface(RuleInventory::HAND_W * RuleInventory::SLOT_W, RuleInventory::HAND_H * RuleInventory::SLOT_H, 8, 325);
	_inv = new Inventory(_game, 960, 600, 0, 0, _parent == 0);
	_txtRank = new Text(100, 10, 40, 38); // added
	_txtMissions = new Text(100, 10, 145, 38); // added
	_txtKills = new Text(100, 10, 250, 38); // added
	_txtStatus = new Text(100, 10, 355, 38); // added
	_txtCraft = new Text(150, 10, 460, 38); // added
	_txtArmor = new Text(150, 10, 565, 38); // added

	_txtTimeUnits = new Text(120, 9, 546, 100);
	_numTimeUnits = new Text(18, 9, 662, 100);
	_barTimeUnits = new Bar(200, 8, 680, 100);

	_txtEnergy = new Text(120, 9, 546, 113);
	_numEnergy = new Text(18, 9, 662, 113);
	_barEnergy = new Bar(200, 8, 680, 113);

	_txtHealth = new Text(120, 9, 546, 126);
	_numHealth = new Text(18, 9, 662, 126);
	_barHealth = new Bar(200, 8, 680, 126);

	_txtFatalWounds = new Text(120, 9, 546, 139);
	_numFatalWounds = new Text(18, 9, 662, 139);
	_barFatalWounds = new Bar(200, 8, 680, 139);

	_txtBravery = new Text(120, 9, 546, 152);
	_numBravery = new Text(18, 9, 662, 152);
	_barBravery = new Bar(200, 8, 680, 152);

	_txtMorale = new Text(120, 9, 546, 165);
	_numMorale = new Text(18, 9, 662, 165);
	_barMorale = new Bar(200, 8, 680, 165);

	_txtReactions = new Text(120, 9, 546, 178);
	_numReactions = new Text(18, 9, 662, 178);
	_barReactions = new Bar(200, 8, 680, 178);

	_txtFiring = new Text(120, 9, 546, 191);
	_numFiring = new Text(18, 9, 662, 191);
	_barFiring = new Bar(200, 8, 680, 191);

	_txtThrowing = new Text(120, 9, 546, 204);
	_numThrowing = new Text(18, 9, 662, 204);
	_barThrowing = new Bar(200, 8, 680, 204);

	_txtMelee = new Text(120, 9, 546, 217);
	_numMelee = new Text(18, 9, 662, 217);
	_barMelee = new Bar(200, 8, 680, 217);

	_txtStrength = new Text(120, 9, 546, 230);
	_numStrength = new Text(18, 9, 662, 230);
	_barStrength = new Bar(200, 8, 680, 230);

	_txtPsiStrength = new Text(120, 9, 546, 243);
	_numPsiStrength = new Text(18, 9, 662, 243);
	_barPsiStrength = new Bar(200, 8, 680, 243);

	_txtPsiSkill = new Text(120, 9, 546, 256);
	_numPsiSkill = new Text(18, 9, 662, 256);
	_barPsiSkill = new Bar(200, 8, 680, 256);

	_txtFrontArmor = new Text(120, 9, 546, 282);
	_numFrontArmor = new Text(18, 9, 662, 282);
	_barFrontArmor = new Bar(200, 8, 680, 282);

	_txtLeftArmor = new Text(120, 9, 546, 295);
	_numLeftArmor = new Text(18, 9, 662, 295);
	_barLeftArmor = new Bar(200, 8, 680, 295);

	_txtRightArmor = new Text(120, 9, 546, 308);
	_numRightArmor = new Text(18, 9, 662, 308);
	_barRightArmor = new Bar(200, 8, 680, 308);

	_txtRearArmor = new Text(120, 9, 546, 321);
	_numRearArmor = new Text(18, 9, 662, 321);
	_barRearArmor = new Bar(200, 8, 680, 321);

	_txtUnderArmor = new Text(120, 9, 546, 321);
	_numUnderArmor = new Text(18, 9, 662, 321);
	_barUnderArmor = new Bar(200, 8, 680, 321);

	if (!_mindProbe)
	{
		_btnPrev = new TextButton(14, 18, 2, 2);
		_btnNext = new TextButton(14, 18, 304, 2);
	}

	// Set palette
	setPalette("PAL_BATTLESCAPE");

	add(_bg);
	add(_soldier);
	add(_rank);
	add(_txtName);
	add(_txtItem);
	add(_txtAmmo);
	add(_btnOk);
	add(_btnPrev);
	add(_btnNext);
	add(_btnUnload);
	add(_btnGround);
	add(_btnCreateTemplate);
	add(_btnApplyTemplate);
	add(_selAmmo);
	add(_inv);

	add(_txtRank); // added
	add(_txtCraft); // added
	add(_txtMissions); // added
	add(_txtKills); // added
	add(_txtStatus); // added
	add(_txtArmor); // added

	//Adding all Items for Soldier Info
	add(_txtTimeUnits);
	add(_numTimeUnits);
	add(_barTimeUnits);

	add(_txtEnergy);
	add(_numEnergy);
	add(_barEnergy);

	add(_txtHealth);
	add(_numHealth);
	add(_barHealth);

	add(_txtFatalWounds);
	add(_numFatalWounds);
	add(_barFatalWounds);

	add(_txtBravery);
	add(_numBravery);
	add(_barBravery);

	add(_txtMorale);
	add(_numMorale);
	add(_barMorale);

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

	add(_txtFrontArmor);
	add(_numFrontArmor);
	add(_barFrontArmor);

	add(_txtLeftArmor);
	add(_numLeftArmor);
	add(_barLeftArmor);

	add(_txtRightArmor);
	add(_numRightArmor);
	add(_barRightArmor);

	add(_txtRearArmor);
	add(_numRearArmor);
	add(_barRearArmor);

	add(_txtUnderArmor);
	add(_numUnderArmor);
	add(_barUnderArmor);

	if (!_mindProbe)
	{
		add(_btnPrev);
		add(_btnNext);
	}

	//centerAllSurfaces();

	// Set up objects
	_game->getResourcePack()->getSurface("HDTAC01.PNG")->blit(_bg);

	_txtName->setColor(Palette::blockOffset(4));
	_txtName->setBig();
	_txtName->setHighContrast(true);

	_txtItem->setColor(Palette::blockOffset(3));
	_txtItem->setHighContrast(true);

	_txtAmmo->setColor(Palette::blockOffset(4));
	_txtAmmo->setSecondaryColor(Palette::blockOffset(1));
	_txtAmmo->setAlign(ALIGN_LEFT);
	_txtAmmo->setHighContrast(true);

	_btnOk->onMouseClick((ActionHandler)&InventoryState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&InventoryState::btnOkClick, Options::keyCancel);
	_btnOk->onKeyboardPress((ActionHandler)&InventoryState::btnOkClick, Options::keyBattleInventory);
	_btnOk->setTooltip("STR_OK");
	_btnOk->onMouseIn((ActionHandler)&InventoryState::txtTooltipIn);
	_btnOk->onMouseOut((ActionHandler)&InventoryState::txtTooltipOut);

	_btnPrev->onMouseClick((ActionHandler)&InventoryState::btnPrevClick);
	_btnPrev->onKeyboardPress((ActionHandler)&InventoryState::btnPrevClick, Options::keyBattlePrevUnit);
	_btnPrev->setTooltip("STR_PREVIOUS_UNIT");
	_btnPrev->onMouseIn((ActionHandler)&InventoryState::txtTooltipIn);
	_btnPrev->onMouseOut((ActionHandler)&InventoryState::txtTooltipOut);

	_btnNext->onMouseClick((ActionHandler)&InventoryState::btnNextClick);
	_btnNext->onKeyboardPress((ActionHandler)&InventoryState::btnNextClick, Options::keyBattleNextUnit);
	_btnNext->setTooltip("STR_NEXT_UNIT");
	_btnNext->onMouseIn((ActionHandler)&InventoryState::txtTooltipIn);
	_btnNext->onMouseOut((ActionHandler)&InventoryState::txtTooltipOut);

	_btnUnload->onMouseClick((ActionHandler)&InventoryState::btnUnloadClick);
	_btnUnload->setTooltip("STR_UNLOAD_WEAPON");
	_btnUnload->onMouseIn((ActionHandler)&InventoryState::txtTooltipIn);
	_btnUnload->onMouseOut((ActionHandler)&InventoryState::txtTooltipOut);

	_btnGround->onMouseClick((ActionHandler)&InventoryState::btnGroundClick);
	_btnGround->setTooltip("STR_SCROLL_RIGHT");
	_btnGround->onMouseIn((ActionHandler)&InventoryState::txtTooltipIn);
	_btnGround->onMouseOut((ActionHandler)&InventoryState::txtTooltipOut);

	_btnCreateTemplate->onMouseClick((ActionHandler)&InventoryState::btnCreateTemplateClick);
	_btnCreateTemplate->onKeyboardPress((ActionHandler)&InventoryState::btnCreateTemplateClick, Options::keyInvCreateTemplate);
	_btnCreateTemplate->setTooltip("STR_CREATE_INVENTORY_TEMPLATE");
	_btnCreateTemplate->onMouseIn((ActionHandler)&InventoryState::txtTooltipIn);
	_btnCreateTemplate->onMouseOut((ActionHandler)&InventoryState::txtTooltipOut);

	_btnApplyTemplate->onMouseClick((ActionHandler)&InventoryState::btnApplyTemplateClick);
	_btnApplyTemplate->onKeyboardPress((ActionHandler)&InventoryState::btnApplyTemplateClick, Options::keyInvApplyTemplate);
	_btnApplyTemplate->onKeyboardPress((ActionHandler)&InventoryState::onClearInventory, Options::keyInvClear);
	_btnApplyTemplate->setTooltip("STR_APPLY_INVENTORY_TEMPLATE");
	_btnApplyTemplate->onMouseIn((ActionHandler)&InventoryState::txtTooltipIn);
	_btnApplyTemplate->onMouseOut((ActionHandler)&InventoryState::txtTooltipOut);

	_txtTimeUnits->setColor(Palette::blockOffset(3));
	_txtTimeUnits->setHighContrast(true);
	_txtTimeUnits->setText(tr("STR_TIME_UNITS"));

	_numTimeUnits->setColor(Palette::blockOffset(9));
	_numTimeUnits->setHighContrast(true);

	_barTimeUnits->setColor(Palette::blockOffset(4));
	_barTimeUnits->setScale(1.0);

	_txtEnergy->setColor(Palette::blockOffset(3));
	_txtEnergy->setHighContrast(true);
	_txtEnergy->setText(tr("STR_ENERGY"));

	_numEnergy->setColor(Palette::blockOffset(9));
	_numEnergy->setHighContrast(true);

	_barEnergy->setColor(Palette::blockOffset(9));
	_barEnergy->setScale(1.0);

	_txtHealth->setColor(Palette::blockOffset(3));
	_txtHealth->setHighContrast(true);
	_txtHealth->setText(tr("STR_HEALTH"));

	_numHealth->setColor(Palette::blockOffset(9));
	_numHealth->setHighContrast(true);

	_barHealth->setColor(Palette::blockOffset(2));
	_barHealth->setColor2(Palette::blockOffset(5) + 2);
	_barHealth->setScale(1.0);

	_txtFatalWounds->setColor(Palette::blockOffset(3));
	_txtFatalWounds->setHighContrast(true);
	_txtFatalWounds->setText(tr("STR_FATAL_WOUNDS"));

	_numFatalWounds->setColor(Palette::blockOffset(9));
	_numFatalWounds->setHighContrast(true);

	_barFatalWounds->setColor(Palette::blockOffset(2));
	_barFatalWounds->setScale(1.0);

	_txtBravery->setColor(Palette::blockOffset(3));
	_txtBravery->setHighContrast(true);
	_txtBravery->setText(tr("STR_BRAVERY"));

	_numBravery->setColor(Palette::blockOffset(9));
	_numBravery->setHighContrast(true);

	_barBravery->setColor(Palette::blockOffset(12));
	_barBravery->setScale(1.0);

	_txtMorale->setColor(Palette::blockOffset(3));
	_txtMorale->setHighContrast(true);
	_txtMorale->setText(tr("STR_MORALE"));

	_numMorale->setColor(Palette::blockOffset(9));
	_numMorale->setHighContrast(true);

	_barMorale->setColor(Palette::blockOffset(12));
	_barMorale->setScale(1.0);

	_txtReactions->setColor(Palette::blockOffset(3));
	_txtReactions->setHighContrast(true);
	_txtReactions->setText(tr("STR_REACTIONS"));

	_numReactions->setColor(Palette::blockOffset(9));
	_numReactions->setHighContrast(true);

	_barReactions->setColor(Palette::blockOffset(9));
	_barReactions->setScale(1.0);

	_txtFiring->setColor(Palette::blockOffset(3));
	_txtFiring->setHighContrast(true);
	_txtFiring->setText(tr("STR_FIRING_ACCURACY"));

	_numFiring->setColor(Palette::blockOffset(9));
	_numFiring->setHighContrast(true);

	_barFiring->setColor(Palette::blockOffset(8));
	_barFiring->setScale(1.0);

	_txtThrowing->setColor(Palette::blockOffset(3));
	_txtThrowing->setHighContrast(true);
	_txtThrowing->setText(tr("STR_THROWING_ACCURACY"));

	_numThrowing->setColor(Palette::blockOffset(9));
	_numThrowing->setHighContrast(true);

	_barThrowing->setColor(Palette::blockOffset(6));
	_barThrowing->setScale(1.0);

	_txtMelee->setColor(Palette::blockOffset(3));
	_txtMelee->setHighContrast(true);
	_txtMelee->setText(tr("STR_MELEE_ACCURACY"));

	_numMelee->setColor(Palette::blockOffset(9));
	_numMelee->setHighContrast(true);

	_barMelee->setColor(Palette::blockOffset(14));
	_barMelee->setScale(1.0);

	_txtStrength->setColor(Palette::blockOffset(3));
	_txtStrength->setHighContrast(true);
	_txtStrength->setText(tr("STR_STRENGTH"));

	_numStrength->setColor(Palette::blockOffset(9));
	_numStrength->setHighContrast(true);

	_barStrength->setColor(Palette::blockOffset(3));
	_barStrength->setScale(1.0);

	_txtPsiStrength->setColor(Palette::blockOffset(3));
	_txtPsiStrength->setHighContrast(true);
	_txtPsiStrength->setText(tr("STR_PSIONIC_STRENGTH"));

	_numPsiStrength->setColor(Palette::blockOffset(9));
	_numPsiStrength->setHighContrast(true);

	_barPsiStrength->setColor(Palette::blockOffset(12));
	_barPsiStrength->setScale(1.0);

	_txtPsiSkill->setColor(Palette::blockOffset(3));
	_txtPsiSkill->setHighContrast(true);
	_txtPsiSkill->setText(tr("STR_PSIONIC_SKILL"));

	_numPsiSkill->setColor(Palette::blockOffset(9));
	_numPsiSkill->setHighContrast(true);

	_barPsiSkill->setColor(Palette::blockOffset(12));
	_barPsiSkill->setScale(1.0);

	_txtFrontArmor->setColor(Palette::blockOffset(3));
	_txtFrontArmor->setHighContrast(true);
	_txtFrontArmor->setText(tr("STR_FRONT_ARMOR_UC"));

	_numFrontArmor->setColor(Palette::blockOffset(9));
	_numFrontArmor->setHighContrast(true);

	_barFrontArmor->setColor(Palette::blockOffset(5));
	_barFrontArmor->setScale(1.0);

	_txtLeftArmor->setColor(Palette::blockOffset(3));
	_txtLeftArmor->setHighContrast(true);
	_txtLeftArmor->setText(tr("STR_LEFT_ARMOR_UC"));

	_numLeftArmor->setColor(Palette::blockOffset(9));
	_numLeftArmor->setHighContrast(true);

	_barLeftArmor->setColor(Palette::blockOffset(5));
	_barLeftArmor->setScale(1.0);

	_txtRightArmor->setColor(Palette::blockOffset(3));
	_txtRightArmor->setHighContrast(true);
	_txtRightArmor->setText(tr("STR_RIGHT_ARMOR_UC"));

	_numRightArmor->setColor(Palette::blockOffset(9));
	_numRightArmor->setHighContrast(true);

	_barRightArmor->setColor(Palette::blockOffset(5));
	_barRightArmor->setScale(1.0);

	_txtRearArmor->setColor(Palette::blockOffset(3));
	_txtRearArmor->setHighContrast(true);
	_txtRearArmor->setText(tr("STR_REAR_ARMOR_UC"));

	_numRearArmor->setColor(Palette::blockOffset(9));
	_numRearArmor->setHighContrast(true);

	_barRearArmor->setColor(Palette::blockOffset(5));
	_barRearArmor->setScale(1.0);

	_txtUnderArmor->setColor(Palette::blockOffset(3));
	_txtUnderArmor->setHighContrast(true);
	_txtUnderArmor->setText(tr("STR_UNDER_ARMOR_UC"));

	_numUnderArmor->setColor(Palette::blockOffset(9));
	_numUnderArmor->setHighContrast(true);

	_barUnderArmor->setColor(Palette::blockOffset(5));
	_barUnderArmor->setScale(1.0);

	if (!_mindProbe)
	{
		_btnPrev = new TextButton(14, 18, 2, 2);
		_btnNext = new TextButton(14, 18, 304, 2);
	}

	// only use copy/paste buttons in setup (i.e. non-tu) mode
	if (_tu)
	{
		_btnCreateTemplate->setVisible(false);
		_btnApplyTemplate->setVisible(false);
	}
	else
	{
		_updateTemplateButtons(true);
	}

	_inv->draw();
	_inv->setTuMode(_tu);
	_inv->setSelectedUnit(_game->getSavedGame()->getSavedBattle()->getSelectedUnit());
	_inv->onMouseClick((ActionHandler)&InventoryState::invClick, 0);
	_inv->onMouseOver((ActionHandler)&InventoryState::invMouseOver);
	_inv->onMouseOut((ActionHandler)&InventoryState::invMouseOut);

	_currentTooltip = "";

	//Setup object for the top row labels of stats
	_txtRank->setColor(Palette::blockOffset(3));
	_txtRank->setSecondaryColor(Palette::blockOffset(9));
	_txtRank->setHighContrast(true);

	_txtCraft->setColor(Palette::blockOffset(3));
	_txtCraft->setSecondaryColor(Palette::blockOffset(9));
	_txtCraft->setHighContrast(true);

	_txtMissions->setColor(Palette::blockOffset(3));
	_txtMissions->setSecondaryColor(Palette::blockOffset(9));
	_txtMissions->setHighContrast(true);

	_txtKills->setColor(Palette::blockOffset(3));
	_txtKills->setSecondaryColor(Palette::blockOffset(9));
	_txtKills->setHighContrast(true);

	_txtStatus->setColor(Palette::blockOffset(3));
	_txtStatus->setSecondaryColor(Palette::blockOffset(9));
	_txtStatus->setHighContrast(true);

	_txtArmor->setColor(Palette::blockOffset(3));
	_txtArmor->setSecondaryColor(Palette::blockOffset(9));
	_txtArmor->setHighContrast(true);
}

static void _clearInventoryTemplate(std::vector<EquipmentLayoutItem*> &inventoryTemplate)
{
	for (std::vector<EquipmentLayoutItem*>::iterator eraseIt = inventoryTemplate.begin();
		 eraseIt != inventoryTemplate.end();
		 eraseIt = inventoryTemplate.erase(eraseIt))
	{
		delete *eraseIt;
	}
}

/**
 *
 */
InventoryState::~InventoryState()
{
	_clearInventoryTemplate(_curInventoryTemplate);

	if (_battleGame->getTileEngine())
	{
		if (Options::maximizeInfoScreens)
		{
			Screen::updateScale(Options::battlescapeScale, Options::battlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, true);
			_game->getScreen()->resetDisplay(false);
		}
		Tile *inventoryTile = _battleGame->getSelectedUnit()->getTile();
		_battleGame->getTileEngine()->applyGravity(inventoryTile);
		_battleGame->getTileEngine()->calculateTerrainLighting(); // dropping/picking up flares
		_battleGame->getTileEngine()->recalculateFOV();
	}
	else
	{
		Screen::updateScale(Options::geoscapeScale, Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, true);
		_game->getScreen()->resetDisplay(false);
	}
}

/**
 * Updates all soldier stats when the soldier changes.
 */
void InventoryState::init()
{
	State::init();
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

	_txtName->setBig();
	_txtName->setText(unit->getName(_game->getLanguage()));
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

	updateStats();
	_refreshMouse();
}

/**
 * Updates the soldier stas and bars when Stats button is selected (Weight, TU).
 */
void InventoryState::updateStats()
{
	BattleUnit *unit = _battleGame->getSelectedUnit();

	// Adding info menu  
	_txtRank->setText(tr("STR_RANK_").arg(tr(unit->getRankString())));
	_txtCraft->setText(tr("STR_CRAFT_").arg(unit->getGeoscapeSoldier()->getCraftString(_game->getLanguage())));
	_txtMissions->setText(tr("STR_MISSIONS").arg(unit->getGeoscapeSoldier()->getMissions()));
	_txtKills->setText(tr("STR_KILLS").arg(unit->getGeoscapeSoldier()->getKills()));

	// Adding text for all bars
	std::wostringstream ss;
	ss << unit->getTimeUnits();
	_numTimeUnits->setText(ss.str());
	_barTimeUnits->setMax(unit->getStats()->tu);
	_barTimeUnits->setValue(unit->getTimeUnits());

	ss.str(L"");
	// aliens have their rank in their "name", soldiers don't
	if (unit->getType() == "SOLDIER")
	{
		ss << tr(unit->getRankString());
		ss << " ";
	}
	ss << unit->getName(_game->getLanguage(), BattlescapeGame::_debugPlay);
	_txtName->setBig();
	_txtName->setText(ss.str());

	ss.str(L"");
	ss << unit->getEnergy();
	_numEnergy->setText(ss.str());
	_barEnergy->setMax(unit->getStats()->stamina);
	_barEnergy->setValue(unit->getEnergy());

	ss.str(L"");
	ss << unit->getHealth();
	_numHealth->setText(ss.str());
	_barHealth->setMax(unit->getStats()->health);
	_barHealth->setValue(unit->getHealth());
	_barHealth->setValue2(unit->getStunlevel());

	ss.str(L"");
	ss << unit->getFatalWounds();
	_numFatalWounds->setText(ss.str());
	_barFatalWounds->setMax(unit->getFatalWounds());
	_barFatalWounds->setValue(unit->getFatalWounds());

	ss.str(L"");
	ss << unit->getStats()->bravery;
	_numBravery->setText(ss.str());
	_barBravery->setMax(unit->getStats()->bravery);
	_barBravery->setValue(unit->getStats()->bravery);

	ss.str(L"");
	ss << unit->getMorale();
	_numMorale->setText(ss.str());
	_barMorale->setMax(100);
	_barMorale->setValue(unit->getMorale());

	ss.str(L"");
	ss << unit->getStats()->reactions;
	_numReactions->setText(ss.str());
	_barReactions->setMax(unit->getStats()->reactions);
	_barReactions->setValue(unit->getStats()->reactions);

	ss.str(L"");
	ss << (int)((unit->getStats()->firing * unit->getHealth()) / unit->getStats()->health);
	_numFiring->setText(ss.str());
	_barFiring->setMax(unit->getStats()->firing);
	_barFiring->setValue((unit->getStats()->firing * unit->getHealth()) / unit->getStats()->health);

	ss.str(L"");
	ss << (int)((unit->getStats()->throwing * unit->getHealth()) / unit->getStats()->health);
	_numThrowing->setText(ss.str());
	_barThrowing->setMax(unit->getStats()->throwing);
	_barThrowing->setValue((unit->getStats()->throwing * unit->getHealth()) / unit->getStats()->health);

	ss.str(L"");
	ss << (int)((unit->getStats()->melee * unit->getHealth()) / unit->getStats()->health);
	_numMelee->setText(ss.str());
	_barMelee->setMax(unit->getStats()->melee);
	_barMelee->setValue((unit->getStats()->melee * unit->getHealth()) / unit->getStats()->health);

	ss.str(L"");
	ss << unit->getStats()->strength;
	_numStrength->setText(ss.str());
	_barStrength->setMax(unit->getStats()->strength);
	_barStrength->setValue(unit->getStats()->strength);

	if (unit->getStats()->psiSkill > 0 || (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getRuleset()->getPsiRequirements())))
	{
		ss.str(L"");
		ss << unit->getStats()->psiStrength;
		_numPsiStrength->setText(ss.str());
		_barPsiStrength->setMax(unit->getStats()->psiStrength);
		_barPsiStrength->setValue(unit->getStats()->psiStrength);

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

	if (unit->getStats()->psiSkill > 0)
	{
		ss.str(L"");
		ss << unit->getStats()->psiSkill;
		_numPsiSkill->setText(ss.str());
		_barPsiSkill->setMax(unit->getStats()->psiSkill);
		_barPsiSkill->setValue(unit->getStats()->psiSkill);

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

	ss.str(L"");
	ss << unit->getArmor(SIDE_FRONT);
	_numFrontArmor->setText(ss.str());
	_barFrontArmor->setMax(unit->getArmor()->getFrontArmor());
	_barFrontArmor->setValue(unit->getArmor(SIDE_FRONT));

	ss.str(L"");
	ss << unit->getArmor(SIDE_LEFT);
	_numLeftArmor->setText(ss.str());
	_barLeftArmor->setMax(unit->getArmor()->getSideArmor());
	_barLeftArmor->setValue(unit->getArmor(SIDE_LEFT));

	ss.str(L"");
	ss << unit->getArmor(SIDE_RIGHT);
	_numRightArmor->setText(ss.str());
	_barRightArmor->setMax(unit->getArmor()->getSideArmor());
	_barRightArmor->setValue(unit->getArmor(SIDE_RIGHT));

	ss.str(L"");
	ss << unit->getArmor(SIDE_REAR);
	_numRearArmor->setText(ss.str());
	_barRearArmor->setMax(unit->getArmor()->getRearArmor());
	_barRearArmor->setValue(unit->getArmor(SIDE_REAR));

	ss.str(L"");
	ss << unit->getArmor(SIDE_UNDER);
	_numUnderArmor->setText(ss.str());
	_barUnderArmor->setMax(unit->getArmor()->getUnderArmor());
	_barUnderArmor->setValue(unit->getArmor(SIDE_UNDER));

}

/**
 * Saves the soldiers' equipment-layout.
 */
void InventoryState::saveEquipmentLayout()
{
	for (std::vector<BattleUnit*>::iterator i = _battleGame->getUnits()->begin(); i != _battleGame->getUnits()->end(); ++i)
	{
		// we need X-Com soldiers only
		if (0 == (*i)->getGeoscapeSoldier()) continue;

		std::vector<EquipmentLayoutItem*> *layoutItems = (*i)->getGeoscapeSoldier()->getEquipmentLayout();

		// clear the previous save
		if (!layoutItems->empty())
		{
			for (std::vector<EquipmentLayoutItem*>::iterator j = layoutItems->begin(); j != layoutItems->end(); ++j)
				delete *j;
			layoutItems->clear();
		}

		// save the soldier's items
		// note: with using getInventory() we are skipping the ammos loaded, (they're not owned) because we handle the loaded-ammos separately (inside)
		for (std::vector<BattleItem*>::iterator j = (*i)->getInventory()->begin(); j != (*i)->getInventory()->end(); ++j)
		{
			std::string ammo;
			if ((*j)->needsAmmo() && 0 != (*j)->getAmmoItem()) ammo = (*j)->getAmmoItem()->getRules()->getType();
			else ammo = "NONE";
			layoutItems->push_back(new EquipmentLayoutItem(
				(*j)->getRules()->getType(),
				(*j)->getSlot()->getId(),
				(*j)->getSlotX(),
				(*j)->getSlotY(),
				ammo,
				(*j)->getFuseTimer()
			));
		}
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void InventoryState::btnOkClick(Action *)
{
	if (_inv->getSelectedItem() != 0)
		return;
	_game->popState();
	Tile *inventoryTile = _battleGame->getSelectedUnit()->getTile();
	if (!_tu)
	{
		saveEquipmentLayout();
		_battleGame->resetUnitTiles();
		if (_battleGame->getTurn() == 1)
		{
			_battleGame->randomizeItemLocations(inventoryTile);
			if (inventoryTile->getUnit())
			{
				// make sure we select the unit closest to the ramp.
				_battleGame->setSelectedUnit(inventoryTile->getUnit());
			}
		}

		// initialize xcom units for battle
		for (std::vector<BattleUnit*>::iterator j = _battleGame->getUnits()->begin(); j != _battleGame->getUnits()->end(); ++j)
		{
			if ((*j)->getOriginalFaction() != FACTION_PLAYER || (*j)->isOut())
			{
				continue;
			}

			(*j)->prepareNewTurn();
		}
	}
}

/**
 * Selects the previous soldier.
 * @param action Pointer to an action.
 */
void InventoryState::btnPrevClick(Action *)
{
	if (_inv->getSelectedItem() != 0)
	{
		return;
	}

	if (_parent)
	{
		_parent->selectPreviousPlayerUnit(false, false, true);
	}
	else
	{
		_battleGame->selectPreviousPlayerUnit(false, false, true);
	}
	init();
}

/**
 * Selects the next soldier.
 * @param action Pointer to an action.
 */
void InventoryState::btnNextClick(Action *)
{
	if (_inv->getSelectedItem() != 0)
	{
		return;
	}
	if (_parent)
	{
		_parent->selectNextPlayerUnit(false, false, true);
	}
	else
	{
		_battleGame->selectNextPlayerUnit(false, false, true);
	}
	init();
}

/**
 * Unloads the selected weapon.
 * @param action Pointer to an action.
 */
void InventoryState::btnUnloadClick(Action *)
{
	if (_inv->unload())
	{
		_txtItem->setText(L"");
		_txtAmmo->setText(L"");
		_selAmmo->clear();
		updateStats();
	}
}

/**
 * Shows more ground items / rearranges them.
 * @param action Pointer to an action.
 */
void InventoryState::btnGroundClick(Action *)
{
	_inv->arrangeGround();
}

/**
 * Shows the unit info screen.
 * @param action Pointer to an action.
 */
void InventoryState::btnRankClick(Action *)
{
	_game->pushState(new UnitInfoState(_battleGame->getSelectedUnit(), _parent, true, false));
}

void InventoryState::btnCreateTemplateClick(Action *action)
{
	// don't accept clicks when moving items
	if (_inv->getSelectedItem() != 0)
	{
		return;
	}

	// clear current template
	_clearInventoryTemplate(_curInventoryTemplate);

	// copy inventory instead of just keeping a pointer to it.  that way
	// create/apply can be used as an undo button for a single unit and will
	// also work as expected if inventory is modified after 'create' is clicked
	std::vector<BattleItem*> *unitInv = _battleGame->getSelectedUnit()->getInventory();
	for (std::vector<BattleItem*>::iterator j = unitInv->begin(); j != unitInv->end(); ++j)
	{
		std::string ammo;
		if ((*j)->needsAmmo() && (*j)->getAmmoItem())
		{
			ammo = (*j)->getAmmoItem()->getRules()->getType();
		}
		else
		{
			ammo = "NONE";
		}

		_curInventoryTemplate.push_back(new EquipmentLayoutItem(
				(*j)->getRules()->getType(),
				(*j)->getSlot()->getId(),
				(*j)->getSlotX(),
				(*j)->getSlotY(),
				ammo,
				(*j)->getFuseTimer()));
	}

	// give audio feedback
	_game->getResourcePack()->getSound("BATTLE.CAT", 38)->play();
	_refreshMouse();
}

static void _clearInventory(Game *game, std::vector<BattleItem*> *unitInv, Tile *groundTile)
{
	RuleInventory *groundRuleInv = game->getRuleset()->getInventory("STR_GROUND");

	// clear unit's inventory (i.e. move everything to the ground)
	for (std::vector<BattleItem*>::iterator i = unitInv->begin(); i != unitInv->end(); )
	{
		(*i)->setOwner(NULL);
		groundTile->addItem(*i, groundRuleInv);
		i = unitInv->erase(i);
	}
}

void InventoryState::btnApplyTemplateClick(Action *action)
{
	// don't accept clicks when moving items
	// it's ok if the template is empty -- it will just result in clearing the
	// unit's inventory
	if (_inv->getSelectedItem() != 0)
	{
		return;
	}

	BattleUnit               *unit          = _battleGame->getSelectedUnit();
	std::vector<BattleItem*> *unitInv       = unit->getInventory();
	Tile                     *groundTile    = unit->getTile();
	std::vector<BattleItem*> *groundInv     = groundTile->getInventory();
	RuleInventory            *groundRuleInv = _game->getRuleset()->getInventory("STR_GROUND");

	_clearInventory(_game, unitInv, groundTile);

	// attempt to replicate inventory template by grabbing corresponding items
	// from the ground.  if any item is not found on the ground, display warning
	// message, but continue attempting to fulfill the template as best we can
	bool itemMissing = false;
	std::vector<EquipmentLayoutItem*>::iterator templateIt;
	for (templateIt = _curInventoryTemplate.begin(); templateIt != _curInventoryTemplate.end(); ++templateIt)
	{
		// search for template item in ground inventory
		std::vector<BattleItem*>::iterator groundItem;
		const bool needsAmmo = !_game->getRuleset()->getItem((*templateIt)->getItemType())->getCompatibleAmmo()->empty();
		bool found = false;
		bool rescan = true;
		while (rescan)
		{
			rescan = false;

			const std::string targetAmmo = (*templateIt)->getAmmoItem();
			BattleItem *matchedWeapon = NULL;
			BattleItem *matchedAmmo   = NULL;
			for (groundItem = groundInv->begin(); groundItem != groundInv->end(); ++groundItem)
			{
				// if we find the appropriate ammo, remember it for later for if we find
				// the right weapon but with the wrong ammo
				const std::string groundItemName = (*groundItem)->getRules()->getType();
				if (needsAmmo && targetAmmo == groundItemName)
				{
					matchedAmmo = *groundItem;
				}

				if ((*templateIt)->getItemType() == groundItemName)
				{
					// if the loaded ammo doesn't match the template item's,
					// remember the weapon for later and continue scanning
					BattleItem *loadedAmmo = (*groundItem)->getAmmoItem();
					if ((needsAmmo && loadedAmmo && targetAmmo != loadedAmmo->getRules()->getType())
					 || (needsAmmo && !loadedAmmo))
					{
						// remember the last matched weapon for simplicity (but prefer empty weapons if any are found)
						if (!matchedWeapon || matchedWeapon->getAmmoItem())
						{
							matchedWeapon = *groundItem;
						}
						continue;
					}

					// move matched item from ground to the appropriate inv slot
					(*groundItem)->setOwner(unit);
					(*groundItem)->setSlot(_game->getRuleset()->getInventory((*templateIt)->getSlot()));
					(*groundItem)->setSlotX((*templateIt)->getSlotX());
					(*groundItem)->setSlotY((*templateIt)->getSlotY());
					(*groundItem)->setFuseTimer((*templateIt)->getFuseTimer());
					unitInv->push_back(*groundItem);
					groundInv->erase(groundItem);
					found = true;
					break;
				}
			}

			// if we failed to find an exact match, but found unloaded ammo and
			// the right weapon, unload the target weapon, load the right ammo, and use it
			if (!found && matchedWeapon && (!needsAmmo || matchedAmmo))
			{
				// unload the existing ammo (if any) from the weapon
				BattleItem *loadedAmmo = matchedWeapon->getAmmoItem();
				if (loadedAmmo)
				{
					groundTile->addItem(loadedAmmo, groundRuleInv);
					matchedWeapon->setAmmoItem(NULL);
				}

				// load the correct ammo into the weapon
				if (matchedAmmo)
				{
					matchedWeapon->setAmmoItem(matchedAmmo);
					groundTile->removeItem(matchedAmmo);
				}

				// rescan and pick up the newly-loaded/unloaded weapon
				rescan = true;
			}
		}

		if (!found)
		{
			itemMissing = true;
		}
	}

	if (itemMissing)
	{
		_inv->showWarning(tr("STR_NOT_ENOUGH_ITEMS_FOR_TEMPLATE"));
	}

	// refresh ui
	_inv->arrangeGround(false);
	updateStats();
	_refreshMouse();

	// give audio feedback
	_game->getResourcePack()->getSound("BATTLE.CAT", 38)->play();
}

void InventoryState::_refreshMouse()
{
	// send a mouse motion event to refresh any hover actions
	int x, y;
	SDL_GetMouseState(&x, &y);
	SDL_WarpMouse(x+1, y);
	
	// move the mouse back to avoid cursor creep
	SDL_WarpMouse(x, y);
}

void InventoryState::onClearInventory(Action *action)
{
	// don't accept clicks when moving items
	if (_inv->getSelectedItem() != 0)
	{
		return;
	}

	BattleUnit               *unit       = _battleGame->getSelectedUnit();
	std::vector<BattleItem*> *unitInv    = unit->getInventory();
	Tile                     *groundTile = unit->getTile();

	_clearInventory(_game, unitInv, groundTile);

	// refresh ui
	_inv->arrangeGround(false);
	updateStats();
	_refreshMouse();

	// give audio feedback
	_game->getResourcePack()->getSound("BATTLE.CAT", 38)->play();
}

/**
 * Updates item info.
 * @param action Pointer to an action.
 */
void InventoryState::invClick(Action *)
{
	updateStats();
}

/**
 * Shows item info.
 * @param action Pointer to an action.
 */
void InventoryState::invMouseOver(Action *)
{
	if (_inv->getSelectedItem() != 0)
	{
		return;
	}

	BattleItem *item = _inv->getMouseOverItem();
	if (item != 0)
	{
		if (item->getUnit() && item->getUnit()->getStatus() == STATUS_UNCONSCIOUS)
		{
			_txtItem->setText(item->getUnit()->getName(_game->getLanguage()));
		}
		else
		{
			if (_game->getSavedGame()->isResearched(item->getRules()->getRequirements()))
			{
				_txtItem->setText(tr(item->getRules()->getName()));
			}
			else
			{
				_txtItem->setText(tr("STR_ALIEN_ARTIFACT"));
			}
		}
		std::wstring s;
		if (item->getAmmoItem() != 0 && item->needsAmmo())
		{
			s = tr("STR_AMMO_ROUNDS_LEFT").arg(item->getAmmoItem()->getAmmoQuantity());
			SDL_Rect r;
			r.x = 0;
			r.y = 0;
			r.w = RuleInventory::HAND_W * RuleInventory::SLOT_W;
			r.h = RuleInventory::HAND_H * RuleInventory::SLOT_H;
			_selAmmo->drawRect(&r, Palette::blockOffset(0)+8);
			r.x++;
			r.y++;
			r.w -= 2;
			r.h -= 2;
			_selAmmo->drawRect(&r, Palette::blockOffset(0)+15);
			item->getAmmoItem()->getRules()->drawHandSprite(_game->getResourcePack()->getSurfaceSet("BIGOBS.PCK"), _selAmmo);
			_updateTemplateButtons(false);
		}
		else
		{
			_selAmmo->clear();
			_updateTemplateButtons(!_tu);
		}
		if (item->getAmmoQuantity() != 0 && item->needsAmmo())
		{
			s = tr("STR_AMMO_ROUNDS_LEFT").arg(item->getAmmoQuantity());
		}
		else if (item->getRules()->getBattleType() == BT_MEDIKIT)
		{
			s = tr("STR_MEDI_KIT_QUANTITIES_LEFT").arg(item->getPainKillerQuantity()).arg(item->getStimulantQuantity()).arg(item->getHealQuantity());
		}
		_txtAmmo->setText(s);
	}
	else
	{
		if (_currentTooltip == "")
		{
			_txtItem->setText(L"");
		}
		_txtAmmo->setText(L"");
		_selAmmo->clear();
		_updateTemplateButtons(!_tu);
	}
}

/**
 * Hides item info.
 * @param action Pointer to an action.
 */
void InventoryState::invMouseOut(Action *)
{
	_txtItem->setText(L"");
	_txtAmmo->setText(L"");
	_selAmmo->clear();
	_updateTemplateButtons(!_tu);
}

/**
 * Takes care of any events from the core game engine.
 * @param action Pointer to an action.
 */
void InventoryState::handle(Action *action)
{
	State::handle(action);

#ifndef __MORPHOS__
	if (action->getDetails()->type == SDL_MOUSEBUTTONDOWN)
	{
		if (action->getDetails()->button.button == SDL_BUTTON_X1)
		{
			btnNextClick(action);
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_X2)
		{
			btnPrevClick(action);
		}
	}
#endif
}

/**
* Shows a tooltip for the appropriate button.
* @param action Pointer to an action.
*/
void InventoryState::txtTooltipIn(Action *action)
{
	if (_inv->getSelectedItem() == 0 && Options::battleTooltips)
	{
		_currentTooltip = action->getSender()->getTooltip();
		_txtItem->setText(tr(_currentTooltip));
	}
}

/**
* Clears the tooltip text.
* @param action Pointer to an action.
*/
void InventoryState::txtTooltipOut(Action *action)
{
	if (_inv->getSelectedItem() == 0 && Options::battleTooltips)
	{
		if (_currentTooltip == action->getSender()->getTooltip())
		{
			_currentTooltip = "";
			_txtItem->setText(L"");
		}
	}
}

void InventoryState::_updateTemplateButtons(bool isVisible)
{
	if (isVisible)
	{
		if (_curInventoryTemplate.empty())
		{
			// use "empty template" icons
			_game->getResourcePack()->getSurface("InvCopy")->blit(_btnCreateTemplate);
			_game->getResourcePack()->getSurface("InvPasteEmpty")->blit(_btnApplyTemplate);
		}
		else
		{
			// use "active template" icons
			_game->getResourcePack()->getSurface("InvCopyActive")->blit(_btnCreateTemplate);
			_game->getResourcePack()->getSurface("InvPaste")->blit(_btnApplyTemplate);
		}
	}
	else
	{
		_btnCreateTemplate->clear();
		_btnApplyTemplate->clear();
	}
}

}

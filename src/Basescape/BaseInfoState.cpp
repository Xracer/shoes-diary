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
#include "BaseInfoState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/Bar.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Interface/TextList.h"
#include "../Interface/Window.h"
#include "../Engine/Surface.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleCraft.h"
#include "MiniBaseView.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "MonthlyCostsState.h"
#include "TransfersState.h"
#include "StoresState.h"
#include "BasescapeState.h"
#include "../Savegame/Transfer.h"
#include "../Savegame/ItemContainer.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Base Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param state Pointer to the Basescape state.
 */
BaseInfoState::BaseInfoState(Base *base, BasescapeState *state) : _base(base), _state(state)
{
	// Create objects for Base Info
//	_window = new Window(this, 320, 200, 0, 0);
//	_bg = new Surface(960, 600, 0, 0);
//	_mini = new MiniBaseView(128, 16, 182, 8);
//	_btnOk = new TextButton(30, 14, 10, 180);
//	_btnTransfers = new TextButton(80, 14, 46, 180);
//	_btnStores = new TextButton(80, 14, 132, 180);
//	_btnMonthlyCosts = new TextButton(92, 14, 218, 180);
//	_edtBase = new TextEdit(this, 127, 16, 8, 8);

	_txtPersonnel = new Text(300, 9, 8, 10);
	_txtSoldiers = new Text(114, 9, 8, 21);
	_numSoldiers = new Text(40, 9, 126, 21);
	_barSoldiers = new Bar(150, 5, 166, 23);
	_txtEngineers = new Text(114, 9, 8, 31);
	_numEngineers = new Text(40, 9, 126, 31);
	_barEngineers = new Bar(150, 5, 166, 33);
	_txtScientists = new Text(114, 9, 8, 41);
	_numScientists = new Text(40, 9, 126, 41);
	_barScientists = new Bar(150, 5, 166, 43);

	_txtSpace = new Text(300, 9, 8, 52);
	_txtQuarters = new Text(114, 9, 8, 63);
	_numQuarters = new Text(40, 9, 126, 63);
	_barQuarters = new Bar(150, 5, 166, 65);
	_txtStores = new Text(114, 9, 8, 73);
	_numStores = new Text(40, 9, 126, 73);
	_barStores = new Bar(150, 5, 166, 75);
	_txtLaboratories = new Text(114, 9, 8, 83);
	_numLaboratories = new Text(40, 9, 126, 83);
	_barLaboratories = new Bar(150, 5, 166, 85);
	_txtWorkshops = new Text(114, 9, 8, 83);
	_numWorkshops = new Text(40, 9, 126, 83);
	_barWorkshops = new Bar(150, 5, 166, 85);
	if (Options::storageLimitsEnforced)
	{
		_txtContainment = new Text(114, 9, 8, 103);
		_numContainment = new Text(40, 9, 126, 103);
		_barContainment = new Bar(150, 5, 166, 105);
	}
	_txtHangars = new Text(114, 9, 8, Options::storageLimitsEnforced ? 113 : 103);
	_numHangars = new Text(40, 9, 126, Options::storageLimitsEnforced ? 113 : 103);
	_barHangars = new Bar(150, 5, 166, Options::storageLimitsEnforced ? 115 : 105);

	_txtDefense = new Text(114, 9, 8, Options::storageLimitsEnforced ? 127 : 118);
	_numDefense = new Text(40, 9, 126, Options::storageLimitsEnforced ? 127 : 118);
	_barDefense = new Bar(150, 5, 166, Options::storageLimitsEnforced ? 129 : 120);
	_txtShortRange = new Text(114, 9, 8, Options::storageLimitsEnforced ? 137 : 133);
	_numShortRange = new Text(40, 9, 126, Options::storageLimitsEnforced ? 137 : 133);
	_barShortRange = new Bar(150, 5, 166, Options::storageLimitsEnforced ? 139 : 135);
	_txtLongRange = new Text(114, 9, 8, Options::storageLimitsEnforced ? 147 : 143);
	_numLongRange = new Text(40, 9, 126, Options::storageLimitsEnforced ? 147 : 143);
	_barLongRange = new Bar(150, 5, 166, Options::storageLimitsEnforced ? 149 : 145);

	// Create objects for monthly cost y + 150
	_txtTitleCost = new Text(310, 10, 8, 162);
	_txtCost = new Text(80, 9, 115, 172);
	_txtQuantity = new Text(55, 9, 195, 172);
	_txtTotal = new Text(60, 9, 249, 172);
	_txtRental = new Text(150, 9, 8, 172);
	_txtSalaries = new Text(150, 9, 8, 204);
	_txtIncome = new Text(150, 9, 8, 255); 
	_lstCrafts = new TextList(288, 24, 8, 182);
	_lstSalaries = new TextList(300, 30, 8, 214);
	_lstMaintenance = new TextList(300, 9, 8, 245);
	_lstTotal = new TextList(100, 9, 205, 255);
	
	// Create objects for stores y + 352
	_windowStores = new Window(this, 288, 128, 8, 270);
	_txtTitleStores = new Text(310, 10, 8, 270);
	_txtItemStores = new Text(142, 9, 8, 280);
	_txtQuantityStores = new Text(88, 9, 152, 280);
	_txtSpaceUsed = new Text(74, 9, 240, 280);
	_lstStores = new TextList(288, 130, 8, 290);

	// Create objects for transfer y + 55
	_windowTransfer = new Window(this, 273, 112, 8, 534);
	_txtTitleTransfer = new Text(278, 10, 8, 430);
	_txtItem = new Text(114, 9, 8, 441); 
	_txtQuantityTranfer = new Text(54, 9, 152, 441);
	_txtArrivalTime = new Text(112, 9, 212, 441);
	_lstTransfers = new TextList(273, 112, 8, 451);

	// Set palette
	setPalette("PAL_BASESCAPE");

//	add(_bg);
//	add(_mini);
//	add(_btnOk);
//	add(_btnTransfers);
//	add(_btnStores);
//	add(_btnMonthlyCosts);
//	add(_edtBase);

	add(_txtPersonnel);
	add(_txtSoldiers);
	add(_numSoldiers);
	add(_barSoldiers);
	add(_txtEngineers);
	add(_numEngineers);
	add(_barEngineers);
	add(_txtScientists);
	add(_numScientists);
	add(_barScientists);

	add(_txtSpace);
	add(_txtQuarters);
	add(_numQuarters);
	add(_barQuarters);
	add(_txtStores);
	add(_numStores);
	add(_barStores);
	add(_txtLaboratories);
	add(_numLaboratories);
	add(_barLaboratories);
	add(_txtWorkshops);
	add(_numWorkshops);
	add(_barWorkshops);
	if (Options::storageLimitsEnforced)
	{
		add(_txtContainment);
		add(_numContainment);
		add(_barContainment);
	}
	add(_txtHangars);
	add(_numHangars);
	add(_barHangars);

	add(_txtDefense);
	add(_numDefense);
	add(_barDefense);
	add(_txtShortRange);
	add(_numShortRange);
	add(_barShortRange);
	add(_txtLongRange);
	add(_numLongRange);
	add(_barLongRange);

	//add items for Monthly Cost
	add(_txtTitleCost);
	add(_txtCost);
	add(_txtQuantity);
	add(_txtTotal);
	add(_txtRental);
	add(_txtSalaries);
	add(_txtIncome);
	add(_lstCrafts);
	add(_lstSalaries);
	add(_lstMaintenance);
	add(_lstTotal);

	//add items for stores
	add(_windowStores);
	add(_txtTitleStores);
	add(_txtItemStores);
	add(_txtQuantityStores);
	add(_txtSpaceUsed);
	add(_lstStores);

	//add items for transfer
	add(_windowTransfer);
	add(_txtTitleTransfer);
	add(_txtItem);
	add(_txtQuantityTranfer);
	add(_txtArrivalTime);
	add(_lstTransfers);

//	centerAllSurfaces();

	// Set up objects
/*	std::ostringstream ss;
	if (Options::storageLimitsEnforced)
	{
		ss << "ALT";
	}
	ss << "BACK07.SCR";
	_game->getResourcePack()->getSurface(ss.str())->blit(_bg);
	*/
	/*
	_mini->setTexture(_game->getResourcePack()->getSurfaceSet("BASEBITS.PCK"));
	_mini->setBases(_game->getSavedGame()->getBases());
	for (size_t i = 0; i < _game->getSavedGame()->getBases()->size(); ++i)
	{
		if (_game->getSavedGame()->getBases()->at(i) == _base)
		{
			_mini->setSelectedBase(i);
			break;
		}
	}
	_mini->onMouseClick((ActionHandler)&BaseInfoState::miniClick);
	_mini->onKeyboardPress((ActionHandler)&BaseInfoState::handleKeyPress);
	
	_btnOk->setColor(Palette::blockOffset(15)+6);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BaseInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&BaseInfoState::btnOkClick, Options::keyCancel);

	_btnTransfers->setColor(Palette::blockOffset(15)+6);
	_btnTransfers->setText(tr("STR_TRANSFERS_UC"));
	_btnTransfers->onMouseClick((ActionHandler)&BaseInfoState::btnTransfersClick);

	_btnStores->setColor(Palette::blockOffset(15)+6);
	_btnStores->setText(tr("STR_STORES_UC"));
	_btnStores->onMouseClick((ActionHandler)&BaseInfoState::btnStoresClick);

	_btnMonthlyCosts->setColor(Palette::blockOffset(15)+6);
	_btnMonthlyCosts->setText(tr("STR_MONTHLY_COSTS"));
	_btnMonthlyCosts->onMouseClick((ActionHandler)&BaseInfoState::btnMonthlyCostsClick);
	
	_edtBase->setColor(Palette::blockOffset(15)+1);
	_edtBase->setBig();
	_edtBase->onChange((ActionHandler)&BaseInfoState::edtBaseChange);
	*/
	// Setup all objects for monthly cost
	_txtPersonnel->setColor(Palette::blockOffset(15)+1);
	_txtPersonnel->setText(tr("STR_PERSONNEL_AVAILABLE_PERSONNEL_TOTAL"));

	_txtSoldiers->setColor(Palette::blockOffset(13)+5);
	_txtSoldiers->setText(tr("STR_SOLDIERS"));

	_numSoldiers->setColor(Palette::blockOffset(13));

	_barSoldiers->setColor(Palette::blockOffset(1));
	_barSoldiers->setScale(1.0);

	_txtEngineers->setColor(Palette::blockOffset(13)+5);
	_txtEngineers->setText(tr("STR_ENGINEERS"));

	_numEngineers->setColor(Palette::blockOffset(13));

	_barEngineers->setColor(Palette::blockOffset(1));
	_barEngineers->setScale(1.0);

	_txtScientists->setColor(Palette::blockOffset(13)+5);
	_txtScientists->setText(tr("STR_SCIENTISTS"));

	_numScientists->setColor(Palette::blockOffset(13));

	_barScientists->setColor(Palette::blockOffset(1));
	_barScientists->setScale(1.0);


	_txtSpace->setColor(Palette::blockOffset(15)+1);
	_txtSpace->setText(tr("STR_SPACE_USED_SPACE_AVAILABLE"));

	_txtQuarters->setColor(Palette::blockOffset(13)+5);
	_txtQuarters->setText(tr("STR_LIVING_QUARTERS_PLURAL"));

	_numQuarters->setColor(Palette::blockOffset(13));

	_barQuarters->setColor(Palette::blockOffset(3));
	_barQuarters->setScale(0.5);

	_txtStores->setColor(Palette::blockOffset(13)+5);
	_txtStores->setText(tr("STR_STORES"));

	_numStores->setColor(Palette::blockOffset(13));

	_barStores->setColor(Palette::blockOffset(3));
	_barStores->setScale(0.5);

	_txtLaboratories->setColor(Palette::blockOffset(13)+5);
	_txtLaboratories->setText(tr("STR_LABORATORIES"));

	_numLaboratories->setColor(Palette::blockOffset(13));

	_barLaboratories->setColor(Palette::blockOffset(3));
	_barLaboratories->setScale(0.5);

	_txtWorkshops->setColor(Palette::blockOffset(13)+5);
	_txtWorkshops->setText(tr("STR_WORK_SHOPS"));

	_numWorkshops->setColor(Palette::blockOffset(13));

	_barWorkshops->setColor(Palette::blockOffset(3));
	_barWorkshops->setScale(0.5);

	if (Options::storageLimitsEnforced)
	{
		_txtContainment->setColor(Palette::blockOffset(13)+5);
		_txtContainment->setText(tr("STR_ALIEN_CONTAINMENT"));

		_numContainment->setColor(Palette::blockOffset(13));

		_barContainment->setColor(Palette::blockOffset(3));
		_barContainment->setScale(0.5);
	}

	_txtHangars->setColor(Palette::blockOffset(13)+5);
	_txtHangars->setText(tr("STR_HANGARS"));

	_numHangars->setColor(Palette::blockOffset(13));

	_barHangars->setColor(Palette::blockOffset(3));
	_barHangars->setScale(18.0);


	_txtDefense->setColor(Palette::blockOffset(13)+5);
	_txtDefense->setText(tr("STR_DEFENSE_STRENGTH"));

	_numDefense->setColor(Palette::blockOffset(13));

	_barDefense->setColor(Palette::blockOffset(2));
	_barDefense->setScale(0.125);

	_txtShortRange->setColor(Palette::blockOffset(13)+5);
	_txtShortRange->setText(tr("STR_SHORT_RANGE_DETECTION"));

	_numShortRange->setColor(Palette::blockOffset(13));

	_barShortRange->setColor(Palette::blockOffset(8));
	_barShortRange->setScale(25.0);

	_txtLongRange->setColor(Palette::blockOffset(13)+5);
	_txtLongRange->setText(tr("STR_LONG_RANGE_DETECTION"));

	_numLongRange->setColor(Palette::blockOffset(13));

	_barLongRange->setColor(Palette::blockOffset(8));
	_barLongRange->setScale(25.0);

	// Setup all objects for monthly cost
	_txtTitleCost->setColor(Palette::blockOffset(15) + 1);
//	_txtTitleCost->setBig();
	_txtTitleCost->setAlign(ALIGN_LEFT);
	_txtTitleCost->setText(tr("STR_MONTHLY_COSTS"));

	_txtCost->setColor(Palette::blockOffset(15) + 1);
	_txtCost->setText(tr("STR_COST_PER_UNIT"));

	_txtQuantity->setColor(Palette::blockOffset(15) + 1);
	_txtQuantity->setText(tr("STR_QUANTITY"));

	_txtTotal->setColor(Palette::blockOffset(15) + 1);
	_txtTotal->setText(tr("STR_TOTAL"));

	_txtRental->setColor(Palette::blockOffset(15) + 1);
	_txtRental->setText(tr("STR_CRAFT_RENTAL"));

	_txtSalaries->setColor(Palette::blockOffset(15) + 1);
	_txtSalaries->setText(tr("STR_SALARIES"));

	_txtIncome->setColor(Palette::blockOffset(13) + 10);
	std::wostringstream ss1;
	ss1 << tr("STR_INCOME") << L"=" << Text::formatFunding(_game->getSavedGame()->getCountryFunding());
	_txtIncome->setText(ss1.str());

	_lstCrafts->setColor(Palette::blockOffset(13) + 10);
	_lstCrafts->setColumns(4, 125, 70, 44, 60);
	_lstCrafts->setDot(true);

	const std::vector<std::string> &crafts = _game->getRuleset()->getCraftsList();
	for (std::vector<std::string>::const_iterator i = crafts.begin(); i != crafts.end(); ++i)
	{
		RuleCraft *craft = _game->getRuleset()->getCraft(*i);
		if (craft->getRentCost() != 0 && _game->getSavedGame()->isResearched(craft->getRequirements()))
		{
			std::wostringstream ss2;
			ss2 << _base->getCraftCount((*i));
			_lstCrafts->addRow(4, tr(*i).c_str(), Text::formatFunding(craft->getRentCost()).c_str(), ss2.str().c_str(), Text::formatFunding(_base->getCraftCount(*i) * craft->getRentCost()).c_str());
		}
	}

	_lstSalaries->setColor(Palette::blockOffset(13) + 10);
	_lstSalaries->setColumns(4, 125, 70, 44, 60);
	_lstSalaries->setDot(true);

	std::wostringstream ss4;
	ss4 << _base->getSoldiers()->size();
	_lstSalaries->addRow(4, tr("STR_SOLDIERS").c_str(), Text::formatFunding(_game->getRuleset()->getSoldierCost()).c_str(), ss4.str().c_str(), Text::formatFunding(_base->getSoldiers()->size() * _game->getRuleset()->getSoldierCost()).c_str());
	std::wostringstream ss5;
	ss5 << _base->getTotalEngineers();
	_lstSalaries->addRow(4, tr("STR_ENGINEERS").c_str(), Text::formatFunding(_game->getRuleset()->getEngineerCost()).c_str(), ss5.str().c_str(), Text::formatFunding(_base->getTotalEngineers() * _game->getRuleset()->getEngineerCost()).c_str());
	std::wostringstream ss6;
	ss6 << _base->getTotalScientists();
	_lstSalaries->addRow(4, tr("STR_SCIENTISTS").c_str(), Text::formatFunding(_game->getRuleset()->getScientistCost()).c_str(), ss6.str().c_str(), Text::formatFunding(_base->getTotalScientists() * _game->getRuleset()->getScientistCost()).c_str());

	_lstMaintenance->setColor(Palette::blockOffset(13) + 10);
	_lstMaintenance->setColumns(2, 239, 60);
	_lstMaintenance->setDot(true);
	_lstMaintenance->addRow(2, tr("STR_BASE_MAINTENANCE").c_str(), Text::formatFunding(_base->getFacilityMaintenance()).c_str());
	_lstMaintenance->setCellColor(0, 0, Palette::blockOffset(15) + 1);

	_lstTotal->setColor(Palette::blockOffset(13));
	_lstTotal->setColumns(2, 44, 55);
	_lstTotal->setDot(true);
	_lstTotal->addRow(2, tr("STR_TOTAL").c_str(), Text::formatFunding(_base->getMonthlyMaintenace()).c_str());

	// Setup all objects for transfer
	_txtTitleTransfer->setColor(Palette::blockOffset(15) + 1);
//	_txtTitleTransfer->setBig();
	_txtTitleTransfer->setAlign(ALIGN_LEFT);
	_txtTitleTransfer->setText(tr("STR_TRANSFERS"));

	_txtItem->setColor(Palette::blockOffset(15) + 1);
	_txtItem->setText(tr("STR_ITEM"));

	_txtQuantityTranfer->setColor(Palette::blockOffset(15) + 1);
	_txtQuantityTranfer->setText(tr("STR_QUANTITY_UC"));

	_txtArrivalTime->setColor(Palette::blockOffset(15) + 1);
	_txtArrivalTime->setText(tr("STR_ARRIVAL_TIME_HOURS"));

	_lstTransfers->setColor(Palette::blockOffset(13) + 10);
	_lstTransfers->setArrowColor(Palette::blockOffset(15) + 6);
	_lstTransfers->setColumns(3, 155, 75, 46);
	_lstTransfers->setSelectable(true);
	_lstTransfers->setBackground(_windowTransfer);
	_lstTransfers->setMargin(2);

	for (std::vector<Transfer*>::iterator i = _base->getTransfers()->begin(); i != _base->getTransfers()->end(); ++i)
	{
		std::wostringstream ss, ss2;
		ss << (*i)->getQuantity();
		ss2 << (*i)->getHours();
		_lstTransfers->addRow(3, (*i)->getName(_game->getLanguage()).c_str(), ss.str().c_str(), ss2.str().c_str());
	}

	//Setup all objects for stores
	_txtTitleStores->setColor(Palette::blockOffset(15) + 1);
//	_txtTitleStores->setBig();
	_txtTitleStores->setAlign(ALIGN_LEFT);
	_txtTitleStores->setText(tr("STR_STORES"));

	_txtItemStores->setColor(Palette::blockOffset(15) + 1);
	_txtItemStores->setText(tr("STR_ITEM"));

	_txtQuantityStores->setColor(Palette::blockOffset(15) + 1);
	_txtQuantityStores->setText(tr("STR_QUANTITY_UC"));

	_txtSpaceUsed->setColor(Palette::blockOffset(15) + 1);
	_txtSpaceUsed->setText(tr("STR_SPACE_USED_UC"));

	_lstStores->setColor(Palette::blockOffset(13) + 10);
	_lstStores->setColumns(3, 162, 92, 32);
	_lstStores->setSelectable(true);
	_lstStores->setBackground(_windowStores);
	_lstStores->setMargin(2);

	const std::vector<std::string> &items = _game->getRuleset()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		int qty = _base->getItems()->getItem(*i);
		if (qty > 0)
		{
			RuleItem *rule = _game->getRuleset()->getItem(*i);
			std::wostringstream ss, ss2;
			ss << qty;
			ss2 << qty * rule->getSize();
			_lstStores->addRow(3, tr(*i).c_str(), ss.str().c_str(), ss2.str().c_str());
		}
	}
}

/**
 *
 */
BaseInfoState::~BaseInfoState()
{

}

/**
 * The player can change the selected base.
 */
void BaseInfoState::init()
{
	State::init();
//	_edtBase->setText(_base->getName());

	std::wostringstream ss;
	ss << _base->getAvailableSoldiers() << ":" << _base->getTotalSoldiers();
	_numSoldiers->setText(ss.str());

	_barSoldiers->setMax(_base->getTotalSoldiers());
	_barSoldiers->setValue(_base->getAvailableSoldiers());

	std::wostringstream ss2;
	ss2 << _base->getAvailableEngineers() << ":" << _base->getTotalEngineers();
	_numEngineers->setText(ss2.str());

	_barEngineers->setMax(_base->getTotalEngineers());
	_barEngineers->setValue(_base->getAvailableEngineers());

	std::wostringstream ss3;
	ss3 << _base->getAvailableScientists() << ":" << _base->getTotalScientists();
	_numScientists->setText(ss3.str());

	_barScientists->setMax(_base->getTotalScientists());
	_barScientists->setValue(_base->getAvailableScientists());


	std::wostringstream ss4;
	ss4 << _base->getUsedQuarters() << ":" << _base->getAvailableQuarters();
	_numQuarters->setText(ss4.str());

	_barQuarters->setMax(_base->getAvailableQuarters());
	_barQuarters->setValue(_base->getUsedQuarters());

	std::wostringstream ss5;
	ss5 << (int)floor(_base->getUsedStores() + 0.05) << ":" << _base->getAvailableStores();
	_numStores->setText(ss5.str());

	_barStores->setMax(_base->getAvailableStores());
	_barStores->setValue((int)floor(_base->getUsedStores() + 0.05));

	std::wostringstream ss6;
	ss6 << _base->getUsedLaboratories() << ":" << _base->getAvailableLaboratories();
	_numLaboratories->setText(ss6.str());

	_barLaboratories->setMax(_base->getAvailableLaboratories());
	_barLaboratories->setValue(_base->getUsedLaboratories());

	std::wostringstream ss7;
	ss7 << _base->getUsedWorkshops() << ":" << _base->getAvailableWorkshops();
	_numWorkshops->setText(ss7.str());

	_barWorkshops->setMax(_base->getAvailableWorkshops());
	_barWorkshops->setValue(_base->getUsedWorkshops());

	if (Options::storageLimitsEnforced)
	{
		std::wostringstream ss72;
		ss72 << _base->getUsedContainment() << ":" << _base->getAvailableContainment();
		_numContainment->setText(ss72.str());

		_barContainment->setMax(_base->getAvailableContainment());
		_barContainment->setValue(_base->getUsedContainment());
	}

	std::wostringstream ss8;
	ss8 << _base->getUsedHangars() << ":" << _base->getAvailableHangars();
	_numHangars->setText(ss8.str());

	_barHangars->setMax(_base->getAvailableHangars());
	_barHangars->setValue(_base->getUsedHangars());


	std::wostringstream ss9;
	ss9 << _base->getDefenseValue();
	_numDefense->setText(ss9.str());

	_barDefense->setMax(_base->getDefenseValue());
	_barDefense->setValue(_base->getDefenseValue());

	std::wostringstream ss10;
	ss10 << _base->getShortRangeDetection();
	_numShortRange->setText(ss10.str());

	_barShortRange->setMax(_base->getShortRangeDetection());
	_barShortRange->setValue(_base->getShortRangeDetection());

	std::wostringstream ss11;
	ss11 << _base->getLongRangeDetection();
	_numLongRange->setText(ss11.str());

	_barLongRange->setMax(_base->getLongRangeDetection());
	_barLongRange->setValue(_base->getLongRangeDetection());
}

/**
 * Changes the base name.
 * @param action Pointer to an action.
 
void BaseInfoState::edtBaseChange(Action *action)
{
	_base->setName(_edtBase->getText());
}

/**
 * Selects a new base to display.
 * @param action Pointer to an action.
 
void BaseInfoState::miniClick(Action *)
{
	size_t base = _mini->getHoveredBase();
	if (base < _game->getSavedGame()->getBases()->size())
	{
		_mini->setSelectedBase(base);
		_base = _game->getSavedGame()->getBases()->at(base);
		_state->setBase(_base);
		init();
	}
}

/**
 * Selects a new base to display.
 * @param action Pointer to an action.
 
void BaseInfoState::handleKeyPress(Action *action)
{
	if (action->getDetails()->type == SDL_KEYDOWN)
	{
		SDLKey baseKeys[] = {Options::keyBaseSelect1,
			                 Options::keyBaseSelect2,
			                 Options::keyBaseSelect3,
			                 Options::keyBaseSelect4,
			                 Options::keyBaseSelect5,
			                 Options::keyBaseSelect6,
			                 Options::keyBaseSelect7,
			                 Options::keyBaseSelect8};
		int key = action->getDetails()->key.keysym.sym;
		for (size_t i = 0; i < _game->getSavedGame()->getBases()->size(); ++i)
		{
			if (key == baseKeys[i])
			{
				_mini->setSelectedBase(i);
				_base = _game->getSavedGame()->getBases()->at(i);
				_state->setBase(_base);
				init();
				break;
			}
		}
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 
void BaseInfoState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Goes to the Transfers window.
 * @param action Pointer to an action.
 
void BaseInfoState::btnTransfersClick(Action *)
{
	_game->pushState(new TransfersState(_base));
}

/**
 * Goes to the Stores screen.
 * @param action Pointer to an action.
 
void BaseInfoState::btnStoresClick(Action *)
{
	_game->pushState(new StoresState(_base));
}

/**
 * Goes to the Monthly Costs screen.
 * @param action Pointer to an action.
 
void BaseInfoState::btnMonthlyCostsClick(Action *)
{
	_game->pushState(new MonthlyCostsState(_base));
}
*/
}

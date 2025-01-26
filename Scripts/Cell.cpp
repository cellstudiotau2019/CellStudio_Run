#include "Cell.h"

void Cell::save(IOManager* ioMgr, bool withReceptors)
{
	ioMgr->writeUInt32(birthTime);

	ioMgr->writeBool(isMovable);
	ioMgr->writeBool(isScanningActive);
	ioMgr->writeBool(isDead);

	ioMgr->writeUInt32(lastMovementTick);
	ioMgr->writeUInt32(nextMovementTick);
	ioMgr->writeUInt32(lastUpdateTick);
	ioMgr->writeUInt32(nextUpdateTick);
	ioMgr->writeUInt32(nextStateUpdateTick);
	ioMgr->writeUInt32(lastStateUpdateTick);
	ioMgr->writeUInt32(deltaStateUpdateTick);
	// ioMgr->writeInt32(nextInteractionID);

	ioMgr->writeInt32(cellConglomerateID);
	ioMgr->writeInt32(cellStartConglomerateID);
	ioMgr->writeUInt32(cellStartConglomerateTick);

	ioMgr->writeVector<uint32_t>(proximityCellsID);
	ioMgr->writeVector<uint32_t>(scanningCellID);
	ioMgr->writeVector<uint32_t>(beingScannedByCellID);

	// ioMgr->writeInt8(populationNameIndex);
	// ioMgr->writeInt16(cellId);
	// ioMgr->writeInt8((int8_t)bioCellType);

	// model
	// float radius = cellMesh->getRadius();
	// ioMgr->writeFloat(radius);

	// float3 center = cellMesh->getPosition();
	// ioMgr->writeFloat3(center);
	// ioMgr->writeFloat(cellSpeed);
	// ioMgr->writeFloat3(cellDirection);
	// ioMgr->writeFloat(cellScale);

	// ioMgr->writeInt16(cellRegionCellType.first);
	// ioMgr->writeInt16(cellRegionCellType.second);

	// receptorsState.save(ioMgr, desc, withReceptors);
	// deathState.save(ioMgr, desc, withReceptors);
	// scanState.save(ioMgr, desc, withReceptors);
	// prolifirationState.save(ioMgr, desc, withReceptors);

	// saving the maps
	// ioMgr->writeMap(lastTimeSubEventEvaluatedTrue);
	// ioMgr->writeMap(otherCellIdForReceptorReceptorRule);
	// ioMgr->writeMap(timeRuleCoolDownTimerStartedTable);
}

void Cell::load(IOManager* ioMgr)
{
	this->birthTime = ioMgr->readUInt32();
	this->isMovable = ioMgr->readBool();
	this->isScanningActive = ioMgr->readBool();
	this->isDead = ioMgr->readBool();

	this->lastMovementTick = ioMgr->readUInt32();
	this->nextMovementTick = ioMgr->readUInt32();
	this->lastUpdateTick = ioMgr->readUInt32();
	this->nextUpdateTick = ioMgr->readUInt32();
	this->nextStateUpdateTick = ioMgr->readUInt32();
	this->lastStateUpdateTick = ioMgr->readUInt32();
	this->deltaStateUpdateTick = ioMgr->readUInt32();
	// this->nextInteractionID = ioMgr->readUInt32();

	this->proximityCellsID = ioMgr->readVector<uint32_t>();
	this->scanningCellID = ioMgr->readVector<uint32_t>();
	this->beingScannedByCellID = ioMgr->readVector<uint32_t>();

	//ioMgr->readInt8(desc, &populationNameIndex);
	//ioMgr->readInt16(desc, &cellId);
	//ioMgr->readInt8(desc, (signed char*)&bioCellType);

	// model
	// float radius = ioMgr->readFloat();

	// glm::vec3 center = ioMgr->readFloat3();

	//build3DModel(radius, center, cellScale);

	//ioMgr->readFloat(desc, &cellSpeed);
	//ioMgr->readFloat3(desc, &cellDirection);
	//ioMgr->readFloat(desc, &cellScale);

	//ioMgr->readInt16(desc, &(cellRegionCellType.first));
	//ioMgr->readInt16(desc, &(cellRegionCellType.second));

	//receptorsState.load(ioMgr, desc);
	//deathState.load(ioMgr, desc);
	//scanState.load(ioMgr, desc);
	//prolifirationState.load(ioMgr, desc);

	//lastTimeSubEventEvaluatedTrue.clear();
	//otherCellIdForReceptorReceptorRule.clear();
	//timeRuleCoolDownTimerStartedTable.clear();

	//ioMgr->readMap(desc, &lastTimeSubEventEvaluatedTrue);
	//ioMgr->readMap(desc, &otherCellIdForReceptorReceptorRule);
	//ioMgr->readMap(desc, &timeRuleCoolDownTimerStartedTable);
}

void Cell::saveCellInfo()
{
	updateTime.push_back(SimulationCounter::getSimulationTick());
	saveData();
};

////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------CELL--------------------------------------------//
////////////////////////////////////////////////////////////////////////////////////////////

Cell::Cell(const CellPopulationData & _populationData, uint32_t cellInstanceID)  : cellPopulationData(_populationData)
{
	birthTime = SimulationCounter::getSimulationTick();
	cellState.store(STATE::BASE);
	isMovable.store(true);
	isScanningActive.store(false);
	isDead.store(false);

	lastMovementTick.store(0);
	nextMovementTick.store(-1);
	lastUpdateTick.store(0);
	nextUpdateTick.store(0);
	nextStateUpdateTick.store(0);
	lastStateUpdateTick.store(0);
	deltaStateUpdateTick.store(0);
	nextInteractionID = std::make_shared<interactionData>();

	cellConglomerateID.store(-1);
	cellStartConglomerateID.store(-1);
	cellStartConglomerateTick.store(0);

	//Create the cell based on population
	rulesData = std::map<uint16_t, CellRulesData>(cellPopulationData.rulesPopulationData);
	cellRegion = SimulationManager::getRegion(cellPopulationData.regionID);
	resetCell(true);

	//ADD RECEPTOR
	for (int receptorIdx = 0; receptorIdx < cellPopulationData.receptorCount; receptorIdx++)
	{
		uint16_t receptorID = cellPopulationData.allReceptorIDs[receptorIdx];
		addReceptors(cellPopulationData.getReceptorPopulation(receptorID));
	}
}

Cell::~Cell()
{
	cellRegion = nullptr;
}

void Cell::resetCell(bool initialize)
{

	if (initialize) {

		cellTransformData.setPopulationID(cellPopulationData.cellPopulationID);
 		cellTransformData.setColor(cellPopulationData.meshData->inputColor);		
		cellTransformData.setColorBB(glm::vec4(0, 1, 0, 1));
		setIsScanningActive(true);
	}

	float radius = cellPopulationData.sizeDistribution->getRandFloat();
	setRadius(radius);

	cellTransformData.setDirection(glm::normalize(RandomWrapper::get3DPointU(glm::fvec3(-1.f), glm::fvec3(1.f))));
	cellTransformData.setScale(glm::vec3(radius / cellPopulationData.meshData->radiusMesh));
	cellTransformData.setSpeed(cellPopulationData.speedDistribution->getRandFloat());
	cellTransformData.setStokesSpeed(1);

	cellTransformData.setScanTime(cellPopulationData.scanTimeDistribution->getRandFloat());
	cellTransformData.setRefractaryTime(cellPopulationData.refractary_time_distribution->getRandFloat());

	float currentSpeed = getSpeed();

	if (currentSpeed > cellRegion->getRegionMaximumSpeed())
	{
		cellRegion->setRegionMaximumSpeed(currentSpeed);
	}

	//float R = cellTransformData->getRadius() / 1000000;
	//cellTransformData->setStokesSpeed(((2.f / 9.f)*9.8f) * pow(R,2) * );
}

//--------------------------GET/SET TRANSFORM -------------------------

void Cell::setInstanceID(uint32_t _cellID)
{
	cellTransformData.setInstanceID(_cellID);
}
uint32_t  Cell::getInstanceID()
{
	return cellTransformData.getInstanceID();
}
uint16_t Cell::getPopulationID()
{
	return cellPopulationData.cellPopulationID;
}
bool Cell::isAScanningCell()
{
	return cellPopulationData.isScanningCell;
};

void Cell::setPosition(const glm::vec3 & _position)
{
	cellTransformData.setPosition(_position);
}
void Cell::addOffset(const glm::vec3 &_offset)
{
	cellTransformData.addPosition(_offset);
}
glm::vec3 Cell::getPosition()
{
	glm::vec3 position = cellTransformData.getPosition();
	return  position;
}
glm::vec3 Cell::getUpdatedPosition()
{
	std::lock_guard<std::mutex> guard(mutexTransform);
	
	uint32_t currentTick = SimulationCounter::getSimulationTick();
	float deltaTimeUpdate = tickToSecond(currentTick - getLastMovementTick());
	
	glm::vec3 position = getPosition();
	if (deltaTimeUpdate > 0 && getMovability())
	{
		position = position + (getDirection() * getSpeed() * deltaTimeUpdate) + (glm::vec3(0, -1, 0) * getStokesSpeed() * deltaTimeUpdate);
	}

	return position;
}
glm::vec3 Cell::getPositionAt(uint32_t biotick)
{
	float deltaTimeUpdate = tickToSecond(biotick - getLastMovementTick());

	if (deltaTimeUpdate > 0 && getMovability())
	{
		return getPosition() + (getDirection() * getSpeed() * deltaTimeUpdate) + (glm::vec3(0, -1, 0) *getStokesSpeed() * deltaTimeUpdate);
	}
	else
	{
		return getPosition();
	}
}
void Cell::setDirection(const glm::vec3 & _direction)
{
	cellTransformData.setDirection(_direction);
}
glm::vec3 Cell::getDirection()
{
	glm::vec3 direction = cellTransformData.getDirection();
	return direction;
}
void Cell::setScale(const glm::vec3 & _scale)
{
	cellTransformData.setScale(_scale);
	cellTransformData.setRadius((cellPopulationData.meshData->radiusMesh) * _scale.x);
}
glm::vec3 Cell::getScale()
{
	glm::vec3 scale = cellTransformData.getScale();
	return scale;
};
void Cell::setRadius(float _radius)
{
	cellTransformData.setRadius(_radius);
	cellTransformData.setScale(glm::vec3(_radius / cellPopulationData.meshData->radiusMesh));
}
float Cell::getRadius()
{
	return cellTransformData.getRadius();
}

void Cell::setSpeed(float _speed)
{
	cellTransformData.setSpeed(_speed);
}
float Cell::getSpeed()
{
	return cellTransformData.getSpeed();
}
void Cell::resetSpeed()
{
	if (getMovability())
		cellTransformData.setSpeed(cellPopulationData.speedDistribution->getRandFloat());
}
void Cell::setStokesSpeed(float _speed)
{
	cellTransformData.setStokesSpeed(_speed);
}
float Cell::getStokesSpeed()
{
	return cellTransformData.getStokesSpeed();;
}
float Cell::getAllSpeed()
{
	return (cellTransformData.getStokesSpeed() + cellTransformData.getSpeed());
};

void Cell::setScanTime(float _scanTime) 
{
	cellTransformData.setScanTime(_scanTime);
}
float Cell::getScanTime() 
{
	return cellTransformData.getScanTime();;
};
void Cell::setRefractaryTime(float _refractaryTime) 
{
	cellTransformData.setRefractaryTime(_refractaryTime);
};
float Cell::getRefractaryTime() 
{
	return cellTransformData.getRefractaryTime();
};

void Cell::setColor(const glm::vec4 & inputColor)
{
	cellTransformData.setColor(inputColor);
};
glm::vec4 Cell::getColor()
{
	return cellTransformData.getColor();
};
void Cell::setColorBB(const glm::vec4 & inputColorBB)
{
	cellTransformData.setColorBB(inputColorBB);
};
glm::vec4 Cell::getColorBB()
{
	return cellTransformData.getColorBB();
};
void  Cell::setGraphicInstanceIdx(uint32_t _graphicInstanceIdx)
{
	cellTransformData.setGraphicInstanceIdx(_graphicInstanceIdx);
}
uint32_t  Cell::getGraphicInstanceIdx()
{
	return cellTransformData.getGraphicInstanceIdx();
}

//--------------------------RECEPTOR----------------------//

void Cell::setReceptorMap(const uint16_t& receptorID, const std::pair<float, float>& receptorInfo)
{
	std::lock_guard<std::mutex> guard(mutexTransform);
	cellTransformData.setReceptorMap(receptorID, receptorInfo);
};
const std::pair<float, float>& Cell::getReceptorMap(uint16_t receptorID)
{
	return cellTransformData.getReceptorMap(receptorID);
};

void Cell::addReceptors(const ReceptorPopulationData& receptorData)
{
	//Add receptor expression and binded number
	uint16_t receptorPopulationID = receptorData.receptorPopulationID;

	float receptorExpression = receptorData.expressionDistribution->getRandFloat();
	float receptorBinded = receptorData.bindingDistribution->getRandFloat();

	cellTransformData.setReceptorMap(receptorPopulationID, std::make_pair(0.0f, 0.0f));
	setReceptorExpression(receptorPopulationID,receptorData.expressionDistribution->getRandFloat());
	setReceptorBindedFraction(receptorPopulationID,receptorData.bindingDistribution->getRandFloat());

	//DEFINE TRANSFORMATION OF SINGLE RECEPTOR
	//INT32 receptorAmount = receptorData.amountInterval.x + RandomWrapper::getNextFraction() * (receptorData.amountInterval.y - receptorData.amountInterval.x);
	//float receptorSpeed = receptorData.speedInterval.x + RandomWrapper::getNextFraction() * (receptorData.speedInterval.y - receptorData.speedInterval.x);
	//std::vector<ReceptorTransform*>* receptorTransformList = cellTransformData->receptorTransform[receptorData->receptorPopulationID];
	//for (INT32 receptorIdx = 0; receptorIdx < receptorAmount; receptorIdx++)
	//{
		//ReceptorTransform * newReceptorTransform = new ReceptorTransform();
		//INT32 randomVertex = RandomWrapper::getNextInt(cellMeshData->verticesLength);
		//newReceptorTransform->position = (cellMeshData->vertices)[randomVertex];
		//newReceptorTransform->rotation = (cellMeshData->vertices)[getRandomAdjacentVertex(randomVertex)];
		//newReceptorTransform->speed = receptorSpeed;
		//newReceptorTransform->receptorType = stringToReceptorType(receptorData->receptorName);
		//receptorTransformList->push_back(newReceptorTransform);
		//receptorsState.receptorsCurrentAndNextVertex[lastReceptorGroupIdx]->push_back(make_int16_2(randomVertex, randomAdjacentVertex));
		//receptorsState.percentageOfDistanceToNextVertex[lastReceptorGroupIdx]->push_back(0);
	//}
	//getTransform()->receptorTransform = receptorTransformList;
}

float Cell::setReceptorOffsetBindedFraction(uint16_t receptorPopulationID, float offsetFractionBinding)
{
	std::pair<float, float> currentReceptorData = getReceptorMap(receptorPopulationID);

	float newFractionReceptorBinding = currentReceptorData.first + offsetFractionBinding;

	if (newFractionReceptorBinding < 0.0f) { newFractionReceptorBinding = 0.01f; }
	else if (newFractionReceptorBinding > 1.0f) { newFractionReceptorBinding = 1.0f; }

	currentReceptorData.first = newFractionReceptorBinding;
	setReceptorMap(receptorPopulationID, currentReceptorData);

	return newFractionReceptorBinding;
}
void Cell::setReceptorBindedFraction(uint16_t receptorPopulationID, float receptorBinded)
{
	std::pair<float, float> currentReceptorData = getReceptorMap(receptorPopulationID);

	if (receptorBinded < 0.0f) { receptorBinded = 0.01f; }
	if (receptorBinded > 1.0f) { receptorBinded = 1.0f; }

	currentReceptorData.first = receptorBinded;
	setReceptorMap(receptorPopulationID, currentReceptorData);
};

float Cell::setReceptorOffsetExpression(uint16_t receptorPopulationID, float offsetFractionExpression)
{
	std::pair<float, float>  currentReceptorData = getReceptorMap(receptorPopulationID);

	float newFractionReceptorExpression = currentReceptorData.second + offsetFractionExpression;

	if (newFractionReceptorExpression < 0.0f) { newFractionReceptorExpression = 0.01f; }
	if (newFractionReceptorExpression > 1.0f) { newFractionReceptorExpression = 1.0f; }

	currentReceptorData.second = newFractionReceptorExpression;

	setReceptorMap(receptorPopulationID, currentReceptorData);

	return newFractionReceptorExpression;
}
void Cell::setReceptorExpression(uint16_t receptorPopulationID, float receptorExpression)
{
	std::pair<float, float> currentReceptorData = getReceptorMap(receptorPopulationID);

	if (receptorExpression < 0.0f) { receptorExpression = 0.01f; }
	if (receptorExpression > 1.0f) { receptorExpression = 1.0f; }

	currentReceptorData.second = receptorExpression;
	setReceptorMap(receptorPopulationID, currentReceptorData);
};

bool Cell::hasReceptor(const uint16_t receptorPopulationID) const
{
	for (uint16_t ii = 0; ii < cellPopulationData.receptorCount; ii++)
	{
		uint16_t receptorID = cellPopulationData.allReceptorIDs[ii];
		if (receptorID == receptorPopulationID)
		{
			return true;
		}
	}
	return false;
}

//--------------------------MOVEMENT -------------------------

void Cell::move()
{
	std::lock_guard<std::mutex> guard(mutexTransform);
	uint32_t currentTick = SimulationCounter::getSimulationTick();
	if (getNextMovementTick() != currentTick) { return; }

	float deltaTime = tickToSecond(currentTick - getLastMovementTick());

	if (getMovability())
	{
		addOffset((getDirection() * getSpeed() * deltaTime) + (glm::vec3(0, -1, 0) * getStokesSpeed() * deltaTime));
		clearMaskInstanceID();
	}

	setLastMovementTick(currentTick);
}

//--------------------------INTERACTION -------------------------
void Cell::addToLOG(std::string log)
{
	std::lock_guard<std::mutex> guard(mutexLog);
	logString.push_back(log);
}
void Cell::setNextInteractionID(std::shared_ptr<interactionData> nextTick)
{
	nextInteractionID = nextTick;
}
std::shared_ptr<interactionData> Cell::getNextInteractionID()
{
	return nextInteractionID;
}
void Cell::setNextMovementTick(uint32_t _nextMovementTick)
{
	nextMovementTick.store(_nextMovementTick);
}
uint32_t Cell::getNextMovementTick()
{
	return nextMovementTick.load();
}
void Cell::setLastMovementTick(uint32_t _lastMovementTick)
{
	lastMovementTick.store(_lastMovementTick);
};
uint32_t Cell::getLastMovementTick()
{
	return lastMovementTick.load();
};
void Cell::setNextUpdateTick(uint32_t _nextUpdateTick)
{
	nextUpdateTick.store(_nextUpdateTick);
}
uint32_t Cell::getNextUpdateTick()
{
	return nextUpdateTick.load();;
}
void  Cell::setNextStateUpdateTick(uint32_t _nextStateUpdateTick)
{
	nextStateUpdateTick.store(_nextStateUpdateTick);
};
uint32_t  Cell::getNextStateUpdateTick()
{
	return nextStateUpdateTick.load();
};
void  Cell::setLastStateUpdateTick(uint32_t _lastStateUpdateTick)
{
	lastStateUpdateTick.store(_lastStateUpdateTick);
};
uint32_t  Cell::getLastStateUpdateTick()
{
	return lastStateUpdateTick.load();
};
void  Cell::setDeltaStateUpdateTick(uint32_t _deltStateUpdateTick)
{
	deltaStateUpdateTick.store(_deltStateUpdateTick);
};
uint32_t  Cell::getDeltaStateUpdateTick()
{
	return deltaStateUpdateTick.load();
};

std::shared_ptr<interactionData> Cell::updateNextInteraction(uint32_t & newInteractionTick, float & newDistance, const uint32_t& timeLimit)
{
	float closestTime = std::numeric_limits<float>::max();
	std::lock_guard<std::mutex> guard(mutexMapCPA);
	uint32_t currentTick = SimulationCounter::getSimulationTick();

	newInteractionTick = 0;
	std::shared_ptr<interactionData> newInteraction = std::make_shared<interactionData>();
	newDistance        = 0;

	for (mapCPAit = mapinstanceIDCPATime.begin(); mapCPAit != mapinstanceIDCPATime.end(); mapCPAit++)
	{
		std::pair<uint32_t, EInteractionType> interaction = mapCPAit->first;
		std::pair<float, float> CPAInfo = mapCPAit->second;

		float CPATime = CPAInfo.first;
		float CPADistance = CPAInfo.second;

		//If the cell is scanning or is in conglomerate don't bother other cell
		//if (cellInstanceID >= 0 && (isInConglomerate() || !emptyScanningCellID()))
		//{ continue; }
		if (CPATime < closestTime && !getMaskInstanceID(interaction.first))
		{
			uint32_t nextPossibleEventTick = secondToTick(std::max(tickToSecond(currentTick + 1), CPATime));

			//if ((nextPossibleTick - (20)) > currentTick + 1) { nextPossibleTick = nextPossibleTick - 20; }	
			//else { nextPossibleTick = currentTick + 1; }

			closestTime = CPATime;

			newInteraction = std::make_shared<interactionData>(interaction.second, interaction.first);

			newInteractionTick           = nextPossibleEventTick;
			newDistance                  = CPADistance;
		}
	}

	//Time limit is the time of next update. So if the time of interaction(with cell or wall) is bigger than the time to update
	//It is waited the next update.
	if ((newInteractionTick <= currentTick) || (newInteractionTick > timeLimit))
	{
		newInteraction->type = EInteractionType::NO_HIT;
		newInteraction->otherHitID = 0;

		newInteractionTick = timeLimit;
		newDistance        = 100 / 2.f;
	}

	return newInteraction;
}

//------------------PROXIMITY/SCANNING INFORMATION------------------

void Cell::setState(STATE newState)
{
	cellState.store(newState);
};
STATE Cell::getState()
{
	return cellState.load();
};

void Cell::setProximityCells(const std::vector<uint32_t> & _cellsID)
{
	std::lock_guard<std::mutex> guard(mutexVectorProximity);
	proximityCellsID = std::vector<uint32_t>(_cellsID);
}
void Cell::getProximityCells(std::vector<uint32_t> & _cellsID)
{
	std::lock_guard<std::mutex> guard(mutexVectorProximity);
	_cellsID = proximityCellsID;
}

void Cell::addScanningCellID(uint32_t _cellID)
{
	std::lock_guard<std::mutex> guard(mutexScanning);
	if (!isScanningCellID(_cellID))
		scanningCellID.push_back(_cellID);
};
uint32_t Cell::getScanningCellID()
{
	std::lock_guard<std::mutex> guard(mutexScanning);
	if (!emptyScanningCellID())
		return scanningCellID.back();
	else
		return -1;
};
void Cell::removeScanningCellID(uint32_t _cellID)
{
	std::lock_guard<std::mutex> guard(mutexScanning);
	scanningCellID.erase(std::remove(scanningCellID.begin(), scanningCellID.end(), _cellID), scanningCellID.end());
};
bool Cell::isScanningCellID(uint32_t _cellID)
{
	return std::find(scanningCellID.begin(), scanningCellID.end(), _cellID) != scanningCellID.end();
};
bool Cell::emptyScanningCellID()
{
	return scanningCellID.empty();
};
void Cell::clearScanningCellID()
{
	std::lock_guard<std::mutex> guard(mutexScanning);
	scanningCellID.clear();
};

void Cell::addBeingScannedByCellID(uint32_t _cellInstanceID)
{
	std::lock_guard<std::mutex> guard(mutexScanning);
	if (!isBeingScannedByCellID(_cellInstanceID))
		beingScannedByCellID.push_back(_cellInstanceID);
};
uint32_t Cell::getBeingScannedByCellID()
{
	std::lock_guard<std::mutex> guard(mutexScanning);
	if (!emptyBeingScannedByCellID())
		return beingScannedByCellID[0];

	else return -1;
};
void Cell::removeBeingScannedByCellID(uint32_t _cellInstanceID)
{
	std::lock_guard<std::mutex> guard(mutexScanning);
	beingScannedByCellID.erase(std::remove(beingScannedByCellID.begin(), beingScannedByCellID.end(), _cellInstanceID), beingScannedByCellID.end());

};
bool Cell::isBeingScannedByCellID(uint32_t _cellInstanceID)
{
	return std::find(beingScannedByCellID.begin(), beingScannedByCellID.end(), _cellInstanceID) != beingScannedByCellID.end();
};
void Cell::clearBeingScannedByCellID()
{
	std::lock_guard<std::mutex> guard(mutexScanning);
	beingScannedByCellID.clear();
};
bool Cell::emptyBeingScannedByCellID()
{
	return beingScannedByCellID.empty();
};

void Cell::addCPAToMap(uint32_t instanceID, EInteractionType interactionType, float CPA_time, float CPA_distance)
{
	std::lock_guard<std::mutex> guard(mutexMapCPA);
	mapinstanceIDCPATime[std::pair<uint32_t, EInteractionType>(instanceID, interactionType)] = std::pair<float, float>(CPA_time, CPA_distance);
}

float Cell::getCPAFromMap(uint32_t instanceID, EInteractionType interactionType)
{
	std::lock_guard<std::mutex> guard(mutexMapCPA);
	std::unordered_map<std::pair<uint32_t, EInteractionType>, std::pair<float, float>>::iterator mapCPAItFind =
		mapinstanceIDCPATime.find(std::pair<uint32_t, EInteractionType>(instanceID, interactionType));

	float timeMap = -1;

	if (mapCPAItFind != mapinstanceIDCPATime.end())
	{
		timeMap = mapinstanceIDCPATime[std::pair<uint32_t, EInteractionType>(instanceID, interactionType)].first;
	}
	return timeMap;
};
float Cell::getCPADistanceFromMap(uint32_t instanceID, EInteractionType interactionType)
{
	std::lock_guard<std::mutex> guard(mutexMapCPA);
	std::unordered_map<std::pair<uint32_t, EInteractionType>, std::pair<float, float>>::iterator mapCPAItFind =
		mapinstanceIDCPATime.find(std::pair<uint32_t, EInteractionType>(instanceID, interactionType));

	float distanceMap = -1;

	if (mapCPAItFind != mapinstanceIDCPATime.end())
	{
		std::pair<uint32_t, EInteractionType> key = std::pair<uint32_t, EInteractionType>(instanceID, interactionType);
		distanceMap = mapinstanceIDCPATime[key].second;
	}
	return distanceMap;
};
bool Cell::deleteCPAFromMap(uint32_t instanceID, EInteractionType interactionType)
{
	std::lock_guard<std::mutex> guard(mutexMapCPA);
	std::pair<uint32_t, EInteractionType> mapKeyPair = std::pair<uint32_t, EInteractionType>(instanceID, interactionType);

	mapCPAit = mapinstanceIDCPATime.find(mapKeyPair);

	if (mapCPAit != mapinstanceIDCPATime.end())
	{
		mapinstanceIDCPATime.erase(mapCPAit);
		return true;
	}

	return false;
}
void Cell::clearCPAMap()
{
	std::lock_guard<std::mutex> guard(mutexMapCPA);
	mapinstanceIDCPATime.clear();

	lastUpdateTick.store(SimulationCounter::getSimulationTick()); //Map is cleared only on on Update
}

void Cell::activateMaskInstanceID(uint32_t cellInstanceID)
{
	std::lock_guard<std::mutex> guard(mutexMapMask);
	mapMaskinstanceID[cellInstanceID] = true;
};
void Cell::deactivateMaskInstanceID(uint32_t cellInstanceID)
{
	std::lock_guard<std::mutex> guard(mutexMapMask);
	mapMaskinstanceID[cellInstanceID] = false;
};
void Cell::clearMaskInstanceID()
{
	std::lock_guard<std::mutex> guard(mutexMapMask);
	mapMaskinstanceID.clear();
};
bool Cell::getMaskInstanceID(int32_t cellInstanceID)
{
	std::lock_guard<std::mutex> guard(mutexMapMask);
	mapMaskinstanceIDit = mapMaskinstanceID.find(cellInstanceID);

	if (mapMaskinstanceIDit != mapMaskinstanceID.end())
	{
		return mapMaskinstanceIDit->second;
	}
	else
		return false;
};

void Cell::setConglomerateID(int32_t nextTick)
{
	cellConglomerateID.store(nextTick);
}
int32_t Cell::getConglomerateID()
{
	return cellConglomerateID.load();;
}
bool Cell::isInConglomerate()
{
	return (cellConglomerateID.load() >= 0);
};
void Cell::setStartConglomerateID(int32_t conglomerateID)
{
	cellStartConglomerateID.store(conglomerateID);
};
int32_t Cell::getStartConglomerateID()
{
	return cellStartConglomerateID.load();
};
void Cell::setStartConglomerateTick(int32_t conglomerateTick)
{
	cellStartConglomerateTick.store(conglomerateTick);

};
uint32_t Cell::getStartConglomerateTick()
{
	return cellStartConglomerateTick.load();
};


////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------RULES-------------------------------------------//
////////////////////////////////////////////////////////////////////////////////////////////

void Cell::updateReceptorEvent(uint32_t otherCellInstanceID)
{
	//Control all the receptor event with the cell just met
	uint32_t currentTick = SimulationCounter::getSimulationTick();
	uint32_t instanceID = getInstanceID();

	Cell* otherCell = cellRegion->getCell(otherCellInstanceID);
	int32_t conglomerateIDCell1 = this->getConglomerateID();
	int32_t conglomerateIDCell2 = otherCell->getConglomerateID();

	std::map<uint16_t, CellRulesData>::iterator rulesDataIt;
	
	for (rulesDataIt = rulesData.begin(); rulesDataIt != rulesData.end(); rulesDataIt++)
	{
		uint16_t ruleID        = rulesDataIt->first;
		CellRulesData ruleData = rulesDataIt->second;
		const Rule& rule       = SimulationData::getRuleFromMap(ruleID);

		bool allReceptorEventComplete       = true;	 //Flag to understand if all receptor event are complete
		bool isReceptorEvent                = false; //Flag if there is at least one receptor event

		for (uint16_t eventIdx = 0; eventIdx < rule.getEventsCount(); eventIdx++)
		{
			uint16_t eventID    = (rule.getEventsID())[eventIdx];
			const Event & event = rule.getEvent(eventID);
			if (event.isEmpty) continue;

			EventType eventType = event.getType();

			if (eventType != EventType::RECEPTOR_RECEPTOR_BINDING || !ruleData.CellEnvolvedInEvent[eventID]) { continue; }

			isReceptorEvent = true; // There is at leat one receptor event
			const ReceptorReceptorEvent& receptorReceptorEvent = dynamic_cast<const ReceptorReceptorEvent&>(event);  // Works
			
			if (Region::matchCellsPopulations(receptorReceptorEvent, *this, *otherCell) && isActivable(otherCellInstanceID))
			{
				if (conglomerateIDCell1 < 0 && 
					conglomerateIDCell2 < 0)
				{
					otherCell->rulesData[ruleID].EventComplete[eventID] = true;
					rulesData[ruleID].EventComplete[eventID] = true;
				}
				else if (
					cellRegion->checkConglomerateNumber(conglomerateIDCell2, getPopulationID()) &&
					conglomerateIDCell1 < 0 &&
					conglomerateIDCell2 >= 0)
				{
					rulesData[ruleID].EventComplete[eventID] = true;
				}
				else
				{
					allReceptorEventComplete &= false; //If one is false it gets false
				}
			}
			else 
			{
				allReceptorEventComplete &= false; //If one is false it gets false
			}
		} //end for event for

		if (allReceptorEventComplete && isReceptorEvent)
		{		
			if (conglomerateIDCell1 < 0 && conglomerateIDCell2 < 0)
			{
				int32_t conglomerateID = SimulationCounter::getNextConglomerateID();
				cellRegion->addCellToConglomerate(conglomerateID, getInstanceID());
				cellRegion->addCellToConglomerate(conglomerateID, otherCellInstanceID);

				otherCell->setState(STATE::CONGLOMERATE);
				setState(STATE::CONGLOMERATE);
				setStartConglomerateID(otherCellInstanceID);
				setStartConglomerateTick(currentTick);

				//std::string logString =
				//	"xx>(" +
				//	std::to_string(SimulationCounter::getSimulationTick()) +
				//	") START CONGLOMERATE With: " +
				//	std::to_string(otherCellInstanceID);
				//addToLOG(logString);
				//updateRuleActions();
				//otherCell->updateRuleActions();
				//SimulationData::addInstruction(stopConglomerateInstruction);
			}
			if (conglomerateIDCell1 < 0 && conglomerateIDCell2 >= 0)
			{
				cellRegion->addCellToConglomerate(conglomerateIDCell2, getInstanceID());
				setState(STATE::CONGLOMERATE);
				setStartConglomerateID(otherCellInstanceID);
				setStartConglomerateTick(currentTick);

				//std::string logString = "xx>(" +
				//	std::to_string(SimulationCounter::getSimulationTick()) +
				//	") START CONGLOMERATE With: " +
				//	std::to_string(otherCellInstanceID);
				//addToLOG(logString);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------DIYING------------------------------------------//
////////////////////////////////////////////////////////////////////////////////////////////

void Cell::killCell()
{
	setIsDead(true);

	if (getState() != STATE::CONGLOMERATE &&
		getState() != STATE::SCANNING &&
		getState() != STATE::APOPTOSIS &&
		emptyBeingScannedByCellID() &&
		emptyScanningCellID())
	{
		setState(STATE::APOPTOSIS);
		setSpeed(0);
		setDirection(glm::vec3(0));
		setRadius(getRadius() / 2.f);

		SimulationData::updateCell3DInstance(cellRegion->getID(), getInstanceID());
	}
}
#ifndef __CELL_H_
#define __CELL_H_

#include <map>
#include <iostream>
#include <algorithm>

#include "Simulation/SimulationManager.h"
#include "Simulation/SimulationData.h"
#include "Manager/IOManager.h"
#include "Manager/Saveable.h"

class Region;
//========================================================
// THe class Cell represent the Agent of the simulation.
// Here are performed all the action that require only the cell knowledge
// For what concern action to higher level (interaction wqith other cell, release in the environment)
// is the region which execute the action
//========================================================

struct pair_hash 
{
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const 
	{
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2;  
    }
};

class Cell
{
private:
	
	std::mutex mutexTransform;
	std::mutex mutexVectorProximity;
	std::mutex mutexMapCPA;
	std::mutex mutexMapMask;
	std::mutex mutexScanning;
	std::mutex mutexAction;

	uint32_t		  birthTime;			// Tick when the cell is createl
	std::atomic<bool> isMovable;			// True if it can move
	std::atomic<bool> isScanningActive;		// True if it is a scanning cell
	std::atomic<bool> isDead;				// True if the cell is dead

	//Cell Region Movement Interaction
	std::atomic<uint32_t> lastMovementTick;		// last movement time
	std::atomic<uint32_t> nextMovementTick;		// next movement scheduled time
	std::atomic<uint32_t> lastUpdateTick;		// Last update time
	std::atomic<uint32_t> nextUpdateTick;		//next update scheduled time
	std::atomic<uint32_t> lastStateUpdateTick;	// last state update tim,e
	std::atomic<uint32_t> nextStateUpdateTick;	// next state updated scheduled time
	std::atomic<uint32_t> deltaStateUpdateTick;	
	std::shared_ptr<interactionData> nextInteractionID;		//ID of the next cell that has to interact with this cell 

	std::atomic<int32_t> cellConglomerateID;
	std::atomic<int32_t> cellStartConglomerateID;
	std::atomic<uint32_t> cellStartConglomerateTick;

	std::vector<uint32_t> proximityCellsID;			//Vector of proximal cell returned by the tree
	std::vector<uint32_t> scanningCellID;			//Vector of cell ID being scanned by this Cell
	std::vector<uint32_t> beingScannedByCellID;		//Vector of cell ID scanning this Cell

	std::unordered_map<std::pair<uint32_t, EInteractionType>, std::pair<float, float>, pair_hash>			mapinstanceIDCPATime;	//Map of all cell in proximity with distance
	std::unordered_map<std::pair<uint32_t, EInteractionType>, std::pair<float, float>>::iterator	mapCPAit;

	std::unordered_map<int32_t, bool>								mapMaskinstanceID;		//Map of all Cell Masked by this Cell
	std::unordered_map<int32_t, bool> ::iterator					mapMaskinstanceIDit;

protected:

	Region* cellRegion;
	const CellPopulationData & cellPopulationData;

	std::atomic<STATE> cellState; 
	TransformData cellTransformData;

	std::map<uint16_t, CellRulesData>	rulesData;		//Date regarding the rules of which this cell follow
	std::map<uint16_t, float>			moleculesData;	//Data regarding the environmental molecule sensed by the cell


public:

	void save(IOManager* ioMgr, bool withReceptors);
	void load(IOManager* ioMgr);

	//Save Data
	std::mutex mutexLog;

	void saveCellInfo();
	std::vector<uint32_t> updateTime;
	std::vector<float> updateDistance;
	
	//Vector to locally save information !!!It can be moved everything to region
	std::vector<float> ActivationVector;
	std::vector<float> PD1Expression;
	std::vector<float> PDL1Expression;
	std::vector<float> CD80Expression;
	std::vector<float> CD28Expression;
	std::vector<float> IFNCell;

	std::vector<std::string> stateCollection;
	std::vector<uint16_t> stateCollectionNr;
	std::vector<std::string> logString;

	Cell(const CellPopulationData & _populationData, uint32_t cellInstanceID);
	~Cell();
	void resetCell(bool initialize);

	//GET-SET TRANSFORM  
	void setInstanceID(uint32_t _cellID);
	uint32_t getInstanceID();
	uint16_t getPopulationID();
	bool isAScanningCell();

	// --------------- CELL METHODS --------------------- 
	void setPosition(const glm::vec3 & _position);
	void addOffset(const glm::vec3 & _offset);
	glm::vec3 getPosition();
	glm::vec3 getUpdatedPosition();
	glm::vec3 getPositionAt(uint32_t time);
	void setDirection(const glm::vec3 & _direction);
	glm::vec3 getDirection();
	void setScale(const glm::vec3 &_direction);
	glm::vec3 getScale();
	void setRadius(float _radius);
	float getRadius();
	void setSpeed(float _speed);
	float getSpeed();
	void resetSpeed();
	void setStokesSpeed(float _speed);
	float getStokesSpeed();
	float getAllSpeed();

	void setScanTime(float _scanTime);
	float getScanTime();
	void setRefractaryTime(float _refractaryTime);
	float getRefractaryTime();

	void setColor(const glm::vec4 & inputColor);
	glm::vec4 getColor();
	void setColorBB(const glm::vec4 &  inputColorBB);
	glm::vec4 getColorBB();
	void  setGraphicInstanceIdx(uint32_t _graphicInstanceIdx);
	uint32_t getGraphicInstanceIdx();

	//RECEPTORS ------------------ 
	void setReceptorMap(const uint16_t & receptorID, const std::pair<float, float> & receptorInfo);
	const std::pair<float, float> & getReceptorMap(uint16_t receptorID);
	void addReceptors(const ReceptorPopulationData& receptorData);
	void setReceptorBindedFraction(uint16_t receptorPopulationID, float receptorBinded);
	float setReceptorOffsetBindedFraction(uint16_t receptorPopulationID, float offsetPercentage);
	void setReceptorExpression(uint16_t receptorPopulationID, float receptorExpression);
	float setReceptorOffsetExpression(uint16_t receptorPopulationID, float offsetPercentage);
	bool hasReceptor(uint16_t receptorType) const;

	//FLAG get set
	uint32_t getBirthTime()                  { return birthTime; }
	void setIsDead(bool _isDead)             { isDead.store(_isDead); };
	bool getIsDead()                         { return isDead.load(); };
	void setMovability(bool _isMovable)      { isMovable.store(_isMovable); };
	bool getMovability()                     { return isMovable.load(); };
	void setIsScanningActive(bool _isActive) { isScanningActive.store(_isActive); };
	bool getIsScanningActive()               { return isScanningActive.load(); };
	
	void addToLOG(std::string log);

	//Movement
	void move();	

	//Interaction
	void setNextInteractionID(std::shared_ptr<interactionData> nextInteractionID);
	std::shared_ptr<interactionData> getNextInteractionID();
	void setNextMovementTick(uint32_t nextMovementTick);
	uint32_t  getNextMovementTick();
	void setLastMovementTick(uint32_t nextMovementTick);
	void setNextUpdateTick(uint32_t _nextUpdateTick);
	uint32_t getNextUpdateTick();
	uint32_t  getLastMovementTick();
	void setNextStateUpdateTick(uint32_t nextStateUpdateTick);
	uint32_t getNextStateUpdateTick();
	void setLastStateUpdateTick(uint32_t lastStateUpdateTick);
	uint32_t getLastStateUpdateTick();
	void setDeltaStateUpdateTick(uint32_t deltStateUpdateTick);
	uint32_t getDeltaStateUpdateTick();

	std::shared_ptr<interactionData> updateNextInteraction(uint32_t& newNextTick, float & distance, const uint32_t& timeLimit);

	void setState(STATE newState);
	STATE getState();
	
	//Proximity scanning Info
	void setProximityCells(const std::vector<uint32_t> & _cellsID);
	void getProximityCells(std::vector<uint32_t> & _cellsID);

	void addScanningCellID(uint32_t _cellInstanceID);
	uint32_t getScanningCellID();
	void removeScanningCellID(uint32_t _cellInstanceID);
	void clearScanningCellID();
	bool isScanningCellID(uint32_t _cellInstanceID);
	bool emptyScanningCellID();

	void addBeingScannedByCellID(uint32_t _cellInstanceID);
	uint32_t getBeingScannedByCellID();
	void removeBeingScannedByCellID(uint32_t _cellInstanceID);
	void clearBeingScannedByCellID();
	bool isBeingScannedByCellID(uint32_t _cellInstanceID);
	bool emptyBeingScannedByCellID();

	void  addCPAToMap(uint32_t instanceID, EInteractionType interactionType, float CPA_time, float CPA_distance);
	float getCPAFromMap(uint32_t instanceID, EInteractionType interactionType);
	float getCPADistanceFromMap(uint32_t instanceID, EInteractionType interactionType);
	bool  deleteCPAFromMap(uint32_t instanceID, EInteractionType interactionType);
	void  clearCPAMap();

	void activateMaskInstanceID(uint32_t instanceID);
	void deactivateMaskInstanceID(uint32_t instanceID);
	void clearMaskInstanceID();
	bool getMaskInstanceID(int32_t instanceID);

	void setConglomerateID(int32_t conglomerateIDx);
	int32_t getConglomerateID();
	void setStartConglomerateID(int32_t conglomerateIDx);
	int32_t getStartConglomerateID();
	void setStartConglomerateTick(int32_t conglomerateTick);
	uint32_t getStartConglomerateTick();
	bool isInConglomerate();

	// ----------- RULES -----------------------
	//Event Update
	void updateReceptorEvent(uint32_t otherCellID);


	void killCell();

	// ----------- VIRTUAL FOR SPECIALIZATION BASED ON CELL -----------------------

	virtual void update() {};
	virtual void updateOnRule(uint16_t ruleID) {};
	virtual float evaluateActivation(uint32_t cellInstanceID) { return 0.f; };
	virtual bool isActivable(uint32_t cellInstanceID)		  { return false; };

	virtual void saveData() {};
};
#endif
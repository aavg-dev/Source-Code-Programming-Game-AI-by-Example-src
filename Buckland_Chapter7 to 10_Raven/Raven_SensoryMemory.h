#ifndef RAVEN_SENSORY_SYSTEM_H
#define RAVEN_SENSORY_SYSTEM_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:
//
//  Author: Mat Buckland (ai-junkie.com)
//
//  Desc:
//
//-----------------------------------------------------------------------------
#include <map>
#include <list>
#include "../Common/2D/Vector2D.h"

class Raven_Bot;


//Stores information about each of the opponents present in the field
//The owner of this class will have a MemoryRecord instance for each 
// of the existing opponents in Raven_SensoryMemory
class MemoryRecord
{
public:
  
  //records the time the opponent was last sensed (seen or heard). This
  //is used to determine if a bot can 'remember' this record or not. 
  //(if CurrentTime() - m_dTimeLastSensed is greater than the bot's
  //memory span, the data in this record is made unavailable to clients)
  double       fTimeLastSensed;

  //it can be useful to know how long an opponent has been visible. This 
  //variable is tagged with the current time whenever an opponent first becomes
  //visible. It's then a simple matter to calculate how long the opponent has
  //been in view (CurrentTime - fTimeBecameVisible)
  double       fTimeBecameVisible;

  //it can also be useful to know the last time an opponent was seen
  double       fTimeLastVisible;


  //store the last time we got hit by a specific bot and how
  //much damage that bot made to us 
  //int --> total damage  
  //double --> last hit time
  std::pair<int, double> fLastHitTimeAndTotalDamagePerBot;

  //a vector marking the position where the opponent was last sensed. This can
  // be used to help hunt down an opponent if it goes out of view
  Vector2D    vLastSensedPosition;

  //set to true if opponent is within the field of view of the owner
  bool        bWithinFOV;

  //set to true if there is no obstruction between the opponent and the owner, 
  //permitting a shot.
  bool        bShootable;
  

  MemoryRecord():fTimeLastSensed(-999),
            fTimeBecameVisible(-999),
            fTimeLastVisible(0),
            bWithinFOV(false),
            bShootable(false)
  {}
};



class Raven_SensoryMemory
{
private:

  typedef std::map<Raven_Bot*, MemoryRecord> MemoryMap;

private:
  
  //the owner of this instance
  Raven_Bot* m_pOwner;

  //this container is used to simulate memory of sensory events. A MemoryRecord
  //is created for each opponent in the environment. Each record is updated 
  //whenever the opponent is encountered. (when it is seen or heard)
  MemoryMap  m_MemoryMap;

  //a bot has a memory span equivalent to this value. When a bot requests a 
  //list of all recently sensed opponents this value is used to determine if 
  //the bot is able to remember an opponent or not.
  double      m_dMemorySpan;

  //this methods checks to see if there is an existing record for pBot. If
  //not a new MemoryRecord record is made and added to the memory map.(called
  //by UpdateWithSoundSource & UpdateVision)
  void       MakeNewRecordIfNotAlreadyPresent(Raven_Bot* pBot);

public:

  Raven_SensoryMemory(Raven_Bot* owner, double MemorySpan);

  //this method is used to update the memory map whenever an opponent makes
  //a noise
  void     UpdateWithSoundSource(Raven_Bot* pNoiseMaker);

  //this removes a bot's record from memory
  void     RemoveBotFromMemory(Raven_Bot* pBot);

  //this method iterates through all the opponents in the game world and 
  //updates the records of those that are in the owner's FOV
  void     UpdateVision();

  //this method updates the registry of hits the bot has received from 
  //the rest of the bots with info of the damage taken
  //void     UpdateHitsReceived(Raven_Bot* pBot, int damageReceived);
  void     UpdateHitsReceived(Raven_Bot* pDamageDealeR, int damageReceived);


  bool     isOpponentShootable(Raven_Bot* pOpponent)const;
  bool     isOpponentWithinFOV(Raven_Bot* pOpponent)const;
  Vector2D GetLastRecordedPositionOfOpponent(Raven_Bot* pOpponent)const;
  double    GetTimeOpponentHasBeenVisible(Raven_Bot* pOpponent)const;
  double    GetTimeSinceLastSensed(Raven_Bot* pOpponent)const;
  double    GetTimeOpponentHasBeenOutOfView(Raven_Bot* pOpponent)const;
  double  GetTotalDamageReceived(Raven_Bot* pOpponent) const;


  //this method returns a list of all the opponents that have had their
  //records updated within the last m_dMemorySpan seconds.
  std::list<Raven_Bot*> GetListOfRecentlySensedOpponents()const;

  void     RenderBoxesAroundRecentlySensed()const;

};


#endif
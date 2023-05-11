#include "Raven_TargetingSystem.h"
#include "Raven_Bot.h"
#include "Raven_SensoryMemory.h"
#include "lua/Raven_Scriptor.h"
#include <functional>



//-------------------------------- ctor ---------------------------------------
//-----------------------------------------------------------------------------
Raven_TargetingSystem::Raven_TargetingSystem(Raven_Bot* owner, TargetSelectionCriteria criteria):m_pOwner(owner),
                                                               m_pCurrentTarget(0), m_currentCriteria(criteria)
{}



//----------------------------- Update ----------------------------------------

//-----------------------------------------------------------------------------
void Raven_TargetingSystem::Update()
{
  m_pCurrentTarget       = nullptr;

  //grab a list of all the opponents the owner can sense
  std::list<Raven_Bot*> SensedBots;
  SensedBots = m_pOwner->GetSensoryMem()->GetListOfRecentlySensedOpponents();
  
  std::list<Raven_Bot*>::const_iterator curBot = SensedBots.begin();

    ///Assign current target depending on different criteria
    switch (m_currentCriteria)
    {
    case closer_distance:
    default:
    {
        double ClosestDistSoFar = MaxDouble;
        for (curBot; curBot != SensedBots.end(); ++curBot)
        {
            //make sure the bot is alive and that it is not the owner
            if (!((*curBot)->isAlive()) || (*curBot == m_pOwner))
            {
                continue;
            }

            double dist = Vec2DDistanceSq((*curBot)->Pos(), m_pOwner->Pos());
            if (dist < ClosestDistSoFar)
            {
                ClosestDistSoFar = dist;
                m_pCurrentTarget = *curBot;
            }
        }
    }
    break;
    case most_damaged_enemy:
    {
        int WeakestEnemyHealth = script->GetInt("Bot_MaxHealth");
        for (curBot; curBot != SensedBots.end(); ++curBot)
        {
            //make sure the bot is alive and that it is not the owner
            if (!((*curBot)->isAlive()) || (*curBot == m_pOwner))
            {
                continue;
            }

            if ((*curBot)->Health() <= WeakestEnemyHealth) {
                WeakestEnemyHealth = (*curBot)->Health();
                m_pCurrentTarget = *curBot;
            }
        }
    }
    break;
    case most_damaged_received:
    {
        int DamageReceived = 0;
        for (curBot; curBot != SensedBots.end(); ++curBot)
        {
            //make sure the bot is alive and that it is not the owner
            if (!((*curBot)->isAlive()) || (*curBot == m_pOwner))
            {
                continue;
            }

            int curBotDamageReceived = GetTotalDamageReceived(*curBot);
            if (curBotDamageReceived >= DamageReceived) {
                DamageReceived = curBotDamageReceived;
                m_pCurrentTarget = *curBot;
            }
        }
    }
    break;
    
    case invalid:
        break;
    }


      
    
 }





bool Raven_TargetingSystem::isTargetWithinFOV()const
{
  return m_pOwner->GetSensoryMem()->isOpponentWithinFOV(m_pCurrentTarget);
}

bool Raven_TargetingSystem::isTargetShootable()const
{
  return m_pOwner->GetSensoryMem()->isOpponentShootable(m_pCurrentTarget);
}

Vector2D Raven_TargetingSystem::GetLastRecordedPosition()const
{
  return m_pOwner->GetSensoryMem()->GetLastRecordedPositionOfOpponent(m_pCurrentTarget);
}

double Raven_TargetingSystem::GetTimeTargetHasBeenVisible()const
{
  return m_pOwner->GetSensoryMem()->GetTimeOpponentHasBeenVisible(m_pCurrentTarget);
}

double Raven_TargetingSystem::GetTimeTargetHasBeenOutOfView()const
{
  return m_pOwner->GetSensoryMem()->GetTimeOpponentHasBeenOutOfView(m_pCurrentTarget);
}

double Raven_TargetingSystem::GetTotalDamageReceived(Raven_Bot* pOpponent)const
{
    return m_pOwner->GetSensoryMem()->GetTotalDamageReceived(pOpponent);
}

double Raven_TargetingSystem::GetTotalDamageReceived()const
{
    return GetTotalDamageReceived(m_pCurrentTarget);
}

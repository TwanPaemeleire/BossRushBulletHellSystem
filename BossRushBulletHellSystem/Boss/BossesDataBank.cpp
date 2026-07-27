#include "BossesDataBank.h"
#include <IdCreator.h>
#include <Attacks/BossAction.h>
#include "BossActionsFunctions.h"
#include <Utils/Blackboard.h>

// This is a temp way of creating it!!! Need to make some sort of factory with file save/loading for this asap

BossesDataBank::BossesDataBank()
{
	BossActionsMap =
	{
		{
			CreateId("TestAction"),
			BossAction
			{
				.ActionId = CreateId("TestAction"),
				.OnUpdateFunction = BossActionsFunctions::TestFunction,
			}
		}
	};

	BossesConfigCollectionsMap =
	{
		{
			CreateId("TestBoss"),
			BossConfigCollection
			{
				.BossConfigsMap =
				{
					{
						1,
						Bossconfig
						{
							.PossibleActionsPerPhaseIds =
							{
								{
									BossPhaseActionConfig
									{
										.ChanceWeight = 20,
										.Id = CreateId("TestAction"),
										.Blackboard = Blackboard()
										
									}
								}
							}
						}
					}
				}
			}
		}
	};
}
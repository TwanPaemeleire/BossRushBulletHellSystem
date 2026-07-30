#include "BossesDataBank.h"
#include <IdCreator.h>
#include <BossActions/BossAction.h>
#include <BossActions/BossActionsFunctions.h>

BossesDataBank::BossesDataBank()
{
	BossActionsMap =
	{
		{
			CreateId("RotatingShooters"),
			BossAction
			{
				.ActionId = CreateId("RotatingShooters"),
				.OnStartFunction = BossActionsFunctions::RotatingShootersStart,
				.OnUpdateFunction = BossActionsFunctions::RotatingShootersUpdate,
				.OnFinishedFunction = BossActionsFunctions::RotatingShootersStop,
				.OnStoppedFunction = BossActionsFunctions::RotatingShootersStop,
			}
		}
	};
}
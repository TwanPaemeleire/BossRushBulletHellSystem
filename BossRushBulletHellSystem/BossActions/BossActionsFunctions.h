#pragma once

namespace BossActionsFunctions
{
	// Params needed: AmountOfShooters, AmountOfFullRotations, ShootersRotationSpeed, ShootersRotationSpeedApplyTime, BulletShootDelay
	void RotatingShootersStart();
	void RotatingShootersUpdate();
	void RotatingShootersStop();
}
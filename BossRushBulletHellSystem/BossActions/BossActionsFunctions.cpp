#include "BossActionsFunctions.h"
#include <EntityManager.h>
#include <Boss/RuntimeCurrentBossData.h>
#include <IdCreator.h>
#include <CustomCoroutine.h>
#include <cstdlib>
#include <ctime>
#include <Vector2.h>
#include <BloodTime.h>
#include <MathUtils.h>

using namespace Bloodforge;

void BossActionsFunctions::RotatingShootersStart()
{
    EntityManager& entityManager = EntityManager::GetInstance();
    RuntimeCurrentBossData& runtimeBossData = entityManager.GetOrCreateFirstEntityWithComponents<RuntimeCurrentBossData>().GetComponent<RuntimeCurrentBossData>();
    Blackboard& currentBlackboard = runtimeBossData.GetCurrentBlackboard();

    srand(time(0));
    int randomNum = rand() % 2;
    int randomDirection = std::rand();
    currentBlackboard.Set<bool>(CreateId("RotatingClockwise"), randomDirection == 0);
    currentBlackboard.Set<std::vector<float>>(CreateId("ShootingAngles"), {});
    currentBlackboard.Set<std::vector<int>>(CreateId("RotatingCoroutines"), {});
    currentBlackboard.Set<std::vector<int>>(CreateId("ShootingCoroutines"), {});

    int amountOfShooters = currentBlackboard.Get<int>(CreateId("AmountOfShooters"));

    float shooterAngleInterval = 360.0f / (float)amountOfShooters;
    for (int shooterIdx = 0; shooterIdx < amountOfShooters; ++shooterIdx)
    {
        float angle = shooterIdx * shooterAngleInterval;
        currentBlackboard.Get<std::vector<float>>(CreateId("ShootingAngles")).emplace_back(angle);
        currentBlackboard.Get<std::vector<int>>(CreateId("RotatingCoroutines")).emplace_back(StartCoroutine(ShootWhileRotating(angle, shooterIdx)));
    }
}

void BossActionsFunctions::RotatingShootersUpdate()
{
}

Coroutine ShootWhileRotating(float startAngle, int shooterIdx)
{
    EntityManager& entityManager = EntityManager::GetInstance();
    RuntimeCurrentBossData& runtimeBossData = entityManager.GetOrCreateFirstEntityWithComponents<RuntimeCurrentBossData>().GetComponent<RuntimeCurrentBossData>();
    Blackboard& currentBlackboard = runtimeBossData.GetCurrentBlackboard();

    std::vector<float>& shootingAngles = currentBlackboard.Get<std::vector<float>>(CreateId("ShootingAngles"));
    float amountOfFullRotations = currentBlackboard.Get<float>(CreateId("AmountOfFullRotations"));
    float shootersRotationSpeed = currentBlackboard.Get<float>(CreateId("ShootersRotationSpeed"));
    float shootersRotationSpeedApplyTime = currentBlackboard.Get<float>(CreateId("ShootersRotationSpeedApplyTime"));
    bool rotatingClockwise = currentBlackboard.Get<bool>(CreateId("RotatingClockwise"));

    float angleTracker = 0.0f;
    shootingAngles[shooterIdx] = startAngle;

    currentBlackboard.Get<std::vector<int>>(CreateId("ShootingCoroutines")).emplace_back(StartCoroutine(ShootBullet(shooterIdx)));

    float elapsedTime = 0.0f;
    while (angleTracker < amountOfFullRotations * 360.0f)
    {
        elapsedTime += BloodTime::GetInstance().DeltaTime;
        float smoothSpeedIncrease = MathUtils::SmoothLerp(0.0f, shootersRotationSpeed, elapsedTime / shootersRotationSpeedApplyTime);
        float angleToAdd = smoothSpeedIncrease * BloodTime::GetInstance().DeltaTime;

        shootingAngles = currentBlackboard.Get<std::vector<float>>(CreateId("ShootingAngles"));
        if (!rotatingClockwise) shootingAngles[shooterIdx] -= angleToAdd;
        else shootingAngles[shooterIdx] += angleToAdd;
        angleTracker += angleToAdd;
        WaitUntilNextFrame();
    }

    runtimeBossData = entityManager.GetFirstEntityWithComponents<RuntimeCurrentBossData>().value().GetComponent<RuntimeCurrentBossData>();
    runtimeBossData.CurrentActionFinishedTrigger = true;
}

Coroutine ShootBullet(int shooterIdx)
{
    while (true)
    {
        EntityManager& entityManager = EntityManager::GetInstance();
        RuntimeCurrentBossData& runtimeBossData = entityManager.GetOrCreateFirstEntityWithComponents<RuntimeCurrentBossData>().GetComponent<RuntimeCurrentBossData>();
        Blackboard& currentBlackboard = runtimeBossData.GetCurrentBlackboard();

        float angle = currentBlackboard.Get<std::vector<float>>(CreateId("ShootingAngles"))[shooterIdx] * Vector2::DegreesToRadians;
        Vector2 direction;
        direction.X = std::cos(angle);
        direction.Y = std::sin(angle);

        // Spawn bullet here
        WaitForSeconds(currentBlackboard.Get<float>(CreateId("BulletShootDelay")));
    }
}

void BossActionsFunctions::RotatingShootersStop()
{
    EntityManager& entityManager = EntityManager::GetInstance();
    RuntimeCurrentBossData& runtimeBossData = entityManager.GetOrCreateFirstEntityWithComponents<RuntimeCurrentBossData>().GetComponent<RuntimeCurrentBossData>();
    Blackboard& currentBlackboard = runtimeBossData.GetCurrentBlackboard();

    for (int id: currentBlackboard.Get<std::vector<int>>(CreateId("RotatingCoroutines")))
    {
        StopCoroutine(id);
    }

    for (int id : currentBlackboard.Get<std::vector<int>>(CreateId("ShootingCoroutines")))
    {
        StopCoroutine(id);
    }
}
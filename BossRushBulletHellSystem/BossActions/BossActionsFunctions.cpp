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
#include <ResourceManager.h>
#include <SpriteAnimatorComponent.h>
#include <SpriteComponent.h>
#include <Projectiles/BasicProjectile.h>
#include <RectColliderComponent.h>

using namespace Bloodforge;

Coroutine ShootBullet(int shooterIdx)
{
    while (true)
    {
        EntityManager& entityManager = EntityManager::GetInstance();
        RuntimeCurrentBossData& runtimeBossData = entityManager.GetOrCreateFirstEntityWithComponents<RuntimeCurrentBossData>().GetComponent<RuntimeCurrentBossData>();
        Blackboard& currentBlackboard = runtimeBossData.GetCurrentBlackboard();

        Entity& bulletEntity = entityManager.CreateEntity();
		bulletEntity.Tag = CreateId("BossProjectile");
        int bulletEntityId = bulletEntity.Id;

        SpriteComponent* sprite = entityManager.AddComponent<SpriteComponent>(bulletEntityId);
        sprite->SetTexture(ResourceManager::GetInstance().LoadTexture("Projectile.png"));
        sprite->SetCustomSourceRect({ 0.f, 0.f, sprite->GetTexture()->GetWidth() / 5.0f, sprite->GetTexture()->GetHeight() });

        SpriteAnimatorComponent* animator = entityManager.AddComponent<SpriteAnimatorComponent>(bulletEntityId);
        AnimationData animData;
        animData.NumberOfFrames = 5;
        animData.Texture = ResourceManager::GetInstance().LoadTexture("Projectile.png");
        animData.ShouldLoop = true;
        animator->AddAnimation(CreateId("ProjectileAnim"), animData);
        animator->PlayAnimation(CreateId("ProjectileAnim"));

        BasicProjectile* projectile = entityManager.AddComponent<BasicProjectile>(bulletEntityId);
        projectile->Speed = 200.0f;

        runtimeBossData = entityManager.GetFirstEntityWithComponents<RuntimeCurrentBossData>().value().GetComponent<RuntimeCurrentBossData>();
        currentBlackboard = runtimeBossData.GetCurrentBlackboard();

        std::vector<float>& shootingAngles = currentBlackboard.Get<std::vector<float>>(CreateId("ShootingAngles"));

        TransformComponent* transform = entityManager.GetComponent<TransformComponent>(bulletEntityId);
        transform->SetLocalRotation(shootingAngles[shooterIdx]);
        transform->SetLocalPosition(currentBlackboard.Get<Vector2>(CreateId("BulletSpawnPos")));

		RectColliderComponent* collider = entityManager.AddComponent<RectColliderComponent>(bulletEntityId);
		collider->SetSize({ 42.0f, 20.0f });

        co_await WaitForSeconds(currentBlackboard.Get<float>(CreateId("BulletShootDelay")));
    }
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
        co_await WaitUntilNextFrame();
    }

    runtimeBossData = entityManager.GetFirstEntityWithComponents<RuntimeCurrentBossData>().value().GetComponent<RuntimeCurrentBossData>();
    runtimeBossData.CurrentActionFinishedTrigger = true;
}

void BossActionsFunctions::RotatingShootersStart()
{
    EntityManager& entityManager = EntityManager::GetInstance();
    RuntimeCurrentBossData& runtimeBossData = entityManager.GetOrCreateFirstEntityWithComponents<RuntimeCurrentBossData>().GetComponent<RuntimeCurrentBossData>();
    Blackboard& currentBlackboard = runtimeBossData.GetCurrentBlackboard();

    bool clockwise = (std::rand() % 2) == 0;
    currentBlackboard.Set(CreateId("RotatingClockwise"), clockwise);
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
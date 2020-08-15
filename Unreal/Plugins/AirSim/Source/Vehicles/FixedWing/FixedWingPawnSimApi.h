#pragma once

#include "CoreMinimal.h"

#include "PawnSimApi.h"
#include "vehicles/fixedwing/FixedWingPhysicsBody.hpp"
#include "vehicles/fixedwing/FixedWingParams.hpp"
#include "physics//Kinematics.hpp"
#include "common/Common.hpp"
#include "common/CommonStructs.hpp"
#include "common/common_utils/UniqueValueMap.hpp" 
#include "FixedWingPawnEvents.h"
#include <future>


class FixedWingPawnSimApi : public PawnSimApi
{
public:
    typedef msr::airlib::real_T real_T;
    typedef msr::airlib::Utils Utils;
    typedef msr::airlib::FixedWingPhysicsBody FixedWing;
    typedef msr::airlib::StateReporter StateReporter;
    typedef msr::airlib::UpdatableObject UpdatableObject;
    typedef msr::airlib::Pose Pose;

    typedef FixedWingPawnEvents::FixedWingElevatorInfo FixedWingElevatorInfo;
    typedef FixedWingPawnEvents::FixedWingAileronInfo FixedWingAileronInfo;
    typedef FixedWingPawnEvents::FixedWingRudderInfo FixedWingRudderInfo;

public:
    virtual void initialize() override;

    virtual ~FixedWingPawnSimApi() = default;

    //VehicleSimApiBase interface
    //implements game interface to update pawn
    FixedWingPawnSimApi(const Params& params);
    virtual void updateRenderedState(float dt) override;
    virtual void updateRendering(float dt) override;

    //PhysicsBody interface
    //this just wrapped around MultiRotor physics body
    virtual void resetImplementation() override;
    virtual void update() override;
    virtual void reportState(StateReporter& reporter) override;
    virtual UpdatableObject* getPhysicsBody() override;

    virtual void setPose(const Pose& pose, bool ignore_collision) override;
    virtual void pawnTick(float dt) override;

    msr::airlib::FixedWingApiBase* getVehicleApi() const
    {
        return vehicle_api_.get();
    }

    virtual msr::airlib::VehicleApiBase* getVehicleApiBase() const override
    {
        return vehicle_api_.get();
    }

private:
    std::unique_ptr<msr::airlib::FixedWingApiBase> vehicle_api_;
    std::unique_ptr<msr::airlib::FixedWingParams> vehicle_params_;

    std::unique_ptr<FixedWing> fixedwing_physics_body_;
    unsigned int control_count;
    std::vector<FixedWingElevatorInfo> elevator_info_;
    std::vector<FixedWingAileronInfo> aileron_info_;
    std::vector<FixedWingRudderInfo> rudder_info_;

    //show info on collision response from physics engine
    CollisionResponse collision_response;

    FixedWingPawnEvents* pawn_events_;

    //when pose needs to set from non-physics thread, we set it as pending
    bool pending_pose_collisions_;
    enum class PendingPoseStatus {
        NonePending, RenderStatePending, RenderPending
    } pending_pose_status_;
    Pose pending_phys_pose_; //force new pose through API

    //reset must happen while World is locked so its async task initiated from API thread
    bool reset_pending_;
    bool did_reset_;
    std::packaged_task<void()> reset_task_;

    Pose last_phys_pose_; //for trace lines showing vehicle path
    std::vector<std::string> vehicle_api_messages_;
};
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Odometry_Tests
#define _USE_MATH_DEFINES
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <chrono>
#include <thread>
#include "Arduino.h"
#include "OdometryManager.h"
#include "TwoWheelOdometryManager.h"
#include "Encoder.h"
#include "RotaryEncoder.h"
#include "MockEncoder.h"

BOOST_AUTO_TEST_SUITE(TestOdometry)

BOOST_AUTO_TEST_CASE(TestOdometryManagerContructionAndInterface) {
    // TwoWheelOdometryManager expects two encoders and dimensions for the robot.
    // It should also be useable through the generic OdometryManager interface

    OdometryManager* odoManager = new TwoWheelOdometryManager(1,1, new RotaryEncoder(0, 1), new RotaryEncoder(1, 1));
    // should be able to call these methods
    odoManager->GetLinearVelocity();
    odoManager->GetAngularVelocity();
    delete odoManager;
}

BOOST_AUTO_TEST_CASE(TestGetLinearVelocity) {
    MockEncoder* leftEncoder = new MockEncoder;
    MockEncoder* rightEncoder = new MockEncoder;

    TwoWheelOdometryManager odometryManager(100, 50, leftEncoder, rightEncoder);
    
    float wheelCircumference = 2 * M_PI * 50;
    // implement unicylce model. v = 1/2(v_l + v_r)

    // going straight
    leftEncoder->SetDirection(Encoder::Direction::FORWARDS);
    rightEncoder->SetDirection(Encoder::Direction::FORWARDS);
    for(float i = 0; i < 2; i += 0.1) {
        leftEncoder->SetFrequency(i);
        rightEncoder->SetFrequency(i);
        float leftSpeed = leftEncoder->GetFrequency() * wheelCircumference;
        float rightSpeed = rightEncoder->GetFrequency() * wheelCircumference;
        int speedShouldBe = 1.0/2 * (leftSpeed + rightSpeed);

        BOOST_CHECK_EQUAL(speedShouldBe, odometryManager.GetLinearVelocity());
    }

    // turning
    leftEncoder->SetFrequency(1);
    for(float i = 0; i < 2; i += 0.1) {
        rightEncoder->SetFrequency(i);
        float leftSpeed = leftEncoder->GetFrequency() * wheelCircumference;
        float rightSpeed = rightEncoder->GetFrequency() * wheelCircumference;
        int speedShouldBe = 1.0/2 * (leftSpeed + rightSpeed);

        BOOST_CHECK_EQUAL(speedShouldBe, odometryManager.GetLinearVelocity());
    }

    // turning on spot
    leftEncoder->SetDirection(Encoder::Direction::FORWARDS);
    rightEncoder->SetDirection(Encoder::Direction::BACKWARDS);
    for(float i = 0; i < 2; i += 0.1) {
        leftEncoder->SetFrequency(i);
        rightEncoder->SetFrequency(i);
        BOOST_CHECK_EQUAL(0, odometryManager.GetLinearVelocity());
    }

    // backing up
    leftEncoder->SetDirection(Encoder::Direction::BACKWARDS);
    rightEncoder->SetDirection(Encoder::Direction::BACKWARDS);
    for(float i = 0; i < 2; i += 0.1) {
        leftEncoder->SetFrequency(i);
        rightEncoder->SetFrequency(i);
        float leftSpeed = -leftEncoder->GetFrequency() * wheelCircumference;
        float rightSpeed = -rightEncoder->GetFrequency() * wheelCircumference;
        int speedShouldBe = 1.0/2 * (leftSpeed + rightSpeed);
        BOOST_CHECK_EQUAL(speedShouldBe, odometryManager.GetLinearVelocity());
    }
    delete leftEncoder;
    delete rightEncoder;
}

BOOST_AUTO_TEST_CASE(TestAngularVelocity) {
    MockEncoder* leftEncoder = new MockEncoder;
    MockEncoder* rightEncoder = new MockEncoder;

    int robotWidth = 100;
    int wheelRadius = 50;
    float wheelCircumference = 2 * M_PI * 50;
    TwoWheelOdometryManager odometryManager(robotWidth, wheelRadius, leftEncoder, rightEncoder);
    // implement unicylce model. ω = 1/L(v_l - v_r)
    
    leftEncoder->SetFrequency(0);
    rightEncoder->SetFrequency(0);
    leftEncoder->SetDirection(Encoder::Direction::FORWARDS);
    rightEncoder->SetDirection(Encoder::Direction::FORWARDS);
    for (float i = 0.1; i < 2; i+= 0.1) {
        leftEncoder->SetFrequency(i);
        float leftSpeed = i * wheelCircumference;
        float angularVelocityShouldBe = 1.0/robotWidth * (leftSpeed - 0);
        BOOST_CHECK_EQUAL(angularVelocityShouldBe, odometryManager.GetAngularVelocity());
    }

    delete leftEncoder;
    delete rightEncoder;
}

BOOST_AUTO_TEST_CASE(TestRotaryEncoderConstructionAndInterface) {
    //RotaryEncoder has an Encoder interface
    Encoder* encoder = new RotaryEncoder(0, 1000/3.0);

    //should be able to call these methods
    encoder->GetFrequency();
    delete encoder;
}

BOOST_AUTO_TEST_CASE(TestRotaryEncoderGetFreqency) {
    RotaryEncoder(0, 1000/3.0);
    for(int i = 0; i < 50; i++) {
        //ArduinoUno.TriggerInterrupt(0);
        std::this_thread::sleep_for(std::chrono::microseconds(5));
    }
}
    
BOOST_AUTO_TEST_SUITE_END()

#include <DGM/classes/Controller.hpp>
#include <algorithm>
#include <cassert>

void dgm::Controller::update()
{
    controllerConnected =
        XInputGetState(controllerIndex, &xstate) == ERROR_SUCCESS;
}

bool dgm::Controller::isInputToggled(const int code) const
{
    auto& binding = bindings.at(code);

    // Extra checks on mouse and keyboard are needed because:
    const bool mousePressed = isMouseInputToggled(binding);
    const bool keyPressed = isKeyboardInputToggled(binding);
    const bool gamepadButtonPressed = isGamepadInputToggled(binding);
    const bool axisToggled = std::abs(getAxisValue(binding)) > 0.5f;
    const bool pressed =
        mousePressed || keyPressed || gamepadButtonPressed || axisToggled;

    if (pressed && binding.releaseOnRead && !binding.released)
    {
        binding.released = true;
        return true;
    }

    return pressed ? not bindings.at(code).released
                   : (bindings.at(code).released =
                          false); // assignment is on purpose, returns false and
                                  // marks the key as physically released
}

float dgm::Controller::getInputValue(const int code) const
{
    auto& binding = bindings.at(code);
    if (isMouseInputToggled(binding) || isKeyboardInputToggled(binding)
        || isGamepadInputToggled(binding))
        return bindings[code].negateMultiplier;

    if (!controllerConnected) return 0.f;

    return getAxisValue(binding);
}

void dgm::Controller::vibrate(
    const std::uint16_t leftMotorForce,
    const std::uint16_t rightMotorForce) noexcept
{
    XINPUT_VIBRATION vibration;
    vibration.wLeftMotorSpeed = static_cast<WORD>(leftMotorForce);
    vibration.wRightMotorSpeed = static_cast<WORD>(rightMotorForce);
    XInputSetState(controllerIndex, &vibration);
}

void dgm::Controller::bindInput(const int code, dgm::Xbox::Axis axis)
{
    bindings[code].xaxis = axis;
    // If enum code is for negative half of axis
    if (static_cast<std::size_t>(axis) >= 200)
        bindings[code].negateMultiplier = -1.f;
}

constinit const SHORT S_0 = 0;
constinit const SHORT STICK_MAX = 32767;
constinit const SHORT STICK_MAX_N = -32767;
constinit const float STICK_MAX_F = static_cast<float>(STICK_MAX);

constexpr static float getNormalizedAxisValue(
    const dgm::Xbox::Axis axis, XINPUT_GAMEPAD state) noexcept
{
    // Normalize everything to intervals <-1, 1>
    // see docs for more details:
    // https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad
    switch (axis)
    {
    case dgm::Xbox::Axis::LStickXpos:
        return std::clamp<SHORT>(state.sThumbLX, 0, STICK_MAX) / STICK_MAX_F;
    case dgm::Xbox::Axis::LStickXneg:
        return std::clamp<SHORT>(state.sThumbLX, -STICK_MAX, 0) / STICK_MAX_F;
    case dgm::Xbox::Axis::LStickYpos:
        return std::clamp<SHORT>(state.sThumbLY, 0, STICK_MAX) / STICK_MAX_F;
    case dgm::Xbox::Axis::LStickYneg:
        return std::clamp<SHORT>(state.sThumbLY, -STICK_MAX, 0) / STICK_MAX_F;
    case dgm::Xbox::Axis::RStickXpos:
        return std::clamp<SHORT>(state.sThumbRX, 0, STICK_MAX) / STICK_MAX_F;
    case dgm::Xbox::Axis::RStickXneg:
        return std::clamp<SHORT>(state.sThumbRX, -STICK_MAX, 0) / STICK_MAX_F;
    case dgm::Xbox::Axis::RStickYpos:
        return std::clamp<SHORT>(state.sThumbRY, 0, STICK_MAX) / STICK_MAX_F;
    case dgm::Xbox::Axis::RStickYneg:
        return std::clamp<SHORT>(state.sThumbRY, -STICK_MAX, 0) / STICK_MAX_F;
    case dgm::Xbox::Axis::LTrigger:
        return state.bLeftTrigger / 255.f;
    case dgm::Xbox::Axis::RTrigger:
        return state.bRightTrigger / 255.f;
    default:
        return 0.f;
    }
}

float dgm::Controller::getAxisValue(const Binding& binding) const noexcept
{
    if (!controllerConnected) return 0.f;
    const auto value = getNormalizedAxisValue(binding.xaxis, xstate.Gamepad);
    return (std::abs(value) < controllerDeadzone) ? 0.f : value;
}

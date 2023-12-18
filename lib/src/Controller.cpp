#include <DGM/classes/Controller.hpp>
#include <algorithm>
#include <cassert>

void dgm::Controller::update(const dgm::Time&)
{
    controllerConnected =
        XInputGetState(controllerIndex, &xstate) == ERROR_SUCCESS;
}

bool dgm::Controller::isToggled(const int code) const
{
    // Extra checks on mouse and keyboard are needed because:
    // sf::Mouse::isButtonPressed(sf::Mouse::ButtonCount) returns true
    // sf::Keyboard::isKeyPressed(sf::Keyboard::Unknown) return true
    const bool mousePressed =
        (bindings.at(code).btn != sf::Mouse::ButtonCount)
        && sf::Mouse::isButtonPressed(bindings.at(code).btn);
    const bool keyPressed =
        (bindings.at(code).key != sf::Keyboard::Unknown)
        && sf::Keyboard::isKeyPressed(bindings.at(code).key);
    const bool gamepadButtonPressed =
        controllerConnected
        && (xstate.Gamepad.wButtons
            & static_cast<WORD>(bindings.at(code).xbtn));
    const bool pressed = mousePressed || keyPressed || gamepadButtonPressed;
    return pressed ? not bindings.at(code).released
                   : (bindings.at(code).released =
                          false); // assignment is on purpose
}

float dgm::Controller::getValue(const int code) const
{
    constexpr int HALF_SHORT_MAX = 32767;

    if (isToggled(code)) return bindings[code].negateMultiplier;
    if (!controllerConnected) return 0.f;

    // Normalize everything to intervals <-1, 1>
    // see docs for more details:
    // https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad
    switch (bindings.at(code).xaxis)
    {
        // case dgm::Xbox::Axis::LStickX:    return
        // std::clamp(int(xstate.Gamepad.sThumbLX), -HALF_SHORT_MAX,
        // HALF_SHORT_MAX) / float(HALF_SHORT_MAX);
    case dgm::Xbox::Axis::LStickXpos:
        return std::clamp(int(xstate.Gamepad.sThumbLX), 0, HALF_SHORT_MAX)
               / float(HALF_SHORT_MAX);
    case dgm::Xbox::Axis::LStickXneg:
        return std::clamp(int(xstate.Gamepad.sThumbLX), -HALF_SHORT_MAX, 0)
               / float(HALF_SHORT_MAX);
        // case dgm::Xbox::Axis::LStickY:    return
        // std::clamp(int(xstate.Gamepad.sThumbLY), -HALF_SHORT_MAX,
        // HALF_SHORT_MAX) / float(HALF_SHORT_MAX);
    case dgm::Xbox::Axis::LStickYpos:
        return -std::clamp(int(xstate.Gamepad.sThumbLY), 0, HALF_SHORT_MAX)
               / float(HALF_SHORT_MAX);
    case dgm::Xbox::Axis::LStickYneg:
        return -std::clamp(int(xstate.Gamepad.sThumbLY), -HALF_SHORT_MAX, 0)
               / float(HALF_SHORT_MAX);
        // case dgm::Xbox::Axis::RStickX:    return
        // std::clamp(int(xstate.Gamepad.sThumbRX), -HALF_SHORT_MAX,
        // HALF_SHORT_MAX) / float(HALF_SHORT_MAX);
    case dgm::Xbox::Axis::RStickXpos:
        return std::clamp(int(xstate.Gamepad.sThumbRX), 0, HALF_SHORT_MAX)
               / float(HALF_SHORT_MAX);
    case dgm::Xbox::Axis::RStickXneg:
        return std::clamp(int(xstate.Gamepad.sThumbRX), -HALF_SHORT_MAX, 0)
               / float(HALF_SHORT_MAX);
        // case dgm::Xbox::Axis::RStickY:    return
        // std::clamp(int(xstate.Gamepad.sThumbRY), -HALF_SHORT_MAX,
        // HALF_SHORT_MAX) / float(HALF_SHORT_MAX);
    case dgm::Xbox::Axis::RStickYpos:
        return -std::clamp(int(xstate.Gamepad.sThumbRY), 0, HALF_SHORT_MAX)
               / float(HALF_SHORT_MAX);
    case dgm::Xbox::Axis::RStickYneg:
        return -std::clamp(int(xstate.Gamepad.sThumbRY), -HALF_SHORT_MAX, 0)
               / float(HALF_SHORT_MAX);
    case dgm::Xbox::Axis::LTrigger:
        return xstate.Gamepad.bLeftTrigger / 255.f;
    case dgm::Xbox::Axis::RTrigger:
        return xstate.Gamepad.bRightTrigger / 255.f;
    default:
        return 0.f;
    }
}

void dgm::Controller::setControllerVibration(
    const float left, const float right)
{
    assert(
        0.f <= left && left <= 1.f
        && "Parameter left must be in <0.f, 1.f> range");
    assert(
        0.f <= right && right <= 1.f
        && "Parameter right must be in <0.f, 1.f> range");

    XINPUT_VIBRATION vibration;
    vibration.wLeftMotorSpeed = static_cast<WORD>(left * 65535);
    vibration.wRightMotorSpeed = static_cast<WORD>(right * 65535);
    XInputSetState(controllerIndex, &vibration);
}

void dgm::Controller::bindInput(const int code, sf::Keyboard::Key key)
{
    assert(
        bindings[code].key == sf::Keyboard::Unknown
        && "Trying to rebind keyboard key");
    bindings[code].key = key;
}

void dgm::Controller::bindInput(const int code, sf::Mouse::Button btn)
{
    assert(
        bindings[code].btn == sf::Mouse::Button::ButtonCount
        && "Trying to rebind mouse button");
    bindings[code].btn = btn;
}

void dgm::Controller::bindInput(const int code, dgm::Xbox::Button btn)
{
    assert(
        bindings[code].xbtn == Xbox::Button::Unknown
        && "Trying to rebind Xbox button");
    bindings[code].xbtn = btn;
}

void dgm::Controller::bindInput(const int code, dgm::Xbox::Axis axis)
{
    assert(
        bindings[code].xaxis == Xbox::Axis::Unknown
        && "Trying to rebind Xbox axis");
    bindings[code].xaxis = axis;
    // If enum code is for negative half of axis
    if (static_cast<std::size_t>(axis) >= 200)
        bindings[code].negateMultiplier = -1.f;
}

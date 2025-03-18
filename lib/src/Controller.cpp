#include <DGM/classes/Controller.hpp>
#include <DGM/classes/Error.hpp>

dgm::NativeGamepadInput dgm::translateGamepadCode(
    GamepadCode code, const sf::Joystick::Identification& identity)
{
    if (identity.vendorId == 0x045E) // Microsoft
    {
        if (identity.productId == 0x02FF) // Controller (Xbox One For Windows)
        {
            switch (code)
            {
                using enum GamepadCode;
            case A:
                return 0u;
            case B:
                return 1u;
            case X:
                return 2u;
            case Y:
                return 3u;
            case Start:
                return 7u;
            case Select:
                return 6u;
            case Capture:
                return 12u;
            case LBumper:
                return 4u;
            case RBumper:
                return 5u;
            case LTrigger:
                return std::pair { sf::Joystick::Axis::Z, AxisHalf::Positive };
            case RTrigger:
                return std::pair { sf::Joystick::Axis::Z, AxisHalf::Negative };
            case DPadLeft:
                return std::pair { sf::Joystick::Axis::PovX,
                                   AxisHalf::Negative };
            case DPadRight:
                return std::pair { sf::Joystick::Axis::PovX,
                                   AxisHalf::Positive };
            case DPadUp:
                return std::pair { sf::Joystick::Axis::PovY,
                                   AxisHalf::Positive };
            case DPadDown:
                return std::pair { sf::Joystick::Axis::PovY,
                                   AxisHalf::Negative };
            case LStickLeft:
                return std::pair { sf::Joystick::Axis::X, AxisHalf::Negative };
            case LStickRight:
                return std::pair { sf::Joystick::Axis::X, AxisHalf::Positive };
            case LStickUp:
                return std::pair { sf::Joystick::Axis::Y, AxisHalf::Negative };
            case LStickDown:
                return std::pair { sf::Joystick::Axis::Y, AxisHalf::Positive };
            case LStickPress:
                return 8u;
            case RStickLeft:
                return std::pair { sf::Joystick::Axis::U, AxisHalf::Negative };
            case RStickRight:
                return std::pair { sf::Joystick::Axis::U, AxisHalf::Positive };
            case RStickUp:
                return std::pair { sf::Joystick::Axis::V, AxisHalf::Negative };
            case RStickDown:
                return std::pair { sf::Joystick::Axis::V, AxisHalf::Positive };
            case RStickPress:
                return 14u;
            }
        }
    }
    else if (identity.vendorId == 0x358A) // Backbone
    {
        if (identity.productId == 0x0)
        {
            switch (code)
            {
                using enum GamepadCode;
            case A:
                return 4u;
            case B:
                return 5u;
            case X:
                return 7u;
            case Y:
                return 8u;
            case Start:
                return 15u;
            case Select:
                return 14u;
            case Capture:
                return 0u; // not a real button, unused
            case LBumper:
                return 10u;
            case RBumper:
                return 11u;
            case LTrigger:
                return 12u;
            case RTrigger:
                return 13u;
            case DPadLeft:
                return std::pair { sf::Joystick::Axis::PovX,
                                   AxisHalf::Negative };
            case DPadRight:
                return std::pair { sf::Joystick::Axis::PovX,
                                   AxisHalf::Positive };
            case DPadUp:
                return std::pair { sf::Joystick::Axis::PovY,
                                   AxisHalf::Positive };
            case DPadDown:
                return std::pair { sf::Joystick::Axis::PovY,
                                   AxisHalf::Negative };
            case LStickLeft:
                return std::pair { sf::Joystick::Axis::X, AxisHalf::Negative };
            case LStickRight:
                return std::pair { sf::Joystick::Axis::X, AxisHalf::Positive };
            case LStickUp:
                return std::pair { sf::Joystick::Axis::Y, AxisHalf::Negative };
            case LStickDown:
                return std::pair { sf::Joystick::Axis::Y, AxisHalf::Positive };
            case LStickPress:
                return 17u;
            case RStickLeft:
                return std::pair { sf::Joystick::Axis::Z, AxisHalf::Negative };
            case RStickRight:
                return std::pair { sf::Joystick::Axis::Z, AxisHalf::Positive };
            case RStickUp:
                return std::pair { sf::Joystick::Axis::R, AxisHalf::Negative };
            case RStickDown:
                return std::pair { sf::Joystick::Axis::R, AxisHalf::Positive };
            case RStickPress:
                return 18u;
            }
        }
    }
    else if (identity.vendorId == 0x0079) // DragonRise Inc. / Canyon?
    {
        if (identity.productId == 0x0304) // Canyon controller GPW3
        {
            switch (code)
            {
                using enum GamepadCode;
            case A:
                return 0u;
            case B:
                return 1u;
            case X:
                return 3u;
            case Y:
                return 4u;
            case Start:
                return 11u;
            case Select:
                return 10u;
            case Capture:
                return 12u;
            case LBumper:
                return 6u;
            case RBumper:
                return 7u;
            case LTrigger:
                return 8u;
            case RTrigger:
                return 9u;
            case DPadLeft:
                return std::pair { sf::Joystick::Axis::PovX,
                                   AxisHalf::Negative };
            case DPadRight:
                return std::pair { sf::Joystick::Axis::PovX,
                                   AxisHalf::Positive };
            case DPadUp:
                return std::pair { sf::Joystick::Axis::PovY,
                                   AxisHalf::Positive };
            case DPadDown:
                return std::pair { sf::Joystick::Axis::PovY,
                                   AxisHalf::Negative };
            case LStickLeft:
                return std::pair { sf::Joystick::Axis::X, AxisHalf::Negative };
            case LStickRight:
                return std::pair { sf::Joystick::Axis::X, AxisHalf::Positive };
            case LStickUp:
                return std::pair { sf::Joystick::Axis::Y, AxisHalf::Negative };
            case LStickDown:
                return std::pair { sf::Joystick::Axis::Y, AxisHalf::Positive };
            case LStickPress:
                return 13u;
            case RStickLeft:
                return std::pair { sf::Joystick::Axis::Z, AxisHalf::Negative };
            case RStickRight:
                return std::pair { sf::Joystick::Axis::Z, AxisHalf::Positive };
            case RStickUp:
                return std::pair { sf::Joystick::Axis::R, AxisHalf::Negative };
            case RStickDown:
                return std::pair { sf::Joystick::Axis::R, AxisHalf::Positive };
            case RStickPress:
                return 14u;
            }
        }
    }

    throw dgm::Exception("Unrecognized gamepad: " + identity.name);
}

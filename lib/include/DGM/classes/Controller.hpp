#pragma once

#include <DGM/classes/Compatibility.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cassert>
#include <map>
#include <type_traits>
#include <utility>
#include <variant>

namespace dgm
{
    enum class [[nodiscard]] GamepadCode
    {
        A,
        B,
        X,
        Y,
        Start,
        Select,
        Capture,
        LBumper,
        RBumper,
        LTrigger,
        RTrigger,
        DPadLeft,
        DPadRight,
        DPadUp,
        DPadDown,
        LStickLeft,
        LStickRight,
        LStickUp,
        LStickDown,
        LStickPress,
        RStickLeft,
        RStickRight,
        RStickUp,
        RStickDown,
        RStickPress,
    };

    enum class [[nodiscard]] AxisHalf
    {
        Negative,
        Positive,
    };

    using NativeGamepadInput =
        std::variant<size_t, std::pair<sf::Joystick::Axis, AxisHalf>>;

    NODISCARD_RESULT NativeGamepadInput translateGamepadCode(
        GamepadCode code, const sf::Joystick::Identification& identity);

    enum class [[nodiscard]] DigitalReadKind
    {
        OnPress,
        OnHold,
    };

    /**
     *  \brief Concrete implementation of controller that interfaces
     *  with keyboard, mouse and Xbox controller or controllers that
     *  use the XInput drivers.
     *
     *  Expected workflow is to call bindInput to bind keys and buttons
     *  to particular numerical code representing certain action.
     *
     *  You can then call update, keyPressed and getAxísValue with
     *  those codes to get current state for that input, disregarding
     *  of physical input device.
     *
     *  You can even bind for example keyboard key and xbox controller
     *  button to the same input code.
     */
    template<class Action>
        requires std::is_scoped_enum_v<Action>
    class [[nodiscard]] Controller final
    {
    public:
        NODISCARD_RESULT bool readDigital(Action code) const
        {
            assert(bindings.contains(code));
            auto& binding = bindings.at(code);

            const bool mousePressed = isMouseInputToggled(binding);
            const bool keyPressed = isKeyboardInputToggled(binding);
            const bool gamepadButtonPressed = isGamepadInputToggled(binding);
            const bool axisToggled = std::abs(getAxisValue(binding)) > 0.5f;
            const bool pressed = mousePressed || keyPressed
                                 || gamepadButtonPressed || axisToggled;

            if (pressed && !binding.released)
            {
                if (binding.readKind == DigitalReadKind::OnPress
                    && !binding.released)
                {
                    binding.released = true;
                }

                return true;
            }

            return false;
        }

        NODISCARD_RESULT float readAnalog(Action code) const
        {
            assert(bindings.contains(code));
            auto& binding = bindings.at(code);
            if (isMouseInputToggled(binding) || isKeyboardInputToggled(binding)
                || isGamepadInputToggled(binding))
                return bindings[code].axisHalf == AxisHalf::Positive ? 1.f
                                                                     : -1.f;

            if (!sf::Joystick::isConnected(controllerIndex)) return 0.f;

            return getAxisValue(binding);
        }

        /**
         * \brief Marks input as released
         *
         * \details Only works in conjuction with isInputToggled().
         * Once an action is marked as released then
         * isInputToggled() will return FALSE until user had released
         * the input physically and then pressed it again.
         * With this, one can emulate sf::Event::keyPressed behaviour.
         *
         * \note In order to function properly, isInputToggled should be called
         * every frame (to ensure that controller will notice the released key)
         */
        void forceRelease(Action code)
        {
            bindings[code].released = true;
        }

        /**
         *  \brief Bind keyboard key to numerical action code
         */
        inline void bindInput(
            const Action code,
            const sf::Keyboard::Key key,
            DigitalReadKind readKind = DigitalReadKind::OnHold)
        {
            bindings[code].key = key;
            bindings[code].readKind = readKind;
        }

        /**
         *  \brief Bind mouse button to numerical action code
         */
        inline void bindInput(
            const Action code,
            const sf::Mouse::Button btn,
            DigitalReadKind readKind = DigitalReadKind::OnHold)
        {
            bindings[code].mouseButton = btn;
            bindings[code].readKind = readKind;
        }

        /**
         *  \brief Bind xbox controller button to numerical action code
         */
        inline void bindInput(
            const Action code,
            const unsigned joystickButtonIdx,
            const DigitalReadKind readKind = DigitalReadKind::OnHold)
        {
            bindings[code].gamepadButton = joystickButtonIdx;
            bindings[code].readKind = readKind;
        }

        /**
         *  \brief Bind xbox controller axis to numerical action code
         */
        void bindInput(
            const Action code,
            const sf::Joystick::Axis axis,
            const AxisHalf axisHalf)
        {
            bindings[code].axis = axis;
            bindings[code].axisHalf = axisHalf;
        }

        /**
         *  \brief Set an index of a gamepad that should be used
         */
        constexpr void setGamepadIndex(const unsigned short index) noexcept
        {
            controllerIndex = index;
        }

        /**
         * \brief Set deadzone for controller axii
         *
         * Deadzone is a number in interval <0.f, 1.f>
         */
        void setGamepadDeadzone(const float deadzone)
        {
            assert(0.f <= deadzone && deadzone <= 1.f);
            controllerDeadzone = deadzone;
        }

    protected:
        struct Binding
        {
            bool released = false;
            DigitalReadKind readKind = DigitalReadKind::OnHold;
            sf::Keyboard::Key key = sf::Keyboard::Key::Unknown;
            sf::Mouse::Button mouseButton =
                static_cast<sf::Mouse::Button>(sf::Mouse::ButtonCount);
            sf::Joystick::Axis axis =
                static_cast<sf::Joystick::Axis>(sf::Joystick::AxisCount);
            AxisHalf axisHalf = AxisHalf::Positive;
            unsigned gamepadButton = sf::Joystick::ButtonCount;
        };

    protected:
        NODISCARD_RESULT inline bool
        isMouseInputToggled(const Binding& binding) const noexcept
        {
            // Extra check is required because:
            // sf::Mouse::isButtonPressed(sf::Mouse::ButtonCount) returns true
            return (binding.mouseButton
                    != static_cast<sf::Mouse::Button>(sf::Mouse::ButtonCount))
                   && sf::Mouse::isButtonPressed(binding.mouseButton);
        }

        NODISCARD_RESULT inline bool
        isKeyboardInputToggled(const Binding& binding) const noexcept
        {
            // Extra check is required because:
            // sf::Keyboard::isKeyPressed(sf::Keyboard::Unknown) return true
            return (binding.key != sf::Keyboard::Key::Unknown)
                   && sf::Keyboard::isKeyPressed(binding.key);
        }

        NODISCARD_RESULT inline bool
        isGamepadInputToggled(const Binding& binding) const noexcept
        {
            return (
                binding.gamepadButton != sf::Joystick::ButtonCount
                && sf::Joystick::isButtonPressed(
                    controllerIndex, binding.gamepadButton));
        }

        NODISCARD_RESULT float getAxisValue(const Binding& binding) const
        {
            if (std::to_underlying(binding.axis) == sf::Joystick::AxisCount)
                return 0.f;
            return std::clamp(
                sf::Joystick::getAxisPosition(controllerIndex, binding.axis)
                    / 100.f,
                binding.axisHalf == dgm::AxisHalf::Negative ? -1.f : 0.f,
                binding.axisHalf == dgm::AxisHalf::Positive ? 1.f : 0.f);
        }

    private:
        mutable std::map<Action, Binding> bindings = {};
        unsigned short controllerIndex = 0;
        float controllerDeadzone = 0.1f;
    };
} // namespace dgm
#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <Windows.h>
#include <Xinput.h>
#include <cassert>
#include <map>

namespace dgm
{
    /**
     *  \brief Xbox controller related data structures
     */
    namespace Xbox
    {
        /**
         *  \brief Enumerator for buttons on Xbox controller
         */
        enum class Button : std::size_t
        {
            Unknown = 0x0000,
            DPadUp = 0x0001,
            DPadDown = 0x0002,
            DPadLeft = 0x0004,
            DPadRight = 0x0008,
            Start = 0x0010,
            Back = 0x0020,
            LStick = 0x0040,
            RStick = 0x0080,
            LBumper = 0x0100,
            RBumper = 0x0200,
            A = 0x1000,
            B = 0x2000,
            X = 0x4000,
            Y = 0x8000,
        };

        /**
         *  \brief Enumeration of non-binary axii on Xbox controller
         */
        enum class Axis : std::size_t
        {
            Unknown = 0,
            LTrigger = 1,
            RTrigger,
            LStickXpos = 100,
            LStickYpos,
            RStickXpos,
            RStickYpos,
            LStickXneg = 200,
            LStickYneg,
            RStickXneg,
            RStickYneg
        };
    } // namespace Xbox

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
    class [[nodiscard]] Controller final
    {
    public:
        /**
        * Modifies how the input is reported.
        * If set to ReportKeyPress, the isInputToggled will
        * only return true the first time it is called while the key is pressed.
        * The user needs to physically release the key and re-press it to
        * report the input second time.
        *
        * In previous versions of the library, the same behavior had to be
        * achieved by:
        *
        * \code
        if (input.isInputToggled(code)) {
            input.releaseKey(code);
            // ...
        }
        * \endcode
        *
        * ReportKeyHold is the default behavior and reports the key press
        * for as long as the key is physically pressed (unless releaseKey was
        called).
        * This is how the library behaved up until now.
        */
        enum class [[nodiscard]] InputBehavior
        {
            ReportKeyPress,
            ReportKeyHold,
        };

    public:
        /**
         *  \brief Update state on xbox controller
         *
         *  You only need to call this method if you intend on using
         *  the controller. You don't need to call this if you only
         *  use mouse and keyboard.
         */
        void update();

        /**
         * \brief Test whether particular input code is pressed
         *
         * If input code was previously released using releaseInput,
         * then the physical input has to be physically released and
         * pressed again for this function to return true again.
         *
         * This can be useful for example when you want to implement
         * a revolver gun and you want the player to press the fire
         * button separately for every single bullet.
         */
        [[nodiscard]] bool isInputToggled(const int code) const;

        /**
         * \brief Get analog value for associated input
         *
         * Remarks:
         *
         * Both trigger Axii return value from 0.f to 1.f.
         *
         * Keyboard key, Xbox gamepad buttons and mouse keys always return -1.f,
         * 0.f or 1.f based on whether positive or negative Axis was associated
         * with the same input.
         *
         * Positive Axii return 0.f to 1.f negative Axii return -1.f to 0.f
         *
         * If controller deadzone was configured and queried Axis
         * holds a abs(value) < deadzone, then 0.f is returned by this method.
         *
         * \releaseInput does not affect this method.
         */
        [[nodiscard]] float getInputValue(const int code) const;

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
        void releaseInput(const int code) noexcept
        {
            bindings[code].released = true;
        }

        /**
         *  \brief Test if controller is connected
         *
         *  \pre setControllerIndex was called, otherwise index 0 is used
         *  \pre update was called, this method returns value valid since last
         * update call
         */
        [[nodiscard]] constexpr bool isControllerConnected() const noexcept
        {
            return controllerConnected;
        }

        /**
         *  \brief Bind keyboard key to numerical action code
         */
        inline void bindInput(
            const int code,
            sf::Keyboard::Key key,
            InputBehavior behavior = InputBehavior::ReportKeyHold)
        {
            bindings[code].key = key;
            bindings[code].releaseOnRead =
                behavior == InputBehavior::ReportKeyPress;
        }

        /**
         *  \brief Bind mouse button to numerical action code
         */
        inline void bindInput(
            const int code,
            sf::Mouse::Button btn,
            InputBehavior behavior = InputBehavior::ReportKeyHold)
        {
            bindings[code].btn = btn;
            bindings[code].releaseOnRead =
                behavior == InputBehavior::ReportKeyPress;
        }

        /**
         *  \brief Bind xbox controller button to numerical action code
         */
        inline void bindInput(
            const int code,
            dgm::Xbox::Button btn,
            InputBehavior behavior = InputBehavior::ReportKeyHold)
        {
            bindings[code].xbtn = btn;
            bindings[code].releaseOnRead =
                behavior == InputBehavior::ReportKeyPress;
        }

        /**
         *  \brief Bind xbox controller axis to numerical action code
         */
        void bindInput(const int code, dgm::Xbox::Axis axis);

        /**
         *  \brief Set an index of a gamepad that should be used
         */
        constexpr void setGamepadIndex(const unsigned short index) noexcept
        {
            controllerIndex = index;
        }

        /**
         * \brief Set a vibration force on a connected gamepad
         *
         * std::numerical_limits<std::uint16_t>::max() means
         * maximum vibration force.
         */
        void vibrate(
            const std::uint16_t leftMotorForce,
            const std::uint16_t rightMotorForce) noexcept;

        /**
         * \brief Resets vibrations on a connected gamepad
         */
        inline void stopVibrating() noexcept(noexcept(vibrate(0, 0)))
        {
            vibrate(0, 0);
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
            bool releaseOnRead = false;
            float negateMultiplier = 1.f;
            sf::Keyboard::Key key = sf::Keyboard::Key::Unknown;
            sf::Mouse::Button btn = sf::Mouse::Button::ButtonCount;
            dgm::Xbox::Button xbtn = dgm::Xbox::Button::Unknown;
            dgm::Xbox::Axis xaxis = dgm::Xbox::Axis::Unknown;
        };

    protected:
        [[nodiscard]] inline bool
        isMouseInputToggled(const Binding& binding) const noexcept
        {
            // Extra check is required because:
            // sf::Mouse::isButtonPressed(sf::Mouse::ButtonCount) returns true
            return (binding.btn != sf::Mouse::ButtonCount)
                   && sf::Mouse::isButtonPressed(binding.btn);
        }

        [[nodiscard]] inline bool
        isKeyboardInputToggled(const Binding& binding) const noexcept
        {
            // Extra check is required because:
            // sf::Keyboard::isKeyPressed(sf::Keyboard::Unknown) return true
            return (binding.key != sf::Keyboard::Unknown)
                   && sf::Keyboard::isKeyPressed(binding.key);
        }

        [[nodiscard]] inline bool
        isGamepadInputToggled(const Binding& binding) const noexcept
        {
            return controllerConnected
                   && (xstate.Gamepad.wButtons
                       & static_cast<WORD>(binding.xbtn));
        }

        [[nodiscard]] float getAxisValue(const Binding& binding) const noexcept;

    protected:
        mutable std::map<int, Binding> bindings = {};
        XINPUT_STATE xstate = {};
        unsigned short controllerIndex = 0;
        bool controllerConnected = false;
        float controllerDeadzone = 0.1f;
    };
} // namespace dgm
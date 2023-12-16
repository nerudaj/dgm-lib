#pragma once

#include <DGM/classes/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <Windows.h>
#include <Xinput.h>
#include <map>

namespace dgm
{
    /**
     *  \warn This interface is deprecated!
     *
     *  \brief Abstraction over physical input devices
     *
     *  Inputs are abstracted through action codes. Concrete implementation
     *  should provide a way to bind physical inputs of chosen devices to
     *  abstract action codes.
     */
    class /*[[deprecated]]*/ AbstractController
    {
    public:
        /**
         *  \brief Returns true if input associated with code is toggled
         */
        [[nodiscard]] virtual bool isToggled(const int code) const = 0;

        /**
         *  \brief Marks input as released
         *
         *  Function isToggled will not return TRUE until underlying physical
         *  input is released.
         */
        virtual void releaseKey(const int code) = 0;

        /**
         *  \brief Returns analog value for given action code
         */
        [[nodiscard]] virtual float getValue(const int code) const = 0;

        virtual ~AbstractController() = default;
    };

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
    class Controller : public AbstractController
    {
    protected:
        struct Binding
        {
            bool released = false;
            float negateMultiplier = 1.f;
            sf::Keyboard::Key key = sf::Keyboard::Key::Unknown;
            sf::Mouse::Button btn = sf::Mouse::Button::ButtonCount;
            dgm::Xbox::Button xbtn = dgm::Xbox::Button::Unknown;
            dgm::Xbox::Axis xaxis = dgm::Xbox::Axis::Unknown;
        };

        mutable std::map<int, Binding> bindings = {};
        XINPUT_STATE xstate = {};
        unsigned short controllerIndex = 0;
        bool controllerConnected = false;

    public:
        /**
         *  \brief Update state on xbox controller
         *
         *  You only need to call this method if you intend on using
         *  the controller. You don't need to call this if you only
         *  use mouse and keyboard.
         */
        [[deprecated]] void update(const ::dgm::Time& time);

        /**
         *  \brief Update state on xbox controller
         *
         *  You only need to call this method if you intend on using
         *  the controller. You don't need to call this if you only
         *  use mouse and keyboard.
         */
        void update();

        /**
         *  \brief Test if controller is connected
         *
         *  \pre setControllerIndex was called, otherwise index 0 is used
         *  \pre update was called, this method returns value valid since last
         * update call
         */
        [[nodiscard]] bool isControllerConnected() const noexcept
        {
            return controllerConnected;
        }

        /**
         * \brief Test whether particular input code is pressed
         *
         * If input code was previously released using releaseKey then
         * physical key/button must first be released, this function called
         * and only then will this function return TRUE again.
         *
         * This function does NOT work for dgm::Xbox::Axis bindings (use
         * getAxisValue for those).
         */
        [[nodiscard]] bool isToggled(const int code) const;

        /**
         * \brief Marks input as released
         *
         * \details Only works in conjuction with keyPressed().
         * Once an action is marked as released then
         * keyPressed() will return FALSE until point where user
         * had released the input physically and then pressed it again.
         * With this, one can emulate sf::Event::keyPressed behaviour.
         *
         * \note In order to function properly, keyPressed should be called
         * every frame (to ensure that controller will notice the released key)
         */
        void releaseKey(const int code) noexcept
        {
            bindings[code].released = true;
        }

        /**
         *  \brief Get analog value of bound action code
         *
         *  If stick X/Y axis is bound to action, value from -1.f to 1.f will be
         * returned based on the position of the stick.
         *
         *  Both triggers return value from 0.f to 1.f. If button or keyboard
         * key is bound to this action code as well, this function will
         * return 1.f if such input is pressed.
         *
         *  If stick X/Y pos axis is bound to action, value from 0.f to 1.f will
         * be returned based on the position of the stick only on positive half
         * of range. If button or keyboard key is bound to this action code as
         * well, this function will return 1.f if such input is pressed.
         *
         *  Similar applies to negative half of the stick range - function
         * returns value from -1.f to 0.f and -1.f for buttons.
         */
        [[nodiscard]] float getValue(const int code) const override;

        /**
         *  \brief Bind keyboard key to numerical action code
         */
        void bindInput(const int code, sf::Keyboard::Key key);

        /**
         *  \brief Bind mouse button to numerical action code
         */
        void bindInput(const int code, sf::Mouse::Button btn);

        /**
         *  \brief Bind xbox controller button to numerical action code
         */
        void bindInput(const int code, dgm::Xbox::Button btn);

        /**
         *  \brief Bind xbox controller axis to numerical action code
         */
        void bindInput(const int code, dgm::Xbox::Axis axis);

        /**
         *  \brief Set index of required controller
         */
        void setControllerIndex(const unsigned short index) noexcept
        {
            controllerIndex = index;
        }

        /**
         *  \brief Set vibration force on controller
         */
        void setControllerVibration(
            const float leftMotorForce, const float rightMotorForce);

        [[nodiscard]] Controller() noexcept = default;
    };
} // namespace dgm
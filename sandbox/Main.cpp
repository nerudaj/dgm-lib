#include <DGM/dgm.hpp>

enum Action
{
	Up, Down, Left, Right,
	LeftStickVerticalPos, LeftStickVerticalNeg, LeftStickHorizontalPos, LeftStickHorizontalNeg,
	RightStickVerticalPos, RightStickVerticalNeg, RightStickHorizontalPos, RightStickHorizontalNeg,
	A, B, X, Y,
	LeftBumper, RightBumper,
	LeftTrigger, RightTrigger,
	Back, Start
};

int main(int, char* [])
{
	dgm::Window window({ 1920, 1080 }, "Sandbox", false);
	sf::Event event;
	dgm::Time time;

	dgm::Controller input;
	input.bindInput(Action::Up, sf::Keyboard::Up);
	input.bindInput(Action::Down, sf::Keyboard::Down);
	input.bindInput(Action::Left, sf::Keyboard::Left);
	input.bindInput(Action::Right, sf::Keyboard::Right);
	input.bindInput(Action::Up, dgm::Xbox::Button::DPadUp);
	input.bindInput(Action::Down, dgm::Xbox::Button::DPadDown);
	input.bindInput(Action::Left, dgm::Xbox::Button::DPadLeft);
	input.bindInput(Action::Right, dgm::Xbox::Button::DPadRight);
	input.bindInput(Action::LeftStickVerticalPos, dgm::Xbox::Axis::LStickYpos);
	input.bindInput(Action::LeftStickVerticalNeg, dgm::Xbox::Axis::LStickYneg);
	input.bindInput(Action::LeftStickHorizontalPos, dgm::Xbox::Axis::LStickXpos);
	input.bindInput(Action::LeftStickHorizontalNeg, dgm::Xbox::Axis::LStickXneg);
	input.bindInput(Action::RightStickVerticalPos, dgm::Xbox::Axis::RStickYpos);
	input.bindInput(Action::RightStickVerticalNeg, dgm::Xbox::Axis::RStickYneg);
	input.bindInput(Action::RightStickHorizontalPos, dgm::Xbox::Axis::RStickXpos);
	input.bindInput(Action::RightStickHorizontalNeg, dgm::Xbox::Axis::RStickXneg);
	input.bindInput(Action::A, sf::Keyboard::A);
	input.bindInput(Action::B, sf::Keyboard::B);
	input.bindInput(Action::X, sf::Keyboard::X);
	input.bindInput(Action::Y, sf::Keyboard::Y);
	input.bindInput(Action::A, dgm::Xbox::Button::A);
	input.bindInput(Action::B, dgm::Xbox::Button::B);
	input.bindInput(Action::X, dgm::Xbox::Button::X);
	input.bindInput(Action::Y, dgm::Xbox::Button::Y);
	input.bindInput(Action::LeftBumper, sf::Keyboard::Q);
	input.bindInput(Action::RightBumper, sf::Keyboard::W);
	input.bindInput(Action::LeftTrigger, sf::Keyboard::E);
	input.bindInput(Action::RightTrigger, sf::Keyboard::R);
	input.bindInput(Action::LeftBumper, dgm::Xbox::Button::LBumper);
	input.bindInput(Action::RightBumper, dgm::Xbox::Button::RBumper);
	input.bindInput(Action::LeftTrigger, dgm::Xbox::Axis::LTrigger);
	input.bindInput(Action::RightTrigger, dgm::Xbox::Axis::RTrigger);
	input.bindInput(Action::Back, sf::Keyboard::N);
	input.bindInput(Action::Start, sf::Keyboard::M);
	input.bindInput(Action::Back, dgm::Xbox::Button::Back);
	input.bindInput(Action::Start, dgm::Xbox::Button::Start);

	sf::RectangleShape lTrigger, rTrigger, dpadUp, dpadLeft, dpadDown, dpadRight;
	sf::RectangleShape a, b, x, y, back, start, lBumper, rBumper;
	sf::CircleShape leftOutline, rightOutline, leftDot, rightDot;

	auto getDirection = [&input](const int hNeg, const int hPos, const int vNeg, const int vPos)->sf::Vector2f
	{
		return sf::Vector2f(
			input.getValue(hNeg) + input.getValue(hPos),
			-input.getValue(vNeg) - input.getValue(vPos)
		) * 100.f;
	};

	lTrigger.setPosition(0.f, 0.f);
	rTrigger.setPosition(110.f, 0.f);
	lBumper.setPosition(0.f, 20.f);
	lBumper.setSize({ 20.f, 20.f });
	rBumper.setPosition(90.f, 20.f);
	rBumper.setSize({ 20.f, 20.f });

	dpadUp.setSize({ 10.f, 20.f });
	dpadUp.setOrigin({ 5.f, 20.f });
	dpadUp.setPosition({ 100.f, 100.f });
	dpadDown.setSize({ 10.f, 20.f });
	dpadDown.setOrigin({ 5.f, 0.f });
	dpadDown.setPosition({ 100.f, 100.f });
	dpadLeft.setSize({ 20.f, 10.f });
	dpadLeft.setOrigin({ 20.f, 5.f });
	dpadLeft.setPosition({ 100.f, 100.f });
	dpadRight.setSize({ 20.f, 10.f });
	dpadRight.setOrigin({ 0.f, 5.f });
	dpadRight.setPosition({ 100.f, 100.f });

	y.setSize({ 10.f, 20.f });
	y.setOrigin({ 5.f, 20.f });
	y.setPosition({ 200.f, 100.f });
	a.setSize({ 10.f, 20.f });
	a.setOrigin({ 5.f, 0.f });
	a.setPosition({ 200.f, 100.f });
	x.setSize({ 20.f, 10.f });
	x.setOrigin({ 20.f, 5.f });
	x.setPosition({ 200.f, 100.f });
	b.setSize({ 20.f, 10.f });
	b.setOrigin({ 0.f, 5.f });
	b.setPosition({ 200.f, 100.f });

	back.setSize({ 30.f, 10.f });
	back.setPosition(100.f, 300.f);
	start.setSize({ 30.f, 10.f });
	start.setPosition(200.f, 300.f);

	leftOutline.setOutlineColor(sf::Color::White);
	leftOutline.setOutlineThickness(2.f);
	leftOutline.setRadius(50.f);
	leftOutline.setPosition({ 100.f, 400.f });
	leftOutline.setFillColor(sf::Color::Transparent);
	rightOutline.setOutlineColor(sf::Color::White);
	rightOutline.setOutlineThickness(2.f);
	rightOutline.setRadius(50.f);
	rightOutline.setPosition({ 200.f, 400.f });
	rightOutline.setFillColor(sf::Color::Transparent);

	leftDot.setRadius(3.f);
	leftDot.setFillColor(sf::Color::White);
	rightDot.setRadius(3.f);
	rightDot.setFillColor(sf::Color::White);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		}

		// Logic
		time.reset();

		input.update(time);

		lTrigger.setSize({ input.getValue(Action::LeftTrigger) * 100.f, 10.f });
		rTrigger.setSize({ input.getValue(Action::RightTrigger) * 100.f, 10.f });

		lBumper.setFillColor(input.isToggled(Action::LeftBumper) ? sf::Color::Green : sf::Color::Red);
		rBumper.setFillColor(input.isToggled(Action::RightBumper) ? sf::Color::Green : sf::Color::Red);

		dpadUp.setFillColor(input.isToggled(Action::Up) ? sf::Color::Green : sf::Color::Red);
		dpadDown.setFillColor(input.isToggled(Action::Down) ? sf::Color::Green : sf::Color::Red);
		dpadLeft.setFillColor(input.isToggled(Action::Left) ? sf::Color::Green : sf::Color::Red);
		dpadRight.setFillColor(input.isToggled(Action::Right) ? sf::Color::Green : sf::Color::Red);

		a.setFillColor(input.isToggled(Action::A) ? sf::Color::Green : sf::Color::Red);
		b.setFillColor(input.isToggled(Action::B) ? sf::Color::Green : sf::Color::Red);
		x.setFillColor(input.isToggled(Action::X) ? sf::Color::Green : sf::Color::Red);
		y.setFillColor(input.isToggled(Action::Y) ? sf::Color::Green : sf::Color::Red);

		back.setFillColor(input.isToggled(Action::Back) ? sf::Color::Green : sf::Color::Red);
		start.setFillColor(input.isToggled(Action::Start) ? sf::Color::Green : sf::Color::Red);

		leftDot.setPosition(sf::Vector2f(150.f, 450.f) + getDirection(Action::LeftStickHorizontalNeg, Action::LeftStickHorizontalPos, Action::LeftStickVerticalNeg, Action::LeftStickVerticalPos));
		rightDot.setPosition(sf::Vector2f(250.f, 450.f) + getDirection(Action::RightStickHorizontalNeg, Action::RightStickHorizontalPos, Action::RightStickVerticalNeg, Action::RightStickVerticalPos));

		input.setControllerVibration(input.getValue(Action::LeftTrigger), input.getValue(Action::RightTrigger));

		// Draw
		window.beginDraw();

		window.draw(lTrigger);
		window.draw(rTrigger);
		window.draw(lBumper);
		window.draw(rBumper);

		window.draw(dpadUp);
		window.draw(dpadDown);
		window.draw(dpadLeft);
		window.draw(dpadRight);

		window.draw(a);
		window.draw(b);
		window.draw(x);
		window.draw(y);

		window.draw(back);
		window.draw(start);

		window.draw(leftOutline);
		window.draw(rightOutline);
		window.draw(leftDot);
		window.draw(rightDot);

		window.endDraw();
	}

	return 0;
}

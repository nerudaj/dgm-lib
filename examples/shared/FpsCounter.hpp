#pragma once

class [[nodiscard]] FpsCounter final
{
public:
    void update(const dgm::Time& time)
    {
        maxFps =
            std::max(static_cast<unsigned>(1.f / time.getDeltaTime()), maxFps);
        elapsed += time.getDeltaTime();

        if (elapsed > 1.f)
        {
            text = std::to_string(maxFps);
            maxFps = 0;
            elapsed = 0.f;
        }
    }

    const std::string& getString() const noexcept
    {
        return text;
    }

private:
    float elapsed = 0.f;
    unsigned maxFps = 0;
    std::string text = "0";
};

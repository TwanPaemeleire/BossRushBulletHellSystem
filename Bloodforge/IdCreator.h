#pragma once
template <int length> struct CustomId
{
    consteval static unsigned int _calculate(const char* const text, unsigned int& value)
    {
        const unsigned int character = CustomId<length - 1>::_calculate(text, value);
        value = character + (value << 6) + (value << 16) - value;
        return text[length - 1];
    }
    consteval static unsigned int calculate(const char* const text)
    {
        unsigned int value = 0;
        const auto character = _calculate(text, value);
        return character + (value << 6) + (value << 16) - value;
    }
};
template <> struct CustomId<1>
{
    consteval static int _calculate(const char* const text, unsigned int&) { return text[0]; }
};
template <size_t N> consteval unsigned int CreateId(const char(&text)[N])
{
    return CustomId<N - 1>::calculate(text);
}
#pragma once
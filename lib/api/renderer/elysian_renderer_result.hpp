#ifndef ELYSIAN_RENDERER_RESULT_HPP
#define ELYSIAN_RENDERER_RESULT_HPP

#include <vulkan/vulkan.h>
#include <cstring>
#include <string>
#include <unordered_map>

namespace elysian::renderer {

    class Version {
    public:
        constexpr Version(uint16_t major, uint16_t minor, uint16_t patch):
            Version(VK_MAKE_VERSION(major, minor, patch)) {}

        constexpr Version(uint32_t version): m_value(version) {}

        operator uint32_t() const { return getValue(); }

        uint32_t getValue(void) const { return m_value; }
        uint16_t getMajor(void) const { return VK_VERSION_MAJOR(m_value); }
        uint16_t getMinor(void) const { return VK_VERSION_MINOR(m_value); }
        uint16_t getPatch(void) const { return VK_VERSION_PATCH(m_value); }

        std::string toString(void) const;

    private:
        uint32_t m_value = 0;
    };

    class Result {
    public:
        Result(VkResult result=VK_ERROR_UNKNOWN);

        operator bool() const;
        bool succeeded(void) const;
        VkResult getCode(void) const;
        const char* toString(void) const;

    private:
        VkResult m_result;
    };

    inline Result::Result(VkResult result):
        m_result(result)
    {}

    inline Result::operator bool() const { return succeeded(); }
    inline bool Result::succeeded(void) const { return getCode() == VK_SUCCESS; }
    inline VkResult Result::getCode(void) const { return m_result; }
    inline const char* Result::toString(void) const { return "TEMP"; }

    inline std::string Version::toString(void) const {
        char buff[64];
        snprintf(buff, sizeof(buff), "%u.%u.%u", getMajor(), getMinor(), getPatch());
        return buff;
    }


    /// Utility class used for stringifying bitfields into printable, user-friendly strings for
    /// logging and UI.
    ///
    /// Usage:
    /// BitFieldStringifier stringifier("BIT1", "BIT2", "BIT3");
    /// uint32_t bitMask = 0x3;
    /// QString string = stringifier.stringifyMask(bitMask);
    ///
    /// string == "BIT1|BIT2";
    ///
    template<typename T>
    class BitFieldStringifier
    {
        std::unordered_map<T, std::string> m_fieldNames;

    public:
        // Regular constructor, assumes all bit field strings are consecutive
        BitFieldStringifier(std::initializer_list<std::string> consecutiveFlags);
        // Specific constructor, can handle sparse bitfields
        BitFieldStringifier(std::initializer_list<std::pair<T, std::string>> sparseFlags);

        // Returns the string name corresponding to the given bit position
        std::string bitFieldString(T bitPos) const;
        // Adds a string name for the given bit position
        void addBitFieldString(T bitPos, std::string field);

        // Returns the stringified version of the given bitmask
        std::string stringifyMask(T mask) const;
    };

    template<typename T>
    BitFieldStringifier<T>::BitFieldStringifier(std::initializer_list<std::string> consecutiveFlags)
    {
        T count = 0;
        for (auto&& str : consecutiveFlags)
        {
            m_fieldNames.insert((0x1 << count), std::move(str));
            ++count;
        }
        assert(count < sizeof(T) * 8); // verify we haven't gone too far
    }

    template<typename T>
    BitFieldStringifier<T>::BitFieldStringifier(std::initializer_list<std::pair<T, std::string>> sparseFlags)
    {
        //m_fieldNames.insert(sparseFlags);
    }

    template<typename T>
    std::string BitFieldStringifier<T>::bitFieldString(T bit) const
    {
        const auto& it = m_fieldNames.find(bit);
        //return (it != m_fieldNames.end()) ? it.value() : std::string("");
    }

    template<typename T>
    std::string BitFieldStringifier<T>::stringifyMask(T mask) const
    {
        std::string string;
        bool    first = true;
        for (T b = 0; b < sizeof(T) * 8; ++b)
        {
            if (std::string field = bitFieldString(mask & (0x1 << b)); field.length())
            {
                if (!first)
                {
                    string += "|";
                }
                else
                {
                    first = false;
                }

                string += field;
            }
        }

        return string;
    }


}

#endif // ELYSIAN_RENDERER_RESULT_HPP

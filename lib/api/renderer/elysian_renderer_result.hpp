#ifndef ELYSIAN_RENDERER_RESULT_HPP
#define ELYSIAN_RENDERER_RESULT_HPP

#include <vulkan/vulkan.h>
#include <cstring>
#include <string>
#include <unordered_map>

namespace elysian::renderer {

template<typename Struct>
class VariableLengthStruct {
public:

    VariableLengthStruct(size_t initialSize=sizeof(Struct), const void* pInitialData=nullptr) {
        setSize(initialSize);
    }

    VariableLengthStruct(const Struct& rhs):
        VariableLengthStruct(sizeof(Struct))
    {
        memcpy(m_pData, &rhs, sizeof(Struct));
    }

    ~VariableLengthStruct(void)  { setSize(0); }

    const VariableLengthStruct& operator=(const Struct& rhs) {
        setData(rhs);
    }

    operator        Struct*() { return m_pData; }
    operator        const Struct*() const { return m_pData; }
    operator        Struct() const {
        Struct value;
        memset(&value,  0, sizeof(Struct));
        if(!isNull()) memcpy(&value, getData(), getSize());
        return value;
    }

    void setData(const Struct& rhs, size_t size=sizeof(Struct)) {
        setData(static_cast<const void*>(&rhs), size);
    }

    void setData(const void* pData, size_t size) {
        assert(pData && size);
        setSize(size);
        memcpy(pData, &rhs, size);
    }

    void setSize(size_t newSize) {
        if(!m_pData) {
            m_pData = static_cast<Struct*>(malloc(newSize));
            memset(m_pData, 0, newSize);
        } else if(newSize != getSize()) {
            m_pData = realloc(m_pData, newSize);
        } else if(!size) {
            free(m_pData);
            m_pData = nullptr;
        }

        m_dataSize = newSize;
    }

    const Struct*   getData(void) const { return m_pData; }
    Struct*         getData(void) { return m_pData; }
    size_t          getSize(void) const { return m_dataSize; }
    //size_t          getCopiedSize(void) const { return m_copiedSize; }
    //void            setCopiedSize(size_t size) { m_copiedSize = size; }
    bool            isValid(void) const { return getData() && getSize() >= sizeof(Struct); }
    bool            isNull(void) const { return !getData() || !getSize(); }
    //bool            isPopulated(void) const { return isValid() && getCopiedSize(); }

private:
    Struct* m_pData       = nullptr;
    size_t  m_dataSize    = 0;
    //size_t  m_copiedSize  = 0;
};

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

    inline std::string Version::toString(void) const { // WHAT THE FUCK
        char buff[64];
        snprintf(buff, sizeof(buff), "%u.%u.%u", getMajor(), getMinor(), getPatch());
        return buff;
    }

    template<typename V>
    using ResultValue = std::pair<Result, V>;


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

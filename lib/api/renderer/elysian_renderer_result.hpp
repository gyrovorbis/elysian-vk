#ifndef ELYSIAN_RENDERER_RESULT_HPP
#define ELYSIAN_RENDERER_RESULT_HPP

namespace elysian::renderer {

    class Result {
    public:
        Result(VkResult result=VK_ERROR_UNKNOWN);

        operator bool();
        bool succeeded(void) const;
        int getCode(void) const;
        const char* toString(void) const;

    private:
        VkResult m_result;
    };

}

#endif // ELYSIAN_RENDERER_RESULT_HPP

#include <QDebug>
#include <vulkan/vulkan.h>
#include <renderer/elysian_renderer.hpp>
#include <renderer/elysian_renderer_instance.hpp>
#include <renderer/elysian_renderer_debug_log.hpp>
#include <renderer/elysian_renderer_debug_messenger.hpp>
#include <renderer/elysian_renderer_device.hpp>
#include <renderer/elysian_renderer_command.hpp>
#include <renderer/elysian_renderer_compute_pipeline.hpp>
#include <renderer/elysian_renderer_buffer.hpp>
#include <renderer/elysian_renderer_memory.hpp>
#include <iostream>

using namespace elysian::renderer;

class QtLog: public elysian::renderer::DebugLog {
public:

    virtual void push(void) override {
        ++m_depth;
    }
    virtual void pop(void) override {
        Q_ASSERT(m_depth--);
    }

    virtual void write(Source source, Severity severity, va_list args, const char* pFormat) override {
        const QString buff = QString::vasprintf(pFormat, args);

        QString msg;
        QString src;
        switch(source) {
        case Source::Renderer:              /*src = "Renderer";*/    break;
        case Source::Driver_General:        src = "[Driver] ";      break;
        case Source::Driver_Validation:     src = "[Validation] ";  break;
        case Source::Driver_Performance:    src = "[Performance] "; break;
        }

        for(int i = 0; i < m_depth; ++i) msg += "    ";
        msg += QString("%1%2").arg(src).arg(buff);

        switch(severity) {
        case Severity::Verbose:
        case Severity::Info:
            //qDebug() << msg; break;
            std::cout << msg.toStdString() << std::endl; break;
        case Severity::Warning:
        case Severity::Error:
            std::cerr << msg.toStdString() << std::endl; break;
       }
    }

private:
    int m_depth = 0;

};

int main()
{
    Renderer* pRenderer = new Renderer(new Renderer::Initializer{
                                           .pLog = new QtLog(),
                                           .pAllocator = nullptr,
                                           .pInstanceInitializer = new elysian::renderer::InstanceInitializer {
                                            .info = InstanceCreateInfo(
                                               {},
                                              {"VK_LAYER_KHRONOS_validation"},//, "VK_LAYER_RENDERDOC_Capture"}
                                                   {"VK_KHR_device_group_creation", "VK_KHR_device_group"} ),
                                               new DebugUtilsMessengerEXTCreateInfo()
                                           }
                                       });


    auto* pDevice = pRenderer->createDevice("Device1",
                            pRenderer->getPhysicalDevice(0),
                            new DeviceCreateInfo {
                            {
                                {
                                    .properties = { "Group1", 0 },
                                    .queueProperties = {
                                        { "1", 1.0f }
                                    }
                                },
                                {
                                    .properties = { "Group2", 1 },
                                    .queueProperties = {
                                        { "a", 1.0f}
                                    }
                                }
                            }
                        });


    auto* pPipeline = pDevice->createComputePipeline(ComputePipelineCreateInfo(0, //create flags
                              new ShaderStage(0, //create flags
                                          VK_SHADER_STAGE_COMPUTE_BIT,
                                          "/assets/shaders/computeTest.glsl",
                                          "main"),
                              new Layout({
                                      { "Set1", 0, 0, {
                                          { "inputBuffer", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT  },
                                          { "outputBuffer", 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT }
                                      }},
                                     { "Set2", 0, 0, {
                                         { "inputBuffer", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT  },
                                         { "outputBuffer", 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT }
                                     }}
                                  })));


    // Create input/output buffers
    auto* pInputBuffer = pDevice->createBuffer({
        0, //flags
        100, //size
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        //OPTIONAL FOR DIRECTLY CREATING + BINDING DEVICE MEMORY UPON CREATION
        DeviceMemoryAllocateInfo(100,   //  allocation size
                                 0)     //  memory type index
    });

    auto* pOutputBuffer = pDevice->createBuffer({
        0, //flags
        100, //size
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, //usage flags
        DeviceMemoryAllocateInfo(100,   //    allocation size
                                 0)     //    memory type index
    });

    // Create + write descriptor sets
    auto* pDescriptorSetGroup = pPipeline->createDescriptorSetGroup(pDescriptorPool);
    pDescriptorSetGroup->writeSets({
                                        { { 0, 0  },                  pInput }, //reference binding via <setIndex, bindingIndex>
                                        { { "Set2", "outputBuffer" }, pOutput } //reference binding via <setName, bindingName>
                                   });

    // Bind descriptor sets to pipeline
    pCmdBuffer->cmdBindDescriptorSets(pPipeline, pDescriptorSetGroup);


    auto* pDescriptorPool = pDevice->createDescriptorPool(new DescriptorPoolCreateInfo(0, //flags
                                                                                        200, //maxSets
                                                                                        {
                                                                                           { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
                                                                                           { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10}
                                                                                        }));
    typedef struct VkDescriptorSetAllocateInfo {
        VkStructureType                 sType;
        const void*                     pNext;
        VkDescriptorPool                descriptorPool;
        uint32_t                        descriptorSetCount;
        const VkDescriptorSetLayout*    pSetLayouts;
    } VkDescriptorSetAllocateInfo;

    VkDescriptorSetAllocateInfo {
                VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                nullptr,
                m_pPool->getHandle(),
                vkLayouts.size(),
                vkLayouts.data()
            };

    //create descriptor set
    auto* pDescriptorSetGroup = pDescriptorPool->createDescriptorSetGroup("Main Group", {pPipeline->getLayout()->getDescriptorSetLayout(0)});
    //write/update descriptor set
    auto* pDescriptorSet = pDescriptorSetGroup->getSet();
#if 0
    typedef struct VkWriteDescriptorSet {
        VkStructureType                  sType;
        const void*                      pNext;
        VkDescriptorSet                  dstSet;
        uint32_t                         dstBinding;
        uint32_t                         dstArrayElement;
        uint32_t                         descriptorCount;
        VkDescriptorType                 descriptorType;
        const VkDescriptorImageInfo*     pImageInfo;
        const VkDescriptorBufferInfo*    pBufferInfo;
        const VkBufferView*              pTexelBufferView;
    } VkWriteDescriptorSet;
#endif
    //have to specify whole fucking write structure for each binding!!!
    //Can automatically figure out:
    //  dstSet, descriptorType
    //Can default value:
    //  dstArrayElement, descriptorCount
    pDescriptorSet->write({
                            {
                                0, //binding 0
                                pInputBuffer
                            },
                            {
                                1, //binding 1
                                pOutputBuffer
                            }
                        });

    auto* pCmdPool = pDevice->createCommandPool(new CommandPoolCreateInfo(0));

    auto* pCmdGroup = pCmdPool->createGroup(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

    auto* pCmdBuffer = pCmdGroup->getBuffer(0);
    pCmdBuffer->begin(VkCommandBufferBeginInfo{0}); // default flags shouldn't need dick!
        pCmdBuffer->cmdBindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE, pPipeline); // can figure out bind point automatically...
        pCmdBuffer->cmdBindDescriptorSets(VK_PIPELINE_BIND_POINT_COMPUTE, pPipeline->layout, 0, 1, DESCRIPTOR_SETS, 0, nullptr);
        pCmdBuffer->cmdBufferUpdate(pInputBuffer); //write input values to device input buffer
        pCmdBuffer->cmdDispatch(100, 1, 1);
    pCmdBuffer->end();

    auto* pQueue = pDevice->getQueue(0);
    for(int i = 0; i < 100; ++i) {
        pQueue->submit({pCmdBuffer});
        pQueue->waitIdle();
        pOutputBuffer->getMemory()->mapMemory(); //map + copy memory back to host
    }






    delete pRenderer;
    return 0;
}

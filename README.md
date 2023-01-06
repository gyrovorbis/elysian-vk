# ElysianVk #
ElysianVk is the intial Vulkan back-end implementation of the Elysian Shadows renderer. Our original renderer was a combination of OpenGL with OpenCL to give the highest-level OpenGL capabilities accross ever platform, especially on MacOS where the supported version was much lower. OpenCL interop allowed us to still have compute shaders and generic GPU buffers. All was good in the world... until Apple deprecated BOTH APIs. It immediately become time to choose another rendering path, and Vulkan, despite not technically supporting MacOS was the no-brainer for its cross-platform support and low-level performance. This codebase has actually been mostly developed under MacOS with MoltenVk, so we still intend to support the platform.

## Status ##
While this is not currently the area in the tech stack where I am working, it will absolutely be imperative to return here and finish up. Currently, there's not so much of a high-level "renderer" implemented so much as a low-level C++ Vulkan utility API and back-end which would eventually power said renderer as I work my way up the graphics stack. 

## API Features ##
* Custom CPU-side Allocators
* Debug Logger/Debug Utils Messenger Extension
* Instances, Instance Extensions
* Physical Devices, Logical Devices, Device Extensions
* DeviceMemory
* Buffers and Buffer Views
* Queues, QueueGroups
* Commands, Command Buffer Groups, Command Pools, Command Allocators
* Fences, Semaphores, Events
* Pipelines, ComputePipelines, GraphicsPipelines
* Pipeline Layouts, Shader Stages, BindingDescriptions
* DescriptorSets, DescriptorSetGroups, DescriptorSetLayoutBindings
* ShaderModule
* FrameBuffers
* SwapChain
* QueryPool
* RenderPass, Subpasses, Attachments

## Dependencies ##
AMD's "VMA" library, or "Vulkan Memory Allocator" is contained within the lib folder as a submodule for helping to manage memory allocation and resource creation. 

## Testing ##
There is only a slight amount of testing that has been done, within the testing folder, and it has a hard depdency on Qt and QTest; however, we've recently created our own cross-platform unit test framework within our core stack library which this will be ported to and built upon. 

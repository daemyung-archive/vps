//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#include <doctest.h>
#include <vps.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <iostream>

using namespace std;
using namespace doctest;

TEST_SUITE_BEGIN("image threshold to zero test suite");

//----------------------------------------------------------------------------------------------------------------------

TEST_CASE("test image threshold to zero")
{
    /*Window_desc desc;

    desc.title = default_title;
    desc.extent = default_extent;

    auto window = make_unique<Window>(desc);
    CHECK(window);

    REQUIRE(window->title() == default_title);
    REQUIRE(window->extent() == default_extent);*/


		VkResult result;
		VkInstance instance;

		{
			const char* layerNames = "VK_LAYER_KHRONOS_validation";

			VkInstanceCreateInfo create_info {};
			create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			create_info.enabledLayerCount = 1;
			create_info.ppEnabledLayerNames = &layerNames;

			result = vkCreateInstance(&create_info, nullptr, &instance);
		}

		VkPhysicalDevice physical_device;

		{
			uint32_t cnt = 1;
			result = vkEnumeratePhysicalDevices(instance, &cnt, &physical_device);
		}

		VkDevice device;

		{
			float priority = 1.0f;

			VkDeviceQueueCreateInfo queue_info {};
			queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_info.queueFamilyIndex = 0;
			queue_info.queueCount = 1;
			queue_info.pQueuePriorities = &priority;

			VkDeviceCreateInfo info {};

			info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			info.queueCreateInfoCount = 1;
			info.pQueueCreateInfos = &queue_info;

			result = vkCreateDevice(physical_device, &info, nullptr, &device);
		}

		VmaAllocator allocator;

		{
			VmaAllocatorCreateInfo create_info {};

			create_info.physicalDevice = physical_device;
			create_info.device = device;
			create_info.instance = instance;

			vmaCreateAllocator(&create_info, &allocator);
		}

		int x, y, n;
		unsigned char* contents;

		{
			contents = stbi_load(VPS_ASSET_PATH"/image/lena.bmp", &x, &y, &n, STBI_rgb_alpha);
			// contents = stbi_load("C:/Users/djang/repos/vps/build/vps/src.png", &x, &y, &n, STBI_rgb_alpha);
		}

		VkImage src_image;
		VmaAllocation src_allocation;
		VkImageView src_image_view;

		{
			VkImageCreateInfo image_info {};

			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
			image_info.extent.width = x;
			image_info.extent.height = y;
			image_info.extent.depth = 1;
			image_info.mipLevels = 1;
			image_info.arrayLayers = 1;
			image_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_info.tiling = VK_IMAGE_TILING_LINEAR;
			// image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_info.usage = VK_IMAGE_USAGE_STORAGE_BIT;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			VmaAllocationCreateInfo allocation_info {};

			allocation_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

			vmaCreateImage(allocator, &image_info, &allocation_info, &src_image, &src_allocation, nullptr);

			VkImageViewCreateInfo view_info {};

			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.image = src_image;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.layerCount = 1;
			view_info.subresourceRange.levelCount = 1;

			vkCreateImageView(device, &view_info, nullptr, &src_image_view);

		}

		{
			void* ptr;
			vmaMapMemory(allocator, src_allocation, &ptr);

			memcpy(ptr, contents, x * y * 4);

			stbi_image_free(contents);
		}

		VkImage dst_image;
		VmaAllocation dst_allocation;
		VkImageView dst_image_view;
		VmaAllocationInfo i {};

		{
			VkImageCreateInfo image_info {};

			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
			image_info.extent.width = x;
			image_info.extent.height = y;
			image_info.extent.depth = 1;
			image_info.mipLevels = 1;
			image_info.arrayLayers = 1;
			image_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_info.tiling = VK_IMAGE_TILING_LINEAR;
			image_info.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			VmaAllocationCreateInfo allocation_info {};

			allocation_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;



			vmaCreateImage(allocator, &image_info, &allocation_info, &dst_image, &dst_allocation, &i);

			VkImageViewCreateInfo view_info {};

			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.image = dst_image;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.layerCount = 1;
			view_info.subresourceRange.levelCount = 1;

			vkCreateImageView(device, &view_info, nullptr, &dst_image_view);
		}

		VkBuffer dump;
		VmaAllocation dump_alloc;
		{
			VkBufferCreateInfo buffer_create_info {};

			buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_create_info.size = x * y * 4;
			buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

			VmaAllocationCreateInfo allocation_create_info {};

			allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_COPY;

			result = vmaCreateBuffer(allocator, &buffer_create_info, &allocation_create_info,
									 &dump, &dump_alloc, nullptr);
		}

		VkCommandPool command_pool;

		{
			VkCommandPoolCreateInfo create_info {};

			create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			create_info.queueFamilyIndex = 0;

			vkCreateCommandPool(device, &create_info, nullptr, &command_pool);
		}

		VkCommandBuffer cmd_buf;

		{
			VkCommandBufferAllocateInfo allocate_info {};
			allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocate_info.commandPool = command_pool;
			allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocate_info.commandBufferCount = 1;

			vkAllocateCommandBuffers(device, &allocate_info, &cmd_buf);
		}

		VkQueue queue;
		vkGetDeviceQueue(device, 0, 0, &queue);


		VpsContextCreateInfo createInfo {};

		createInfo.instance = instance;
		createInfo.physicalDevice = physical_device;
		createInfo.device = device;

		VpsContext context = VK_NULL_HANDLE;
		vpsCreateContext(&createInfo, &context);


		VkCommandBufferBeginInfo bi {};

		bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(cmd_buf, &bi);

		{
			VkImageMemoryBarrier barriers[2];

			barriers[0] = {};
			barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barriers[0].srcAccessMask = 0;
			barriers[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barriers[0].newLayout = VK_IMAGE_LAYOUT_GENERAL;
			barriers[0].image = src_image;
			barriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barriers[0].subresourceRange.baseArrayLayer = 0;
			barriers[0].subresourceRange.baseMipLevel = 0;
			barriers[0].subresourceRange.layerCount = 1;
			barriers[0].subresourceRange.levelCount = 1;

			barriers[1] = {};
			barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barriers[1].srcAccessMask = 0;
			barriers[1].dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			barriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barriers[1].newLayout = VK_IMAGE_LAYOUT_GENERAL;
			barriers[1].image = dst_image;
			barriers[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barriers[1].subresourceRange.baseArrayLayer = 0;
			barriers[1].subresourceRange.baseMipLevel = 0;
			barriers[1].subresourceRange.layerCount = 1;
			barriers[1].subresourceRange.levelCount = 1;

			vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 2, barriers);
		}

		vpsCmdImageThresholdToZero(context, cmd_buf, src_image_view, dst_image_view, 0.2, nullptr);

		{
			VkImageMemoryBarrier barrier;

			barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.image = dst_image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;

			vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		}

		{
			VkBufferImageCopy region {};

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageSubresource.mipLevel = 0;
			region.imageExtent.width = x;
			region.imageExtent.height = y;
			region.imageExtent.depth = 1;

			vkCmdCopyImageToBuffer(cmd_buf, dst_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dump, 1, &region);
		}


		vkEndCommandBuffer(cmd_buf);

		VkSubmitInfo si {};
		si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		si.commandBufferCount = 1;
		si.pCommandBuffers = &cmd_buf;

		vkQueueSubmit(queue, 1, &si, VK_NULL_HANDLE);
		vkDeviceWaitIdle(device);

		void* p;
		vmaMapMemory(allocator, dump_alloc, &p);

		stbi_write_png("result.png", x, y, STBI_rgb_alpha, p, 0);



		// vpsDestoryContext(context);

}

//----------------------------------------------------------------------------------------------------------------------

TEST_SUITE_END();

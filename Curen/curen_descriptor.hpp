#pragma once
#include "curen_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace Curen {

    class CurenDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(CurenDevice& CurenDevice) : CurenDevice{ CurenDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<CurenDescriptorSetLayout> build() const;

        private:
            CurenDevice& CurenDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        CurenDescriptorSetLayout(
            CurenDevice& curenDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~CurenDescriptorSetLayout();
        CurenDescriptorSetLayout(const CurenDescriptorSetLayout&) = delete;
        CurenDescriptorSetLayout& operator=(const CurenDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        CurenDevice& m_curenDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class CurenDescriptorWriter;
    };

    class CurenDescriptorPool {
    public:
        class Builder {
        public:
            Builder(CurenDevice& CurenDevice) : CurenDevice{ CurenDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<CurenDescriptorPool> build() const;

        private:
            CurenDevice& CurenDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        CurenDescriptorPool(
            CurenDevice& CurenDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~CurenDescriptorPool();
        CurenDescriptorPool(const CurenDescriptorPool&) = delete;
        CurenDescriptorPool& operator=(const CurenDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        CurenDevice& m_curenDevice;
        VkDescriptorPool descriptorPool;

        friend class CurenDescriptorWriter;
    };

    class CurenDescriptorWriter {
    public:
        CurenDescriptorWriter(CurenDescriptorSetLayout& setLayout, CurenDescriptorPool& pool);

        CurenDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        CurenDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        CurenDescriptorSetLayout& setLayout;
        CurenDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace Curen
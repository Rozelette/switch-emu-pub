#pragma once
#include "deviceInterface.h"

namespace gpu {

class NvhostCtrlGpuInterface : public DeviceInterface {

    uint32_t ZcullGetCtxSize(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t ZcullGetInfo(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t GetCharacteristics(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t GetTpcMasks(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t GetL2State(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);

public:
    NvhostCtrlGpuInterface() {
        registerFunctions({
            { 1, static_cast<InterfaceFunc>(&NvhostCtrlGpuInterface::ZcullGetCtxSize), "ZcullGetCtxSize" },
            { 2, static_cast<InterfaceFunc>(&NvhostCtrlGpuInterface::ZcullGetInfo), "ZcullGetInfo" },
            { 5, static_cast<InterfaceFunc>(&NvhostCtrlGpuInterface::GetCharacteristics), "GetCharacteristics" },
            { 6, static_cast<InterfaceFunc>(&NvhostCtrlGpuInterface::GetTpcMasks), "GetTpcMasks" },
            { 20, static_cast<InterfaceFunc>(&NvhostCtrlGpuInterface::GetL2State), "GetL2State" }, // TODO wiki says this is ZBC_GET_ACTIVE_SLOT_MASK, but libnx calls this GetL2State
        });
    }

    std::string getPath() {
        return "/dev/nvhost-ctrl-gpu";
    }

private:
    // TODO own .h?
    struct ZcullGetCtxSizeArgs {
        uint32_t size;
    };

    // TODO verify actual values
    // values from gr_gk20a_init_zcull and gr_gk20a_get_zcull_info in \kernel\nvgpu\drivers\gpu\nvgpu\gk20a\gr_gk20a.c
    struct ZcullGetInfoArgs {
        uint32_t widthAlignPixels = 0x20; // gr->tpc_count * 16
        uint32_t heightAlignPixels = 0x20; // 32
        //uint32_t pixelSquaresByAliquots = 256; // XXX this is a guess, gr->zcb_count * 16 * 16 * gr->tpc_count / (gr->gpc_count * gr->gpc_tpc_count[0]);
        uint32_t pixelSquaresByAliquots = 0x400; // from https://github.com/reswitched/Mephisto/pull/25
        //uint32_t aliquotTotal = 0x10; // XXX this is a guess, gr_gpc0_zcull_total_ram_size_num_aliquots_f(gk20a_readl(g, gr_gpc0_zcull_total_ram_size_r()));
        uint32_t aliquotTotal = 0x800; // from https://github.com/reswitched/Mephisto/pull/25
        uint32_t regionByteMultiplier = 0x20; // gr->gpc_count * gr_zcull_bytes_per_aliquot_per_gpu_v()
        uint32_t regionHeaderSize = 0x20; // nvgpu_get_litter_value(g, GPU_LIT_NUM_GPCS) * gr_zcull_save_restore_header_bytes_per_gpc_v()
        uint32_t subregionHeaderSize = 0xC0; // nvgpu_get_litter_value(g, GPU_LIT_NUM_GPCS) * gr_zcull_save_restore_subregion_header_bytes_per_gpc_v()
        uint32_t subregionWidthAlignPixels = 0x20; // gr->tpc_count * gr_gpc0_zcull_zcsize_width_subregion__multiple_v()
        uint32_t subregionHeightAlignPixels = 0x40; // gr_gpc0_zcull_zcsize_height_subregion__multiple_v()
        uint32_t subregionCount = 0x10; // gr_zcull_subregion_qty_v()
    };

    struct GpuCharacteristics {
        uint32_t arch = 0x120; // NVGPU_GPU_ARCH_GM200
        uint32_t impl = 0xB; // NVGPU_GPU_IMPL_GM20B
        uint32_t rev = 0xA1; // Revision A1
        uint32_t numGpc = 0x1;
        uint64_t L2CacheSize = 0x40000;
        uint64_t onBoardVideoMemorySize = 0x0; // Not used
        uint32_t numTpcPerGpc = 0x2;
        uint32_t busType = 0x20; // NVGPU_GPU_BUS_TYPE_AXI
        uint32_t bigPageSize = 0x20000;
        uint32_t compressedPageSize = 0x20000;
        uint32_t pdeCoverageBitCount = 0x1B;
        uint32_t avaliableBigPageSizes = 0x30000;
        uint32_t gpcMask = 0x1;
        uint32_t smArchSmVersion = 0x503; // Maxwell Generation 5.0.3?
        uint32_t smArchSpaVersion = 0x503; // Maxwell Generation 5.0.3?
        uint32_t smArchWarpCount = 0x80;
        uint32_t gpuVaBitCount = 0x28;
        uint32_t reserved = 0x0;
        uint64_t flags = 0x55;
        uint32_t twodClass = 0x902D; // FERMI_TWOD_A
        uint32_t threedClass = 0xB197; // MAXWELL_B
        uint32_t computeClass = 0xB1C0; // MAXWELL_COMPUTE_B
        uint32_t gpfifoClass = 0xB06F; // MAXWELL_CHANNEL_GPFIFO_A
        uint32_t inlineToMemoryClass = 0xA140; // KEPLER_INLINE_TO_MEMORY_B
        uint32_t dmaCopyClass = 0xB0B5; // MAXWELL_DMA_COPY_A
        uint32_t maxFbpsCount = 0x1;
        uint32_t fbpEnMask = 0x0; // Disabled
        uint32_t maxLtcPerFbp = 0x2;
        uint32_t maxLtsPerLtc = 0x1;
        uint32_t maxTexPerTpc = 0x0; // Not supported
        uint32_t maxGpcCount = 0x1;
        uint32_t ropL2EnMask0 = 0x21D70; // fuse_status_opt_rop_l2_fbp_r
        uint32_t ropL2EnMask1 = 0x0;
        uint64_t chipname = 0x6230326D67; // "gm20b"
        uint64_t grCompbitStoreBaseHw = 0x0; // Not supported
    };

    struct GetCharacteristicsArgs {
        uint64_t size;
        uint64_t addr;
        GpuCharacteristics gc;
    };

    struct GetTpcMasksArgs {
        uint32_t bufSize; // should always be 0x4 (sizeof(u32) * numGpc)
        uint32_t pad;
        uint64_t bufAddr;
        uint64_t unknown_tpcMasks; // Is this where the masks are written to, inlined like in GetCharacteristics? So the addr is ignored?
        // Seems strange it is a u64 when bufSize is 4 
    };

    struct GetL2StateArgs {
        uint32_t mask; // always 0x7
        uint32_t flush; // active flush bit field
    };
};

} // namespace gpu

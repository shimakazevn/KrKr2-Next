//---------------------------------------------------------------------------
/*
    LZ4FS Storage Media Plugin for Kirikiri 2 Next
    Provides transparent LZ4 decompression for lzfs:// protocol
*/
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#include "base/StorageIntf.h"
#include "base/UtilStreams.h"
#include "plugin/PluginImpl.h"
#include "lz4frame.h"
#include "lz4.h"

#include <spdlog/spdlog.h>
#include <vector>
#include <cstring>

namespace {

class tTVPLZ4StorageMedia : public iTVPStorageMedia {
    tjs_int RefCount = 1;
    ttstr MediaName;

    static ttstr ResolvePath(const ttstr &name) {
        ttstr res = name;
        if(res.StartsWith(TJS_W("./"))) {
            res = ttstr(res.c_str() + 2);
        } else if(res.StartsWith(TJS_W("/"))) {
            res = ttstr(res.c_str() + 1);
        }
        return res;
    }

public:
    tTVPLZ4StorageMedia() : MediaName(TJS_W("lzfs")) {}
    ~tTVPLZ4StorageMedia() override = default;

    void AddRef() override { RefCount++; }
    void Release() override {
        if(RefCount <= 1) delete this;
        else RefCount--;
    }

    void GetName(ttstr &name) override { name = MediaName; }

    void NormalizeDomainName(ttstr &name) override {
        tjs_char *p = name.Independ();
        while(*p) {
            if(*p >= TJS_W('A') && *p <= TJS_W('Z')) *p += TJS_W('a') - TJS_W('A');
            p++;
        }
    }

    void NormalizePathName(ttstr &name) override {
        tjs_char *p = name.Independ();
        while(*p) {
            if(*p == TJS_W('\\')) *p = TJS_W('/');
            if(*p >= TJS_W('A') && *p <= TJS_W('Z')) *p += TJS_W('a') - TJS_W('A');
            p++;
        }
    }

    bool CheckExistentStorage(const ttstr &name) override {
        ttstr resolved = ResolvePath(name);
        if(resolved.IsEmpty()) return false;
        return TVPIsExistentStorage(resolved);
    }

    tTJSBinaryStream *Open(const ttstr &name, tjs_uint32 flags) override {
        ttstr resolved = ResolvePath(name);
        if(resolved.IsEmpty()) return nullptr;

        tTJSBinaryStream *rawStream = nullptr;
        try {
            rawStream = TVPCreateStream(resolved, flags);
        } catch(...) {
            rawStream = nullptr;
        }
        if(!rawStream) return nullptr;

        tjs_uint64 totalSize = rawStream->GetSize();
        if(totalSize < 4) {
            return rawStream;
        }

        uint32_t magic = 0;
        if(rawStream->Read(&magic, 4) != 4) {
            rawStream->SetPosition(0);
            return rawStream;
        }

        // LZ4 Frame magic: 0x184D2204 (little-endian)
        if(magic == 0x184D2204) {
            std::vector<uint8_t> compressed(static_cast<size_t>(totalSize));
            rawStream->SetPosition(0);
            rawStream->Read(compressed.data(), static_cast<tjs_uint>(totalSize));
            delete rawStream;
            rawStream = nullptr;

            LZ4F_dctx* dctx = nullptr;
            LZ4F_errorCode_t err = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
            if(LZ4F_isError(err) || !dctx) {
                spdlog::error("LZ4F_createDecompressionContext failed: {}", LZ4F_getErrorName(err));
                return nullptr;
            }

            std::vector<uint8_t> decompressed;
            decompressed.reserve(compressed.size() * 3);

            const uint8_t* srcPtr = compressed.data();
            size_t srcSizeRemaining = compressed.size();
            uint8_t dstChunk[64 * 1024];

            while(srcSizeRemaining > 0) {
                size_t srcRead = srcSizeRemaining;
                size_t dstWritten = sizeof(dstChunk);

                size_t ret = LZ4F_decompress(dctx, dstChunk, &dstWritten, srcPtr, &srcRead, nullptr);
                if(LZ4F_isError(ret)) {
                    spdlog::error("LZ4F_decompress error: {}", LZ4F_getErrorName(ret));
                    LZ4F_freeDecompressionContext(dctx);
                    return nullptr;
                }

                if(dstWritten > 0) {
                    decompressed.insert(decompressed.end(), dstChunk, dstChunk + dstWritten);
                }

                srcPtr += srcRead;
                srcSizeRemaining -= srcRead;

                if(ret == 0) {
                    break;
                }
            }

            LZ4F_freeDecompressionContext(dctx);

            auto *memStream = new tTVPMemoryStream(decompressed.data(), static_cast<tjs_uint>(decompressed.size()));
            return memStream;
        }

        // Uncompressed stream - seek back and return directly
        rawStream->SetPosition(0);
        return rawStream;
    }

    void GetListAt(const ttstr &, iTVPStorageLister *) override {}

    void GetLocallyAccessibleName(ttstr &name) override {
        name.Clear();
    }
};

} // namespace

void TVPRegisterLZ4FSPlugin() {
    static bool registered = false;
    if(registered) return;
    registered = true;

    auto *media = new tTVPLZ4StorageMedia();
    TVPRegisterStorageMedia(media);
    media->Release();
    spdlog::info("Registered LZ4FS storage media (lzfs://)");
}
